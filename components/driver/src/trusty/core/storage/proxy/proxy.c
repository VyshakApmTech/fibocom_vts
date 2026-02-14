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
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "ipc.h"
#include "log.h"
#include "storage.h"
#include "storage_proxy.h"

#define SS_FS_NS "ns"
#define SS_FS_NS_PATH "/sprd_ss"

static uint8_t req_buffer[REQ_BUFFER_SIZE + 1];

static const char *ss_fs;
static const char *ss_data_root;
static const char *ss_srv_name = STORAGE_DISK_PROXY_PORT;

static int handle_req(struct storage_msg *msg, const void *req, size_t req_len)
{
    int rc;

    if ((msg->flags & STORAGE_MSG_FLAG_POST_COMMIT) &&
        (msg->cmd != STORAGE_RPMB_SEND))
    {
        /*
         * handling post commit messages on non rpmb commands are not
         * implemented as there is no use case for this yet.
         */
        trusty_error(OSI_LOGPAR_SI, "%s cmd 0x%x: post commit option is not implemented\n", STORAGE_STR, msg->cmd);
        msg->result = STORAGE_ERR_UNIMPLEMENTED;
        return ipc_respond(msg, NULL, 0);
    }

    if (msg->flags & STORAGE_MSG_FLAG_PRE_COMMIT)
    {
        rc = storage_sync_checkpoint();
        if (rc < 0)
        {
            msg->result = STORAGE_ERR_GENERIC;
            return ipc_respond(msg, NULL, 0);
        }
    }

    if (STORAGE_RESP_BIT & msg->cmd)
    {
        if (STORAGE_NO_ERROR != msg->result)
        {
            if (STORAGE_ERR_NOT_PROGRAM_RPMB_KEY == msg->result)
            {
                trusty_error(OSI_LOGPAR_S, "%s rpmb don't program key\n", STORAGE_STR);
                return 0; //if not 0, process will be restart and connect ro server
            }
            else if (STORAGE_ERR_RPMB_KEY_DIFFERENT == msg->result)
            {
                trusty_error(OSI_LOGPAR_S, "%s rpmb key different\n");
                return 0; //if not 0, process will be restart and connect ro server
            }
            else
            {
                trusty_error(OSI_LOGPAR_SII, "%s response for cmd 0x%x error %d,server has error\n", STORAGE_STR, msg->cmd, msg->result);
                return -1;
            }
        }
        else
        {
            trusty_error(OSI_LOGPAR_SI, "%s response for cmd 0x%x OK\n", STORAGE_STR, msg->cmd);
            return 0;
        }
    }

    if (msg->cmd == STORAGE_SYNC_CHECK)
    {
        rc = storage_sync_checkpoint();
        if (rc < 0)
        {
            msg->result = STORAGE_ERR_GENERIC;
        }
        else
        {
            msg->result = STORAGE_NO_ERROR;
        }
        return ipc_respond(msg, NULL, 0);
    }

    switch (msg->cmd)
    {
    case STORAGE_FILE_DELETE:
        rc = storage_file_delete(msg, req, req_len);
        break;

    case STORAGE_FILE_OPEN:
        rc = storage_file_open(msg, req, req_len);
        break;

    case STORAGE_FILE_CLOSE:
        rc = storage_file_close(msg, req, req_len);
        break;

    case STORAGE_FILE_WRITE:
        rc = storage_file_write(msg, req, req_len);
        break;

    case STORAGE_FILE_READ:
        rc = storage_file_read(msg, req, req_len);
        break;

    case STORAGE_FILE_GET_SIZE:
        rc = storage_file_get_size(msg, req, req_len);
        break;

    case STORAGE_FILE_SET_SIZE:
        rc = storage_file_set_size(msg, req, req_len);
        break;

    default:
        trusty_error(OSI_LOGPAR_SI, "%s unhandled command 0x%x\n", STORAGE_STR, msg->cmd);
        msg->result = STORAGE_ERR_UNIMPLEMENTED;
        rc = 1;
    }

    if (rc > 0)
    {
        /* still need to send response */
        rc = ipc_respond(msg, NULL, 0);
    }
    return rc;
}

static int proxy_loop(void)
{
    ssize_t rc;
    struct storage_msg msg;

    /* enter main message handling loop */
    while (true)
    {

        trusty_debug(OSI_LOGPAR_SI, "%s to ipc_get_msg, in main loop\n", STORAGE_STR);
        /* get incoming message */
        rc = ipc_get_msg(&msg, req_buffer, REQ_BUFFER_SIZE);
        if (rc < 0)
        {
            trusty_error(OSI_LOGPAR_SI, "%s ipc_get_msg error %zd, exit main loop\n", STORAGE_STR, rc);
            return (int)rc;
        }

        trusty_debug(OSI_LOGPAR_SI, "%s to handle_req, in main loop\n", STORAGE_STR);
        /* handle request */
        req_buffer[rc] = 0; /* force zero termination */
        rc = handle_req(&msg, req_buffer, rc);
        if (rc)
        {
            trusty_error(OSI_LOGPAR_SI, "%s handle_req error %zd, exit main loop\n", STORAGE_STR, rc);
            return (int)rc;
        }
    }

    return 0;
}

static int connect_to_ns(void)
{
    struct storage_msg msg;
    int rc = -1;

    msg.cmd = STORAGE_NS_PROXY_CONNECT;
    msg.flags = 0;
    msg.size = sizeof(msg);
    rc = ipc_send_message(&msg, NULL, 0);
    if (0 > rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s ipc_send_msg cmd STORAGE_STORAGE_NS_PROXY_CONNECT error %d\n", STORAGE_STR, rc);
        return rc;
    }

    return 0;
}

int storage_entry(void)
{
    int rc;

    ss_fs = SS_FS_NS;
    ss_data_root = SS_FS_NS_PATH;

    trusty_debug(OSI_LOGPAR_SS, "%s in\n", STORAGE_STR);
    rc = ipc_init();
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SS, "%s ipc_init() failed\n", STORAGE_STR);
        return EXIT_FAILURE;
    }

    if (0 == strncmp(ss_fs, SS_FS_NS, sizeof(SS_FS_NS)))
    {

        /* initialize secure storage directory */
        rc = storage_init(ss_data_root);
        if (rc < 0)
        {
            goto _exit;
        }

        /* connect to Trusty secure storage server */
        rc = ipc_connect(ss_srv_name);
        if (rc < 0)
        {
            goto _exit;
        }

        rc = connect_to_ns();
        if (rc < 0)
        {
            ipc_disconnect();
            goto _exit;
        }

        trusty_debug(OSI_LOGPAR_SS, "%s start proxyd in fs %s\n", STORAGE_STR, ss_fs);

        /* enter main loop */
        rc = proxy_loop();

        trusty_debug(OSI_LOGPAR_SSI, "%s exiting  %s proxy with status (%d)\n", STORAGE_STR, ss_fs, rc);

        ipc_disconnect();
    }
    else
    {
        trusty_error(OSI_LOGPAR_SS, "%s invalid secure storage fs (%s)\n", STORAGE_STR, ss_fs);
        rc = -1;
    }

_exit:
    ipc_destroy();
    return (rc < 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}
