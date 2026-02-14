#ifndef _OC_FIBO_NW_H_
#define _OC_FIBO_NW_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "fibo_opencpu_comm.h"

typedef uint32_t nw_client_handle_type;

/*support network config name*/
#define FIBO_NW_ROAMING_STATUS          "ROAMING_STATUS"
#define FIBO_NW_LTE_TIMING_ADVANCE      "LTE_TIMING_ADVANCE"
#define FIBO_NW_GTRRCREL                "GTRRCREL"
#define FIBO_NW_LTE_SIGNAL_QUALITY_URC      "LTE_SIGNAL_QUALITY_URC"

typedef struct nw_config_rrcrel_s
{
    UINT8 value;
    UINT8 mode;
    UINT8 frequency;
    UINT8 imm_release;
} fibo_nw_config_rrcrel_t;

typedef enum
{
    FIBO_SIGNAL_QUALITY_URC_INIT    = 0x00,   //dont urc
    FIBO_SIGNAL_QUALITY_URC_CONFIG1 = 0x01,   //only use rsrp_range urc
    FIBO_SIGNAL_QUALITY_URC_CONFIG2 = 0x02,   //only use sinr_range urc
    FIBO_SIGNAL_QUALITY_URC_CONFIG3 = 0x03,   //both use rsrp_range and sinr_range urc
    FIBO_SIGNAL_QUALITY_URC_DEFAULT = 0xFF,   //use default urc
}signal_quality_urc_config_type_t;

typedef struct nw_config_signal_quality_urc_s
{
    signal_quality_urc_config_type_t config;
    UINT8 rsrp_range;
    UINT8 sinr_range;
    UINT16 reserve;
} fibo_nw_config_signal_quality_urc_t;

typedef enum
{
    FIBO_NO_JAMMING = 0,
    FIBO_JAMMED,
} fibo_nw_jamdet_state_e;

typedef enum
{
    JDCFG_ENUM_NONE = 0,    //mean this is ignore param.
    JD_ENABLE = 1,    //function switch;The default value is 0
    JD_NSIM,    //sim id;The default value is 0
    JD_URC,    //urc switch;The default value is 0
    JD_URC_PERIOD,    //urc period,0~120s;0: Report when the status changes;other value:period urc;The default value is 0
    JD_PERIOD,    //detection period,0~10s;The default value is 3
    JD_RSRP,    //RSRP threshold,-140 ~ -44;The default value is -105
    JD_RSRQ,    //RSRQ threshold,-19 ~ -3 ;The default value is -15
    JD_RSSI,    //RSSI threshold,-120 ~ -20;The default value is -40
#ifdef CONFIG_GSM_SUPPORT
    JD_RSSI_GSM,    //RSSI_GSM threshold,50 ~ 110;The default value is 80
    JD_SINR,    //SINR threshold,0 ~ 63;The default value is 3
    JD_MINCH,    //minCH threshold,0 ~ 254;The default value is 5
#endif
    JD_SHAKEPERIOD,    //urc shake parameters,0~10s;The default value is 0
    JDCFG_ENUM_MAX
}fibo_nw_jdcfg_e_t;

typedef struct
{
    fibo_nw_jdcfg_e_t jdcfg_enum;
    int value;
} fibo_nw_jamdet_param_set_s;

extern int fibo_nw_get_jamdet_status(nw_client_handle_type h_nw, fibo_nw_jamdet_state_e *status);
extern int fibo_nw_get_jamdet_config(nw_client_handle_type h_nw, fibo_nw_jamdet_param_set_s config[JDCFG_ENUM_MAX]);
extern int fibo_nw_set_jamdet_config(nw_client_handle_type h_nw, fibo_nw_jamdet_param_set_s *config, UINT8 config_num);

/*
fibo_nw_get_config
support name:
name: ROAMING_STATUS         input_param: NULL                        output_param: uint8_t *
name: GTRRCREL               input_param: NULL                        output_param: fibo_nw_config_rrcrel_t *
name: LTE_TIMING_ADVANCE     input_param: NULL                        output_param: int16_t *
*/
extern int fibo_nw_get_config(UINT8 simId, char *name, void *input_param, void *output_param);

/*
fibo_nw_set_config
name: GTRRCREL               input_param: fibo_nw_config_rrcrel_t *   output_param: NULL  
*/
extern int fibo_nw_set_config(UINT8 simId, char *name, void *input_param, void *output_param);


#endif

