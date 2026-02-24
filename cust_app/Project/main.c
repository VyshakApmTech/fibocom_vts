#include "osi_api.h"
#include "osi_log.h"
#include "fibo_opencpu.h"
#include "sim.h"
#include "gps.h"
#include <string.h>
#include "common/log.h"
#include "sms.h"

#define TAG "MAIN"

char *app_ver = "VTS_0001";

/* ===== USER CALLBACKS ===== */

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    LOGD(TAG, "Signal callback sig = %d", sig);
    
    switch (sig) {
        case GAPP_SIG_PDP_ACTIVE_IND:
        {
            UINT8 cid = va_arg(arg, int);
            va_arg(arg, int);  // sim_id - unused
            LOGI(TAG, "PDP ACTIVATED - CID=%d", cid);
            // GSM.GSMState will be updated in activation polling
            break;
        }
        
        case GAPP_SIG_PDP_ACTIVE_OR_DEACTIVE_FAIL_IND:
        {
            UINT8 cid = va_arg(arg, int);
            va_arg(arg, int);  // sim_id - unused
            LOGE(TAG, "PDP FAILED - CID=%d", cid);
            break;
        }
        
        default:
            break;
    }
}

static void at_res_callback(UINT8 *buf, UINT16 len)
{
    LOGD(TAG, "AT Response: %s", buf);
}

static FIBO_CALLBACK_T user_callback = {
    .fibo_signal = sig_res_callback,
    .at_resp = at_res_callback
};

/* ===== USER APPLICATION THREAD ===== */
void vts_main_thread(void *param)
{
    LOGI(TAG, "Main thread started");
    
    sim_init();          // Initialize SIM
    sms_init();          // Register SMS callback
    
    while (1)
    {
        // GPRS state machine (one iteration)
        GprsThreadEntry(param);
        check_all_messages();
        delete_read_messages();
        // Check SMS flag
        if (IsSMS)
        {
            LOGI(TAG, "SMS detected, processing...");
            
            // Read and process all unread messages
            check_all_messages();
            
            
            // IsSMS cleared inside check_unread_messages
        }
        
        // Small sleep to prevent CPU hogging
        fibo_taskSleep(10);
    }
}

/* ===== APPLICATION ENTRY POINT ===== */
void *appimg_enter(void *param)
{
    LOGI(TAG, "Boot");
    
    fibo_thread_create(vts_main_thread, "vts_main", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    // Create GPS thread
    fibo_thread_create(gps_thread_entry, "gps_thread", 8*1024, NULL, OSI_PRIORITY_NORMAL);

    return (void *)&user_callback;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "VTS: Application image exit");
}