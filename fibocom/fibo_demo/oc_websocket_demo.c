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
#ifdef CONFIG_SUPPORT_ALIPAY 
#include "alipay_macro_def.h"
#include "alipay_iot_coll_biz.h"
#include "alipay_iot_signer.h"
#include "ant_log.h"
#include "alipay_iot_sdk.h"
#include "ant_os.h"
#include "ant_event.h"
#include "ant_clib.h"
#include "ant_mem.h"

#define ALI_SDK_QUEUE_L1 (10)
#define ALI_SDK_QUEUE_L2 (20)
#endif


#ifdef CONFIG_SUPPORT_ALIPAY 
static void fibo_alipay_sdk_create(int priority)
{
    int ret = 0;

    fibo_textTrace("%s",__func__);

    ret = alipay_iot_coll_cache_open(DATA_QUEUE_TYPE_MDAP, ALI_SDK_QUEUE_L1, ALI_SDK_QUEUE_L2);
    fibo_textTrace("alipay_iot_coll_cache_open ret = %d", ret);
}
#endif
static void ws_data(void *c, int datatype, bool fin, void *data, uint32 len, void *arg)
{
    fibo_textTrace("fibocom recv %d,%d %.*s\n", datatype, fin, len, (char *)data);
}

static void ws_close(void *c, int reason, void *arg)
{
    fibo_textTrace("fibocom close %d\n", reason);
    fibo_ws_close(c);
}

static void websocket_thread_entry()
{
    uint8_t data_binary[8] = {0x00, 0x01, 0x34, 0x56, 0x78, 0x1a, 0x3b, 0x5a};
    char data[] = "ws socket data test";
    reg_info_t reg_info;
    fibo_pdp_profile_t pdp_profile;
    
    memset(&pdp_profile, 0, sizeof(fibo_pdp_profile_t));
    pdp_profile.cid = 1;

    while(1)
    {
        fibo_reg_info_get(&reg_info, 0);
        fibo_taskSleep(1000);
        fibo_textTrace("[%s-%d]", __FUNCTION__, __LINE__);
        if (reg_info.nStatus == 1)
        {
            //ipv4
            fibo_pdp_active(1, &pdp_profile, 0);
            fibo_taskSleep(3000);
            break;
        }
    }

    fibo_ws_opt_t opt = {5, 10, true, NULL, (INT8*)"kUqIAXaa8D6DAXjHQc4K3g==", 10*1000, NULL, 0};
    void *c = fibo_ws_open((INT8*)"ws://111.231.250.105:39888", &opt, ws_data, ws_close, NULL);
    if (c != NULL)
    {
        fibo_ws_send_text(c, (INT8*)data, strlen(data));
        fibo_taskSleep(1000);
        fibo_ws_send_binary(c, data_binary, 8);
        fibo_taskSleep(30000);
        fibo_ws_close(c);
        fibo_taskSleep(1000);
    }
}

void *appimg_enter(void *param)
{
    fibo_textTrace("application appimg_enter");
    fibo_thread_create(websocket_thread_entry, "websocket_thread_entry", 1024 * 16, NULL, OSI_PRIORITY_NORMAL);
    return NULL;
}

void appimg_exit(void)
{
    fibo_textTrace("application image exit");
}
