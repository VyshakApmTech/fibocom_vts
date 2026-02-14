/* Copyright (C) 2018 FIBOCOM Technologies Limited and/or its affiliates("FIBOCOM").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * FIBOCOM assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. FIBOCOM reserves the right to make changes in the
 * software without notification.  FIBOCOM also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')

#include "osi_log.h"
#include "osi_api.h"
#include "fibo_opencpu.h"

static void prvInvokeGlobalCtors(void)
{
    extern void (*__init_array_start[])();
    extern void (*__init_array_end[])();

    size_t count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; ++i)
        __init_array_start[i]();
}

static void hello_world_demo(void *param)
{
    char deviceSN[20] = {0};
    char imei[32] = {0};
    fibo_textTrace("application thread enter, param 0x%x", param);

    fibo_taskSleep(10*1000);
    fibo_get_sn(deviceSN, 0);
    fibo_textTrace("FIBO APP TEST: fibo_get_sn deviceSN: %s", deviceSN);
    fibo_get_imei(imei, 0);
    fibo_textTrace("FIBO APP TEST: fibo_get_imei imei: %s", imei);

    fibo_taskSleep(5000);

    fibo_thread_delete();
}

int appimg_enter(void *param)
{
    fibo_textTrace("application image enter, param 0x%x", param);

    prvInvokeGlobalCtors();

    fibo_thread_create(hello_world_demo, "mythread", 1024, NULL, OSI_PRIORITY_NORMAL);
    return 0;
}

void appimg_exit(void)
{
    fibo_textTrace("application image exit");
}
