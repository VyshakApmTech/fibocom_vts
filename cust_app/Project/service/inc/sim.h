#ifndef __SIM_H__
#define __SIM_H__

#include "fibo_opencpu.h"

// SIM state enumeration
typedef enum {
    SIM_STATE_UNKNOWN,
    SIM_STATE_INIT,        // Changed from SIM_STATE_IDLE
    SIM_STATE_CHECKING,
    SIM_STATE_READY,
    SIM_STATE_ERROR
} sim_state_t;

/* Add at top with other statics */
typedef enum {
    PDP_STATE_IDLE,
    PDP_STATE_ACTIVATING,
    PDP_STATE_ACTIVE,
    PDP_STATE_FAILED
} pdp_state_t;

// External variables (defined in sim.c)
extern pdp_state_t g_pdp_state;
extern uint8_t g_sim_ready_processed;
extern uint8_t g_subscriber_info_read;
extern uint8_t g_sim_stat;
extern uint8_t g_apn[100];

// SIM service functions
void sim_init(void);
sim_state_t sim_process(void);
BOOL sim_is_ready(void);

// SIM information functions (MOVED FROM MAIN.C)
void read_subscriber_info(void);
void check_network_registration(void);
void get_apn_from_sim(void);

#endif