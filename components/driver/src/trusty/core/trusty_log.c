/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#include "util.h"
#include "ql_ipc.h"
#include "ql_ipc_dev.h"
#include "sm_err.h"
#include "smcall.h"
#include "common.h"
#include "trusty_core.h"
#include "trusty_log.h"
#include "cmsis_core.h"
#include "osi_mem.h"
#include "osi_sysnv.h"

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#include "osi_log.h"

#define TRUSTY_LOG_DEFAULT_SIZE (PAGE_SIZE * 2)
#define TRUSTY_LOG_MAX_SIZE (PAGE_SIZE * 32)
#define TRUSTY_LINE_BUFFER_SIZE 256

#define va2pa(addr) (addr)

struct trusty_log_state
{
    struct trusty_state *ts;

    /*
    * This lock is here to ensure only one consumer will read
    * from the log ring buffer at a time.
    */
    osiMutex_t *lock;
    struct log_rb *log;
    uint32_t get;

    void *pfs_list;
    uint32_t num_pages;

    char line_buffer[TRUSTY_LINE_BUFFER_SIZE];
};

struct trusty_log_state *tls;
int trusty_memlog_dumpable = 0;
osiThread_t *sec_print_memlog_task;

extern int dump_trusty_memlog;

static int log_read_line(struct trusty_log_state *s, int put, int get)
{
    struct log_rb *log = s->log;
    int i;
    char c = '\0';
    size_t max_to_read = MIN((size_t)(put - get), sizeof(s->line_buffer) - 1);
    size_t mask = log->sz - 1;

    for (i = 0; i < max_to_read && c != '\n';)
        s->line_buffer[i++] = c = log->data[get++ & mask];
    s->line_buffer[i] = '\0';

    return i;
}

static void trusty_dump_logs(struct trusty_log_state *s)
{
    struct log_rb *log = s->log;
    uint32_t get, put, alloc;
    int read_chars;

    trusty_assert(is_power_of_2(log->sz));

    /*
     * For this ring buffer, at any given point, alloc >= put >= get.
     * The producer side of the buffer is not locked, so the put and alloc
     * pointers must be read in a defined order (put before alloc) so
     * that the above condition is maintained. A read barrier is needed
     * to make sure the hardware and compiler keep the reads ordered.
     */
    get = s->get;
    while ((put = log->put) != get)
    {
        /* Make sure that the read of put occurs before the read of log data */
        __DSB();

        /* Read a line from the log */
        read_chars = log_read_line(s, put, get);

        /* Force the loads from log_read_line to complete. */
        __DSB();
        alloc = log->alloc;

        /*
	 * Discard the line that was just read if the data could
	 * have been corrupted by the producer.
	 */
        if (alloc - get > log->sz)
        {
            trusty_error(OSI_LOGPAR_SI, "%s log overflow(size 0x%x)\n",
                         TRUSTY_OS_STR, log->sz);
            get = alloc - log->sz;
            continue;
        }

        trusty_os_info(OSI_LOGPAR_SS, "%s %s", TRUSTY_OS_STR, s->line_buffer);
        get += read_chars;
    }
    s->get = get;
}

static int trusty_log_call_notify()
{
    if (trusty_memlog_dumpable != 1)
        return TRUSTY_ERR_NONE;

    trusty_lock(tls->lock);
    trusty_dump_logs(tls);
    trusty_unlock(tls->lock);

    return TRUSTY_ERR_NONE;
}

void print_trusty_memlog(void)
{
    while (1)
    {
        if (dump_trusty_memlog != TRUSTY_CALL_PREPARE)
        {
            trusty_log_call_notify();
            dump_trusty_memlog = TRUSTY_CALL_PREPARE;
        }
        if (!gSysnvTraceEnabled)
        {
            osiThreadSuspend(NULL);
        }
        osiThreadSleep(3000);
    }
}

static bool trusty_supports_logging(struct trusty_dev *tdev)
{
    int ret;

    ret = trusty_std_call32(tdev, SMC_SC_SHARED_LOG_VERSION,
                            TRUSTY_LOG_API_VERSION, 0, 0);
    if (ret == SM_ERR_UNDEFINED_SMC)
    {
        trusty_info(OSI_LOGPAR_S, "%s trusty log not supported on secure side\n", TRUSTY_STR);
        return false;
    }
    else if (ret < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s trusty std call (SMC_SC_SHARED_LOG_VERSION) failed(%d)\n",
                     TRUSTY_STR, ret);
        return false;
    }

    if (ret == TRUSTY_LOG_API_VERSION)
    {
        return true;
    }
    else
    {
        trusty_info(OSI_LOGPAR_SII, "%s trusty log unsupported api version: %d, supported: %d\n",
                    TRUSTY_STR, ret, TRUSTY_LOG_API_VERSION);
        return false;
    }
}

static int trusty_log_alloc_pages(uint32_t size, void **pfs, void **va)
{
    uint32_t *pageframes;
    void *pages;
    uint32_t n, i;
    int ret = 0;

    if (size == 0 || size > (TRUSTY_LOG_MAX_SIZE >> PAGE_SHIFT))
        return TRUSTY_ERR_INVALID_ARGS;

    n = size;

    pageframes = (uint32_t *)trusty_alloc_pages(1);
    if (!pageframes)
    {
        return TRUSTY_ERR_NO_MEMORY;
    }

    pages = trusty_alloc_pages(n);
    if (!pages)
    {
        ret = TRUSTY_ERR_NO_MEMORY;
        goto error_alloc_coherent_pages;
    }

    for (i = 0; i < n; i++)
    {
        pageframes[i] = va2pa((uint32_t)(pages + i * PAGE_SIZE));
    }

    *pfs = pageframes;
    *va = pages;

    trusty_debug(OSI_LOGPAR_SIII, "%s pageframes addr:%x   pages(0/1):%x   %x\n",
                 TRUSTY_STR, pageframes, pageframes[0], pageframes[1]);

    return ret;

error_alloc_coherent_pages:
    trusty_free(pageframes);

    return ret;
}

static int trusty_log_register_buf(struct trusty_dev *tdev, void *pfs_buf,
                                   uint32_t num_pages)
{
    uint32_t *pageframes = (uint32_t *)pfs_buf;
    uint32_t pa;
    uint32_t n = num_pages;
    int ret = 0;

    while (n--)
        pageframes[n] = pageframes[n] >> PAGE_SHIFT;

    pa = va2pa((uint32_t)pageframes);

    trusty_debug(OSI_LOGPAR_SII, "%s pageframes(0/1):%x   %x\n",
                 TRUSTY_STR, pageframes[0], pageframes[1]);

    ret = trusty_std_call32(tdev, SMC_SC_SHARED_LOG_ADD, pa, 0, num_pages);
    if (ret < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s trusty std call (SMC_SC_SHARED_LOG_ADD) failed(%d)\n",
                     TRUSTY_STR, ret);
    }

    return ret;
}

int trusty_log_init(void)
{
    void *pfs_list = NULL, *log_buf = NULL;
    uint32_t num_pages;
    int ret;

    if (!trusty_supports_logging(ts->tdev))
    {
        return TRUSTY_ERR_NOT_SUPPORTED;
    }

    tls = trusty_calloc(1, sizeof(*tls));
    if (!tls)
    {
        ret = TRUSTY_ERR_NO_MEMORY;
        goto error_alloc_state;
    }

    tls->ts = ts;
    tls->lock = osiMutexCreate();

    num_pages = TRUSTY_LOG_DEFAULT_SIZE >> PAGE_SHIFT;
    ret = trusty_log_alloc_pages(num_pages, &pfs_list, &log_buf);
    if (ret < 0)
    {
        goto error_alloc_pages;
    }

    ret = trusty_log_register_buf(tls->ts->tdev, pfs_list, num_pages);
    if (ret < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s register failed(%ld)\n",
                     TRUSTY_STR, ret);
        goto error_register_buf;
    }

    tls->log = log_buf;
    tls->get = 0;
    tls->pfs_list = pfs_list;
    tls->num_pages = num_pages;

    trusty_memlog_dumpable = 1;

    return ret;

error_register_buf:
    if (pfs_list != NULL)
        trusty_free(pfs_list);
    if (log_buf != NULL)
        trusty_free(log_buf);
error_alloc_pages:
    osiMutexDelete(tls->lock);
    trusty_free(tls);
error_alloc_state:
    return ret;
}
