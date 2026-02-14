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
#include "ql_ipc.h"

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#include "osi_log.h"

static int sync_ipc_on_connect_complete(struct trusty_ipc_chan *chan)
{
    trusty_assert(chan);

    chan->complete = 1;
    return TRUSTY_EVENT_HANDLED;
}

static int sync_ipc_on_message(struct trusty_ipc_chan *chan)
{
    trusty_assert(chan);

    chan->complete = 1;
    return TRUSTY_EVENT_HANDLED;
}

static int sync_ipc_on_disconnect(struct trusty_ipc_chan *chan)
{
    trusty_assert(chan);

    chan->complete = TRUSTY_ERR_CHANNEL_CLOSED;
    return TRUSTY_EVENT_HANDLED;
}

static int wait_for_complete(struct trusty_ipc_chan *chan)
{
    int rc;

    chan->complete = 0;
    for (;;)
    {
        rc = trusty_ipc_poll_for_event(chan->dev, chan->handle);
        //rc = trusty_ipc_poll_for_event(chan->dev, 0);
        if (rc < 0)
            return rc;

        if (chan->complete)
            break;

        if (rc == TRUSTY_EVENT_NONE && !trusty_ipc_dev_has_event(chan->dev, 0))
            trusty_ipc_dev_idle(chan->dev, true);
    }

    return chan->complete;
}

static int wait_for_connect(struct trusty_ipc_chan *chan)
{
    //trusty_debug(OSI_LOGPAR_SI, "%s chan 0x%x: waiting for connect\n",
    //TRUSTY_STR, (int)chan->handle);
    return wait_for_complete(chan);
}

static int wait_for_send(struct trusty_ipc_chan *chan)
{
    //trusty_debug(OSI_LOGPAR_SI, "%s chan 0x%x: waiting for send\n",
    //TRUSTY_STR, (int)chan->handle);
    return wait_for_complete(chan);
}

static int wait_for_reply(struct trusty_ipc_chan *chan)
{
    //trusty_debug(OSI_LOGPAR_SI, "%s chan 0x%x: waiting for reply\n",
    //TRUSTY_STR, (int)chan->handle);
    return wait_for_complete(chan);
}

static struct trusty_ipc_ops sync_ipc_ops = {
    .on_connect_complete = sync_ipc_on_connect_complete,
    .on_message = sync_ipc_on_message,
    .on_disconnect = sync_ipc_on_disconnect,
};

void trusty_ipc_chan_init(struct trusty_ipc_chan *chan,
                          struct trusty_ipc_dev *dev)
{
    trusty_assert(chan);
    trusty_assert(dev);

    trusty_memset(chan, 0, sizeof(*chan));

    trusty_debug(OSI_LOGPAR_S, "%s Installing channel ops functions\n", TRUSTY_STR);
    chan->handle = INVALID_IPC_HANDLE;
    chan->dev = dev;
    chan->ops = &sync_ipc_ops;
    chan->ops_ctx = chan;
}

int trusty_ipc_connect(struct trusty_ipc_chan *chan,
                       const char *port,
                       bool wait)
{
    int rc;

    trusty_assert(chan);
    trusty_assert(chan->dev);
    trusty_assert(chan->handle == INVALID_IPC_HANDLE);
    trusty_assert(port);

    rc = trusty_ipc_dev_connect(chan->dev, port, (uint64_t)(uintptr_t)chan);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s init connection failed (%d)\n", TRUSTY_STR, rc);
        return rc;
    }
    chan->handle = (handle_t)rc;
    trusty_debug(OSI_LOGPAR_SI, "%s Channel connected, handle id(0x%x) ... OK\n", TRUSTY_STR, (int)chan->handle);

    /* got valid channel */
    if (wait)
    {
        rc = wait_for_connect(chan);
        if (rc < 0)
        {
            trusty_error(OSI_LOGPAR_SI, "%s wait for connect failed (%d)\n", TRUSTY_STR, rc);
            trusty_ipc_close(chan);
        }
    }

    return rc;
}

int trusty_ipc_close(struct trusty_ipc_chan *chan)
{
    int rc;

    trusty_assert(chan);

    rc = trusty_ipc_dev_close(chan->dev, chan->handle);
    chan->handle = INVALID_IPC_HANDLE;

    return rc;
}

int trusty_ipc_send(struct trusty_ipc_chan *chan,
                    const struct trusty_ipc_iovec *iovs,
                    size_t iovs_cnt,
                    bool wait)
{
    int rc;

    trusty_assert(chan);
    trusty_assert(chan->dev);
    trusty_assert(chan->handle);

Again:
    rc = trusty_ipc_dev_send(chan->dev, chan->handle, iovs, iovs_cnt);
    if (rc == TRUSTY_ERR_SEND_BLOCKED)
    {
        if (wait)
        {
            rc = wait_for_send(chan);
            if (rc < 0)
            {
                trusty_error(OSI_LOGPAR_SI, "%s wait to send failed (%d)\n", TRUSTY_STR, rc);
                return rc;
            }
            goto Again;
        }
    }
    return rc;
}

int trusty_ipc_recv(struct trusty_ipc_chan *chan,
                    const struct trusty_ipc_iovec *iovs,
                    size_t iovs_cnt,
                    bool wait)
{
    int rc;
    trusty_assert(chan);
    trusty_assert(chan->dev);
    trusty_assert(chan->handle);

    if (wait)
    {
        rc = wait_for_reply(chan);
        if (rc < 0)
        {
            trusty_error(OSI_LOGPAR_SI, "%s wait to reply failed (%d)\n", TRUSTY_STR, rc);
            return rc;
        }
    }

    rc = trusty_ipc_dev_recv(chan->dev, chan->handle, iovs, iovs_cnt);
    if (rc < 0)
        trusty_error(OSI_LOGPAR_SI, "%s ipc recv failed (%d)\n", TRUSTY_STR, rc);

    return rc;
}

int trusty_ipc_poll_for_event(struct trusty_ipc_dev *ipc_dev, handle_t handle)
{
    int rc;
    struct trusty_ipc_event evt;
    struct trusty_ipc_chan *chan;

    trusty_assert(ipc_dev);

    rc = trusty_ipc_dev_get_event(ipc_dev, handle, &evt);
    if (rc)
    {
        trusty_error(OSI_LOGPAR_SI, "%s get event failed (%d)\n", TRUSTY_STR, rc);
        return rc;
    }

    /* check if we have an event */
    if (!evt.event)
    {
        trusty_debug(OSI_LOGPAR_S, "%s no event\n", TRUSTY_STR);
        return TRUSTY_EVENT_NONE;
    }

    //trusty_debug(OSI_LOGPAR_SIID, "%s get event:%d handle:0x%x cookie:0x%x\n",
    //TRUSTY_STR, evt.event, evt.handle, evt.cookie);

    chan = (struct trusty_ipc_chan *)(uintptr_t)evt.cookie;
    trusty_assert(chan && chan->ops);

    /* check if we have raw event handler */
    if (chan->ops->on_raw_event)
    {
        /* invoke it first */
        rc = chan->ops->on_raw_event(chan, &evt);
        if (rc < 0)
        {
            trusty_error(OSI_LOGPAR_SII, "%s chan 0x%x: raw event cb returned (%d)\n",
                         TRUSTY_STR, chan->handle, rc);
            return rc;
        }
        if (rc > 0)
            return rc; /* handled */
    }

    if (evt.event & IPC_HANDLE_POLL_ERROR)
    {
        /* something is very wrong */
        trusty_error(OSI_LOGPAR_SI, "%s chan 0x%x: chan in error state\n",
                     TRUSTY_STR, chan->handle);
        return TRUSTY_ERR_GENERIC;
    }

    /* send unblocked should be handled first as it is edge truggered event */
    if (evt.event & IPC_HANDLE_POLL_SEND_UNBLOCKED)
    {
        if (chan->ops->on_send_unblocked)
        {
            rc = chan->ops->on_send_unblocked(chan);
            if (rc < 0)
            {
                trusty_error(OSI_LOGPAR_SII, "%s chan 0x%x: send unblocked cb returned (%d)\n",
                             TRUSTY_STR, chan->handle, rc);
                return rc;
            }
            if (rc > 0)
                return rc; /* handled */
        }
    }

    /* check for connection complete */
    if (evt.event & IPC_HANDLE_POLL_READY)
    {
        if (chan->ops->on_connect_complete)
        {
            rc = chan->ops->on_connect_complete(chan);
            if (rc < 0)
            {
                trusty_error(OSI_LOGPAR_SII, "%s chan 0x%x: ready cb returned (%d)\n",
                             TRUSTY_STR, chan->handle, rc);
                return rc;
            }
            if (rc > 0)
                return rc; /* handled */
        }
    }

    /* check for incomming messages */
    if (evt.event & IPC_HANDLE_POLL_MSG)
    {
        if (chan->ops->on_message)
        {
            rc = chan->ops->on_message(chan);
            if (rc < 0)
            {
                trusty_error(OSI_LOGPAR_SII, "%s chan 0x%x: msg cb returned (%d)\n",
                             TRUSTY_STR, chan->handle, rc);
                return rc;
            }
            if (rc > 0)
                return rc;
        }
    }

    /* check for hangups */
    if (evt.event & IPC_HANDLE_POLL_HUP)
    {
        if (chan->ops->on_disconnect)
        {
            rc = chan->ops->on_disconnect(chan);
            if (rc < 0)
            {
                trusty_error(OSI_LOGPAR_SII, "%s chan 0x%x: hup cb returned (%d)\n",
                             TRUSTY_STR, chan->handle, rc);
                return rc;
            }
            if (rc > 0)
                return rc;
        }
    }

    return TRUSTY_ERR_NONE;
}
