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

static void fibo_phonenum_test();

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    fibo_textTrace("sig_res_callback sig = %d", sig);
     switch (sig)
    {
	    case GAPP_SIG_OPENCPU_API_TEST:
	    {
	         UINT8 test = (UINT8)va_arg(arg, int);
	         fibo_textTrace("test = %d", test);
	        if (test == 1)
	        {	
              fibo_thread_create(fibo_phonenum_test, "fibo_phonenum_test", 1024*32, NULL, OSI_PRIORITY_NORMAL);

	        }
	    }
	    break;

	    default:
	    break;
	    
	}
    
}


static void at_res_callback(UINT8 *buf, UINT16 len)
{
    fibo_textTrace("FIBO <--%s", buf);
}


static FIBO_CALLBACK_T user_callback = {
    .fibo_signal = sig_res_callback,
    .at_resp = at_res_callback};





static void fibo_phonenum_test()
{
	char     phone_num[82]= {0}; //
    uint32_t phone_len = 82;

	fibo_sim_app_id_info_t pt_info = {0};
	pt_info.e_slot_id = 0;//SIMID
	pt_info.e_app  = E_FIBO_SIM_APP_TYPE_3GPP;
	
	int32_t ret = fibo_sim_get_phonenumber(0,&pt_info,phone_num,phone_len);
	if(ret == 0)
	{
        fibo_textTrace("[%s:%d]  num %s" , __FUNCTION__, __LINE__,phone_num); 
    }
    else
    {
       fibo_textTrace("[%s:%d]  ret %d" , __FUNCTION__, __LINE__,ret); 
    }
}

static void hello_world_demo(void *param)
{
    fibo_textTrace("application thread enter, param 0x%x", param);

    for (int n = 0; n < 100000; n++)
    {
        fibo_textTrace("hello world %d", n);
        fibo_taskSleep(200);
    }

    fibo_thread_delete();
}

void* appimg_enter(void *param)
{
    fibo_textTrace("application image enter, param 0x%x", param);


    fibo_thread_create(hello_world_demo, "mythread", 1024*2, NULL, OSI_PRIORITY_NORMAL);
    return (void *)&user_callback;
}

void appimg_exit(void)
{
    fibo_textTrace("application image exit");
}
