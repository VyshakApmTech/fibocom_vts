#include "osi_api.h"
#include "osi_log.h"

#ifdef CONFIG_APPIMG_APP_DEMO
#include "app.h"
#include "app_pdp.h"
#include "app_socket.h"
#else
char *app_ver = "APP_0001";
#endif

#include "app_image.h"
#include "fibo_opencpu.h"

#ifndef CONFIG_APPIMG_APP_DEMO
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
    for (int n = 0; n < 10; n++)
    {
        fibo_textTrace("app image hello world %d", n);
        fibo_taskSleep(500);
    }
}
#else

static void app_run()
{
    fibo_textTrace("app run");

    fibo_taskSleep(1000);
    const char *cmd = "ATI\r\n";
    fibo_at_send((const UINT8 *)cmd, strlen(cmd));

    app_pdp_demo();
    app_socket_demo();
}
#endif


void *appimg_enter(void *param)
{
    fibo_textTrace("app image enter");

#ifndef CONFIG_APPIMG_APP_DEMO
    fibo_thread_create(hello_world_demo, "mythread", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    return (void *)&user_callback;
#else
    return app_init(app_run);
#endif
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}
