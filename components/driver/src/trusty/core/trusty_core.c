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
#include "trusty_core.h"
#include "ql_ipc_dev.h"
#include "ql_ipc.h"
#include "common.h"
#include "smcall.h"

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#include "osi_log.h"

struct trusty_state *ts;
osiMutex_t *smc_lock;

const char *trusty_version_str_get(struct trusty_state *s)
{
    return s->version_str;
}

static void trusty_init_version(struct trusty_state *s)
{
    int ret;
    int version_str_len;
    int i;

    ret = trusty_fast_call32(s->tdev, SMC_FC_GET_VERSION_STR, -1, 0, 0);
    if (ret <= 0)
        goto err_get_size;

    version_str_len = ret;

    s->version_str = trusty_calloc(1, version_str_len + 1);
    for (i = 0; i < version_str_len; i++)
    {
        ret = trusty_fast_call32(s->tdev, SMC_FC_GET_VERSION_STR, i, 0, 0);
        if (ret < 0)
            goto err_get_char;
        s->version_str[i] = ret;
    }
    s->version_str[i] = '\0';

    trusty_info(OSI_LOGPAR_SS, "%s trusty version: %s\n",
                TRUSTY_STR, s->version_str);

    return;

err_get_char:
    trusty_free(s->version_str);
    s->version_str = NULL;
err_get_size:
    trusty_error(OSI_LOGPAR_SI, "%s failed to get version: %d\n",
                 TRUSTY_STR, ret);
}

int trusty_core_init(void)
{
    int ret;

    ts = trusty_calloc(1, (sizeof(*ts)));
    if (!ts)
    {
        ret = TRUSTY_ERR_NO_MEMORY;
        goto err_allocate_state;
    }

    ts->tdev = trusty_calloc(1, (sizeof(*(ts->tdev))));
    if (!ts->tdev)
    {
        ret = TRUSTY_ERR_NO_MEMORY;
        goto err_allocate_dev;
    }

    ts->tdev->lock = osiMutexCreate();

    trusty_init_version(ts);

    ret = trusty_init_api_version(ts->tdev);
    if (ret < 0)
        goto err_api_version;

    smc_lock = osiMutexCreate();

    return 0;

err_api_version:
    if (ts->version_str)
    {
        trusty_free(ts->version_str);
    }
    osiMutexDelete(ts->tdev->lock);
err_allocate_dev:
    trusty_free(ts);
err_allocate_state:
    return ret;
}
