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
#include "trusty_log.h"
#include "trusty_irq.h"

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#include "osi_log.h"

void drvTrustyInit(void)
{
    trusty_core_init();
    trusty_log_init();
    trusty_irq_init();

    sec_print_memlog_task = osiThreadCreate("print trusty memlog", (osiCallback_t)print_trusty_memlog,
                                            NULL, OSI_PRIORITY_NORMAL, 2048, 0);

    secirq_proc_task = osiThreadCreate("process security irq",
                                       (osiCallback_t)process_security_irq,
                                       NULL, OSI_PRIORITY_NORMAL, 2048, 64);
}
