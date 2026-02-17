#include "osi_api.h"
#include "osi_log.h"
#include "fibo_opencpu.h"
#include "sim.h"
#include <string.h>

char *app_ver = "VTS_0001";

// External variables
extern gsm_state_t g_gsm_state;
extern pdp_state_t g_pdp_state;
extern network_info_t g_network;

/* ===== USER CALLBACKS ===== */

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    fibo_textTrace("VTS: Signal callback sig = %d", sig);
    
    switch (sig) {
        case GAPP_SIG_PDP_ACTIVE_IND:
        {
            UINT8 cid = va_arg(arg, int);
            UINT8 sim_id = va_arg(arg, int);
            fibo_textTrace("VTS: PDP ACTIVATED SUCCESSFULLY - CID=%d", cid);
            gprs_activation_callback(cid, sim_id);
            break;
        }
        
        case GAPP_SIG_PDP_ACTIVE_OR_DEACTIVE_FAIL_IND:
        {
            UINT8 cid = va_arg(arg, int);
            UINT8 sim_id = va_arg(arg, int);
            fibo_textTrace("VTS: PDP ACTIVATION FAILED - CID=%d", cid);
            g_pdp_state = PDP_STATE_IDLE;
            break;
        }
        
        default:
            break;
    }
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

    while (1)
    {
        // Single function handles all GSM/GPRS states
        sim_process();
        
        // Application can check state and do other tasks
        if (g_gsm_state == GSM_STATE_GPRS_ACTIVE) {
            static uint8_t app_ready_printed = 0;
            if (!app_ready_printed) {
                fibo_textTrace("VTS: Application ready - GPRS active");
                app_ready_printed = 1;
                // Start your application tasks here
            }
        }
        
        fibo_taskSleep(1000);
    }
}

/* ===== APPLICATION ENTRY POINT ===== */
void *appimg_enter(void *param)
{
    fibo_textTrace("VTS Boot");
    
    sim_init();
    fibo_thread_create(vts_main_thread, "vts_main", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    
    return (void *)&user_callback;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "VTS: Application image exit");
}