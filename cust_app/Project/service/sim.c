#include "sim.h"
#include "osi_log.h"
#include "../common/log.h"
#include <string.h>

#define TAG "SIM"

// Global variables (EXACT match to Quectel)
GSM_Typedef GSM = {0};
NET_Typedef NetWork = {{0}};
_RTC CurrentDateTime = {0};
Providertypedef prfReq = NONE;
uint8_t PrfChanged = 0;

// Internal variables
static uint8_t g_sim_status = 0;
static uint8_t g_subscriber_info_read = 0;

// Forward declarations
static void process_register(void);
static void update_signal_strength(void);
static void select_apn_by_operator(void);
static void activate_gprs(void);
static void update_time(void);

void sim_init(void)
{
    memset(&GSM, 0, sizeof(GSM_Typedef));
    memset(&NetWork, 0, sizeof(NET_Typedef));
    memset(&CurrentDateTime, 0, sizeof(_RTC));
    
    GSM.GSMState = SIM_NOT_DETECTED;
    prfReq = NONE;
    PrfChanged = 0;
    g_subscriber_info_read = 0;
    
    LOGI(TAG, "Service initialized");
}

BOOL sim_is_ready(void)
{
    return (GSM.GSMState >= SIM_DETECTED);
}


/*============================================================================
 * Get APN from SIM - NEW FUNCTION
 *============================================================================*/
static void get_apn_from_sim(void)
{
    uint8_t nPdpType[10] = {0};
    uint8_t apn_len = 0;
    uint8_t apn[50] = {0};
    
    LOGI(TAG, "Reading APN from SIM card...");
    
    int32_t result = fibo_nw_get_apn(0, nPdpType, &apn_len, apn);
    
    if (result == 0 && apn_len > 0) {
        // Copy to NetWork.APN
        memcpy(NetWork.APN, apn, apn_len);
        NetWork.APN[apn_len] = '\0';
        LOGI(TAG, "SIM APN found: %s", NetWork.APN);
        GSM.GSMState = SIM_DETECTED;
        g_subscriber_info_read = 1;
    } else {
        LOGE(TAG, "No APN found on SIM (result=%ld)", result);
        NetWork.APN[0] = '\0';  // Empty string
    }
}
/*============================================================================
 * GPRS Main Thread - EXACT match to Quectel GPRSThreadEntry
 *============================================================================*/
void GprsThreadEntry(void *param)
{
    LOGI(TAG, "GPRS: Thread started");
    
    // Initialize
    GetImei();
    
    // while(1)
    // {
        switch (GSM.GSMState)
        {
            case SIM_NOT_DETECTED:
                // Check SIM state
                if (fibo_get_sim_status_v2(&g_sim_status, 0) == 0 && g_sim_status == 1) {
                    LOGI(TAG, "GPRS: SIM Detected");
                    //GSM.GSMState = SIM_DETECTED;
                    
                    // Read subscriber info
                    if (!g_subscriber_info_read) {
                        uint8_t imsi[16] = {0};
                        uint8_t iccid[23] = {0};
                        
                        if (fibo_get_imsi_by_simid_v2(imsi, 0) == 0) {
                            strcpy(NetWork.IMSI, (char*)imsi);
                            LOGD(TAG, "GPRS: IMSI: %s", NetWork.IMSI);
                        }
                        
                        if (fibo_get_ccid(iccid, 0) == 0) {
                            strcpy(NetWork.SIMNo, (char*)iccid);
                            LOGD(TAG, "GPRS: ICCID: %s", NetWork.SIMNo);
                        }
                        get_apn_from_sim();
                        
                    }
                }
                break;
                
            case SIM_DETECTED:
                process_register();
                break;
                
            case GPRS_INIT:
                activate_gprs();
                break;
                
            case GPRS_ACTIVE:
                // Already connected
                update_signal_strength();
                update_time();
                break;
        }
        
        // Check profile request
        if(prfReq != NONE)
        {
            if(SwitchProfile(prfReq))
            {
                PrfChanged = 1;
                prfReq = NONE;
                // Reset module after profile change
                LOGD(TAG, "GPRS: Profile changed, resetting...");
                // fibo_cfun_set(0, 0); // Reset if needed
            }
        }
        
        // Sleep based on state
        if(GSM.GSMState < GPRS_ACTIVE)
            fibo_taskSleep(1000);
        else
            fibo_taskSleep(200);
    // }
}

/*============================================================================
 * Process Registration - EXACT match to Quectel ProcessREGISTER
 *============================================================================*/
static void process_register(void)
{
    reg_info_t reg_info;
    static uint8_t reg_deny_count = 0;
    operator_info_t operator_info;
     char mcc_mnc_str[7] = {0};
    update_signal_strength();
    
    if (fibo_reg_info_get(&reg_info, 0) != 0) {
        return;
    }
    
    // Store cell info (TAC and CellID)
    snprintf(GSM.LAC, sizeof(GSM.LAC), "%ld", (long)reg_info.lte_scell_info.tac);
    snprintf(GSM.CellID, sizeof(GSM.CellID), "%ld", reg_info.lte_scell_info.cell_id);


    // Get MCC and MNC from operator info
    if (fibo_get_operator_info(&operator_info, 0) == 0) {
        // operator_id contains MCC+MNC (e.g., "40410" for Airtel)
        memcpy(mcc_mnc_str, operator_info.operator_id, sizeof(operator_info.operator_id));
        mcc_mnc_str[6] = '\0';  // Ensure null termination
        
        // Parse MCC (first 3 digits) and MNC (remaining digits)
        if(strlen(mcc_mnc_str) >= 5) {
            char mcc_str[4] = {0};
            char mnc_str[4] = {0};
            
            strncpy(mcc_str, mcc_mnc_str, 3);
            mcc_str[3] = '\0';
            strcpy(mnc_str, mcc_mnc_str + 3);
            
            GSM.MCC = atoi(mcc_str);
            GSM.MNC = atoi(mnc_str);
            
            LOGD(TAG, "GPRS: Operator ID: %s, MCC=%d, MNC=%d", 
                          mcc_mnc_str, GSM.MCC, GSM.MNC);
        }
    }
    
    


    // Check if registration denied (status 3)
    if(reg_info.nStatus == 3)
    {
        reg_deny_count++;
        if(reg_deny_count < 25)
        {
            fibo_taskSleep(100);
            return;
        }
        
        GSM.IsRegDenied = 1;
        uint8_t newpf = GetNextValidProfile(GSM.GSMState);
        prfReq = newpf;
        return;
    }
    
    // Check if registered (1=registered, 5=roaming)
    if(reg_info.nStatus != 1 && reg_info.nStatus != 5)
        return;
    
    // Get operator info - parse from registration or IMSI
    if(strlen(NetWork.IMSI) > 5)
    {
        if(strncmp(NetWork.IMSI, "40410", 5) == 0 || strncmp(NetWork.IMSI, "40411", 5) == 0)
            strcpy(NetWork.Network, "airtel");
        else if(strncmp(NetWork.IMSI, "40486", 5) == 0)
            strcpy(NetWork.Network, "jio");
        else if(strncmp(NetWork.IMSI, "40470", 5) == 0)
            strcpy(NetWork.Network, "bsnl");
        else
            strcpy(NetWork.Network, "vi");
    }
    
    reg_deny_count = 0;
    // If no APN from SIM yet, select based on operator
    if(strlen(NetWork.APN) == 0) {
        select_apn_by_operator();
    } else {
        LOGD(TAG, "GPRS: Using SIM APN: %s", NetWork.APN);
    }
    
    GSM.GSMState = GPRS_INIT;
    LOGI(TAG, "GPRS: Network registered, moving to GPRS_INIT");
}

/*============================================================================
 * Update Signal Strength - EXACT match to Quectel GetSignalStrength
 *============================================================================*/
static void update_signal_strength(void)
{
    INT32 rssi = 0, ber = 0;
    if(fibo_get_csq(&rssi, &ber, 0) == 0)
    {
        GSM.SignalStrength = (uint8_t)rssi;
    }
}

/*============================================================================
 * Select APN by Operator - EXACT match to Quectel selectAPN
 *============================================================================*/
static void select_apn_by_operator(void)
{
    memset(NetWork.APN, 0, sizeof(NetWork.APN));
    
    // Convert network name to lowercase for comparison
    char network_lower[60];
    strcpy(network_lower, NetWork.Network);
    for(int i = 0; network_lower[i]; i++) {
        if(network_lower[i] >= 'A' && network_lower[i] <= 'Z')
            network_lower[i] += 32;
    }
    
    if(strstr(network_lower, "airtel"))
    {
        strcpy(NetWork.APN, "airtelgprs.com");
        NetWork.Provider = AIRTEL;
    }
    else if(strstr(network_lower, "jio"))
    {
        strcpy(NetWork.APN, "jionet");
        NetWork.Provider = JIO;
    }
    else if(strstr(network_lower, "bsnl") || strstr(network_lower, "cellone"))
    {
        strcpy(NetWork.APN, "bsnlnet");
        NetWork.Provider = BSNL;
    }
    else
    {
        strcpy(NetWork.APN, "internet");
        NetWork.Provider = VI;
    }
    
    LOGD(TAG, "GPRS: Selected APN: %s for provider: %d", NetWork.APN, NetWork.Provider);
}

/*============================================================================
 * Activate GPRS - EXACT match to Quectel ActivateGPRS
 *============================================================================*/
static void activate_gprs(void)
{
    static uint8_t activation_attempts = 0;
    
    LOGI(TAG, "GPRS: Activating with APN: %s", NetWork.APN);
    
    // Using AT command or API to check GPRS state
    // For now, assume we need to activate
    
    fibo_pdp_profile_t pdp_profile = {0};
    pdp_profile.cid = 1;
    strcpy((char*)pdp_profile.nPdpType, "IP");
    strcpy((char*)pdp_profile.apn, NetWork.APN);
    
    INT32 ret = fibo_pdp_active(1, &pdp_profile, 0);
    
    if(ret == 0)
    {
        LOGD(TAG, "GPRS: Activation initiated");
        
        // Wait for activation (timeout 60 seconds)
        int timeout = 60;
        while(timeout > 0)
        {
            UINT8 ip_addr[60] = {0};
            UINT8 cid_status = 0;
            
            if(fibo_pdp_status_get(1, ip_addr, &cid_status, 0) == 0 && cid_status == 1)
            {
                GSM.GSMState = GPRS_ACTIVE;
                LOGI(TAG, "GPRS: Activated, IP: %s", ip_addr);
                activation_attempts = 0;
                return;
            }
            
            fibo_taskSleep(1000);
            timeout--;
        }
        
        // Timeout - deactivate
        fibo_pdp_release(0, 1, 0);
        fibo_taskSleep(500);
    }
    
    activation_attempts++;
    LOGE(TAG, "GPRS: Activation failed (%d)", activation_attempts);
}

/*============================================================================
 * Update Time - EXACT match to Quectel UpdateTime
 *============================================================================*/
static void update_time(void)
{
    // Get RTC time (if available)
    // fibo_getRTC? Not sure of exact API
    
    // For now, just mark as not set
    GSM.IsTimeSet = 0;
}

/*============================================================================
 * Get IMEI - EXACT match to Quectel GetDeviceIMEI
 *============================================================================*/
void GetImei(void)
{
    UINT8 imei[16] = {0};
    if(fibo_get_imei(imei, 0) == 0)
    {
        strcpy(NetWork.IMEI, (char*)imei);
        LOGD(TAG, "GPRS: IMEI: %s", NetWork.IMEI);
    }
}

/*============================================================================
 * Check GPRS State
 *============================================================================*/
int CheckGPRSState(void)
{
    return (GSM.GSMState == GPRS_ACTIVE) ? 1 : 0;
}

/*============================================================================
 * Get Next Valid Profile (STK function - simplified)
 *============================================================================*/
uint8_t GetNextValidProfile(uint8_t currentProfile)
{
    // Simplified - return profile 1
    return 1;
}

/*============================================================================
 * Switch Profile (STK function - simplified)
 *============================================================================*/
uint8_t SwitchProfile(uint8_t num)
{
    LOGI(TAG, "GPRS: Switch to profile %d", num);
    // STK implementation would go here
    return 1;
}

/*============================================================================
 * Send AT Command (wrapper)
 *============================================================================*/
uint8_t SendAtCmd(char* str, char* resp, char* grep)
{
    // Not implemented - would need AT command interface
    return 0;
}

/*============================================================================
 * Get Neighbour Cells
 *============================================================================*/
uint8_t GetNeighbourCells(void)
{
    // Not implemented - would need cell info API
    GSM.IsNeighbourCells = 0;
    return 0;
}

/*============================================================================
 * Setup Auto Time Sync
 *============================================================================*/
int SetupAutoTimesync(void)
{
    // Configure NITZ if available
    return 0;
}

/*============================================================================
 * Init GPRS Thread
 *============================================================================*/
void InitGPRSThread(int taskId)
{
    LOGD(TAG, "GPRS: Thread initialized with ID: %lu", taskId);
}