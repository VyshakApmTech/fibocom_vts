#include "osi_api.h"
#include "osi_log.h"
#include "fibo_opencpu.h"
#include "uartdebug.h"

char *app_ver = "APP_0001";

#include "app_image.h"
#include "fibo_opencpu.h"

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    fibo_textTrace("sig_res_callback  sig = %d", sig);
}

static void at_res_callback(UINT8 *buf, UINT16 len)
{
    fibo_textTrace("FIBO <--%s", buf);
}

static FIBO_CALLBACK_T user_callback = {
    .fibo_signal = sig_res_callback,
    .at_resp = at_res_callback};

static void hello_world_demo()
{
    fibo_textTrace("application thread enter");
    set_app_ver(app_ver);
	fibo_textTrace("Entering to hello_world_demo for loop\n");
	
    for (int n = 0; n < 10; n++)
    {
        fibo_textTrace("app image Changed the lib file in target %d", n);
        fibo_taskSleep(500);
    }
	fibo_taskSleep(2000);
	//const uint8_t *cmd = (const uint8_t *)"AT+CFUN=15\r\n";
    //fibo_at_send(cmd, strlen((const char *)cmd));
    //fibo_thread_delete();
	//fibo_taskSleep(2000);
	//fibo_textTrace("for loop closed entering to reboot\n");
    //fibo_cfun_set(15,0);
}


// void *appimg_enter(void *param)
// {
//     fibo_textTrace("app image enter");

//     fibo_thread_create(hello_world_demo, "mythread", 10*1024, NULL, OSI_PRIORITY_NORMAL);
//     return (void *)&user_callback;
// }

// void appimg_exit(void)
// {S
//     OSI_LOGI(0, "application image exit");
// }
