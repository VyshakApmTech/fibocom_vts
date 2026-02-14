/*
 * Copyright (C) 2013 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "util.h"
#include "ql_ipc.h"
#include "smcall.h"
#include "common.h"
#include "list.h"
#include "trusty_core.h"
#include "trusty_irq.h"
#include "osi_api.h"

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#include "osi_log.h"

#define SPRD_GIC_V2 2

#define REG32(x) (*((volatile uint32_t *)(x)))

#define ICACITVE (0x801380)

struct trusty_irq
{
    struct trusty_irq_state *tis;
    struct hlist_node node;
    unsigned int irq;
    bool percpu;
    bool enable;
    struct trusty_irq *percpu_ptr;
};

struct trusty_irq_irqset
{
    struct hlist_head pending;
    struct hlist_head inactive;
};

struct trusty_irq_state
{
    struct trusty_state *ts;
    struct trusty_irq_irqset normal_irqs;
    struct trusty_irq_irqset *percpu_irqs;
    unsigned int gic_version;
};

struct trusty_irq_state *tis;

unsigned int irq_num;

int trusty_irq_routable = 0;

osiThread_t *secirq_proc_task;

static void trusty_irq_enable_pending_irqs(struct trusty_irq_irqset *irqset)
{
    struct hlist_node *n;
    struct trusty_irq *trusty_irq;

    hlist_for_each_entry_safe(trusty_irq, n, &irqset->pending, node)
    {
        osiIrqEnable(trusty_irq->irq);
        hlist_del(&trusty_irq->node);
        hlist_add_head(&trusty_irq->node, &irqset->inactive);
    }
}

int trusty_irq_call_notify()
{
    if (trusty_irq_routable != 1)
        return TRUSTY_ERR_NONE;

    trusty_irq_enable_pending_irqs(&tis->normal_irqs);
    trusty_irq_enable_pending_irqs(tis->percpu_irqs);

    return TRUSTY_ERR_NONE;
}

void process_security_irq(void)
{
    osiThread_t *thread = osiThreadCurrent();

    for (;;)
    {
        osiEvent_t event = {0};
        osiEventWait(thread, &event);
        switch (event.id)
        {
        case SEND_NOP_EVENT:
            trusty_dev_nop(tis->ts->tdev);
            osiIrqSetPriority(irq_num, 0x80);
            break;
        default:
            break;
        }
    }
}

static int trusty_smc_get_next_irq(unsigned long min_irq, bool per_cpu)
{
    return trusty_fast_call32(tis->ts->tdev, SMC_FC_GET_NEXT_IRQ,
                              min_irq, per_cpu, 0);
}

static void trusty_irq_handler(void *data)
{
    struct trusty_irq *trusty_irq = (struct trusty_irq *)data;
    struct trusty_irq_state *tis = trusty_irq->tis;
    struct trusty_irq_irqset *irqset;
    osiEvent_t event = {SEND_NOP_EVENT};

    trusty_debug(OSI_LOGPAR_SII, "%s irq %d, enable %d\n",
                 TRUSTY_STR, trusty_irq->irq, trusty_irq->enable);

    switch (tis->gic_version)
    {
    case SPRD_GIC_V2:
        if (trusty_irq->percpu)
        {
            osiIrqDisable(trusty_irq->irq);
            REG32(ICACITVE + ((trusty_irq->irq / 32) << 2)) |= 1 << (trusty_irq->irq % 4);
            osiIrqSetPriority(trusty_irq->irq, 0);
            irqset = tis->percpu_irqs;
            irq_num = trusty_irq->irq;
        }
        else
        {
            osiIrqDisable(trusty_irq->irq);
            REG32(ICACITVE + ((trusty_irq->irq / 32) << 2)) |= 1 << (trusty_irq->irq % 4);
            osiIrqSetPriority(trusty_irq->irq, 0);
            irqset = &tis->normal_irqs;
            irq_num = trusty_irq->irq;
        }

        if (trusty_irq->enable)
        {
            hlist_del(&trusty_irq->node);
            hlist_add_head(&trusty_irq->node, &irqset->pending);
        }

        osiEventSend(secirq_proc_task, &event);
        break;

    default:
        trusty_error(OSI_LOGPAR_SI, "%s unexcepted gic version: %d\n",
                     TRUSTY_STR, tis->gic_version);
        return;
    }

    trusty_debug(OSI_LOGPAR_SI, "%s irq %d done\n", TRUSTY_STR, trusty_irq->irq);
}

static int trusty_irq_init_per_cpu_irq(int tirq)
{
    int ret;
    struct trusty_irq *trusty_irq;

    trusty_debug(OSI_LOGPAR_SI, "%s percup irq %d\n", TRUSTY_STR, tirq);

    trusty_irq = trusty_calloc(1, sizeof(*trusty_irq));
    if (!trusty_irq)
    {
        ret = TRUSTY_ERR_NO_MEMORY;
        goto err_alloc_percpu_irq;
    }

    trusty_irq->tis = tis;
    hlist_add_head(&trusty_irq->node, &tis->percpu_irqs->inactive);
    trusty_irq->irq = tirq;
    trusty_irq->percpu = true;
    trusty_irq->percpu_ptr = trusty_irq;

    ret = osiIrqSetHandler(trusty_irq->irq, trusty_irq_handler, trusty_irq);
    if (!ret)
    {
        trusty_error(OSI_LOGPAR_SI, "%s register percpu irq failed %d\n", TRUSTY_STR, ret);
        goto err_register_percpu_irq;
    }

    return 0;

err_register_percpu_irq:
    hlist_del(&trusty_irq->node);
    trusty_free(trusty_irq);
err_alloc_percpu_irq:
    return ret;
}

static int trusty_irq_init_normal_irq(int tirq)
{
    struct trusty_irq *trusty_irq;
    int ret;

    trusty_debug(OSI_LOGPAR_SI, "%s normal irq %d\n", TRUSTY_STR, tirq);

    trusty_irq = trusty_calloc(1, sizeof(*trusty_irq));
    if (!trusty_irq)
    {
        ret = TRUSTY_ERR_NO_MEMORY;
        goto err_alloc_normal_irq;
    }

    trusty_irq->tis = tis;
    trusty_irq->irq = tirq;
    hlist_add_head(&trusty_irq->node, &tis->normal_irqs.inactive);

    ret = osiIrqSetHandler(trusty_irq->irq, trusty_irq_handler, trusty_irq);
    if (!ret)
    {
        trusty_error(OSI_LOGPAR_SI, "%s register normal irq failed %d\n", TRUSTY_STR, ret);
        goto err_register_normal_irq;
    }

    return 0;

err_register_normal_irq:
    hlist_del(&trusty_irq->node);
    trusty_free(trusty_irq);
err_alloc_normal_irq:
    return ret;
}

static int trusty_irq_init_one(int irq, bool per_cpu)
{
    int ret;

    irq = trusty_smc_get_next_irq(irq, per_cpu);
    if (irq < 0)
        return irq;

    if (per_cpu)
        ret = trusty_irq_init_per_cpu_irq(irq);
    else
        ret = trusty_irq_init_normal_irq(irq);

    if (ret)
    {
        trusty_info(OSI_LOGPAR_SI, "%s failed to initialize irq %d, irq will be ignored\n",
                    TRUSTY_STR, irq);
    }

    return irq + 1;
}

static void trusty_irq_enable_irqset(struct trusty_irq_irqset *irqset)
{
    struct trusty_irq *trusty_irq;

    hlist_for_each_entry(trusty_irq, &irqset->inactive, node)
    {
        if (trusty_irq->enable)
        {
            trusty_info(OSI_LOGPAR_SI, "%s irq %d already enabled\n",
                        TRUSTY_STR, trusty_irq->irq);
            continue;
        }

        trusty_debug(OSI_LOGPAR_SI, "%s enable irq %d\n",
                     TRUSTY_STR, trusty_irq->irq);
        osiIrqEnable(trusty_irq->irq);
        trusty_irq->enable = true;
    }
}

int trusty_irq_init()
{
    int ret;
    int irq;

    tis = trusty_calloc(1, sizeof(*tis));
    if (!tis)
    {
        ret = TRUSTY_ERR_NO_MEMORY;
        goto err_alloc_irq_state;
    }

    tis->ts = ts;
    tis->gic_version = SPRD_GIC_V2;

    tis->percpu_irqs = trusty_calloc(1, sizeof(*(tis->percpu_irqs)));
    if (!tis->percpu_irqs)
    {
        ret = TRUSTY_ERR_NO_MEMORY;
        goto err_alloc_pending_percpu_irqs;
    }

    for (irq = 0; irq >= 0;)
        irq = trusty_irq_init_one(irq, true);
    for (irq = 0; irq >= 0;)
        irq = trusty_irq_init_one(irq, false);

    trusty_irq_enable_irqset(tis->percpu_irqs);
    trusty_irq_enable_irqset(&tis->normal_irqs);

    trusty_irq_routable = 1;

    return 0;

err_alloc_pending_percpu_irqs:
    trusty_free(tis);
err_alloc_irq_state:
    return ret;
}
