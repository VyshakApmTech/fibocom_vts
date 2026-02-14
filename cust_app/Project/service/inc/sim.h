#ifndef __SIM_H__
#define __SIM_H__

#include "fibo_opencpu.h"

// SIM state enumeration
typedef enum {
    SIM_STATE_UNKNOWN,
    SIM_STATE_CHECKING,
    SIM_STATE_READY,
    SIM_STATE_ERROR
} sim_state_t;

// SIM service functions
void sim_init(void);
void sim_process(void);
BOOL sim_is_ready(void);

#endif