#include "osi_api.h"
#include "osi_log.h"

char *app_ver = "APP_0001";

#include "app_image.h"
#include "fibo_opencpu.h"

uint8_t g_sim_id = 0;

#define URC_TEXT_NUM 30
#define URC_TEXT_LEN 100

#define CFW_INIT_STATUS_NO_SIM (0x01)
#define CFW_INIT_STATUS_SIM (0x02)
#define CFW_INIT_STATUS_SAT (0x04)
#define CFW_INIT_STATUS_SIMCARD (0x08)
#define CFW_INIT_STATUS_SMS (0x10)
#define CFW_INIT_STATUS_PBK (0x20)

#define CFW_INIT_SIM_WAIT_PIN1 0x8
#define CFW_INIT_SIM_WAIT_PUK1 0x10
#define CFW_INIT_SIM_PIN1_READY 0x20

#define CFW_INIT_SIM_TEST_CARD 0x400
#define CFW_INIT_SIM_NORMAL_CARD 0x800


typedef enum
{
    URC_REPORT_CGREG = 1,
    URC_REPORT_CREG,
    URC_REPORT_ECSQ,
    URC_REPORT_CTZEU,
    URC_REPORT_CSCON,
    URC_REPORT_EDRX,
    URC_REPORT_NWCELLLIST,
    URC_REPORT_MBSINFO,
    URC_REPORT_SIMSTATUS,
}URC_REPORT_TYPE_T;

typedef struct
{
    char *urc_text;
    int urc_type;
}URC_TEXT_MAP_T;

URC_TEXT_MAP_T g_urc_text_map[] =
{
    {"CGREG", URC_REPORT_CGREG},
    {"CREG", URC_REPORT_CREG},
    {"ECSQ", URC_REPORT_ECSQ},
    {"CSCON", URC_REPORT_CSCON},
    {"EDRX", URC_REPORT_EDRX},
    {"NWCELLLIST", URC_REPORT_NWCELLLIST},
    {"MBSINFO", URC_REPORT_MBSINFO},
    {"SIMSTATUS", URC_REPORT_SIMSTATUS},
    {NULL, 0}
};

int fibo_find_urc_report_type(char *report_text)
{
    int index = 0;

    if ('\0' == *report_text && NULL != report_text)
    {
        return 0;
    }

    for (index = 0; (index < sizeof(g_urc_text_map)/sizeof(g_urc_text_map[0])); index++)
    {
        if (NULL == g_urc_text_map[index].urc_text)
        {
            return 0;
        }
        if (0 == memcmp(g_urc_text_map[index].urc_text, report_text, strlen(report_text)))
        {
            return g_urc_text_map[index].urc_type;
        }
    }

    return 0;
}

unsigned int fibo_strings_to_unsigned_int(char *src)
{
    char *endptr = NULL;

    unsigned int val = strtoul(src, &endptr, 10);

    return val;
}

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    fibo_textTrace("sig_res_callback  sig = %d", sig);
    switch (sig)
    {
        case GAPP_SIG_NW_JAMMING_DETECT_IND:
        {
            UINT8 simId = (UINT8)va_arg(arg, int);
            fibo_nw_jamdet_state_e bJamming = (UINT8)va_arg(arg, int);
            fibo_textTrace("sim%d  bJamming = %d", simId, bJamming);
            va_end(arg);
        }
        case GAPP_SIG_REPORT_URC_TEXT:
        {
        char *urc_report = (char *)va_arg(arg, char *);
        char *tmp_urc = NULL;
        const char s1[2] = ":";
        const char s2[2] = ",";
        char urc_text[URC_TEXT_NUM][URC_TEXT_LEN] = {0};
        int len = 0;

        if(urc_report != NULL)
        {
            OSI_PRINTFI("POC DEMO LOG: sig_res_callback urc_report = %s ", urc_report);
            tmp_urc = strtok(urc_report,s1);
            len = strlen (tmp_urc);
            if (strlen (tmp_urc) > URC_TEXT_LEN)
            {
                OSI_PRINTFI("POC DEMO LOG: ERROR URC 0 text length too long");
                len = URC_TEXT_LEN;
            }

            memcpy(urc_text[0], tmp_urc, len);
            OSI_PRINTFI("POC DEMO LOG: urc_text[0]:%s, tmp_urc:%s", urc_text[0], tmp_urc);

            int index = 1;
            while( NULL != tmp_urc )
            {
                tmp_urc = strtok(NULL, s2);
                if (NULL == tmp_urc)
                {
                    break;
                }
                len = strlen (tmp_urc);
                if (strlen (tmp_urc) > URC_TEXT_LEN)
                {
                    OSI_PRINTFI("POC DEMO LOG: ERROR URC %d text length too long", index);
                    len = URC_TEXT_LEN;
                }
                memcpy(urc_text[index], tmp_urc, len);
                OSI_PRINTFI("POC DEMO LOG: urc_text[%d]:%s, tmp_urc:%s", index, urc_text[index], tmp_urc);
                index++;
            }
            OSI_PRINTFI("POC DEMO LOG: index:%d", index);
        }

        URC_REPORT_TYPE_T urc_report_type = 0;

        urc_report_type = (URC_REPORT_TYPE_T)fibo_find_urc_report_type(urc_text[0]);
        OSI_PRINTFI("POC DEMO LOG: urc_report_type:%d", urc_report_type);
        
        int simid = fibo_strings_to_unsigned_int(urc_text[1]);
        switch(urc_report_type)
        {
            case URC_REPORT_CGREG:
            {
                /*CGREG:<SimId>,<status>,<LAC>,<CI>,<RAT>*/
                int status = fibo_strings_to_unsigned_int(urc_text[2]);
                if (1 == status)
                {
                    OSI_PRINTFI("POC DEMO LOG: SIM %d PS domain NORMAL SERVICE", simid);
                }
                else
                {
                    OSI_PRINTFI("POC DEMO LOG: SIM %d PS domain LIMITED SERVICE", simid);
                }
                break;
            }

            case URC_REPORT_CREG:
            {
                /*CREG:<SimId>,<status>,<LAC>,<CI>,<RAT>*/
                int status = fibo_strings_to_unsigned_int(urc_text[2]);
                if (1 == status)
                {
                    OSI_PRINTFI("POC DEMO LOG: SIM %d CS domain NORMAL SERVICE", simid);
                }
                else
                {
                    OSI_PRINTFI("POC DEMO LOG: SIM %d CS domain LIMITED SERVICE", simid);
                }
                
                break;
            }

            case URC_REPORT_ECSQ:
            {
                /*ECSQ:<SimId>,<rssi>,<ber>*/
                int rssi = fibo_strings_to_unsigned_int(urc_text[2]);
                int ber = fibo_strings_to_unsigned_int(urc_text[3]);
                
                OSI_PRINTFI("POC DEMO LOG: SIM %d ECSQ rssi:%d, ber:%d", simid, rssi, ber);
                break;
            }

            case URC_REPORT_CTZEU:
            {
                /*CTZEU:<SimId>,<tz>,<dst>,<time>*/
                int tz = fibo_strings_to_unsigned_int(urc_text[2]);
                OSI_PRINTFI("POC DEMO LOG: SIM %d ECSQ tz:%d", tz);
                break;
            }

            case URC_REPORT_CSCON:
            {
                /*CSCON:<SimId>,<mode>,<state>,<access>*/
                int mode = fibo_strings_to_unsigned_int(urc_text[2]);
                OSI_PRINTFI("POC DEMO LOG: SIM %d URC_REPORT_CSCON mode:%d", simid, mode);
                break;
            }

            case URC_REPORT_EDRX:
            {
                /*EDRX:<SimId>,<edrx_value>,<edrx_ptw>*/
                OSI_PRINTFI("POC DEMO LOG: SIM %d URC_REPORT_EDRX", simid);
                break;
            }

            case URC_REPORT_NWCELLLIST:
            {
                /*NWCELLLIST:<SimId>,<MCC>,<MNC>,<RXLEV>,<CELLID>,<ARFCN>,<LAC>,<BSIC>*/
                OSI_PRINTFI("POC DEMO LOG: SIM %d URC_REPORT_NWCELLLIST", simid);
                break;
            }

            case URC_REPORT_MBSINFO:
            {
                /*MBSINFO:<SimId>,<ARFCN>,<BSIC>,<CELLID> ,<LAI>*/
                OSI_PRINTFI("POC DEMO LOG: SIM %d URC_REPORT_MBSINFO", simid);
                break;
            }

            /*Judge whether the SIM card is normal*/
            case URC_REPORT_SIMSTATUS:
            {
                /*SIMSTATUS: <SimId>,<nType>,<param1>,<param2>*/
                int nType = fibo_strings_to_unsigned_int(urc_text[2]);
                int param1 = fibo_strings_to_unsigned_int(urc_text[3]);
                int param2 = fibo_strings_to_unsigned_int(urc_text[4]);
                int total_sms = 0;

                OSI_PRINTFI("POC DEMO LOG: simid:%d, nType:%d, param1:%d, param2:%d", simid, nType, param1, param2);

                if (0 == simid)
                {
                    switch(nType)
                    {
                        case CFW_INIT_STATUS_SMS:
                            OSI_PRINTFI("POC DEMO LOG: SIM 1 READY");
                            
                            total_sms = (param2 & 0xFF);
                            OSI_PRINTFI("POC DEMO LOG: SIM 1 total_sms:%d", total_sms);
                            break;
                        
                        case CFW_INIT_STATUS_NO_SIM:
                            OSI_PRINTFI("POC DEMO LOG: SIM 1 NO SIM");
                            break;

                        case CFW_INIT_STATUS_SIM:
                            if (param1 & CFW_INIT_SIM_PIN1_READY)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 1 SIM PIN READY");
                            }
                            else if (param1 & CFW_INIT_SIM_WAIT_PIN1)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 1 SIM WAIT PIN");
                            }
                            else if (param1 & CFW_INIT_SIM_WAIT_PUK1)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 1 SIM WAIT PUK");
                            }
                            break;

                        case CFW_INIT_STATUS_SAT:
                            if (!(param1 & CFW_INIT_SIM_SAT))
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 1 SAT OK");
                            }
                            else
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 1 SAT ERROR");
                            }
                            break;
                            
                        case CFW_INIT_STATUS_SIMCARD:
                            if (param1 & CFW_INIT_SIM_NORMAL_CARD)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 1 NORMAL CARD");
                            }
                            else if (param1 & CFW_INIT_SIM_TEST_CARD)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 1 TEST CARD");
                            }

                        default:
                            break;
                    }
                }
                else if (1 == simid)
                {
                    switch(nType)
                    {
                        case CFW_INIT_STATUS_SMS:
                            OSI_PRINTFI("POC DEMO LOG: SIM 2 READY");

                            total_sms = (param2 & 0xFF);
                            OSI_PRINTFI("POC DEMO LOG: SIM 2 total_sms:%d", total_sms);
                            break;
                        
                        case CFW_INIT_STATUS_NO_SIM:
                            OSI_PRINTFI("POC DEMO LOG: SIM 2 NO SIM");
                            break;

                        case CFW_INIT_STATUS_SIM:
                            if (param1 & CFW_INIT_SIM_PIN1_READY)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 2 SIM PIN READY");
                            }
                            else if (param1 & CFW_INIT_SIM_WAIT_PIN1)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 2 SIM WAIT PIN");
                            }
                            else if (param1 & CFW_INIT_SIM_WAIT_PUK1)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 2 SIM WAIT PUK");
                            }
                            break;

                        case CFW_INIT_STATUS_SAT:
                            if (!(param1 & CFW_INIT_SIM_SAT))
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 2 SAT OK");
                            }
                            else
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 2 SAT ERROR");
                            }
                            break;

                        case CFW_INIT_STATUS_SIMCARD:
                            if (param1 & CFW_INIT_SIM_NORMAL_CARD)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 2 NORMAL CARD");
                            }
                            else if (param1 & CFW_INIT_SIM_TEST_CARD)
                            {
                                OSI_PRINTFI("POC DEMO LOG: SIM 2 TEST CARD");
                            }

                        default:
                            break;
                    }
                }

                break;
            }
            
            default:
                break;
        }
        
        va_end(arg);
    }
        break;

        default:
            break;
    }
}


static void at_res_callback(UINT8 *buf, UINT16 len)
{
    fibo_textTrace("FIBO <--%s", buf);
}

static FIBO_CALLBACK_T user_callback = {
    .fibo_signal = sig_res_callback,
    .at_resp = at_res_callback};

/*get network roaming status*/
int fibo_app_get_nw_roaming_status(uint8_t simId, uint8_t *roaming_status)
{
    return fibo_nw_get_config(simId, FIBO_NW_ROAMING_STATUS, NULL, roaming_status);
}

/*get LTE timing advance*/
int fibo_app_get_nw_lte_ta(uint8_t simId, int16_t *lte_ta)
{
    return fibo_nw_get_config(simId, FIBO_NW_LTE_TIMING_ADVANCE, NULL, lte_ta);
}

/*get GTRRCREL value*/
int fibo_app_get_nw_gtrrcrel(uint8_t simId, fibo_nw_config_rrcrel_t *gtrrcrel)
{
    return fibo_nw_get_config(simId, FIBO_NW_GTRRCREL, NULL, gtrrcrel);
}

/*set GTRRCREL value*/
int fibo_app_set_nw_gtrrcrel(uint8_t simId, fibo_nw_config_rrcrel_t *gtrrcrel)
{
    return fibo_nw_set_config(simId, FIBO_NW_GTRRCREL, gtrrcrel, NULL);
}

/*test demo: roaming status*/
void fibo_test_demo_roaming_status(void)
{
    uint8_t roaming_status = 0;
    int ret = 0;

    ret = fibo_app_get_nw_roaming_status(g_sim_id, &roaming_status);

    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST: %s error %d", __FUNCTION__, ret);
        return;
    }

    if (1 == roaming_status)
    {
        fibo_textTrace("FIBO APP TEST: %s camping network: home network", __FUNCTION__); //home network
    }
    else if (5 == roaming_status)
    {
        fibo_textTrace("FIBO APP TEST: %s camping network: roaming network", __FUNCTION__); //roaming network
    }
    else
    {
        fibo_textTrace("FIBO APP TEST: %s camping network: no registered", __FUNCTION__); //no registered;
    }
}


/*test demo: LTE timing advance*/
void fibo_test_demo_lte_timing_advance(void)
{
    int16_t lte_timing_advance = 0;
    int ret = 0;
    
    ret = fibo_app_get_nw_lte_ta(g_sim_id, &lte_timing_advance);

    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST: %s error %d", __FUNCTION__, ret);
        return;
    }

    fibo_textTrace("FIBO APP TEST: %s %d", __FUNCTION__, lte_timing_advance);
}

void fibo_test_demo_gtrrcrel_abnormal_scenario(void)
{
    int ret = 0;
    fibo_nw_config_rrcrel_t gtrrcrel;
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));

    /*mode is 2, out of range 1, return error*/
    gtrrcrel.mode = 2;
    ret = fibo_app_set_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST1: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST1 %s gtrrcrel.frequency:%d, gtrrcrel.mode:%d, gtrrcrel.value:%d",__FUNCTION__, gtrrcrel.frequency, gtrrcrel.mode, gtrrcrel.value);

    /*value 21,out of range 20, return error*/
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));
    gtrrcrel.value = 21;
    ret = fibo_app_set_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST22: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST2 %s gtrrcrel.frequency:%d, gtrrcrel.mode:%d, gtrrcrel.value:%d",__FUNCTION__, gtrrcrel.frequency, gtrrcrel.mode, gtrrcrel.value);

    /*frequecy 31, out of range 30, return error*/
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));
    gtrrcrel.frequency = 31;
    ret = fibo_app_set_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST3: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST3 %s gtrrcrel.frequency:%d, gtrrcrel.mode:%d, gtrrcrel.value:%d",__FUNCTION__, gtrrcrel.frequency, gtrrcrel.mode, gtrrcrel.value);

    /*imm_release 2, out of range 0-1, return error*/
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));
    gtrrcrel.imm_release = 2;
    ret = fibo_app_set_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST3: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST3 %s gtrrcrel.imm_release:%d",__FUNCTION__, gtrrcrel.imm_release);

    return;
}

/*test demo: gtrrcrel set value, get value
value range: 0 - 20
mode range: 0 - 1
frequency range: 0 - 30*/

void fibo_test_demo_gtrrcrel_param_test(void)
{
    int ret = 0;
    
    fibo_nw_config_rrcrel_t gtrrcrel;
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));

    /*get default value*/
    ret = fibo_app_get_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST1: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST1 %s gtrrcrel.frequency:%d, gtrrcrel.mode:%d, gtrrcrel.value:%d",__FUNCTION__, gtrrcrel.frequency, gtrrcrel.mode, gtrrcrel.value);

    /*set gtrrcrel value, value is 3, mode is 1, frequency is 5*/
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));
    gtrrcrel.frequency = 5;
    gtrrcrel.mode = 1;
    gtrrcrel.value = 3;
    ret = fibo_app_set_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST2: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST2 %s gtrrcrel.frequency:%d, gtrrcrel.mode:%d, gtrrcrel.value:%d",__FUNCTION__, gtrrcrel.frequency, gtrrcrel.mode, gtrrcrel.value);

    /*get gtrrcrel value*/
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));
    ret = fibo_app_get_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST3: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST3 %s gtrrcrel.frequency:%d, gtrrcrel.mode:%d, gtrrcrel.value:%d",__FUNCTION__, gtrrcrel.frequency, gtrrcrel.mode, gtrrcrel.value);


}

void fibo_test_demo_gtrrcrel_function_test()
{
    int ret = 0;
    
    fibo_nw_config_rrcrel_t gtrrcrel;
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));
    /*set gtrrcrel value, value is 4, mode is 1, frequency is 10, imm_release is 0*/
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));
    gtrrcrel.frequency = 10;
    gtrrcrel.mode = 1;
    gtrrcrel.value = 4;
    ret = fibo_app_set_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST2: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST2 %s gtrrcrel.frequency:%d, gtrrcrel.mode:%d, gtrrcrel.value:%d",__FUNCTION__, gtrrcrel.frequency, gtrrcrel.mode, gtrrcrel.value);
}

void fibo_test_demo_gtrrcrel_immediate_release()
{
    int ret = 0;
    fibo_nw_config_rrcrel_t gtrrcrel;

    fibo_taskSleep(5*1000);
    
    /*set gtrrcrel value*/
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));
    gtrrcrel.imm_release = 1;
    ret = fibo_app_set_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST4: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST4 %s gtrrcrel.imm_release:%d",__FUNCTION__, gtrrcrel.imm_release);

    /*get gtrrcrel value*/
    memset(&gtrrcrel, 0, sizeof(fibo_nw_config_rrcrel_t));
    ret = fibo_app_get_nw_gtrrcrel(g_sim_id, &gtrrcrel);
    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST5: %s return error: %d", __FUNCTION__, ret);
    }
    fibo_textTrace("FIBO APP TEST5 %s gtrrcrel.imm_release:%d",__FUNCTION__, gtrrcrel.imm_release);
}


void fibo_test_demo_gtrrcrel(void)
{
    int ret = 0;
    
    fibo_test_demo_gtrrcrel_param_test();
    
    fibo_test_demo_gtrrcrel_abnormal_scenario();

    fibo_test_demo_gtrrcrel_immediate_release();

    fibo_test_demo_gtrrcrel_function_test();
}

static uint8_t fibo_app_get_mcc_mnc_from_plmn(uint8_t *plmn, uint8_t *mcc, uint8_t *mnc)
{
    if (plmn == NULL || mcc == NULL || mnc == NULL)
    {
        return 0;
    }
    mcc[0] = plmn[0] & 0x0F;
    mcc[1] = plmn[0] >> 4;
    mcc[2] = plmn[1] & 0x0F;

    mnc[0] = plmn[2] & 0x0F;
    mnc[1] = plmn[2] >> 4;
    mnc[2] = plmn[1] >> 4;

    if (mnc[2] > 9)
    {
        mnc[2] = 0;
        return 2;
    }
    else
    {
        return 3;
    }
}

void fibo_test_demo_get_scellinfo(void)
{
    int ret = 0;
    opencpu_cellinfo_t opencpu_cellinfo;
    uint8_t sMNC_Number = 0;
    uint8_t sMCC[3] = {0};
	uint8_t sMNC[3] = {0};
    
    int lte_cellid = 0;
    int lte_tac = 0;
    int lte_enodeBID = 0;
    int16_t lte_timing_advance = 0;

    int gsm_cellid = 0;
    int gsm_lac = 0;
    uint8_t gsm_timing_advance = 0;

    uint16_t netMode = 0; // 0- TDD, 1 -- FDD;
    uint16_t distance = 0;

    ret = fibo_get_scellinfo(&opencpu_cellinfo, g_sim_id);

    if (0 != ret)
    {
        fibo_textTrace("FIBO APP TEST: %s return error: %d", __FUNCTION__, ret);

        return;
    }

    if (7 == opencpu_cellinfo.curr_rat)
    {
        sMNC_Number = fibo_app_get_mcc_mnc_from_plmn(&opencpu_cellinfo.lte_cellinfo.lteScell.plmn[0], &sMCC[0], &sMNC[0]);
        lte_cellid = opencpu_cellinfo.lte_cellinfo.lteScell.enodeBID << 8 | opencpu_cellinfo.lte_cellinfo.lteScell.cellId;
        lte_tac = opencpu_cellinfo.lte_cellinfo.lteScell.tac;
        lte_enodeBID = opencpu_cellinfo.lte_cellinfo.lteScell.enodeBID;
        fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. LTE info:sMNC_Number:%d, mcc:%d%d%d,mnc:%d%d%d,cell_id:%d,tac:%d, lte_enodeBID: %d",sMNC_Number, sMCC[0],sMCC[1],sMCC[2],sMNC[0],sMNC[1],sMNC[2],lte_cellid,lte_tac, lte_enodeBID);

        netMode = opencpu_cellinfo.lte_cellinfo.lteScell.netMode;
        fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. LTE info:netMode %d", netMode);
        if (0 == netMode)
            fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. LTE info:netMode TDD");
        else if (1 == netMode)
            fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. LTE info:netMode FDD");
        else
            fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. LTE info:netMode is %d not TDD/FDD", netMode);

        fibo_app_get_nw_lte_ta(g_sim_id, &lte_timing_advance);
        fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. LTE info:lte_timing_advance %d", lte_timing_advance);

        /*Calculate the distance between UE and base station based on TA
          LTE range 0 ~1282*/
        if (lte_timing_advance >= 0 && lte_timing_advance < 1283)
        {
            if (0 == netMode)//TDD needs to consider the upstream and downstream conversion time, and 624Ts is the initial TA
            {
                distance = (lte_timing_advance - 624) * 489 / 100;
            }
            else
            {
                distance = lte_timing_advance * 489 / 100;
            }
        }

        fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. LTE info:calculate %d", distance);
    }
    /*if not support GSM, this code need delete, 8850 need delete this code*/
    else
    {
        sMNC_Number = fibo_app_get_mcc_mnc_from_plmn(&opencpu_cellinfo.gsm_cellinfo.pCurrCellInfo.nTSM_LAI[0], &sMCC[0], &sMNC[0]);
        gsm_cellid = opencpu_cellinfo.gsm_cellinfo.pCurrCellInfo.nTSM_CellID[0] << 8 | opencpu_cellinfo.gsm_cellinfo.pCurrCellInfo.nTSM_CellID[1];
        gsm_lac = opencpu_cellinfo.gsm_cellinfo.pCurrCellInfo.nTSM_LAI[3] << 8 | opencpu_cellinfo.gsm_cellinfo.pCurrCellInfo.nTSM_LAI[4];
        fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. GSM info:sMNC_Number:%d, mcc:%d%d%d,mnc:%d%d%d,cell_id:%d,lac:%d",sMNC_Number, sMCC[0],sMCC[1],sMCC[2],sMNC[0],sMNC[1],sMNC[2],gsm_cellid,gsm_lac);

        gsm_timing_advance = opencpu_cellinfo.gsm_cellinfo.pCurrCellInfo.nTSM_TimeADV;
        fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. GSM info:gsm_timing_advance %d", gsm_timing_advance);

        /*Calculate the distance between UE and base station based on TA;
        GSM TA is 255, this is invalid value
        GSM TA range 0 ~ 63*/
        if (gsm_timing_advance >= 0 && gsm_timing_advance < 64)
        {
            distance = gsm_timing_advance * 550;
            fibo_textTrace("FIBO APP TEST fibo_get_scellinfo. GSM info:calculate %d", distance);
        }
    }
}

void fibo_test_demo_jamdet()
{
    int ret = GAPP_RET_OK;

    /*get status.If jamdet function dont enable,fibo_nw_get_jamdet_status will return GAPP_RET_NOT_INIT*/
    fibo_nw_jamdet_state_e status = FIBO_NO_JAMMING;
    ret = fibo_nw_get_jamdet_status(g_sim_id, &status);
    fibo_textTrace("FIBO APP TEST: fibo_nw_get_jamdet_status return: %d status: %d", ret, status);

    fibo_nw_jamdet_param_set_s config_get[JDCFG_ENUM_MAX] = {0};
    ret = fibo_nw_get_jamdet_config(g_sim_id, config_get);
    fibo_textTrace("FIBO APP TEST: fibo_nw_get_jamdet_config return: %d", ret);
    for(int i = 0;i < JDCFG_ENUM_MAX;i++)
        fibo_textTrace("FIBO APP TEST: config[%d]: %d, %d", i, config_get[i].jdcfg_enum, config_get[i].value);

    /*The number of arrays can be less than JDCFG_ENUM_MAX.
    You can only set some parameters, and use default values for other parameters that are not set.
    */
    fibo_nw_jamdet_param_set_s config_set[JDCFG_ENUM_MAX]= {0};
    UINT8 config_num = 0;    //number needs be less than JDCFG_ENUM_MAX.

    //The following example only sets 3 parameters.
    config_set[0].jdcfg_enum = JD_ENABLE;
    config_set[0].value = true;

    config_set[1].jdcfg_enum = JD_URC;
    config_set[1].value = true;

    config_set[2].jdcfg_enum = JD_URC_PERIOD;
    config_set[2].value = 5;

    config_num = 3;

    ret = fibo_nw_set_jamdet_config(g_sim_id, config_set, config_num);
    fibo_textTrace("FIBO APP TEST: fibo_nw_set_jamdet_config return: %d", ret);

    fibo_taskSleep(10*1000);

    //get status and get config
    ret = fibo_nw_get_jamdet_status(g_sim_id, &status);
    fibo_textTrace("FIBO APP TEST: fibo_nw_get_jamdet_status return: %d status: %d", ret, status);

    ret = fibo_nw_get_jamdet_config(g_sim_id, config_get);
    fibo_textTrace("FIBO APP TEST: fibo_nw_get_jamdet_config return: %d", ret);
    for(int i = 0;i < JDCFG_ENUM_MAX;i++)
        fibo_textTrace("FIBO APP TEST: config[%d]: %d, %d", i, config_get[i].jdcfg_enum, config_get[i].value);
}

static void hello_test_demo_fibo_get_apn()
{
    fibo_textTrace("application thread enter");
    int32 ret = 0;
    uint8 nPdpType = 0;
    uint8 apn_len = 0;
    uint8 simId = fibo_get_dualsim();
    uint8_t apn[99] = {0};
    ret = fibo_nw_get_apn(simId, &nPdpType, &apn_len, &apn[0]);
    fibo_textTrace("ret = %d", ret);
    if(ret == GAPP_RET_OK)
    {
        fibo_textTrace("nPdpType = %d,apn_len = %d,apn: %s", nPdpType, apn_len, apn);
    }
}

static void hello_fibo_read_drxtm()
{
    fibo_textTrace("application thread enter");
    while(1)
    {
        int32 ret = 0;
        uint32 drx = 0;
        uint8_t simid = fibo_get_dualsim();
        ret = fibo_nw_get_drxtm(simid, &drx);
        fibo_textTrace("ret = %d, drx = %d", ret, drx);
        fibo_taskSleep(5000);
    }
}

static void fibo_test_demo_cfun()
{
    reg_info_t reg_info;
    uint8_t cfun_mode = 0;
    fibo_reg_info_get(&reg_info, 0);
    fibo_textTrace("FIBO APP TEST fibo_reg_info_get reg_info.nStatus:%d",reg_info.nStatus);

    fibo_cfun_set(cfun_mode,0);
    fibo_taskSleep(1*1000);

    fibo_reg_info_get(&reg_info, 0);
    fibo_textTrace("FIBO APP TEST fibo_reg_info_get reg_info.nStatus:%d",reg_info.nStatus);
    cfun_mode = fibo_cfun_get(0);
    fibo_textTrace("FIBO APP TEST fibo_cfun_get cfun_mode:%d",cfun_mode);

    cfun_mode=1;
    fibo_cfun_set(cfun_mode,0);

    fibo_reg_info_get(&reg_info, 0);
    fibo_textTrace("FIBO APP TEST fibo_reg_info_get reg_info.nStatus:%d",reg_info.nStatus);
    cfun_mode = fibo_cfun_get(0);
    fibo_textTrace("FIBO APP TEST fibo_cfun_get cfun_mode:%d",cfun_mode);
}


static void fibo_test_demo_get_csq()
{
    INT32 rssi = 0;
    INT32 ber = 0;
    uint8_t simid = 0;
    fibo_get_csq(rssi, ber, simid);
    fibo_textTrace("FIBO APP TEST fibo_get_csq rssi:%d,ber:%d",rssi,ber);
}

static void hello_world_demo()
{
    fibo_textTrace("application thread enter");
    set_app_ver(app_ver);

    fibo_taskSleep(3*1000);
    reg_info_t reg_info;

    while(1)
    {
        fibo_reg_info_get(&reg_info, 0);
        if (reg_info.nStatus == 1)
            break;
        fibo_taskSleep(1*1000);
        fibo_textTrace("FIBO APP TEST fibo_reg_info_get reg_info.nStatus:%d",reg_info.nStatus);
    }

    fibo_test_demo_roaming_status();

    fibo_test_demo_lte_timing_advance();

    fibo_test_demo_gtrrcrel();

    fibo_test_demo_get_scellinfo();

    hello_test_demo_fibo_get_apn();

    while(1)
    {
        fibo_taskSleep(3*1000);
        fibo_test_demo_get_scellinfo();
    }

    fibo_test_demo_cfun();
    fibo_test_demo_get_csq();

}


void *appimg_enter(void *param)
{
    fibo_textTrace("app image enter");

    fibo_thread_create(hello_world_demo, "mythread", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    fibo_thread_create(hello_fibo_read_drxtm, "hello_fibo_read_drxtm", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    return (void *)&user_callback;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}

