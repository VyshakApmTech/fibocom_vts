/* Copyright (C) 2022 FIBOCOM Technologies Limited and/or its affiliates("FIBOCOM").
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

#include "fibo_opencpu.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


static void prvInvokeGlobalCtors(void)
{
    extern void (*__init_array_start[])();
    extern void (*__init_array_end[])();

    size_t count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; ++i)
        __init_array_start[i]();
}

static void fibo_demo_test_simpin()
{
    int ret = 0;
    int sim_status = 0;
    unsigned pinremaintries = 0;
    unsigned pukremaintries = 0;
    fibo_textTrace("application thread enter, param 0x%x", param);


    //1 lock pin
    OSI_LOGI(0, "-------fibo_sim_enable_pin-------");
    ret = fibo_sim_lock_pin(1, "1234", 0);
    if (ret != 0)
    {
        OSI_LOGI(0, "fibo_sim_lock_pin lock pin error!");
        return;
    }
    osiThreadSleep(5*1000);



    //2 get pin status
    ret = fibo_sim_get_lock_status(0, sim_status, 0);
    if (ret != 0)
    {
        OSI_LOGI(0, "fibo_sim_get_lock_status error!");
        return;
    }

    OSI_LOGI(0, "fibo_sim_get_lock_status state:%d");


    //3 verify pin
    OSI_LOGI(0, "-------fibo_sim_verify_pin-------");
    ret = fibo_sim_verify_pin("1234",NULL,0);
    if (ret != 0)
    {
        OSI_LOGI(0, "fibo_sim_verify_pin verify pin error!");
        return;
    }
    osiThreadSleep(5*1000);


    //4 lock pin again
    OSI_LOGI(0, "-------fibo_sim_enable_pin-------");
    ret = fibo_sim_lock_pin(1, "1234", 0);
    if (ret != 0)
    {
        OSI_LOGI(0, "fibo_sim_pin_status error!");
        return;
    }
    osiThreadSleep(5*1000);


    //5 unlock pin use wrong pin code
    ret = fibo_sim_unlock_pin(1, "1238", 0);
    if (ret != 0)
    {
        OSI_LOGI(0, "fibo_sim_unlock_pin error!");
        return;
    }
    osiThreadSleep(5*1000);

    //6 get sim status, pinremaintries and pukremaintries
    ret = fibo_sim_pin_status(&state, &pinremaintries, &pukremaintries, 0);
    if (ret != 0)
    {
        OSI_LOGI(0, "fibo_sim_pin_status error!");
        return;
    }

    OSI_LOGI(0, "fibo_sim_pin_status state:%d,pinremaintries:%d,pukremaintries:%d");

    //7 unlock pin use correct pin code
    ret = fibo_sim_unlock_pin(1, "1234", 0);
    if (ret != 0)
    {
        OSI_LOGI(0, "fibo_sim_unlock_pin error!");
        return;
    }
    osiThreadSleep(5*1000);

    //8 get pin status
    ret = fibo_sim_get_lock_status(0, sim_status, 0);
    if (ret != 0)
    {
        OSI_LOGI(0, "fibo_sim_get_lock_status error!");
        return;
    }

    OSI_LOGI(0, "fibo_sim_get_lock_status state:%d");
}

static void imsi_recv_callback(UINT8* imsi)
{
    OSI_PRINTFI("[%s-%d]imsi_recv_callback imsi:%s", __FUNCTION__, __LINE__, imsi);
}


static void fibo_demo_test_ccidandimsi()
{
    uint8_t simId = 0;
    uint8_t ccid[32] = {0};
    if (fibo_get_imsi_asyn(imsi_recv_callback) != 0)
    {
        OSI_PRINTFI("[%s-%d]fibo_get_imsi_asyn", __FUNCTION__, __LINE__);
    }

    if (fibo_get_ccid(ccid, simId) != 0)
    {
        OSI_PRINTFI("[%s-%d]fibo_get_ccid", __FUNCTION__, __LINE__);
    }
}


static void sim_thread_entry(void *param)
{
    fibo_demo_test_simpin();
    fibo_demo_test_ccidandimsi();

    fibo_thread_delete();
}

void* appimg_enter(void *param)
{
    fibo_textTrace("application image enter, param 0x%x", param);
    prvInvokeGlobalCtors();
    fibo_thread_create(sim_thread_entry, "sim_custhread", 1024, NULL, OSI_PRIORITY_NORMAL);
    return 0;
}

void appimg_exit(void)
{
    fibo_textTrace("application image exit");
}

