#include "osi_api.h"
#include "osi_log.h"
#include "fibo_opencpu.h"
#include "sim.h"
#include <string.h>

char *app_ver = "VTS_0001";


// External variables from sim.c
extern pdp_state_t g_pdp_state;
extern uint8_t g_sim_ready_processed;
extern uint8_t g_subscriber_info_read;
extern uint8_t g_sim_stat;
extern uint8_t g_apn[100];


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
            g_pdp_state = PDP_STATE_ACTIVE;
            break;
        }
        
        case GAPP_SIG_PDP_ACTIVE_OR_DEACTIVE_FAIL_IND:
        {
            UINT8 cid = va_arg(arg, int);
            UINT8 sim_id = va_arg(arg, int);
            fibo_textTrace("VTS: PDP ACTIVATION FAILED - CID=%d", cid);
            g_pdp_state = PDP_STATE_FAILED;
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
        // Single function call handles everything
        sim_process();
        
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