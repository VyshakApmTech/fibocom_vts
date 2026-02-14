#include "osi_api.h"
#include "osi_log.h"
#include "fibo_opencpu.h"
#include "fibocom.h"
#include "fibo_old_api.h"
#include "oc_ffs.h"

#define FIBO_MODULE_OLD 0
#define FIBO_MODULE_NEW 1
#define FIBO_MODULE_DM  2

// New module SN prefix
static char *g_sn_prefix[] = {
    "NL6", // LC610N_CN_00_60
    "CN8", // LC610N_CN_00_80
    "EM5", // LC610N_CN_00_13
    "1N4", // LC610N_CN_00_14
    NULL
};

// DM module SN prefix
static char *g_dm_prefix[] = {
    "A05", // LC610N-CN-02-33
    "A06", // LC610N-CN-02-34
    "A07", // LC610N-CN-02-35
    NULL
};
    
static bool module_sn_match(char **sn_filter, char *sn)
{
    char **sn_ptr = sn_filter;

    if (sn_filter == NULL || sn == NULL)
    {
        return false;
    }

    for (; *sn_ptr != NULL; sn_ptr++)
    {
        if (strncmp(sn, *sn_ptr, strlen(*sn_ptr)) == 0)
        {
            return true;
        }
    }

    return false;
}

INT32  fibo_get_cfun_status()
{
    return (INT32)fibo_cfun_get(fibo_get_dualsim());
}

INT32 fibo_get_operatorName(char *prsp)
{
    INT32 ret= 0;
    operator_info_t operator_info;
    memset(&operator_info,0,sizeof(operator_info));
    if(prsp == NULL)
    {
        return -1;
    }
    ret = fibo_get_operator_info(&operator_info,fibo_get_dualsim());
    if(ret == 0)
    {
        memcpy(prsp,operator_info.operator_name,strlen((char *)operator_info.operator_name));
    }
    return ret;
}


UINT16 fibo_read_drxtm(CFW_SIM_ID nSimID)
{
    uint32 drxtm = 0xFFFF;
    INT32 ret = fibo_nw_get_drxtm(nSimID, &drxtm);
    if(ret == 0)
    {
        return (UINT16)drxtm;
    }
    else
    {
        return 0;
    }
}

void fibo_charger_set_notice_callbk(drvChargerNoticeCB_t cb)
{
    fibo_charge_notice_cb_set((fiboChargeNoticeCB_t)cb);
    return;
}

UINT64 fibo_getSysTick_ext(void)
{
    return fibo_get_sys_tick();
}

INT32 fibo_get_chipid(uint32_t *chipid,uint32_t *metaid)
{
    uint32_t u_id[2] = {0};
    if(chipid == NULL || metaid == NULL)
        return -1;

    if (drvGetUId((osiBits64_t *)&u_id[0]) == true)
    {
        memcpy(chipid,&u_id[0],sizeof(uint32_t));
        memcpy(metaid,&u_id[1],sizeof(uint32_t));
    }
    else
    {
        fibo_textTrace("Read UID fail...");
        return -1;
    }
    return 0;
}

INT32 fibo_file_getTotalSize(void)
{
    return fibo_file_get_total_size();
}

INT32 fibo_get_heapinfo(uint32_t * size,uint32_t *avail_size,uint32_t *max_block_size)
{
    return fibo_heapinfo_get(size, avail_size, max_block_size);
}

bool  fibo_set_heartbeat_param(char *serverip, uint16_t port,  uint16_t time, char *hexdata)
{
    return fibo_heartbeat_param_set(serverip, port, time, hexdata);
}

INT32 fibo_get_module_info()
{
    INT32 ncfsnLen = 0;
    char getcfsn[24] = {0};

    memset(getcfsn, 0, 24);
    ncfsnLen = fibo_get_sn(getcfsn, 0);
        
    if (ncfsnLen == -1)
    {
        OSI_PRINTFE("fibo_get_module_info get info error");
        return -1;
    }

    if (module_sn_match(&g_sn_prefix[0], getcfsn))
    {
        return FIBO_MODULE_NEW;
    }

    if (module_sn_match(&g_dm_prefix[0], getcfsn))
    {
        return FIBO_MODULE_DM;
    }

    return FIBO_MODULE_OLD;
}
