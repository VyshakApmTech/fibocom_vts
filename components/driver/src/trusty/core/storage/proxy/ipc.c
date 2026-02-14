/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "util.h"
#include "ql_ipc_dev.h"
#include "ql_ipc.h"

#include "ipc.h"
#include "log.h"

#define MAX_RECONNECT_RETRY_COUNT 5
#define TRUSTY_RECONNECT_TIMEOUT_MS 50000
#define PRINT_TIME 0

static struct trusty_dev tdev; /* There should only be one trusty device */
static struct trusty_ipc_dev *tipc_dev;
static struct trusty_ipc_chan tipc_chan;

int ipc_init(void)
{
    int rc = TRUSTY_ERR_NONE;

    trusty_debug(OSI_LOGPAR_S, "%s Initializing trusty device\n", STORAGE_STR);
    rc = trusty_dev_init(&tdev, NULL);
    if (rc != 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s Initializing trusty device failed (%d)\n",
                     STORAGE_STR, rc);
        return rc;
    }

    trusty_debug(OSI_LOGPAR_S, "%s Initializing trusty IPC device\n", STORAGE_STR);
    rc = trusty_ipc_dev_create(&tipc_dev, &tdev, PAGE_SIZE * 4);
    if (rc != 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s Initializing trusty IPC device failed (%d)\n",
                     STORAGE_STR, rc);
        goto err_dev_shutdown;
    }

    return 0;

err_dev_shutdown:
    trusty_debug(OSI_LOGPAR_S, "%s Shutdown trusty device\n", STORAGE_STR);
    trusty_dev_shutdown(&tdev);
    return rc;
}

void ipc_destroy(void)
{

    trusty_debug(OSI_LOGPAR_S, "%s Shutdown trusty IPC device\n", STORAGE_STR);
    trusty_ipc_dev_shutdown(tipc_dev);

    trusty_debug(OSI_LOGPAR_S, "%s Shutdown trusty device\n", STORAGE_STR);
    trusty_dev_shutdown(&tdev);
}

int ipc_connect(const char *port)
{
    int rc;
    uint retry_cnt = 0;

    trusty_debug(OSI_LOGPAR_S, "%s Initializing trusty IPC chan\n", STORAGE_STR);
    trusty_ipc_chan_init(&tipc_chan, tipc_dev);
    while (true)
    {
        rc = trusty_ipc_connect(&tipc_chan, port, true);
        if (rc >= 0)
            break;

        trusty_error(OSI_LOGPAR_SIS, "%s Failed (%d) to connect to '%s'\n", STORAGE_STR, rc, port);
        if (++retry_cnt > MAX_RECONNECT_RETRY_COUNT)
        {
            trusty_error(OSI_LOGPAR_SI, "%s max number of reconnect retries (%d) has been reached\n", STORAGE_STR, retry_cnt);
            return -1;
        }
        osiThreadSleep(TRUSTY_RECONNECT_TIMEOUT_MS);
    }

    return 0;
}

void ipc_disconnect(void)
{
    int rc;

    trusty_debug(OSI_LOGPAR_S, "%S ipc_disconnect() to disconnect storage server\n", STORAGE_STR);
    rc = trusty_ipc_close(&tipc_chan);
    if (rc != 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s Disconnecting to trusty IPC test service failed (%d)\n", STORAGE_STR, rc);
    }
}

int ipc_get_msg(struct storage_msg *msg, void *req_buf, size_t req_buf_len)
{
    int rc;
    struct trusty_ipc_iovec resp_iovs[2] = {{msg, sizeof(*msg)}, {req_buf, req_buf_len}};

    tipc_chan.complete = 0;
    for (;;)
    {
        rc = trusty_ipc_poll_for_event(tipc_chan.dev, tipc_chan.handle);
        if (rc < 0)
        {
            trusty_error(OSI_LOGPAR_SI, "%s Failed (%d) to trusty_ipc_poll_for_event", STORAGE_STR, rc);
            return rc;
        }

        if (tipc_chan.complete)
        {
            trusty_info(OSI_LOGPAR_S, "%s have event", STORAGE_STR);
            break;
        }

        osiThreadSleep(500);
    }

    rc = trusty_ipc_recv(&tipc_chan, resp_iovs, 2, true);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s Failed (%d) to receive response message", STORAGE_STR, rc);
        return rc;
    }

    /* check for minimum size */
    if (rc < sizeof(*msg))
    {
        trusty_error(OSI_LOGPAR_SI, "%s message is too short (%d bytes received)\n", STORAGE_STR, rc);
        return -1;
    }

    /* check for message completeness */
    if (msg->size != (uint32_t)rc)
    {
        trusty_error(OSI_LOGPAR_SIII, "%s inconsistent message size [cmd=%d] (%d != %d)\n", STORAGE_STR,
                     msg->cmd, msg->size, rc);
        return -1;
    }

    return rc - sizeof(*msg);
}

int ipc_respond(struct storage_msg *msg, void *out, size_t out_size)
{
    ssize_t rc;
    struct trusty_ipc_iovec req_iovs[2] = {{msg, sizeof(*msg)}, {out, out_size}};

    msg->cmd |= STORAGE_RESP_BIT;

    rc = trusty_ipc_send(&tipc_chan, req_iovs, 2, true);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s Failed (%d) to send request message", STORAGE_STR, rc);
        return rc;
    }

    return 0;
}

int ipc_send_message(struct storage_msg *msg, void *out, size_t out_size)
{
    int rc;
    struct trusty_ipc_iovec req_iovs[2] = {{msg, sizeof(*msg)}, {out, out_size}};

    rc = trusty_ipc_send(&tipc_chan, req_iovs, (out ? 2 : 1), true);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s Failed (%d) to send request message", STORAGE_STR, rc);
        return rc;
    }

    return 0;
}

#if 0
int tee_ipc_get_msg(struct tee_storage_msg *msg, void *req_buf, size_t req_buf_len)
{
    int rc;
    struct trusty_ipc_iovec resp_iovs[2] = {{msg, sizeof(*msg)}, {req_buf, req_buf_len}};


    rc = trusty_ipc_recv(&tipc_chan, resp_iovs, 2, true);
    if (rc < 0) {
        trusty_error(OSI_LOGPAR_SI, "%s Failed (%d) to receive response message", STORAGE_STR, rc);
        return rc;
    }

   /* check for minimum size */
   if ((size_t)rc < sizeof(*msg)) {
       trusty_error(OSI_LOGPAR_SI, "%s message is too short (%d bytes received)\n", STORAGE_STR, rc);
       return -1;
   }

   /* check for message completeness */
   if (msg->size != (uint32_t)rc) {
       trusty_error(OSI_LOGPAR_SIII, "%s inconsistent message size [cmd=%d] (%d != %d)\n",
             STORAGE_STR, msg->cmd, msg->size, (uint32_t)rc);
       return -1;
   }

   return rc - (ssize_t)sizeof(*msg);
}


int tee_ipc_respond(struct tee_storage_msg *msg, struct tee_fs_rpc_operation *op, void *out, size_t out_size)
{
    int rc = 0;
    int iov_cnt = 1;
    struct trusty_ipc_iovec req_iovs[3] = {{msg, sizeof(*msg)}};


    msg->cmd |= TEE_STORAGE_RESP_BIT;
    msg->size = sizeof(struct tee_storage_msg);

    if (NULL != op) {
        req_iovs[1].base = op;
        req_iovs[1].len = sizeof(*op);
        msg->size += sizeof(*op);
        iov_cnt++;
    }

    if (out_size > 0 && NULL != out) {
        req_iovs[2].base = out;
        req_iovs[2].len = out_size;
        msg->size += (uint32_t)out_size;
        iov_cnt++;
    }

    rc = trusty_ipc_send(&tipc_chan, req_iovs, iov_cnt, true);
    if (rc < 0) {
        trusty_error(OSI_LOGPAR_SI, "%s Failed (%d) to send request message", STORAGE_STR, rc);
        return rc;
    }

    return 0;
}
#endif
