#include "sim.h"
#include "osi_log.h"
#include <string.h>

static sim_state_t g_sim_state = SIM_STATE_INIT;  // Changed from SIM_STATE_IDLE
static uint8_t g_sim_status = 0;
static int g_check_count = 0;

// Global variables (accessible from main.c via extern)
pdp_state_t g_pdp_state = PDP_STATE_IDLE;
uint8_t g_sim_ready_processed = 0;
uint8_t g_subscriber_info_read = 0;
uint8_t g_sim_stat = 0;
uint8_t g_apn[30] = {0};

void sim_init(void)
{
    g_sim_state = SIM_STATE_INIT;  // Changed from SIM_STATE_IDLE
    g_sim_status = 0;
    g_check_count = 0;
    fibo_textTrace("SIM: Service initialized");
}

sim_state_t sim_process(void)
{
    INT32 ret = 0;
    
    switch (g_sim_state) {
        case SIM_STATE_INIT:
            fibo_textTrace("SIM: Starting SIM check");
            g_sim_state = SIM_STATE_CHECKING;
            g_check_count = 0;
            break;
            
        case SIM_STATE_CHECKING:
            ret = fibo_get_sim_status_v2(&g_sim_status, 0);
            fibo_textTrace("SIM: SIM status check result: ret=%ld, status=%d", ret, g_sim_status);
            
            if (ret == 0 && g_sim_status == 1) {
                fibo_textTrace("SIM: Ready");
                g_sim_state = SIM_STATE_READY;
            } else {
                g_check_count++;
                if (g_check_count > 10) {
                    fibo_textTrace("SIM: Error - timeout (ret=%ld, status=%d)", ret, g_sim_status);
                    g_sim_state = SIM_STATE_ERROR;
                } else {
                    fibo_textTrace("SIM: Not ready yet (ret=%ld, status=%d, attempt=%d)", 
                                  ret, g_sim_status, g_check_count);
                }
            }
            break;
            
        case SIM_STATE_READY:
            // Only run subscriber info and network check once
            if (!g_subscriber_info_read) {
                read_subscriber_info();
                check_network_registration();
            }
            
            
            // PDP activation logic
            if(g_pdp_state == PDP_STATE_IDLE && g_sim_stat == 1) 
            {
                fibo_textTrace("SIM: Initiating PDP activation with APN: %s", g_apn);
                g_pdp_state = PDP_STATE_ACTIVATING;
                
                fibo_pdp_profile_t pdp_profile = {0};
                pdp_profile.cid = 1;
                strcpy((char*)pdp_profile.nPdpType, "IP");
                strcpy((char*)pdp_profile.apn, (char*)g_apn);
                fibo_pdp_active(1, &pdp_profile, 0);
            }
            else if(g_pdp_state == PDP_STATE_ACTIVE) 
            {
                static uint8_t data_ready_printed = 0;
                if (!data_ready_printed) {
                    fibo_textTrace("SIM: Data connection ready");
                    data_ready_printed = 1;
                }
            }
            break;
            
        case SIM_STATE_ERROR:
            // Handle error state if needed
            break;
    }
    
    return g_sim_state;
}

BOOL sim_is_ready(void)
{
    return (g_sim_state == SIM_STATE_READY);
}

/* ===== MOVED FROM MAIN.C ===== */

void read_subscriber_info(void)
{
    uint8_t imsi[16] = {0};
    uint8_t iccid[23] = {0};
    uint8_t mnc_len = 0;
    uint8_t sim_status = 0;
    
    fibo_textTrace("SIM: === SUBSCRIBER INFORMATION ===");
    
    if (fibo_get_sim_status_v2(&sim_status, 0) == 0) {
        fibo_textTrace("SIM: SIM status: %d (1=normal)", sim_status);
    }
    
    if (fibo_get_imsi_by_simid_v2(imsi, 0) == 0) {
        fibo_textTrace("SIM: IMSI: %s", imsi);
    } else {
        fibo_textTrace("SIM: Failed to get IMSI");
    }
    
    if (fibo_get_ccid(iccid, 0) == 0) {
        fibo_textTrace("SIM: ICCID: %s", iccid);
    } else {
        fibo_textTrace("SIM: Failed to get ICCID");
    }
    
    if (fibo_get_mnc_len(&mnc_len, 0) == 0) {
        fibo_textTrace("SIM: MNC length: %d", mnc_len);
    } 
    get_apn_from_sim();
    fibo_textTrace("SIM: ================================");
    //g_subscriber_info_read = 1;
}

void check_network_registration(void)
{
    reg_info_t reg_info;
    
    if (fibo_reg_info_get(&reg_info, 0) == 0) {
        fibo_textTrace("SIM: Network registration - curr_rat=%d, nStatus=%d, TAC=%d, LTE_cellId=%d, GSM_lac=%d, GSM_cellId=%d", 
                reg_info.curr_rat, 
                reg_info.nStatus, 
                reg_info.lte_scell_info.tac,
                reg_info.lte_scell_info.cell_id,
                reg_info.gsm_scell_info.lac,
                reg_info.gsm_scell_info.cell_id);
        
        if (reg_info.nStatus == 1) {
            fibo_textTrace("SIM: Successfully registered to LTE network");
            g_sim_stat = 1;
        } else {
            fibo_textTrace("SIM: Not registered yet (status=%d)", reg_info.nStatus);
        }
    } else {
        fibo_textTrace("SIM: Failed to get registration info");
    }
}

void get_apn_from_sim(void)
{
    uint8_t simId = 0;
    uint8_t nPdpType[10] = {0};
    uint8_t apn_len = 0;
    
    fibo_textTrace("SIM: Reading APN from SIM card...");
    
    int32_t result = fibo_nw_get_apn(simId, nPdpType, &apn_len, g_apn);
    
    if (result == 0 && apn_len > 0) {
        fibo_textTrace("SIM: SIM APN found: %s (length: %d, type: %s)", g_apn, apn_len, nPdpType);
        g_subscriber_info_read = 1;
    } else {
        fibo_textTrace("SIM: No APN found on SIM or read failed (result=%ld)", result);
        strcpy((char*)g_apn, "internet");
        fibo_textTrace("SIM: Using fallback APN: %s", g_apn);
        g_subscriber_info_read = 0;
    }
}