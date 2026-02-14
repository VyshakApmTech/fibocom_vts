#ifndef __OC_FOTA_V2_H_
#define __OC_FOTA_V2_H_
typedef int int32;
typedef char int8;
typedef unsigned char uint8;
typedef unsigned int uint32;

#define FIBO_URL_MAX_LEN        512
#define OTA_V2_DIR_MAX          128
#define OTA_V2_PATH_MAX         256
typedef void (*ota_v2_dl_callback)(int32 status, uint32 len, uint32 total_len);

/***************** Special files definition *********************/
// macro :      FIBO_OTA_V2_DIR_SAVED_FILE
// description:
//              This file saved upgrade directory
#define FIBO_OTA_V2_DIR_SAVED_FILE      "/ota_dir_file"

// macro :      FIBO_OTA_V2_DIR_DEFAULT
// description:
//              System use this directory to OTA upgrade
#define FIBO_OTA_V2_DIR_DEFAULT         "/fota"

/***************** Ota upgrade files definition *********************/
#define FIBO_OTA_V2_WHOLE_FW            "whole_fw.pack"                     // currently not support
#define FIBO_OTA_V2_DELTA_FW            CONFIG_FUPDATE_PACK_FILE_NAME       // delta firmware upgrade package
#define FIBO_OTA_V2_WHOLE_APP           "whole_app.pack"                    // whole app upgrade package
#define FIBO_OTA_V2_DELTA_APP           "delta_app.pack"                    // delta app upgrade package
#define FIBO_OTA_V2_DF_DA_MERGE_PACK    "df_da_merge.pack"                  // delta firmware and delta app merged package
#define FIBO_OTA_V2_WHOLE_APP_DEFAULT   FIBO_OTA_V2_DIR_DEFAULT "/" FIBO_OTA_V2_WHOLE_APP          //Default path of whole app upgrade package
#define FIBO_OTA_V2_DF_DA_MERGE_DEFAULT FIBO_OTA_V2_DIR_DEFAULT "/" FIBO_OTA_V2_DF_DA_MERGE_PACK   //Default path of delta firmware and delta app merged package

/***************** Ota upgrade type *********************/
typedef enum{
    FIBO_UPGRADE_WHOLE_FW               = 0,    //Firmware whole upgrade
    FIBO_UPGRADE_DELTA_FW               = 1,    //Firmware delta upgrade
    FIBO_UPGRADE_WHOLE_APP              = 2,    //APP whole upgrade
    FIBO_UPGRADE_DELTA_APP              = 3,    //APP delta upgrade
    FIBO_UPGRADE_DF_DA_MERGE            = 4     //Delta Firmware and delta APP upgrade
} fibo_ota_v2_type_e;

typedef enum{
    FIBO_OTA_V2_DL_NOT_START            = 0, // Download not start
    FIBO_OTA_V2_DL_IN_PROCESS           = 1, // Download is processing
    FIBO_OTA_V2_DL_SUCCESS              = 2, // Download is successful
    FIBO_OTA_V2_DL_FAIL                 = 3, // Download is failed
} fibo_fotadl_status_e;

/***************** Ota download struction *********************/
typedef struct{
    int32   dl_type;                                 //FTP or HTTP
    int8    *url;                                    //URL, should not be NULL
    int8    *filename;                               //NULL while dl_type is http download
    int8    *username;                               //NULL while dl_type is http download
    int8    *password;                               //NULL while dl_type is http download
    ota_v2_dl_callback dl_callback;    //Callback function of download status and process
} fibo_ota_download_msg_t;

typedef struct{
    int8                        *dir;           //Upgrade directory, "/fota" by default
    bool                        reboot;         //Reboot module after verify upgrade package successfully
    fibo_ota_v2_type_e          fota_type;      //Upgrade type
    fibo_ota_download_msg_t     *ota_dl_msg_p;  //Download struction
}fibo_ota_msg_t;

typedef enum{
    OTA_V2_DOWNLOAD,      //Init download resource
    OTA_V2_UPGRADE        //Init upgrade resource
}ota_v2_init_type_e;

int32 fibo_dm_ota_download(fibo_ota_msg_t *fibo_ota_msg_p);
int32 fibo_dm_ota_upgrade(fibo_ota_msg_t *fibo_ota_msg_p);
#endif  //__OC_FOTA_V2_H__
