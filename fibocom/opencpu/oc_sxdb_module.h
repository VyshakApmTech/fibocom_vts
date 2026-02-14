#ifndef OC_SXDB_MODULE_H
#define OC_SXDB_MODULE_H

#define MODULE_ID_FILE_PATH  "/module_id_file"

typedef enum module_type_s
{
    MOD_G1 = 0,
    MOD_PRIME,
    MOD_G3,
    MOD_ZIGBEE,
    MOD_RF,
    MOD_GPRS,    //5
    MOD_3G,
    MOD_4G,
    MOD_BPSK,
    MOD_FSK,
    MOD_PLC_RF,  //10
    MOD_NB_IOT,
    MOD_RESERVE,
    MOD_RESERVE1,
    MOD_CATM_NB_GPRS_450,
    MOD_CATM_NB_GPRS,
    MOD_GPRS_NB,
    MOD_NB,
    MOD_RF_GATEWAY,
    MOD_HPLC,
    MOD_RESERVE2,
}module_type_t;

typedef enum vendor_type_s
{
    VENDOR_ONSEMI = 0,
    VENDOR_SEMTECH,
    VENDOR_ST,
    VENDOR_REXENSE,
    VENDOR_HUAWEI,
    VENDOR_QUECTEL,
    VENDOR_ZTE,
    VENDOR_NEOWAY,
    VENDOR_HITRENDTECH,
    VENDOR_EASTSOFT,
    VENDOR_RISECOMM,
    VENDOR_SUNRAY,
    VENDOR_LIERDA,
    VENDOR_WUQI,
    VENDOR_HAISI,
    VENDOR_FIBOCOM,
    VENDOR_RESERVE,
}vendor_type_t;

typedef enum
{
    SXDB_MODULE_ID,
    SXDB_MODULE_TYPE,
    SXDB_MODULE_VER,
    SXDB_MODULE_HWVER,
    SXDB_MODULE_SOFTVER,
}module_opt_t;

typedef struct module_info_s
{
    char mod_id[33];
    module_type_t mod_type;
    char mod_ver[65];
    char mod_hwver[33];
    char mod_softver[33];
    char reserve[65];
    char reserve1[65];
}module_info_t;

extern INT32 fibo_get_manufact_type(vendor_type_t *manufacturer_type);
extern INT32 fibo_get_protocol_ver(UINT8 *protocol_ver);
extern INT32 fibo_format_time(UINT8 *utcStr, int format);
extern INT32 fibo_get_module_info_v1(module_info_t *module_info);
extern INT32 fibo_set_module_info_v1(UINT8 *module_value, module_opt_t mode);

#endif
