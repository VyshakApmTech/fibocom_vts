#include "sim.h"
#include "osi_log.h"
#include <string.h>

static uint8_t g_sim_status = 0;
static int g_check_count = 0;

// Global variables
gsm_state_t g_gsm_state = GSM_STATE_SIM_NOT_DETECTED;
pdp_state_t g_pdp_state = PDP_STATE_IDLE;
network_info_t g_network = {0};
uint8_t g_apn[30] = {0};
uint8_t g_subscriber_info_read = 0;

// Forward declarations
static void process_register(void);
static void update_signal_strength(void);

void sim_init(void)
{
    g_gsm_state = GSM_STATE_SIM_NOT_DETECTED;
    g_pdp_state = PDP_STATE_IDLE;
    g_sim_status = 0;
    g_check_count = 0;
    g_subscriber_info_read = 0;
    memset(&g_network, 0, sizeof(network_info_t));
    fibo_textTrace("SIM: Service initialized");
}

BOOL sim_is_ready(void)
{
    return (g_gsm_state >= GSM_STATE_SIM_DETECTED);
}

/*============================================================================
 * SIM State Machine - Matches Quectel GPRSThreadEntry pattern
 *============================================================================*/
void sim_process(void)
{
    switch (g_gsm_state) {
        case GSM_STATE_SIM_NOT_DETECTED:
            // Check SIM state
            if (fibo_get_sim_status_v2(&g_sim_status, 0) == 0 && g_sim_status == 1) {
                fibo_textTrace("SIM: Detected, moving to SIM_DETECTED");
                g_gsm_state = GSM_STATE_SIM_DETECTED;
                g_check_count = 0;
            } else {
                g_check_count++;
                if (g_check_count > 10) {
                    fibo_textTrace("SIM: Not detected after %d attempts", g_check_count);
                }
            }
            break;
            
        case GSM_STATE_SIM_DETECTED:
            // Read subscriber info once
            if (!g_subscriber_info_read) {
                read_subscriber_info();
                g_subscriber_info_read = 1;
            }
            // Process network registration
            process_register();
            break;
            
        case GSM_STATE_GPRS_INIT:
            // Activate GPRS
            activate_gprs();
            break;
            
        case GSM_STATE_GPRS_ACTIVE:
            // Already connected, can do other operations
            update_signal_strength();
            break;
            
        case GSM_STATE_ERROR:
            // Handle error state
            break;
    }
}

/*============================================================================
 * Process Registration - Matches Quectel ProcessREGISTER
 *============================================================================*/
static void process_register(void)
{
    reg_info_t reg_info;
    static uint8_t reg_deny_count = 0;
    
    update_signal_strength();
    
    if (fibo_reg_info_get(&reg_info, 0) != 0) {
        return;
    }
    
    fibo_textTrace("SIM: Registration status: %d", reg_info.nStatus);
    
    // Check if registration denied (status 3 = denied)
    if (reg_info.nStatus == 3) {
        reg_deny_count++;
        if (reg_deny_count > 5) {
            fibo_textTrace("SIM: Registration denied repeatedly");
            // Could switch profile here if SIM toolkit is implemented
        }
        return;
    }
    
    // Check if registered (1=registered, 5=roaming)
    if (reg_info.nStatus == 1 || reg_info.nStatus == 5) {
        fibo_textTrace("SIM: Successfully registered to network");
        g_network.is_registered = 1;
        reg_deny_count = 0;
        
        // Get operator info
        // Note: Fibocom may not have direct operator name function
        // You might need to parse from registration info
        
        // Select APN based on operator
        select_apn_by_operator();
        
        // Move to GPRS_INIT state
        g_gsm_state = GSM_STATE_GPRS_INIT;
    }
}

/*============================================================================
 * Update Signal Strength - Matches Quectel GetSignalStrength
 *============================================================================*/
static void update_signal_strength(void)
{
    INT32 rssi = 0, ber = 0;
    if (fibo_get_csq(&rssi, &ber, 0) == 0) {
        g_network.signal_strength = (uint8_t)rssi;
        fibo_textTrace("SIM: Signal strength: %d", rssi);
    }
}

/*============================================================================
 * Select APN by Operator - Matches Quectel selectAPN
 *============================================================================*/
void select_apn_by_operator(void)
{
    // This is a simplified version - you may need to get operator name
    // Fibocom may not have direct operator name API, so you might need
    // to use IMSI prefix or other methods
    
    // For now, use the APN from SIM or fallback
    if (strlen((char*)g_apn) == 0) {
        get_apn_from_sim();
    }
    
    // If still no APN, use defaults based on IMSI prefix
    if (strlen((char*)g_apn) == 0 || strcmp((char*)g_apn, "internet") == 0) {
        if (strlen(g_network.imsi) > 5) {
            // Check IMSI prefix for operator
            // 40410= Airtel, 40486= Jio, 40470= BSNL, etc.
            if (strncmp(g_network.imsi, "40410", 5) == 0) {
                strcpy((char*)g_apn, "airtelgprs.com");
                fibo_textTrace("SIM: Using Airtel APN from IMSI");
            } else if (strncmp(g_network.imsi, "40486", 5) == 0) {
                strcpy((char*)g_apn, "jionet");
                fibo_textTrace("SIM: Using Jio APN from IMSI");
            } else {
                strcpy((char*)g_apn, "internet");
                fibo_textTrace("SIM: Using default APN");
            }
        }
    }
    
    fibo_textTrace("SIM: Selected APN: %s", g_apn);
}

/*============================================================================
 * Activate GPRS - Matches Quectel ActivateGPRS
 *============================================================================*/
void activate_gprs(void)
{
    static uint8_t activation_attempts = 0;
    
    fibo_textTrace("SIM: Activating GPRS with APN: %s", g_apn);
    
    // If PDP is already active, move to GPRS_ACTIVE
    if (g_pdp_state == PDP_STATE_ACTIVE) {
        g_gsm_state = GSM_STATE_GPRS_ACTIVE;
        fibo_textTrace("SIM: GPRS already active");
        return;
    }
    
    // If already activating, wait
    if (g_pdp_state == PDP_STATE_ACTIVATING) {
        return;
    }
    
    // Start PDP activation
    g_pdp_state = PDP_STATE_ACTIVATING;
    
    fibo_pdp_profile_t pdp_profile = {0};
    pdp_profile.cid = 1;
    strcpy((char*)pdp_profile.nPdpType, "IP");
    strcpy((char*)pdp_profile.apn, (char*)g_apn);
    
    INT32 ret = fibo_pdp_active(1, &pdp_profile, 0);
    
    if (ret != 0) {
        fibo_textTrace("SIM: PDP activation failed to start, ret=%ld", ret);
        activation_attempts++;
        
        if (activation_attempts > 3) {
            fibo_textTrace("SIM: Too many activation failures, staying in GPRS_INIT");
            activation_attempts = 0;
        }
        g_pdp_state = PDP_STATE_IDLE;
    }
}

/*============================================================================
 * GPRS Activation Callback - Called from signal handler
 *============================================================================*/
void gprs_activation_callback(UINT8 cid, UINT8 sim_id)
{
    fibo_textTrace("SIM: PDP activated successfully, CID=%d", cid);
    g_pdp_state = PDP_STATE_ACTIVE;
    g_gsm_state = GSM_STATE_GPRS_ACTIVE;
    
    // Get IP address
    UINT8 ip_addr[60] = {0};
    UINT8 cid_status = 0;
    if (fibo_pdp_status_get(cid, ip_addr, &cid_status, sim_id) == 0 && cid_status == 1) {
        fibo_textTrace("SIM: IP Address: %s", ip_addr);
    }
}

/*============================================================================
 * Read Subscriber Information
 *============================================================================*/
void read_subscriber_info(void)
{
    uint8_t imsi[16] = {0};
    uint8_t iccid[23] = {0};
    
    fibo_textTrace("SIM: === SUBSCRIBER INFORMATION ===");
    
    if (fibo_get_imsi_by_simid_v2(imsi, 0) == 0) {
        strcpy(g_network.imsi, (char*)imsi);
        fibo_textTrace("SIM: IMSI: %s", g_network.imsi);
    }
    
    if (fibo_get_ccid(iccid, 0) == 0) {
        strcpy(g_network.iccid, (char*)iccid);
        fibo_textTrace("SIM: ICCID: %s", g_network.iccid);
    }
    
    get_apn_from_sim();
    fibo_textTrace("SIM: ================================");
}

/*============================================================================
 * Get APN from SIM
 *============================================================================*/
void get_apn_from_sim(void)
{
    uint8_t nPdpType[10] = {0};
    uint8_t apn_len = 0;
    
    fibo_textTrace("SIM: Reading APN from SIM card...");
    
    int32_t result = fibo_nw_get_apn(0, nPdpType, &apn_len, g_apn);
    
    if (result == 0 && apn_len > 0) {
        fibo_textTrace("SIM: SIM APN found: %s", g_apn);
    } else {
        fibo_textTrace("SIM: No APN found on SIM");
        strcpy((char*)g_apn, "");
    }
}

/*============================================================================
 * Check Network Registration (Public wrapper)
 *============================================================================*/
void check_network_registration(void)
{
    // This is now handled in process_register()
    process_register();
}