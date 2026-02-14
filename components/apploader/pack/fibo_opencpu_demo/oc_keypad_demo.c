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
#include "drv_keypad.h"

static void pwr_cb(void* param)
{
    fibo_textTrace("pwr_cb: prvPwrKey_task enter");
    fibo_textTrace("pwr_cb: prvPwrKey_task param 0x%x", param);
    fibo_textTrace("pwr_cb: prvPwrKey_task exit");
}
static void pwr_callback_test(void)
{
    //fibo_set_pwr_callback(pwr_cb,1000);
    fibo_set_pwr_callback_ex(pwr_cb,1000,NULL);
}
static void prvtimeKey_Task(void *ctx)
{
    keypad_info_t key;
    while(1)
    {
        if (fibo_keypad_queue_output(&key))
        {
            fibo_textTrace("keypad_queue: id=%d status=%d press=%d",key.key_id,key.press_or_release,key.long_or_short_press);
        }
        osiThreadSleep(100);
    }
    fibo_textTrace("pwr_cb: prvPwrKey_Task exit");
    fibo_thread_delete();
}
static void prvThreadEntry(void *param)
{
    fibo_textTrace("application thread enter, param 0x%x", param);
    UINT8 pwrkeystatus = 0;
    keypad_info_t keypad = {0};
    while (1)
    {
        osiThreadSleep(500);
        keypad = fibo_get_pwrkeypad_status();
        pwrkeystatus = fibo_get_pwrkeystatus();
        if(pwrkeystatus == 0)
            fibo_textTrace("powerkey is press");
        else
            fibo_textTrace("powerkey is release");
        if (keypad.key_id == 0)
        {
            fibo_textTrace("keypad press_or_release=%d, long_or_short_press=%d\n", keypad.press_or_release, 	         keypad.long_or_short_press);
            if (keypad.press_or_release == KEY_RELEASE)
            {
                fibo_textTrace("keypad pwrkey release");
                //when release, quit
            }else
            {
                fibo_textTrace("keypad pwrkey press");
                break;
                //when press, do something
            }
        }

    }

    fibo_keypad_queue_init(3000);
    fibo_thread_create(prvtimeKey_Task, "KeytimeTask", 4096, NULL, OSI_PRIORITY_NORMAL);
    fibo_pwrkeypad_config(5000,false);//when press powerkey 1000ms,no shutdown

    fibo_thread_delete();
    return;
}

int appimg_enter(void *param)
{
    fibo_textTrace("application image enter, param 0x%x", param);

    fibo_thread_create(prvThreadEntry, "mythread", 1024*4, NULL, OSI_PRIORITY_NORMAL);
    return 0;
}

void appimg_exit(void)
{
    fibo_textTrace("application image exit");
}
