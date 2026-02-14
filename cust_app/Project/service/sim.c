#include "sim.h"
#include "osi_log.h"

static sim_state_t g_sim_state = SIM_STATE_IDLE;
static uint8_t g_sim_status = 0;
static int g_check_count = 0;

void sim_init(void)
{
    g_sim_state = SIM_STATE_IDLE;
    g_sim_status = 0;
    g_check_count = 0;
    fibo_textTrace("SIM: Service initialized");
}

void sim_process(void)
{
    INT32 ret = 0;
    
    switch (g_sim_state) {
        case SIM_STATE_IDLE:
            fibo_textTrace("SIM: Starting SIM check");
            g_sim_state = SIM_STATE_CHECKING;
            g_check_count = 0;
            break;
            
        case SIM_STATE_CHECKING:
            // Non-blocking SIM status check
            ret = fibo_get_sim_status_v2(&g_sim_status, 0);  // simId=0 for single SIM
            
            if (ret == 0 && g_sim_status == 1) {  // Success and SIM ready
                fibo_textTrace("SIM: Ready");
                g_sim_state = SIM_STATE_READY;
            } else {
                g_check_count++;
                if (g_check_count > 10) {  // Timeout after ~50 seconds (10 * 5s)
                    fibo_textTrace("SIM: Error - timeout (ret=%ld, status=%d)", ret, g_sim_status);
                    g_sim_state = SIM_STATE_ERROR;
                } else {
                    fibo_textTrace("SIM: Not ready yet (ret=%ld, status=%d, attempt=%d)", 
                                  ret, g_sim_status, g_check_count);
                }
            }
            break;
            
        case SIM_STATE_READY:
        case SIM_STATE_ERROR:
            // Do nothing, wait for next state change
            break;
    }
}

BOOL sim_is_ready(void)
{
    return (g_sim_state == SIM_STATE_READY);
    //fibo_sim_notify_detect();
}