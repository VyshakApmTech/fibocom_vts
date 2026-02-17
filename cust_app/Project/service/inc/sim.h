#ifndef __SIM_H__
#define __SIM_H__

#include "fibo_opencpu.h"

// GSM/GPRS state enumeration (matching Quectel pattern)
typedef enum {
    GSM_STATE_SIM_NOT_DETECTED = 0,
    GSM_STATE_SIM_DETECTED,
    GSM_STATE_GPRS_INIT,
    GSM_STATE_GPRS_ACTIVE,
    GSM_STATE_ERROR
} gsm_state_t;

// PDP state enumeration
typedef enum {
    PDP_STATE_IDLE,
    PDP_STATE_ACTIVATING,
    PDP_STATE_ACTIVE,
    PDP_STATE_FAILED
} pdp_state_t;

// Network information structure
typedef struct {
    char imsi[16];
    char iccid[23];
    char operator[30];
    char apn[30];
    uint8_t provider;  // 0=AIRTEL, 1=JIO, 2=BSNL, 3=VI
    uint8_t signal_strength;
    uint8_t is_registered;
    uint8_t reg_deny_count;
} network_info_t;

// External variables
extern gsm_state_t g_gsm_state;
extern pdp_state_t g_pdp_state;
extern network_info_t g_network;
extern uint8_t g_apn[30];

// SIM service functions
void sim_init(void);
void sim_process(void);  
BOOL sim_is_ready(void);

// Subscriber information functions
void read_subscriber_info(void);
void check_network_registration(void);
void get_apn_from_sim(void);
void select_apn_by_operator(void);

// GPRS functions
void activate_gprs(void);
void gprs_activation_callback(UINT8 cid, UINT8 sim_id);

#endif
