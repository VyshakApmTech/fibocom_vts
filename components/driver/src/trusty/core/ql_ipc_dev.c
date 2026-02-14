/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "util.h"
#include "ql_ipc_dev.h"
#include "ql_ipc.h"
#include "mem.h"

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#include "osi_log.h"

#define NS_PTE_PHYSADDR(pte) ((pte)&0xFFFFFFFFF000ULL)

#define QL_TIPC_DEV_RESP 0x8000
#define QL_TIPC_DEV_CONNECT 0x1
#define QL_TIPC_DEV_GET_EVENT 0x2
#define QL_TIPC_DEV_SEND 0x3
#define QL_TIPC_DEV_RECV 0x4
#define QL_TIPC_DEV_DISCONNECT 0x5
#define QL_TIPC_DEV_HAS_EVENT 0x6

#define QL_TIPC_DEV_FC_HAS_EVENT 0x100

struct trusty_ipc_cmd_hdr
{
    uint16_t opcode;
    uint16_t flags;
    uint32_t status;
    uint32_t handle;
    uint32_t payload_len;
    uint8_t payload[0];
};

struct trusty_ipc_wait_req
{
    uint64_t reserved;
};

struct trusty_ipc_connect_req
{
    uint64_t cookie;
    uint64_t reserved;
    uint8_t name[0];
};

static size_t iovec_size(const struct trusty_ipc_iovec *iovs, size_t iovs_cnt)
{
    size_t i;
    size_t cb = 0;

    trusty_assert(iovs);

    for (i = 0; i < iovs_cnt; i++)
    {
        cb += iovs[i].len;
    }

    return cb;
}

static size_t iovec_to_buf(void *buf,
                           size_t buf_len,
                           const struct trusty_ipc_iovec *iovs,
                           size_t iovs_cnt)
{
    size_t i;
    size_t buf_pos = 0;

    trusty_assert(iovs);

    for (i = 0; i < iovs_cnt; i++)
    {
        size_t to_copy = (size_t)iovs[i].len;

        if (!to_copy)
            continue;

        if (to_copy > buf_len)
            to_copy = buf_len;

        trusty_memcpy((uint8_t *)buf + buf_pos, iovs[i].base, to_copy);

        buf_pos += to_copy;
        buf_len -= to_copy;

        if (buf_len == 0)
            break;
    }

    return buf_pos;
}

static size_t buf_to_iovec(const struct trusty_ipc_iovec *iovs,
                           size_t iovs_cnt,
                           const void *buf,
                           size_t buf_len)
{
    size_t i;
    size_t copied = 0;
    const uint8_t *buf_ptr = buf;

    trusty_assert(buf_ptr);
    trusty_assert(iovs);

    if (iovs_cnt == 0 || buf_len == 0)
        return 0;

    for (i = 0; i < iovs_cnt; i++)
    {
        size_t to_copy = buf_len;

        if (to_copy > iovs[i].len)
            to_copy = iovs[i].len;

        if (!to_copy)
            continue;

        trusty_memcpy(iovs[i].base, buf_ptr, to_copy);

        copied += to_copy;
        buf_ptr += to_copy;
        buf_len -= to_copy;

        if (buf_len == 0)
            break;
    }

    return copied;
}

static int check_response(struct trusty_ipc_dev *dev,
                          volatile struct trusty_ipc_cmd_hdr *hdr,
                          uint16_t cmd)
{
    if (hdr->opcode != (cmd | QL_TIPC_DEV_RESP))
    {
        /* malformed response */
        trusty_error(OSI_LOGPAR_SI, "%s malformed response cmd: 0x%x\n",
                     TRUSTY_STR, hdr->opcode);
        return TRUSTY_ERR_SECOS_ERR;
    }

    if (hdr->status)
    {
        /* secure OS responded with error: TODO need error code */
        trusty_error(OSI_LOGPAR_SII, "%s cmd 0x%x: status = %d\n",
                     TRUSTY_STR, hdr->opcode, hdr->status);
        return TRUSTY_ERR_SECOS_ERR;
    }

    return TRUSTY_ERR_NONE;
}

int trusty_ipc_dev_create(struct trusty_ipc_dev **idev,
                          struct trusty_dev *tdev,
                          size_t shared_buf_size)
{
    int rc;
    struct trusty_ipc_dev *dev;

    trusty_assert(idev);
    trusty_assert(!(shared_buf_size % PAGE_SIZE));
    trusty_debug(OSI_LOGPAR_S, "%s Create new trusty IPC device\n", TRUSTY_STR);

    /* allocate device context */
    dev = trusty_calloc(1, sizeof(*dev));
    if (!dev)
    {
        trusty_error(OSI_LOGPAR_S, "%s failed to allocate trusty IPC device\n", TRUSTY_STR);
        return TRUSTY_ERR_NO_MEMORY;
    }
    dev->tdev = tdev;

    /* allocate shared buffer */
    dev->buf_size = shared_buf_size;
    dev->buf_vaddr = trusty_alloc_pages(shared_buf_size / PAGE_SIZE);
    if (!dev->buf_vaddr)
    {
        trusty_error(OSI_LOGPAR_S, "%s failed to allocate shared memory\n", TRUSTY_STR);
        rc = TRUSTY_ERR_NO_MEMORY;
        goto err_alloc_pages;
    }

    /* Get memory attributes */
    rc = trusty_encode_page_info(&dev->buf_ns, dev->buf_vaddr);
    if (rc != 0)
    {
        trusty_error(OSI_LOGPAR_S, "%s failed to get shared memory attributes\n", TRUSTY_STR);
        rc = TRUSTY_ERR_GENERIC;
        goto err_page_info;
    }

    /* call secure OS to register shared buffer */
    rc = trusty_dev_init_ipc(dev->tdev, &dev->buf_ns, dev->buf_size);
    if (rc != 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s failed (%d) to create trusty IPC device\n",
                     TRUSTY_STR, rc);
        rc = TRUSTY_ERR_SECOS_ERR;
        goto err_create_sec_dev;
    }

    trusty_debug(OSI_LOGPAR_SI, "%s New trusty IPC device (0x%p)\n", TRUSTY_STR, dev);

    *idev = dev;
    return TRUSTY_ERR_NONE;

err_page_info:
err_create_sec_dev:
    trusty_free_pages(dev->buf_vaddr, dev->buf_size / PAGE_SIZE);
err_alloc_pages:
    trusty_free(dev);
    return rc;
}

void trusty_ipc_dev_shutdown(struct trusty_ipc_dev *dev)
{
    int rc;
    trusty_assert(dev);

    //trusty_debug(OSI_LOGPAR_SI, "%s Shutting down trusty IPC device (0x%p)\n",
    //TRUSTY_STR, dev);

    /* shutdown trusty IPC device */
    rc = trusty_dev_shutdown_ipc(dev->tdev, &dev->buf_ns, dev->buf_size);
    trusty_assert(!rc);
    if (rc != 0)
    {
        trusty_error(OSI_LOGPAR_S, "%s failed (%d) to shutdown trusty IPC device\n",
                     TRUSTY_STR, rc);
    }
    trusty_free_pages(dev->buf_vaddr, dev->buf_size / PAGE_SIZE);
    trusty_free(dev);
}

int trusty_ipc_dev_connect(struct trusty_ipc_dev *dev,
                           const char *port,
                           uint64_t cookie)
{
    int rc;
    size_t port_len;
    volatile struct trusty_ipc_cmd_hdr *cmd;
    struct trusty_ipc_connect_req *req;

    trusty_assert(dev);
    trusty_assert(port);

    trusty_debug(OSI_LOGPAR_SS, "%s Connecting to '%s'\n", TRUSTY_STR, port);

    /* check port name length */
    port_len = trusty_strlen(port) + 1;
    if (port_len > (dev->buf_size - sizeof(*cmd) - sizeof(*req)))
    {
        /* it would not fit into buffer */
        trusty_error(OSI_LOGPAR_SI, "%s port name is too long (%d)\n",
                     TRUSTY_STR, port_len);
        return TRUSTY_ERR_INVALID_ARGS;
    }

    /* prepare command */
    cmd = dev->buf_vaddr;
    trusty_memset((void *)cmd, 0, sizeof(*cmd));
    cmd->opcode = QL_TIPC_DEV_CONNECT;

    /* prepare payload  */
    req = (struct trusty_ipc_connect_req *)cmd->payload;
    trusty_memset((void *)req, 0, sizeof(*req));
    req->cookie = cookie;
    trusty_strcpy((char *)req->name, port);
    cmd->payload_len = sizeof(*req) + port_len;

    /* call secure os */
    rc = trusty_dev_exec_ipc(dev->tdev, &dev->buf_ns,
                             sizeof(*cmd) + cmd->payload_len);
    if (rc)
    {
        /* secure OS returned an error */
        trusty_error(OSI_LOGPAR_SI, "%s secure OS returned (%d)\n", TRUSTY_STR, rc);
        return TRUSTY_ERR_SECOS_ERR;
    }

    rc = check_response(dev, cmd, QL_TIPC_DEV_CONNECT);
    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s connect cmd failed (%d)\n", TRUSTY_STR, rc);
        return rc;
    }

    /* success */
    return cmd->handle;
}

int trusty_ipc_dev_close(struct trusty_ipc_dev *dev, handle_t handle)
{
    int rc;
    volatile struct trusty_ipc_cmd_hdr *cmd;

    trusty_assert(dev);

    //trusty_debug(OSI_LOGPAR_SI, "%s Closing chan 0x%x\n", TRUSTY_STR, handle);

    /* prepare command */
    cmd = dev->buf_vaddr;
    trusty_memset((void *)cmd, 0, sizeof(*cmd));
    cmd->opcode = QL_TIPC_DEV_DISCONNECT;
    cmd->handle = handle;
    /* no payload */

    /* call into secure os */
    rc = trusty_dev_exec_ipc(dev->tdev, &dev->buf_ns,
                             sizeof(*cmd) + cmd->payload_len);
    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s secure OS returned (%d)\n",
                     TRUSTY_STR, rc);
        return TRUSTY_ERR_SECOS_ERR;
    }

    rc = check_response(dev, cmd, QL_TIPC_DEV_DISCONNECT);
    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s disconnect cmd failed (%d)\n",
                     TRUSTY_STR, rc);
        return rc;
    }

    //trusty_debug(OSI_LOGPAR_SI, "%s Closed chan 0x%x\n", TRUSTY_STR, handle);

    return TRUSTY_ERR_NONE;
}

bool trusty_ipc_dev_has_event(struct trusty_ipc_dev *dev, handle_t chan)
{
    int rc;
    bool has_event;
    volatile struct trusty_ipc_cmd_hdr *cmd;

    trusty_assert(dev);

    /* prepare command */
    cmd = dev->buf_vaddr;
    trusty_memset((void *)cmd, 0, sizeof(*cmd));
    //cmd->opcode = QL_TIPC_DEV_FC_HAS_EVENT;
    cmd->opcode = QL_TIPC_DEV_HAS_EVENT;

    cmd->handle = chan;

    /* prepare payload  */
    cmd->payload_len = 0;

    /* call into secure os
    rc = trusty_dev_exec_fc_ipc(dev->tdev, &dev->buf_ns,
                                sizeof(*cmd) + cmd->payload_len);
    */

    rc = trusty_dev_exec_ipc(dev->tdev, &dev->buf_ns,
                             sizeof(*cmd) + cmd->payload_len);

    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s secure OS returned (%d)\n",
                     TRUSTY_STR, rc);
        return false;
    }

    rc = check_response(dev, cmd, QL_TIPC_DEV_FC_HAS_EVENT);
    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s get event cmd failed (%d)\n",
                     TRUSTY_STR, rc);
        return false;
    }

    if ((size_t)cmd->payload_len < sizeof(has_event))
    {
        trusty_error(OSI_LOGPAR_SI, "%s invalid response length (%d)\n",
                     TRUSTY_STR, (size_t)cmd->payload_len);
        return false;
    }

    /* copy out event */
    trusty_memcpy(&has_event, (const void *)cmd->payload, sizeof(has_event));
    return has_event;
}

int trusty_ipc_dev_get_event(struct trusty_ipc_dev *dev,
                             handle_t chan,
                             struct trusty_ipc_event *event)
{
    int rc;
    volatile struct trusty_ipc_cmd_hdr *cmd;

    trusty_assert(dev);
    trusty_assert(event);

    /* prepare command */
    cmd = dev->buf_vaddr;
    trusty_memset((void *)cmd, 0, sizeof(*cmd));
    cmd->opcode = QL_TIPC_DEV_GET_EVENT;
    cmd->handle = chan;

    /* prepare payload  */
    trusty_memset((void *)cmd->payload, 0, sizeof(struct trusty_ipc_wait_req));
    cmd->payload_len = sizeof(struct trusty_ipc_wait_req);

    /* call into secure os */
    rc = trusty_dev_exec_ipc(dev->tdev, &dev->buf_ns,
                             sizeof(*cmd) + cmd->payload_len);
    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s secure OS returned (%d)\n", TRUSTY_STR, rc);
        return TRUSTY_ERR_SECOS_ERR;
    }

    rc = check_response(dev, cmd, QL_TIPC_DEV_GET_EVENT);
    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s get event cmd failed (%d)\n", TRUSTY_STR, rc);
        return rc;
    }

    if ((size_t)cmd->payload_len < sizeof(*event))
    {
        trusty_error(OSI_LOGPAR_SI, "%s invalid response length (%d)\n",
                     TRUSTY_STR, (size_t)cmd->payload_len);
        return TRUSTY_ERR_SECOS_ERR;
    }

    /* copy out event */
    trusty_memcpy(event, (const void *)cmd->payload, sizeof(*event));
    return TRUSTY_ERR_NONE;
}

int trusty_ipc_dev_send(struct trusty_ipc_dev *dev,
                        handle_t chan,
                        const struct trusty_ipc_iovec *iovs,
                        size_t iovs_cnt)
{
    int rc;
    size_t msg_size;
    volatile struct trusty_ipc_cmd_hdr *cmd;

    trusty_assert(dev);
    /* calc message length */
    msg_size = iovec_size(iovs, iovs_cnt);
    if (msg_size > dev->buf_size - sizeof(*cmd))
    {
        /* msg is too big to fit provided buffer */
        trusty_error(OSI_LOGPAR_SII, "%s chan 0x%x: msg is too long (%d)\n",
                     TRUSTY_STR, chan, msg_size);
        return TRUSTY_ERR_MSG_TOO_BIG;
    }

    /* prepare command */
    cmd = dev->buf_vaddr;
    trusty_memset((void *)cmd, 0, sizeof(*cmd));
    cmd->opcode = QL_TIPC_DEV_SEND;
    cmd->handle = chan;

    /* copy in message data */
    cmd->payload_len = (uint32_t)msg_size;
    msg_size = iovec_to_buf(dev->buf_vaddr + sizeof(*cmd),
                            dev->buf_size - sizeof(*cmd), iovs, iovs_cnt);
    trusty_assert(msg_size == (size_t)cmd->payload_len);

    /* call into secure os */
    rc = trusty_dev_exec_ipc(dev->tdev, &dev->buf_ns,
                             sizeof(*cmd) + cmd->payload_len);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s secure OS returned (%d)\n",
                     TRUSTY_STR, rc);
        return TRUSTY_ERR_SECOS_ERR;
    }

    rc = check_response(dev, cmd, QL_TIPC_DEV_SEND);
    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s send msg failed (%d)\n", TRUSTY_STR, rc);
    }

    return rc;
}

int trusty_ipc_dev_recv(struct trusty_ipc_dev *dev,
                        handle_t chan,
                        const struct trusty_ipc_iovec *iovs,
                        size_t iovs_cnt)
{
    int rc;
    size_t copied;
    volatile struct trusty_ipc_cmd_hdr *cmd;

    trusty_assert(dev);

    /* prepare command */
    cmd = dev->buf_vaddr;
    trusty_memset((void *)cmd, 0, sizeof(*cmd));
    cmd->opcode = QL_TIPC_DEV_RECV;
    cmd->handle = chan;
    /* no payload */

    /* call into secure os */
    rc = trusty_dev_exec_ipc(dev->tdev, &dev->buf_ns,
                             sizeof(*cmd) + cmd->payload_len);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s secure OS returned (%d)\n", TRUSTY_STR, rc);
        return TRUSTY_ERR_SECOS_ERR;
    }

    rc = check_response(dev, cmd, QL_TIPC_DEV_RECV);
    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s recv cmd failed (%d)\n", TRUSTY_STR, rc);
        return rc;
    }

    /* copy data out to proper destination */
    copied = buf_to_iovec(iovs, iovs_cnt, (const void *)cmd->payload,
                          cmd->payload_len);
    if (copied != (size_t)cmd->payload_len)
    {
        /* msg is too big to fit provided buffer */
        trusty_error(OSI_LOGPAR_SIII, "%s chan 0x%x: buffer too small (%d vs. %d)\n",
                     TRUSTY_STR, chan, copied, (size_t)cmd->payload_len);
        return TRUSTY_ERR_MSG_TOO_BIG;
    }

    return (int)copied;
}

void trusty_ipc_dev_idle(struct trusty_ipc_dev *dev, bool event_poll)
{
    trusty_idle(dev->tdev, event_poll);
}
