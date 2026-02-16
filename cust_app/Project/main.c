#include "osi_api.h"
#include "osi_log.h"
#include "fibo_opencpu.h"
#include "sim.h"

char *app_ver = "VTS_0001";
/* Add state variables */
static uint8_t g_sim_ready_processed = 0;
static uint8_t g_subscriber_info_read = 0;

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

/* ===== READ SUBSCRIBER INFORMATION ===== */
void read_subscriber_info(void)
{
    uint8_t imsi[16] = {0};
    uint8_t iccid[23] = {0};
    uint8_t mnc_len = 0;
    uint8_t sim_status = 0;
    
    fibo_textTrace("VTS: === SUBSCRIBER INFORMATION ===");
    
    // 1. Get SIM status again (just to confirm)
    if (fibo_get_sim_status_v2(&sim_status, 0) == 0) {
        fibo_textTrace("VTS: SIM status: %d (1=normal)", sim_status);
    }
    
    // 2. Get IMSI (International Mobile Subscriber Identity)
    if (fibo_get_imsi_by_simid_v2(imsi, 0) == 0) {
        fibo_textTrace("VTS: IMSI: %s", imsi);
    } else {
        fibo_textTrace("VTS: Failed to get IMSI");
    }
    
    // 3. Get ICCID (Integrated Circuit Card ID)
    if (fibo_get_ccid(iccid, 0) == 0) {
        fibo_textTrace("VTS: ICCID: %s", iccid);
    } else {
        fibo_textTrace("VTS: Failed to get ICCID");
    }
    
    // 4. Get MNC length (Mobile Network Code length - 2 or 3 digits)
    if (fibo_get_mnc_len(&mnc_len, 0) == 0) {
        fibo_textTrace("VTS: MNC length: %d", mnc_len);
    }
    
    // 5. Get phone number if available (optional)
    char phone_num[20] = {0};
    fibo_sim_app_id_info_t app_info = {0};
    if (fibo_sim_get_phonenumber(0, &app_info, phone_num, sizeof(phone_num)) == 0) {
        if (strlen(phone_num) > 0) {
            fibo_textTrace("VTS: Phone number: %s", phone_num);
        } else {
            fibo_textTrace("VTS: No phone number stored on SIM");
        }
    }
    
    fibo_textTrace("VTS: ================================");
    
    g_subscriber_info_read = 1;
}

/* ===== CHECK NETWORK REGISTRATION ===== */
void check_network_registration(void)
{
    reg_info_t reg_info;
    
    if (fibo_reg_info_get(&reg_info, 0) == 0) {
        fibo_textTrace("VTS: Network registration - curr_rat=%d, nStatus=%d, TAC=%d, LTE_cellId=%d", 
                      reg_info.curr_rat, 
                      reg_info.nStatus, 
                      reg_info.lte_scell_info.tac,
                      reg_info.lte_scell_info.cell_id);
        
        if (reg_info.nStatus == 1) {
            fibo_textTrace("VTS: Successfully registered to LTE network");
        } else {
            fibo_textTrace("VTS: Not registered yet (status=%d)", reg_info.nStatus);
        }
    } else {
        fibo_textTrace("VTS: Failed to get registration info");
    }
}


/* ===== USER APPLICATION THREAD ===== */
void vts_main_thread(void *param)
{
    fibo_textTrace("VTS: Main thread started");

    while (1)
    {
        sim_process();  // Call repeatedly to check SIM status
        
        if (sim_is_ready()) 
        {
            // SIM is ready, you can proceed with other operations
            fibo_textTrace("VTS: SIM ready");
            reg_info_t reg_info;
            if (fibo_reg_info_get(&reg_info, 0) == 0) 
            {
                fibo_textTrace("VTS: Network registration - curr_rat=%d, nStatus=%d, TAC=%d, LTE_cellId=%d, GSM_lac=%d, GSM_cellId=%d", 
                reg_info.curr_rat, 
                reg_info.nStatus, 
                reg_info.lte_scell_info.tac,
                reg_info.lte_scell_info.cell_id,
                reg_info.gsm_scell_info.lac,
                reg_info.gsm_scell_info.cell_id);
            }
            else 
            {
                fibo_textTrace("VTS: Failed to get registration info");
            }

            // STEP 1: Read subscriber information (IMSI, ICCID, etc.)
            read_subscriber_info();
            // STEP 2: Check network registration
            check_network_registration();
        }
        fibo_taskSleep(1000);  // Check every second
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
