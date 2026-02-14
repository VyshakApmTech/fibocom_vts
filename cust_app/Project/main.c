#include "osi_api.h"
#include "osi_log.h"
#include "fibo_opencpu.h"

char *app_ver = "VTS_0001";

/* ===== USER CALLBACKS ===== */

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    fibo_textTrace("VTS: Signal callback sig = %d", sig);
}

static void at_res_callback(UINT8 *buf, UINT16 len)
{
    fibo_textTrace("VTS AT Response: %s", buf);
}

static FIBO_CALLBACK_T user_callback = {
    .fibo_signal = sig_res_callback,
    .at_resp = at_res_callback
};

/* ===== USER APPLICATION THREAD ===== */

void vts_main_thread(void *param)
{
    fibo_textTrace("VTS: Main thread started");

    network_init();

    while (1)
    {
        // if (gps_get_location())
        // {
        //     fibo_textTrace("VTS: Location acquired");
        // }

        // fibo_taskSleep(5000);
    }
}

/* ===== APPLICATION ENTRY POINT ===== */

void *appimg_enter(void *param)
{
    fibo_textTrace("VTS Boot");
    
    // Create main application thread
    fibo_thread_create(vts_main_thread, "vts_main", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    
    return (void *)&user_callback;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "VTS: Application image exit");
}
