#ifndef _FIBO_OPENCPU_H_
#define _FIBO_OPENCPU_H_

#include <stdbool.h>
#include "fibo_opencpu_comm.h"
#include "cfw.h"
#include "stdio.h"
#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"
#include "at_cfg.h"
#include "at_cfw.h"
#include "cfw_dispatch.h"
#include "image_sensor.h"
#include "oc_adc.h"
#include "drv_lcd.h"
#include "fibo_common.h"
#include "audio_types.h"
#include "audio_player.h"
#include "oc_gnss.h"
#include "oc_gprs.h"
#include "drv_spi.h"
#include "fupdate.h"
#ifdef CONFIG_FIBOCOM_XIAONIU_FEATURE
#include "fibo_opencpu_cus.h"
#endif
#ifdef CONFIG_FIBOCOM_OPENCPU
#include "oc_mqtt.h"
#include "oc_socket.h"
#include "oc_heartbeat.h"
#include "oc_mqtt_cloud.h"
#include "oc_http.h"
#include "oc_ftp.h"
#include "oc_ssl.h"
#include "oc_charge.h"
#include "oc_sdc.h"
#include "oc_ffs.h"
#include "oc_gb2312_unicode.h"
#include "oc_mbedtls.h"
#include "oc_ws.h"
#endif
#include "oc_uart.h"
#include "expat.h"
#include "ffw_gpio.h"
#include "ffw_i2c.h"
#include "oc_call.h"
#include "drv_hid.h"
#include "drv_usb.h"
#include "drv_wifi.h"
#include "oc_nw.h"
#include "oc_sms.h"
#include "oc_fota_v2.h"
#include "drv_charger.h"
#include "drv_secure.h"

/* FOTA ERROR CODE */
#define OTA_ERRCODE_MEMLEAK            (-1)
#define OTA_ERRCODE_FSREAD             (-2)
#define OTA_ERRCODE_FSWRITE            (-3)
#define OTA_ERRCODE_APP_CHECKFAIL      (-4)
#define OTA_ERRCODE_FM_CHECKFAIL       (-5)
#define OTA_ERRCODE_APPFOTA_CHECKFAIL  (-6)
#define OTA_ERRCODE_DL_FAIL            (-7)
#define OTA_ERRCODE_BLE_CHECKFAIL      (-8)
#define OTA_ERRCODE_BLE_FS_FULL        (-9)
#define OTA_ERRCODE_SPACE_NOT_ENOUGH   (-10)


/*ERROR CODE*/
#define GAPP_RET_OK (0)
#define GAPP_RET_PARAMS_ERROR (-1)
#define GAPP_RET_NOT_INIT (-2)
#define GAPP_RET_MEMORY_ERROR (-3)
#define GAPP_RET_OPTION_NOT_SUPPORT (-4)
#define GAPP_RET_TIMEOUT (-5)
#define GAPP_RET_UNKNOW_ERROR (-6)
#define GAPP_RET_FILE_ALREADY_OPEN_ERROR (-7)
#define GAPP_RET_NOT_ENOUGH_SPACE_ERROR (-8)
#define GAPP_RET_NOT_VALID_STATE_ERROR (-9)


#define FS_O_RDONLY  O_RDONLY
#define FS_O_WRONLY  O_WRONLY
#define FS_O_RDWR    O_RDWR
#define FS_O_APPEND  O_APPEND
#define FS_O_CREAT   O_CREAT
#define FS_O_TRUNC   O_TRUNC
#define FS_O_EXCL    O_EXCL
#define FS_O_SYNC    O_SYNC

#define FS_SEEK_SET  SEEK_SET
#define FS_SEEK_CUR  SEEK_CUR
#define FS_SEEK_END  SEEK_END

// TTS
#ifndef CTTS_STRING_ENCODE_TYPE_UTF8
#define CTTS_STRING_ENCODE_TYPE_UTF8 (0)
#endif
#ifndef CTTS_STRING_ENCODE_TYPE_GB2312
#define CTTS_STRING_ENCODE_TYPE_GB2312 (1)
#endif
#ifndef CTTS_STRING_ENCODE_TYPE_UNICODE
#define CTTS_STRING_ENCODE_TYPE_UNICODE (2)
#endif
#ifndef CTTS_TEXT_MAX_LEN
#define CTTS_TEXT_MAX_LEN (1024)
#endif
#define TTS_SPEED_MIN -32768 /* slowest voice speed */
#define TTS_SPEED_NORMAL 0   /* normal voice speed (default) */
#define TTS_SPEED_MAX +32767 /* fastest voice speed */

#define TTS_PITCH_MIN -32768 /* lowest voice tone */
#define TTS_PITCH_NORMAL 0   /* normal voice tone (default) */
#define TTS_PITCH_MAX +32767 /* highest voice tone */

#define ivTTS_VOLUME_MIN -32768 /* minimized volume */
#define ivTTS_VOLUME_NORMAL 0   /* normal volume */
#define ivTTS_VOLUME_MAX +32767 /* maximized volume (default) */

#define ivTTS_READDIGIT_AUTO 0      /* decide automatically (default) */
#define ivTTS_READDIGIT_AS_NUMBER 1 /* say digit as number */
#define ivTTS_READDIGIT_AS_VALUE 2  /* say digit as value */

typedef void (*usbStateChangeCallback_t)(uint8_t state, void *cb_ctx);
typedef void (*fiboChargeNoticeCB_t)(UINT32 msg);

typedef enum {
    GAPP_TTS_SUCCESS = 0,
    GAPP_TTS_ERROR = 1,
    GAPP_TTS_PARAMS_ERROR = -1,
    GAPP_TTS_BUSY = -2,
    GAPP_TTS_MEMORY_ERROR = -3
}tts_status_e;

enum fibo_tts_cfg_param
{
    FIBO_OPENCPU_TTS_VOLUME,
    FIBO_OPENCPU_TTS_PLAYMODE,
    FIBO_OPENCPU_TTS_SPEED,
    FIBO_OPENCPU_TTS_PITCH
};
enum fibo_tts_cfg_operation
{
    FIBO_OPENCPU_TTS_GET,
    FIBO_OPENCPU_TTS_SET
};



#define MAX_CODE_TYPE_LEN (8)
#define MAX_CODE_TYPE     (12)


typedef void (*auPlayerCallback_t)(void);

typedef enum
{
    AUDIO_VOICE_VOLUME,
    AUDIO_PLAY_VOLUME,
    AUDIO_TONE_VOLUME,
} AUDIO_VOLUME_MODE_T;

typedef struct{
    uint8 master;       /*Set data transmission to master mode or slave mode (just support 0:master mode)*/
    uint8 tran_mode;    /*Data transmission mode (just support 1:PCM mode)*/
    uint8 sample;       /*Sampling rate (just support 0:8k 1:16k)*/
    uint8 width;        /*Word width (just support 1:16bit)*/
    uint8 channel_count; /*Reserve,Channel num*/
    uint8 out_path;     /*Reserve,Output path: spk/lineout/headset*/
    uint8 in_path;      /*Reserve,Iutput path: mic/headset mic*/
    uint32* reserve;    /*Reserved fields for future expansion */
}f_audio_param_cfg_t;
/*STRUCT*/
typedef struct {
    char codeType[MAX_CODE_TYPE_LEN];
    uint8_t codeValue;
}barCodeEx_t;

typedef enum
{
    AEE_TONE_RING_G5 = 1568,/*1568.0Hz*/
    AEE_TONE_RING_A5 = 880,/*880.0Hz*/
    AEE_TONE_RING_E5 = 1318,/*1318.5Hz*/
} AUDIO_TONE_EX_T;

typedef enum
{
    BOOTCAUSE_BY_SOFT,
    BOOTCAUSE_BY_PIN_RST,
    BOOTCAUSE_BY_PIN_PWR,
    BOOTCAUSE_BY_CHARGE,
    BOOTCAUSE_BY_WDG,
    BOOTCAUSE_BY_ALARM,
} POWER_ON_CAUSE;


typedef struct{
    CFW_TSM_CURR_CELL_INFO pCurrCellInfo;
    CFW_TSM_ALL_NEBCELL_INFO pNeighborCellInfo;
}gsm_cellifo_t;

typedef struct{
    uint8_t curr_rat;
    cfw_nwCapLte_t lte_cellinfo;
#if defined(CONFIG_FIBOCOM_8910_BASE)  //2024-6-25,nisimin,MTC0329-5 Begin. add GSM info
    gsm_cellifo_t  gsm_cellinfo;
#endif  //2024-6-25,nisimin,MTC0329-5 End.
}opencpu_cellinfo_t;


typedef void (*key_callback)(void* param);
typedef void (*fota_download_cb_t)(UINT32 size, UINT32 total);

typedef struct
{
    uint8_t sec;   ///< Second
    uint8_t min;   ///< Minute
    uint8_t hour;  ///< Hour
    uint8_t day;   ///< Day
    uint8_t month; ///< Month
    uint16_t year;  ///< Year
    uint8_t wDay;  ///< Week Day
} hal_rtc_time_t;


#define MAX_AP_NAME_LEN 99
#define MAX_AP_ADDR_LEN 99
#define MAX_AP_NUM 30

typedef struct fibo_wifiscan_info{
    int8_t  ap_rssi;
    uint8_t ap_name[MAX_AP_NAME_LEN];
    uint8_t ap_addr[MAX_AP_ADDR_LEN];
}fibo_wifiscan_ap_info_t;

typedef struct fibo_wifiscan_res{
    int8_t  ap_num;
    fibo_wifiscan_ap_info_t ap_info[MAX_AP_NUM];
    int8_t  result;
}fibo_wifiscan_res_t;

#define LBS_RAWDATA_MAX_LEN 512
#define LBS_DESC_MAX_LEN 256
#define LBS_LOC_MAX_LEN 64

typedef struct fibo_lbs_res{
    char *loc;
    char *desc;
    char *raw;
}fibo_lbs_res_t;

#define SAPP_IO_ID_T uint16_t
#define SAPP_GPIO_CFG_T uint16_t
typedef void (*ISR_CB)(void* param);

typedef struct
{
    bool is_debounce;   // debounce enabled
    bool intr_enable;   //interrupt enabled, only for GPIO input
    bool intr_level;    // true for level interrupt, false for edge interrupt
    bool intr_falling;    //falling edge or level low interrupt enabled
    bool inte_rising;   //rising edge or level high interrupt enabled
    ISR_CB callback;    //interrupt callback
}oc_isr_t;

typedef struct {
    uint8_t reg;
    uint8_t value;
}camera_reg_t;

//Lcd
typedef struct {
    uint8_t command;
    uint8_t length;
    uint8_t *CommandValue;
}lcd_reg_t;

//sweep
typedef struct
{
    CAM_DEV_T CamDev;
    lcdSpec_t lcddev;
    osiThread_t *camTask;
    uint8_t Decodestat;
    bool Openstat;
    bool MemoryState;
    bool gCamPowerOnFlag;
    bool issnapshot;
    uint32_t height;
    uint32_t width;
    uint32_t sweepsize;
    uint32_t times;
} camastae_t;

/* for hui-er-feng & xin da lu */
typedef enum {
    KEY_RELEASE,
    KEY_PRESS,
    SHORT_TIME_PRESS,
    LONG_TIME_PRESS,
}keypad_status;

typedef struct {
    int32_t key_id;
    int8_t long_or_short_press;
    int8_t press_or_release;
}keypad_info_t;

typedef void (*usb_input_callback_t)(atDevice_t *th, void *buf, size_t size, void *arg);
typedef int (*Report_UsbDev_Event) (int msg);
typedef INT32 (*ble_data_recv_callback) (int data_len,char *data);


/* for external pa cfg */
typedef struct
{
    void (*_init)(void);
    void (*_open)(void);
    void (*_close)(void);
    INT32 pin_id;
    INT32 mode;
}paSpec_t;

typedef struct
{
int rsrp; //Reference signal received power, value range: 0-255
int rsrq; //Reference signal reception quality, value range: 0-255
int snr; //Signal to Interference plus Noise Ratio
int ecl; //Signal coverage level, 0: regular coverage 1: extended coverage 2: extreme coverage
}T_CellInfo;

typedef enum{
Mode_NoSet = 0, //Do not use power-saving mode
Mode_PSM = 1, //PSM mode
Mode_DRX, //DRX mode
Mode_eDRX, //eDRX mode
}T_PowerMode;

typedef struct
{
long totalFLash; //FLASH total size
long totalRam; //RAM total size
long totalFree; //Current remaining RAM
}T_MemInfo;



/* for external codec cfg */
typedef struct
{
    bool codeci2c_en; //I2c enabled, assign value 0,1
    void (*codec_init)(void); //Reserved initialization configuration function for i2c chip
    void (*codec_output)(void); //Configure the audio output function reserved for the i2c chip
    void (*codec_input)(void); //Configure the audio input function reserved for the i2c chip
    void (*codec_close)(void); //Reserved shutdown configuration function for i2c chip
    INT32 codecen_pinid; //Codec enable pin 0: No need to configure; non-zero enable pin: must be configured
    UINT8 codecen_mode; //Pull up enable or pull down enable of codecen_pinid, 1: Pull up enable; 2: Pull down enable
    UINT8 i2s_sel; //i2s serial numbers 1, 2
    bool i2sin_en; //sdati_pi enable configuration 0: does not use i2s input pin 1: uses i2s input pin
}codec_Spec_t;

typedef struct{
#ifdef CONFIG_SOC_8850
    int spl_address;
    int spl_size;
#endif
    int boot_address;
    int boot_size;
    int flash_address;
    int flash_size;
    int appimg_flash_address;
    int appimg_flash_size;
    int fs_sys_address;
    int fs_sys_address_size;
    int fs_modem_address;
    int fs_modem_size;
    int fs_factory_address;
    int fs_factory_size;
}fibo_flash_partinfo_t;

typedef struct{
    uint32_t tx_buf_size;        ///< tx buffer size
    uint32_t rx_buf_size;        ///< rx buffer size
    uint32_t tx_dma_size;        ///< tx dma buffer size
    uint32_t rx_dma_size;        ///< rx dma buffer size
    uint32_t tx_wait_timeout;    ///< tx wait timeout ms
}fibo_usb_buffcfg_t;

typedef struct{
    int gSysnvUsbWorkMode;
    int mqtt_num;
    int usb_custom_en;
    uint16_t usb_vid;
    uint16_t usb_pid;
    char *manufacturer;
    char *product;
    int sim1_dect; //sim1 dect gpio num
    uint8_t ecm_auto_dial;
    uint8_t ecm_host_detect;
    bool save_dumpinfo_en;
    bool skip_vbatsense;
    bool ctzu_en;
    // Obtain the current vbat vol. If an internal charging chip is used, getCurbatVol is set to false.
    // If the customer uses an external charging chip, getCurbatVol is set to true;
    bool getCurbatVol;
    bool usb_printer_en;
    fibo_usb_buffcfg_t usb_buffcfg;
    fibo_usb_buffcfg_t printer_buffcfg;
    int sim1_pin;   //sim1 dect FIBO pin number,if this number is not 0 or 0xff,sim_dect is vaild rather than sim1_dect
    int lpg_onoff;  //lpg switch,On is 1, off is 0
    bool usb_hid_acm_en;  //hid acm mode enable
    bool app_whole_reboot; //whether to reboot after app whole upgrade
    bool app_delta_reboot; //whether to reboot after app delta upgrade
    int unzip_without_dir;
    bool ring_tts_en; //played TTS during the ringing phase
    uint16_t usb_bcd;  //usb bcd
    char *serialnum;   //usb serialnum
    bool usb_hid_at_en; //at port switch in hid mode

   /**
    * @brief Set camera reset pin un-used, default as false, aka. camera reset pin is used.
    *           True: camera reset pin is un-used, and it can be used for other function.
    *           False: default value, camera reset pin is used for camera function.
    */
    bool cam_rst_pin_unused;
    
    ///MTC0671-184 sundaqing 20250707 start
    bool http_post_no_copy;
    ///MTC0671-184 sundaqing 20250707 end
    bool uart1TxAtCmd; //Send AT commands using UART1
}fibo_config_t;

typedef struct
{
    bool enable_keypad;    //Whether to enable the keypad function, default value: false, not enabled
    UINT32 timeout;        //If keypad is enabled, timeout is used to distinguish between long press and short press. The default value is 0
} fibo_keypad_info_t;

#define FIBO_NW_NITZ_BUF_LEN 30
typedef struct
{
    char         nitz_time[FIBO_NW_NITZ_BUF_LEN + 1];
    uint64_t     abs_time;   // 0 means invalid
    int8_t       leap_sec;   // 0 means invalid
} fibo_nw_nitz_time_info_t;

typedef enum
{
    //SEND_CMD_GET_VERSION = 0,   // è·åæ å°ççæ¬å·ï¼å¥åä¸ºNULLï¼?åºåä¸?struct mapped_version
    //SEND_CMD_SET_SPECIAL_MODE,  // è®¾ç½®ç¹æ®çå·¥è£ç¶æï¼å¥åä¸?struct special_modeï¼åºåä¸ºNULL
    SEND_CMD_MIN_NUM,
    FIBO_CMD_REFRESH_TABLE_B = 1,
    FIBO_CMD_GET_TABLE_B_CONTENT,
    FIBO_CMD_GET_MCC_MNC,
    FIBO_CMD_REFRESH_TABLE_C,
    FIBO_CMD_GET_OPERATOR_INFO = 5,
    FIBO_CMD_SET_SIGNAL_QUALITY_URC,
    SEND_CMD_MAX_NUM
} SEND_CMD_E;

typedef ffw_pin_mode FIBO_PIN_MODE;

typedef void(*fibo_app_handle_at_cb)(UINT8*, UINT16);
typedef int (*VSIM_COMMAND)(uint8_t *apdu_req, uint16_t apdu_req_len, uint8_t *apdu_rsp, uint16_t *apdu_rsp_len, uint8_t slot);

/*FUNCTION*/
typedef int32_t (*callBackFunc)(void *arg);
typedef struct {
        callBackFunc idleCallBack;
        callBackFunc dumpCallBack;
}CUSTOM_CALLBACK_TYPE;

typedef enum{
    WAKEUP_BY_INCALL = 0,
    WAKEUP_BY_SMS,
    WAKEUP_BY_DATA,
} WAKEUP_TYPE_E;
typedef void (*wakeup_callback_t)(WAKEUP_TYPE_E type);



typedef uint32_t sim_client_handle_type;

typedef uint32_t dm_client_handle_type;


typedef enum
{
    E_FIBO_SIM_SLOT_ID_1          = 1,    /**< Identify card in slot 1.  */
    E_FIBO_SIM_SLOT_ID_2          = 2,    /**< Identify card in slot 2.  */
}e_fibo_sim_slot_id_t;

typedef enum
{
    E_FIBO_SIM_APP_TYPE_UNKNOWN   = 0,    /**<  Unknown application type  */
    E_FIBO_SIM_APP_TYPE_3GPP      = 1,    /**< Identify the SIM/USIM application on the card.  */
    E_FIBO_SIM_APP_TYPE_3GPP2     = 2,    /**< Identify the RUIM/CSIM application on the card.  */
    E_FIBO_SIM_APP_TYPE_ISIM      = 3,    /**< Identify the ISIM application on the card.  */
}e_fibo_sim_app_t;

typedef struct
{
    e_fibo_sim_slot_id_t     e_slot_id;  /**< Indicates the slot to be used. */
    e_fibo_sim_app_t         e_app;      /**< Indicates the type of the application. */
}fibo_sim_app_id_info_t;  /* Type */



//get IMEI and sn
extern INT32 fibo_get_imei(UINT8 *imei, CFW_SIM_ID nSimID);
extern INT32 fibo_get_sn(char* deviceSN, CFW_SIM_ID nSimID);

//app version
extern INT32 fibo_app_version_set(char *ver);
extern INT32 fibo_app_version_get(char *outstr, int32_t outlen);

//sim id
#if (!defined(CONFIG_FIBOCOM_XIAONIU_FEATURE)) && (!defined(CONFIG_FIBOCOM_8910_OLD_API))
extern INT32 fibo_get_ccid(uint8_t *ccid,uint8_t simId);
#else
extern INT32 fibo_get_ccid(uint8_t *ccid);
#endif
extern INT32 fibo_get_ccid_v2(uint8_t *ccid,uint8_t simId);
extern INT32 fibo_get_imsi_asyn(void (*cb)(UINT8 *imsi),uint8_t simId);
//siganl quality
#if (!defined(CONFIG_FIBOCOM_XIAONIU_FEATURE)) && (!defined(CONFIG_FIBOCOM_8910_OLD_API))
extern INT32 fibo_get_csq(INT32 *rssi, INT32 *ber,uint8_t simId);
#else
extern INT32 fibo_get_csq(INT32 *rssi, INT32 *ber);
#endif

//nw local release
#define FIBO_LOCREL_LEVEL_EXE (0xFF)
extern bool fibo_locrel_set(uint8_t level,uint8_t simId);
extern bool fibo_tau_after_locrel_set(uint8_t type,uint8_t simId);
extern uint8_t fibo_tau_after_locrel_get(uint8_t simId);

/**
 * @brief lock plmn(network).
 *
 * @param nMode
    0:enable white list;
    1:enable black lsit;
    2:disalbe black white lsit
 * @param ltePlmnNum: need lock plmn number(max = 5)
 * @param ltePlmnList: plmn list
    eg:ltePlmnList[ltePlmnNum][6] = {"46001","46002"}
 * @return uint8_t 0: successful
 *                -1: failed
**/
uint8_t fibo_plmn_lock_set(uint8_t nMode,uint8_t ltePlmnNum,uint8_t *ltePlmnList,uint8_t simId);


//get and set cfun state
extern INT32 fibo_cfun_set(uint8_t mode,uint8_t sim);
extern UINT8 fibo_cfun_get(uint8_t sim);
extern INT32 fibo_cfun_one();
extern INT32 fibo_cfun_zero();

//app ota
extern INT32 fibo_app_ota(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_app_handle(INT8 *data, UINT32 len);
extern INT32 fibo_appfota_handle_no_reboot(INT8 *data, UINT32 len);
extern INT32 fibo_appfota_ota(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_appfota_handle(INT8 *data, UINT32 len);
extern INT32 fibo_ota_handle_no_reboot(INT8 *data, UINT32 len);
extern INT32 fibo_ota_handle(INT8 *data, UINT32 len);
extern INT32 fibo_open_ota(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_app_check(INT8 *data, UINT32 len);
extern INT32 fibo_app_dl(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_app_update(void);
extern INT32 fibo_ota_errcode(void);
extern void fibo_ota_set_callback(fota_download_cb_t cb);

//firmware ota
extern INT32 fibo_firmware_handle(INT8 *data, UINT32 len);
extern INT32 fibo_firmware_ota(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_firmware_handle_no_reboot(INT8 *data, UINT32 len);
extern INT32 fibo_firmware_dl(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_firmware_update(void);
extern INT32 fibo_firmware_dl_with_no_reboot(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_device_get_update_status(fupdateResult_t *status);

//appfota+firmware ota
extern INT32 fibo_appfw_ota(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_appfw_handle(INT8 *data, UINT32 len);
extern INT32 fibo_appfw_handle_no_reboot(INT8 *data, UINT32 len);
extern INT32 fibo_appfw_handle_by_path_ex(INT8 *path);

//fibo ota api v2
extern int32 fibo_dm_ota_download(fibo_ota_msg_t *fibo_ota_msg_p);
extern int32 fibo_dm_ota_upgrade(fibo_ota_msg_t *fibo_ota_msg_p);

// add cellinfo
extern INT32 fibo_getCellInfo_ex(key_callback cellinfo_refponse_cb,CFW_SIM_ID nSim);
extern INT32 fibo_getCellInfo(opencpu_cellinfo_t *opencpu_cellinfo_ptr,CFW_SIM_ID nSimID);
extern INT32  fibo_get_scellinfo(opencpu_cellinfo_t *opencpu_cellinfo_ptr,CFW_SIM_ID nSimID);

extern int32 fibo_nw_get_apn(uint8 simId, uint8 *nPdpType,uint8 *apn_len, uint8 *apn);

// virtual at channel
extern INT32 fibo_at_send(const UINT8 *cmd, UINT16 len);

typedef void (*at_record_callback)(char *atcmd,unsigned int ret,unsigned short err,char *resp);
extern void at_record_register(at_record_callback cb);
typedef void (*at_urc_callback)(char *urc);
extern void at_urc_register(at_urc_callback cb);
extern char *atCmdGetCmd(void);

//i2c
extern INT32 i2c_open(HAL_I2C_BPS_T bps);
extern INT32 i2c_close(void);
extern INT32 i2c_send_data(UINT32 slaveAddr, UINT32 memAddr, bool is16bit, UINT8 *pData, UINT8 datalen);
extern INT32 i2c_get_data(UINT32 slaveAddr, UINT32 memAddr, bool is16bit, UINT8 *pData, UINT8 datalen);
extern INT32 fibo_i2c_open(HAL_I2C_BPS_T bps,i2c_Handle *i2c_handle);
extern INT32 fibo_i2c_close(i2c_Handle *i2c_handle);
extern INT32 fibo_i2c_Write(i2c_Handle i2c_handle, drvI2cSlave_t idAddress, UINT8 *Buffer, UINT8 bufferlen);
extern INT32 fibo_i2c_Read(i2c_Handle i2c_handle, drvI2cSlave_t idAddress, UINT8 *Buffer, UINT8 bufferlen);
extern INT32 fibo_i2c_xfer(i2c_Handle i2c_handle,drvI2cSlave_t idAddress, UINT8 *send_Buffer, UINT8 send_bufferlen, UINT8 *recv_Buffer, UINT8 recv_bufferlen);

//queue
extern UINT32 fibo_queue_create(UINT32 msg_count, UINT32 msg_size);
extern INT32 fibo_queue_put(UINT32 msg_id, const void *msg, UINT32 timeout);
extern INT32 fibo_queue_put_to_front(UINT32 msg_id, const void *msg, UINT32 timeout);
extern INT32 fibo_queue_get(UINT32 msg_id, void *msg, UINT32 timeout);
extern INT32 fibo_queue_put_isr(UINT32 msg_id, const void *msg);
extern INT32 fibo_queue_put_to_front_isr(UINT32 msg_id, const void *msg);
extern INT32 fibo_queue_get_isr(UINT32 msg_id, void *msg);
extern void fibo_queue_delete(UINT32 msg_id);
extern UINT32 fibo_queue_space_available(UINT32 msg_id);
extern void fibo_queue_reset(UINT32 msg_id);

//mutex
extern UINT32 fibo_mutex_create(void);
extern void fibo_mutex_lock(UINT32 mutex_id);
extern INT32 fibo_mutex_try_lock(UINT32 mutex_id, UINT32 timeout);
extern void fibo_mutex_unlock(UINT32 mutex_id);
extern void fibo_mutex_delete(UINT32 mutex_id);
extern INT32 fibo_rng_generate(void *buf, UINT32 len);

//timer
#ifdef CONFIG_FIBOCOM_8910_OLD_API
extern UINT32 fibo_timer_create(void (*fn)(void *arg), void *arg);
extern bool fibo_timer_start(UINT32 timer_id, UINT32 ms);
extern bool fibo_timer_stop(UINT32 timerid);
#else
extern void* fibo_timer_create(void (*fn)(void *arg), void *arg,bool isIRQ);
extern bool fibo_timer_start(void* timer, UINT32 time,bool isPeriod);
extern INT32 fibo_timer_stop(void * timer);
#endif
extern INT32 fibo_timer_delete(void * timer);
extern UINT32 fibo_rtc_irq_create(void (*fn)(void *arg), void *arg);
extern bool fibo_rtc_irq_start(UINT32 timer_id, UINT32 us);
extern bool fibo_rtc_irq_period_start(UINT32 timer_id, UINT32 ms);
extern UINT32 fibo_setRtc_Irq(UINT32 us,void(* fn)(void * arg),void * arg);
extern UINT32 fibo_setRtc_Irq_period(UINT32 ms, void (*fn)(void *arg), void *arg);
extern INT32 fibo_timer_free(UINT32 timerid);
#ifdef CONFIG_FIBOCOM_XIAONIU_FEATURE
extern UINT32 fibo_timer_new(UINT32 ms, void (*fn)(void *arg), void *arg);
extern UINT32 fibo_timer_period_new(UINT32 ms, void (*fn)(void *arg), void *arg);
#endif

extern INT32 fibo_heapinfo_get(uint32_t * size,uint32_t * avail_size,uint32_t *max_block_size);
extern bool fibo_camera_regs_init(char *Sensor_name, camera_reg_t *cam_regs, int32_t num);
extern bool fibo_camera_read_reg(char *Sensor_name, uint8_t addr, uint8_t *value);
extern bool fibo_camera_write_reg(char *Sensor_name, uint8_t addr, uint8_t data);

extern INT32 fibo_camera_init(void);
extern INT32 fibo_camera_deinit(void);
extern INT32 fibo_camera_GetSensorInfo(CAM_DEV_T * pstCamDevice);
extern INT32 fibo_camera_CaptureImage(UINT16 * * pFrameBuf);
extern INT32 fibo_camera_StartPreview(void);
extern INT32 fibo_camera_StopPreview(void);
extern INT32 fibo_camera_GetPreviewBuf(UINT16 * * pPreviewBuf);
extern INT32 fibo_camera_PrevNotify(UINT16 * pPreviewBuf);
extern void fibo_camera_led_open(bool enable);
extern bool fibo_camera_led_setLevel(uint32_t led_level);
extern void fibo_camera_cleanbuf();

//audio
extern INT32 fibo_audio_start_v2(INT32 type,INT8 * filepath,auPlayerCallback_t cb_ctx);
extern INT32 fibo_audio_play(INT32 type,INT8 *filename);
extern bool fibo_get_audio_status(void);
extern INT32 fibo_audio_stop(void);
extern INT32 fibo_audio_pause(void);
extern INT32 fibo_audio_resume(void);
extern INT32 fibo_audio_mem_start(UINT8 format, UINT8 * buff, UINT32 size, auPlayerCallback_t cb_ctx);
extern INT32 fibo_audio_mem_stop(void);
extern INT32 fibo_audio_list_play(const char fname[][128], int32_t num);
extern INT32 fibo_tone_play(uint8_t tone_state, uint8_t tone_id, uint32_t tone_duration, uint8_t mix_factor);
extern int32 fibo_audio_set_cfg(f_audio_param_cfg_t *cfg);
extern int32 fibo_audio_get_cfg(f_audio_param_cfg_t *cfg);
#ifdef CONFIG_FIBOCOM_FREQ_TONE_SUPPORT
extern int32 fibo_audio_auto_play_freq_tone(uint8_t tone_state, uint32_t low_freq, uint32_t high_freq, uint32_t tone_duration);
#endif
extern bool fibo_audio_status_get(void);
extern INT32 fibo_audio_external_pa_enable(uint16_t pin_id);
extern INT32 fibo_external_pa_cfg(paSpec_t *cfg);
extern INT32 fibo_set_volume(AUDIO_VOLUME_MODE_T mode, uint8_t level);
extern INT32 fibo_get_volume(AUDIO_VOLUME_MODE_T mode);
extern INT32 fibo_set_volume_external(AUDIO_VOLUME_MODE_T mode, uint8_t level);
extern INT32 fibo_get_volume_external(AUDIO_VOLUME_MODE_T mode);
extern INT32 fibo_set_volume_gain(AUDIO_VOLUME_MODE_T mode,uint16_t gain_val);
extern INT32 fibo_get_volume_gain(AUDIO_VOLUME_MODE_T mode);
extern INT32 fibo_external_PA_enable(uint16_t pin_id);
extern INT32 fibo_external_PA_enable_level(uint16_t pin_id);
extern INT32 fibo_audio_mem_play(UINT8 format, UINT8 *buff, UINT32 size, auPlayerCallback_t cb_ctx);

//audio stream
extern INT32 fibo_audio_recorder_stream_start(uint8_t format, uint8_t *audio_buff, uint32_t *Wp, uint32_t buffsize, auAmrnbMode_t amr_mode);
extern INT32 fibo_audio_recorder_stream_stop(void);
extern INT32 fibo_audio_stream_start(uint8_t format, uint8_t *buff, uint32_t *Rp, uint32_t *Wp, uint32_t buffsize, int32_t timeout);
extern INT32 fibo_audio_stream_stop(void);
extern INT32 fibo_audio_stream_stop_nowait(void);

//loopback
extern INT32 fibo_audio_loopback(audevOutput_t output, audevInput_t input, bool is_lp, uint8_t mode);
extern auPlayer_t *auPlayerCreate(void);
extern bool auPlayerStartPipe(auPlayer_t *d, auStreamFormat_t format, const auDecoderParamSet_t *params, struct osiPipe *pipe);
extern bool auPlayerStop(auPlayer_t *d);
extern void fibo_set_aud_delay_flag(void);

//tts
extern bool fibo_tts_is_playing(void);
extern INT32 fibo_tts_start(const UINT8 *pData, INT8 cEncode);
extern INT32 fibo_tts_stop(void);
extern INT32 fibo_tts_voice_speed(INT32 speed);
extern INT32 fibo_tts_voice_pitch(INT32 pitch);
extern INT32 fibo_tts_set_respath(const char *path);
extern INT32 fibo_tts_cfg(INT32 key, INT32 mode, INT32 *value);
typedef void(*fibo_tts_outpcmdata_cb)(const void* outdata, uint32 nsize);
extern int32 fibo_tts_convert_pcm(fibo_tts_outpcmdata_cb callback);


//ecm
extern INT32 fibo_get_ip_mask_gw_of_ecm(char *ip_addr, char *netmask, char *gateway);


extern INT32 fibo_ext_flash_init(UINT8 Pinsel);
extern INT32 fibo_ext_flash_info(UINT32 * pulId, UINT32 * pulCapacity);

//thread
extern void fibo_taskSleep(UINT32 msec);
extern INT32 fibo_thread_create(void *pvTaskCode, INT8 *pcName, UINT32 usStackDepth, void *pvParameters, UINT32 uxPriority);
extern INT32 fibo_thread_create_ex(void *pvTaskCode, char *pcName, UINT32 usStackDepth, void *pvParameters, UINT32 uxPriority, UINT32* pThreadId);
extern void fibo_thread_delete(void);
extern UINT64 fibo_get_sys_tick(void);
extern void fibo_specify_thread_delete(UINT32 uThread);
extern UINT32 fibo_get_thread_stack_remaining_size(UINT32 ThreadId);
extern UINT32 fibo_thread_id(void);

//memory mallc and free
extern void *fibo_malloc(UINT32 size);
extern INT32 fibo_free(void *buffer);

//soft reset and power off
extern INT32 fibo_soft_reset(void);
extern INT32 fibo_soft_power_off(void);

//sem
extern UINT32 fibo_sem_new(UINT8 inivalue);
extern void fibo_sem_free(UINT32 semid);
extern void fibo_sem_wait(UINT32 semid);
extern bool fibo_sem_try_wait(UINT32 semid, UINT32 timeout);
extern void fibo_sem_signal(UINT32 semid);

//sim
extern INT32 fibo_sim_init(void);
extern INT32 fibo_sim_init_v2(uint8_t simid);
extern INT32 fibo_find_sim(void);
extern INT32 fibo_find_sim_v2(uint8_t simid);
extern INT32 fibo_get_sim_status(uint8_t *pucSimStatus);
extern INT32 fibo_get_sim_status_v2(uint8_t *pucSimStatus,uint8_t simId);

//sim detect switch
extern INT8 fibo_sim_detect_switch(uint8_t nSlotStatus);

//Double sim single standby
extern int fibo_set_sim_slot(uint8_t simid);
extern uint8_t fibo_get_sim_slot();
extern int fibo_set_customize_app_register(void *CustomizeHandleFun);



//adc
extern INT32 fibo_hal_adc_init(void);
extern INT32 fibo_hal_adc_deinit(void);
extern INT32 fibo_hal_adc_get_data(hal_adc_channel_t channel, UINT32 scale_value);
extern INT32 fibo_hal_adc_get_data_polling(hal_adc_channel_t channel, UINT32 *data);

extern INT32 fibo_lcd_init(void);
extern INT32 fibo_lcd_deinit(void);
extern INT32 fibo_lcd_FrameTransfer(const lcdFrameBuffer_t * pstFrame, const lcdDisplay_t * pstWindow);
extern INT32 fibo_lcd_SetOverLay(const lcdFrameBuffer_t * pstFrame);
extern INT32 fibo_lcd_Sleep(BOOL mode);
extern INT32 fibo_lcd_SetPixel(UINT16 ulx, UINT16 uly, UINT16 ulcolor);
extern INT32 fibo_lcd_FillRect(const lcdDisplay_t * pstWindow, UINT16 ulBgcolor);
extern INT32 fibo_lcd_SetBrushDirection(lcdDirect_t direct_type);
extern INT32 fibo_lcd_DrawLine(UINT16 ulSx, UINT16 ulSy, UINT16 ulEx, UINT16 ulEy, UINT16 ulColor);
extern INT32 fibo_lcd_Getinfo(uint32_t * puldevid, uint32_t * pulwidth, uint32_t * pulheight);
extern INT32 fibo_lcd_Setvideosize(uint32_t width, uint32_t height, uint32_t imageBufWidth);
extern INT32 fibo_lcd_rstPin_setLevel(SAPP_IO_ID_T id,UINT8 level);
extern INT32 fibo_lcd_register(lcdCfg_t * lcd, void * data);
extern INT32 fibo_lcd_spi_config(uint8_t spilinetype, uint32_t spiclk);
extern INT32 fibo_lcd_spi_set_freq(uint32_t spiclk);
extern INT32 fibo_lcd_spi_set_mode(uint8_t spilinetype);
extern INT32 fibo_lcd_spi_write_cmd(uint16_t cmd);
extern INT32 fibo_lcd_spi_write_data(uint16_t data);
extern INT32 fibo_lcd_spi_read_data(uint16_t addr, uint8_t * pData, uint32_t len);

extern void fibo_lcd_regs_init(lcd_reg_t *regs, int32_t Length);
extern void fibo_lcd_write_reg(uint8_t addr, uint8_t *CommandValue, int32_t Length);
extern void fibo_lcd_read_reg(uint8_t addr, uint8_t *data, int32_t Length);

/**
 * @brief Register a LCD device to the multiple LCD device list.
 * 
 * @param dev_name LCD device name
 * @param lcd_cfg multiple LCD device configuration, including CSX pin, RESET pin, TE pin information, etc.
 * @return int32_t 0: successful
 *                 other value: failed, see 'Err Code' in file drv_lcd.h.
 * 
 * @note  Recording in list only, after that fibo_lcd_init() should be called to initialize this LCD device.
 */
extern int32_t fibo_lcd_reg_multi_device(const char* dev_name, multi_lcd_cfg_t lcd_cfg);

/**
 * @brief Un-register a LCD device from the multiple LCD device list.
 * 
 * @param dev_name LCD device name
 * @return int32_t 0: successful
 *                 other value: failed, see 'Err Code' in file drv_lcd.h.
 * 
 * @note fibo_lcd_deinit() should be called to de-initialize this LCD device first before un-register it.
 */
extern int32_t fibo_lcd_unreg_multi_device(const char* dev_name);

/**
 * @brief Switch to the specified device from the multiple LCD device list. 
 * 
 * @param dev_name LCD device name
 * @return int32_t 0: successful
 *                 other value: failed, see 'Err Code' in file drv_lcd.h.
 * 
 * @note It is recommended to calling fibo_lcd_switch_multi_device() first everytime when you will call anyone of 
 *       fibo_lcd_xxx operation APIs in multi-threaded scenario, even fibo_lcd_init() and fibo_lcd_deinit().
 */
extern int32_t fibo_lcd_switch_multi_device(const char* dev_name);


extern bool fibo_sweep_code(camastae_t *cam, uint16_t *pFrame, uint8_t *pDatabuf, uint8_t *pResult, uint32_t *pReslen, int *type);
extern char * fibo_get_sweep_lib_ver(void);
extern bool fibo_get_decode_author_status();
extern bool fibo_set_bar_code(barCodeEx_t *bc, uint8_t num);

/**
 * @brief The status of sweeping code process
 * 
 */
typedef enum {
    FIBO_SWEEP_CODE_STAT_TYPE_UNKNOWN = 0,  /* unknown status */
    FIBO_SWEEP_CODE_STAT_TYPE_START,        /* start decoding  */
    FIBO_SWEEP_CODE_STAT_TYPE_FOUND,        /* found the bull's eye of the QR code image, but not means decoding finished */
    FIBO_SWEEP_CODE_STAT_TYPE_FINISH,       /* decoding finished, but not means decoding successful */
} f_sweep_code_stat_e;

// The function pointer type of the callback function for sweeping code process.
typedef INT32(*f_sweep_code_stat_cb_t)(f_sweep_code_stat_e stat);

/**
 * @brief Register the callback function for the status of sweeping code process.
 * 
 * @param cb The callback function for sweeping code process.
 * @return INT32 0: Successful
 *               -1: Failed
 */
extern INT32 fibo_sweep_code_register_cb(f_sweep_code_stat_cb_t cb);


extern void *  fibo_app_start(uint32_t appimg_flash_offset,uint32_t appimg_flash_size,uint32_t appimg_ram_offset,uint32_t appimg_ram_size,void *parm);

//local time
extern INT32 fibo_setRTC(hal_rtc_time_t *time);
extern INT32 fibo_getRTC(hal_rtc_time_t *time);
extern INT32 fibo_setRTC_timezone(int8_t timezone);
extern INT32 fibo_getRTC_timezone(void);
extern bool fibo_get_nw_timezone(int8_t *timezone);
extern int fibo_nw_get_nitz_time_info(fibo_nw_nitz_time_info_t *pt_info);
extern INT32 fibo_set_alarm(const char *alarmTime,uint8_t index,const char *alarmRecurr, bool poweroff);
extern INT32 fibo_del_alarm(uint8_t index);

//drx
extern int32 fibo_nw_get_drxtm(uint8 simId, uint32 *drxtm);

//sleep
extern INT32 fibo_setSleepMode(UINT8 time);

extern INT32 fibo_register_wakeupcallback(wakeup_callback_t wakeup_cb);

//lpg
extern INT32 fibo_lpg_switch(UINT8 ucLpgSwitch);

//gpio
extern INT32 fibo_gpio_mode_set(SAPP_IO_ID_T id, UINT8  mode);
extern INT32 fibo_gpio_cfg(SAPP_IO_ID_T id, SAPP_GPIO_CFG_T cfg);
extern INT32 fibo_gpio_set(SAPP_IO_ID_T id, UINT8 level);
extern INT32 fibo_gpio_get(SAPP_IO_ID_T id, UINT8 *level);
extern INT32 fibo_gpio_pull_disable(SAPP_IO_ID_T id);
extern INT32 fibo_gpiomode_init(SAPP_IO_ID_T id);
extern INT32 fibo_gpio_isr_init(SAPP_IO_ID_T id, oc_isr_t* isr_cfg);
extern INT32 fibo_gpio_isr_deinit(SAPP_IO_ID_T id);
extern INT32 fibo_gpio_pull_up_or_down(SAPP_IO_ID_T id, bool is_pull_up);
extern INT32 fibo_gpio_pull_high_resistance(SAPP_IO_ID_T id, bool is_pull_high);
extern INT32 fibo_gpio_driving_set(SAPP_IO_ID_T id,uint8_t driving);
extern INT32 fibo_get_sim_det_level(void);
extern INT32 fibo_gpio_pull_up_or_down_v2(FIBO_PIN_MODE pMode,bool is_sleep,void *reserve);

//mcal
extern INT32 fibo_set_mcal(CFW_NW_NST_TX_PARA_CONFIG *nst_para_config, CFW_SIM_ID nSimID);
extern INT32 fibo_set_mcal_dev(UINT8 ucDevValue,CFW_SIM_ID nSimID);

extern bool fibo_heartbeat_param_set(char *serverip, uint16_t port,  uint16_t time, char *hexdata);
extern bool fibo_enable_heartbeat(uint8_t cid,  uint8_t nsim, int32_t mode, int32_t protocol);
extern bool fibo_set_heartbeat_reconnect_timer(uint16_t time1, uint16_t time2,uint16_t time3,uint16_t time4,uint16_t time5,uint16_t time6);

//MTSM
extern int fibo_get_chip_temperature_by_adc(void);

//dualsim
extern int fibo_set_dualsim(uint8_t simid);
extern uint8_t fibo_get_dualsim();
extern int fibo_set_datasim(uint8_t simid);

//keypad
extern bool fibo_keypad_queue_init(UINT32 timeout);
extern bool fibo_keypad_queue_output(keypad_info_t * key);
extern UINT8 fibo_get_pwrkeystatus(void);
extern bool fibo_keypad_msg_mod(bool en);
extern bool fibo_pwrkeypad_config(UINT32 timeout, bool Ispwroff);
extern keypad_info_t fibo_get_pwrkeypad_status(void);
extern bool fibo_bykeyid_get_status(UINT32 id);

/**
 * @brief Function pointer type of key's commom callback function.
 *
 * @param keyid key id
 * @param state 1: key pressed
 *              2: key released
 * @param ctx Context delievered from register API.
 */
typedef void (*key_cb_t)(INT32 keyid, INT32 state, void *ctx);

/**
 * @brief Register key's commom callback function for all keys (including keypad's keys and powerkey)
 *        and allocate their resources.
 *
 * @param cb Key's commom callback function pointer.
 * @param ctx Context delievered to callback function.
 * @param long_press_time Detection time of long press event in milliseconds:
 *                        < 1000ms : Without long press detection, only press and release events are reported.
 *                        >= 1000ms: Wih long press detection, short press, short release, long press, long release events are reported.
 * @return INT32 0: successful
 *               -1: failed
 */
extern INT32 fibo_key_register_callback(key_cb_t cb, void *ctx, UINT32 long_press_time);

/**
 * @brief Release key's commom callback function and all keys' resources.
 *
 * @return INT32 0: successful
 *               -1: failed
 */
extern INT32 fibo_key_release_callback(void);

//Shut down
extern INT32 fibo_set_pwr_callback_ex(key_callback pwr_cb,UINT16 long_press,void *arg);
extern INT32 fibo_set_pwr_callback(key_callback pwr_cb,UINT16 long_press);
extern INT32 fibo_set_pwr_callback_v1(key_callback pwr_press_cb,key_callback pwr_release_cb,UINT16 long_press);
extern INT32 fibo_set_shutdown_mode(osiShutdownMode_t mode);

//uart
extern INT32 fibo_hal_uart_init(hal_uart_port_t uart_port, hal_uart_config_t *uart_config, uart_input_callback_t recv_cb, void *arg);
extern INT32 fibo_hal_uart_deinit(hal_uart_port_t uart_port);
extern INT32 fibo_hal_uart_put(hal_uart_port_t uart_port, UINT8 *buff, UINT32 len);
extern INT32 fibo_hal_uart_data_to_send(int uart_port);
extern INT32 fibo_pi_read_uartdata(hal_uart_port_t uart_port, UINT8 *buf, UINT32 len, UINT32 timeout);

//uart-to-485
extern INT32 fibo_rs485_init(hal_uart_port_t uart_port, hal_uart_config_t * uart_config, uint16_t ctrl_pin, uart_input_callback_t recv_cb, void * arg);
extern INT32 fibo_rs485_deinit(hal_uart_port_t uart_port);
extern INT32 fibo_rs485_write(int uart_port, UINT8 * buff, UINT32 len, bool wait_finish);
extern INT32 fibo_rs485_read(int uart_port, UINT8 * buff, UINT32 len);
extern INT32 fibo_rs485_read_try(int uart_port, UINT8 * buff, UINT32 len, uint32_t timeout);
extern INT32 fibo_rs485_read_block(int uart_port, UINT8 * buff, UINT32 len, uint32_t timeout);

//extflash
extern void fibo_ext_flash_mode_set(uint8_t mode);
extern bool fibo_ext_flash_poweron(uint8_t pinsel, uint8_t val, uint8_t div); // Notice: This interface will be invalid later, Please use the new API, fibo_ext_flash_poweron_v2.
extern bool fibo_ext_flash_poweron_v2(uint8_t pinsel, uint8_t val, uint8_t div);
extern INT32 fibo_ext_flash_init(uint8_t Pinsel);
extern INT32 fibo_ext_flash_info(UINT32 *pulId, UINT32 *pulCapacity);
extern INT32 fibo_ext_flash_read(UINT32 faddr, UINT8 *data,UINT32 size);
extern INT32 fibo_ext_flash_write(UINT32 faddr, UINT8 *data,UINT32 size);
extern INT32 fibo_ext_flash_erase(UINT32 faddr, UINT32 size);
extern INT32 fibo_ffsmountExtflash(UINT32 uladdr_start, UINT32 ulsize,
                              char *dir,UINT8 spi_pin_sel,
                              bool format_on_fail,bool force_format);

extern INT32 fibo_ffs_unmount_ext_flash(const char *dir);
extern INT32 fibo_ffs_format_ext_flash(const char *dir);

//inner flash
extern INT32 fibo_inner_flash_init(void);
extern INT32 fibo_inner_flash_info(UINT32 *pulId, UINT32 *pulCapacity);
extern INT32 fibo_inner_flash_read(UINT32 faddr, UINT8 *data,UINT32 size);
extern INT32 fibo_inner_flash_write(UINT32 faddr, UINT8 *data,UINT32 size);
extern INT32 fibo_inner_flash_erase(UINT32 faddr, UINT32 size);

//usb
extern atDevice_t *FIBO_usbDevice_init(uint32_t name, usb_input_callback_t recv_cb);
extern int FIBO_usbDevice_send(atDevice_t *th, const void *data, size_t length);
extern int FIBO_usbDevice_State_report(atDevice_t *th, Report_UsbDev_Event  usedev_Event);
extern INT32 FIBO_usbDevice_Deinit(atDevice_t *th);
extern INT8 fibo_usbswitch(uint8_t en);
extern INT32 fibo_set_usbmode(uint8_t usbmode);
extern INT32 fibo_get_usbmode(void);
extern UINT8 fibo_get_Usbisinsert(void);
extern UINT8 fibo_get_Usbisconnect(void);
extern INT32 fibo_usb_status_get(void);
extern INT32 fibo_usb_status_set_callback(usbStateChangeCallback_t cb, void* cb_ctx);


//pwm
extern void fibo_pwm_close(void);
extern bool fibo_pwm_open(int32_t index);
extern void fibo_pwm_config(int32_t index, uint16_t duty, uint16_t arr, uint16_t psc);
extern void fibo_pwm_config_lowpower(int32_t index, uint16_t duty, uint16_t arr, uint16_t psc);
extern bool fibo_pwt_config(uint32_t period_count, uint8_t prescaler, uint32_t duty);
extern bool fibo_pwt_start_or_stop(bool is_start);
extern bool fibo_pwl_level_set(UINT8 index, UINT8 level);


//spi
extern INT32 fibo_spi_open(drvSpiConfig_t cfg, SPIHANDLE *spiFd);
extern INT32 fibo_spi_close(SPIHANDLE *spiFd);
extern INT32 fibo_spi_send(SPIHANDLE spiFd, SPI_IOC spiIoc, void *sendaddr, uint32_t size);
extern INT32 fibo_spi_recv(SPIHANDLE spiFd, SPI_IOC spiIoc, void *readaddr, uint32_t size);
extern INT32 fibo_spi_pinctrl(void);
extern INT32 fibo_spi_send_rcv(SPIHANDLE spiFd, SPI_IOC spiIoc, void *sendaddr, void *readaddr, uint32_t size);

//pmu
extern INT32 fibo_pmu_setlevel(UINT8 pmu_type,UINT8 level);
extern bool fibo_pmu_ctrl(UINT32 id, BOOL enabled, BOOL lp_enabled);
/**
 * @brief Structure of PMU configuration.
 * 
 */
typedef struct {
    bool enable;    // power switch in normal mode
    bool lp_enable; // power switch in low-power mode
    uint32_t level; // voltage value (unit: mV)
} pmu_cfg_t;

/**
 * @brief Setting PMU configuration, including power switch, voltage value, etc. 
 *        It's used to power on or off a PMU with a voltage value.
 * 
 * @param pmu_id PMU ID, defined by platform.
 * @param cfg Structure of PMU configuration.
 * @return int32_t  0: successful
 *                 -1: failed
 */
extern int32_t fibo_pmu_set_cfg(uint32_t pmu_id, pmu_cfg_t cfg);

extern INT32 fibo_lbs_get_gisinfo_v2(char *key, UINT8 type, uint8_t simid, fibo_lbs_res_t *lbsinfo, INT32 timeout);
extern INT32 fibo_wifiscan_v2(uint8_t simid, key_callback cb);
extern INT32 fibo_get_module_work_status(void);

//xml
extern XML_Parser fibo_xml_parser_create(const XML_Char *encodingName);
extern void fibo_xml_set_user_data(XML_Parser parser, void *p);
extern void fibo_xml_set_element_handle(XML_Parser parser,
                                    XML_StartElementHandler start,
                                    XML_EndElementHandler end);
extern void fibo_xml_set_character_data_handler(XML_Parser parser, XML_CharacterDataHandler handler);
extern enum XML_Status fibo_xml_parse(XML_Parser parser, const char *s, int len, int isFinal);
extern void fibo_xml_parser_free(XML_Parser parser);

extern INT32 fibo_set_antcmd_callback(void (*callback)(void *));
extern void fibo_antcmd_callback_urc(char *buf, int len);
extern void fibo_get_rrc_connection_time(uint8_t *rat, uint32_t *time_ms);
extern INT32 fibo_get_deviceSN(char* deviceSN, int size);
extern INT32 fibo_set_deviceSN(const char *deviceSN);
extern INT8 *fibo_get_sw_verno(void);
extern INT8 *fibo_get_verno_ys(void);
#ifdef CONFIG_FIBOCOM_XINGUODU
char *fibo_get_gtpkgver(char *buffer, int size);
#endif

//Log
extern int fibo_textTrace(char *fmt, ...);
extern INT32 fibo_vsnprintf(INT8 *buff,UINT32 n,const INT8 *fmt,va_list arg);

//BASE64
extern INT32 fibo_base64_encode( unsigned char *dst, size_t dlen, size_t *olen,const unsigned char *src, size_t slen );
extern INT32 fibo_base64_decode( unsigned char *dst, size_t dlen, size_t *olen,const unsigned char *src, size_t slen );

//sink
extern INT32 fibo_sink_OnOff(UINT32 sinknum, BOOL onOff);
extern INT32 fibo_sinkLevel_Set(UINT32 sinknum, UINT32 light_level);

//psm
extern bool fibo_psm_mode(int mode,char *tt3412,char *tt3324);

//boot
extern UINT8 fibo_getbootcause(void);

extern void fibo_app_handle_at_init(fibo_app_handle_at_cb callback);
extern void fibo_app_report_default_urc(const char *text, size_t length);
void fibo_tesecase_send_urc(const char *text, size_t length);

extern bool fibo_get_security_flag(void);
extern void fibo_set_security_flag(void);

extern INT32 fibo_log_to_usb(bool on);
extern INT32 fibo_get_log_flag(void);

//sfile
extern INT32 fibo_sfile_init(const INT32 *path);
extern INT32 fibo_sfile_size(const INT32 *path);
extern INT32 fibo_sfile_read(const INT32 *path, UINT8 *buf, UINT32 count);
extern INT32 fibo_sfile_write(const INT32 *path, UINT8 *buf, UINT32 count);

//ble basic and service function
extern INT32 fibo_ble_onoff(uint8_t onoff);
extern INT32 fibo_ble_get_ver(char *ver);
extern INT32 fibo_ble_read_addr(uint8_t addr_type,char *addr);
extern INT32 fibo_ble_set_read_name(uint8_t set_read,uint8_t *name,uint8_t name_type);
extern INT32 fibo_ble_get_connect_state(int *state);
extern INT32 fibo_ble_set_dev_param(int param_count,int AdvMin,int AdvMax,int AdvType,int OwnAddrType,int DirectAddrType,int AdvChannMap,int AdvFilter,char *DirectAddr);
extern INT32 fibo_ble_set_dev_data(int data_len,char *data);
extern INT32 fibo_ble_enable_dev(int onoff);
extern INT32 fibo_ble_send_data(int data_len,char *data);
extern INT32 fibo_ble_set_read_mtu(uint8_t set_read, int *mtu);
extern INT32 fibo_ble_get_peer_mtu(int *mtu);
extern INT32 fibo_ble_set_scan_response_data(int data_len,char *data);
extern INT32 fibo_ble_recv_data(ble_data_recv_callback callback);
extern INT32 fibo_ble_data_request(int data_len,void *data, int timeout);
extern INT32 fibo_ble_gtsend_data(int index, int len, void* data);
extern INT32 fibo_ble_set_gtuuid(int index, void *uuid, UINT8 attr);
extern INT32 fibo_ble_response_to_read(int len, void* data);

//gnss
extern INT32 fibo_gnss_lte_prior_set(int prior);

//codec
extern INT32 fibo_extern_codec_cfg(codec_Spec_t *cfg);

//ble ota
extern INT32 fibo_ble_ota(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_ble_handle(INT8 *data, UINT32 len);

//ri switch
INT32 fibo_uart_ring_switch(bool ri_on);

//pwm
extern bool fibo_pwm_close_v2(int32_t index);
#endif /*_FIBO_OPENCPU_H_*/

//ble sleep
extern INT32 fibo_ble_sleep_mode(int32_t mode);

//device Number
extern INT32 fibo_set_get_deviceNUM(INT32 type,INT8 *deviceNUM,size_t length);
extern INT8 fibo_get_cgmm(UINT8 *p_cgmm_info, UINT8 cgmm_info_size);
extern INT8 *fibo_get_hw_verno(void);


//dotMatrixLcd
extern void fibo_dotMatrixLcdSetDisplayParam(uint8_t contrastRatio_coarse, uint8_t contrastRatio_fine, uint8_t bias);
extern void fibo_dotMatrixLcdSendData(uint8_t column, uint8_t row, void *buff, uint32_t size);

extern INT32 fibo_ble_uuid_config(int type,void *uuid);
extern INT32 fibo_ble_disconnect(int index);

extern INT32 fibo_softReset(void);
extern int fibo_get_NetInfo_by_simid(uint32_t *pNetInfo, uint8_t DataSize, uint8_t simId);
extern void fibo_switch_sim_handle(UINT8 mode);

extern int fibo_sim_lock_pin(uint16_t nFac,char *uPwd,uint8_t simid);
extern int fibo_sim_unlock_pin(uint16_t nFac,char *uPwd,uint8_t simid);
extern int fibo_sim_get_lock_status(uint16_t nFac,int *state,uint8_t simid);
extern int fibo_sim_enable_pin(uint16_t nFac,char *uPwd,uint8_t simid);
extern int fibo_sim_disable_pin(uint16_t nFac,char *uPwd,uint8_t simid);
extern int fibo_sim_change_pin(char *pin, char *newpin, uint8_t simid);

extern int fibo_sim_pin_status(int *state, unsigned *pinremaintries,unsigned *pukremaintries,uint8_t simid);
extern int fibo_sim_verify_pin(char *pinorpuk,char *newpin,int sim);

extern INT32 fibo_get_sim_tel_num(void (*cb)(UINT8 *sim_tel_num), uint8_t simId);
extern INT32 fibo_get_imsi(UINT8 *imsi);
extern void fibo_get_chip_id(UINT8 *chipid);

extern INT32 fibo_set_imei(UINT8 *imei, CFW_SIM_ID nSimID);
extern INT32 fibo_get_imsi_by_simid(UINT8 *imsi,uint8_t simId);
extern INT32 fibo_get_imsi_by_simid_v2(UINT8 *imsi,uint8_t simId);
extern uint8_t fibo_get_sim_protect(uint32_t sim_id);
extern INT32 fibo_set_sim_protect(uint8_t op_mode,uint32_t pEFID[8], uint32_t sim_id);
extern INT32 fibo_get_mnc_len(uint8_t *mnc_len,uint8_t simId);

extern void fibo_vsim_init(VSIM_COMMAND vsim_apdu_process, uint8_t nSimID);
extern void fibo_vsim_close(VSIM_COMMAND vsim_apdu_process_fn, uint8_t nSimID);
extern INT32 fibo_xiaoshui_get_mcc_mnc(CFW_SIM_ID nSim, char *mcc, char *mnc);

#ifdef CONFIG_FIBOCOM_XIAONIU_FEATURE
extern INT32 fibo_niu_ota_handle(INT8 *data, UINT32 len);
extern void fibo_niu_ota_set_callback(fota_download_cb_t cb);
extern INT32 fibo_niu_app_dl(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_niu_app_update(void);
extern INT32 fibo_niu_firmware_handle(INT8 *data, UINT32 len);
extern INT32 fibo_niu_firmware_dl(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
extern INT32 fibo_niu_firmware_update(void);
extern INT32 fibo_niu_appfota_handle(INT8 *data, UINT32 len);
extern INT32 fibo_niu_appfw_handle(INT8 *data, UINT32 len);
extern INT32 fibo_niu_appfota_update(void);
extern INT32 fibo_niu_appfw_update(void);
#endif

extern bool fibo_TY_wakeup_init(UINT8 pin,UINT8 level);
extern bool fibo_TY_wakeup_enable(void);
extern bool fibo_TY_wakeup_disable(void);
extern int fibo_get_4g_temperature(void);

extern INT32 fibo_setRTC_timezone_v2(int8_t timezone);
extern INT32 fibo_getRTC_timezone_v2(void);

extern int HAL_Get_Imei(char *IMEI);
extern int HAL_Get_Cereg(void);
extern int HAL_Get_Iccid(char *iccid);
extern int HAL_Get_IMSI(char *imsi);
extern int HAL_Get_Signal_Strength(void);
extern int HAL_Get_CGMI(char* manufacturer);
extern int HAL_Get_CGMM(char* moduleType);
extern int HAL_Get_ChipManufacturer(char* chipManufacturer);
extern int HAL_Get_ChipType(char* chipType);
extern int HAL_Get_SoftwareVersion(char* sVerion);
extern int HAL_Get_HardwareVersion(char* hVersion);
extern int HAL_Get_NueStats(T_CellInfo* cellInfo);
extern int HAL_Get_LBSParams(char* tac,char* ci);
extern int HAL_Get_PowerMode(T_PowerMode* psm);
extern int HAL_Get_Cops(char* cops);
extern int HAL_Get_CCLK(char* dateTime);
extern int HAL_Get_MemeryInfo(T_MemInfo* memInfo);
extern int HAL_Get_APN(char* apn);

#ifdef CONFIG_MC661_CN_19_70_CUDMP
extern void HAL_Ota_Set_FwSize(unsigned int size);
extern int HAL_Ota_Update_Stage(void);
extern bool HAL_Ota_is_MiniSys(void);
extern int HAL_Ota_Enable_MiniSys(void);
extern bool HAL_Ota_Update_Init(void);
extern int HAL_Ota_Write_Flash(char* data, unsigned int size);
extern bool HAL_Ota_Check_Fw_Validate(void);
extern void HAL_Ota_Update_Start(void);
extern void HAL_Ota_Update_Deinit(void);
#endif

extern int fibo_timer_is_running(void* timer);
extern INT32 fibo_timer_remaining(void* timer);
extern bool fibo_get_log_status(uint8_t * Apsetport, uint8_t * Apport, uint8_t * Apenable, uint8_t * Cpport, uint8_t * Cpenable);
extern INT32 fibo_send_cmd(int cmd, void* in_param, unsigned int in_len, void* out_param, unsigned int *out_len);

extern UINT32 fibo_enterCS(void);
extern void fibo_exitCS(UINT32 status);
extern INT32 fibo_clean_dumpinfo(void);
extern INT32 fibo_read_dumpinfo(UINT32 *plcount, char *pDumpInfo);
extern char * fibo_get_chip_unique_id(uint32_t *len);
INT32 fibo_wifiscan_v3(uint8_t simid, int maxnum, key_callback cb);
/*MTC1001-37 start zhaihaojie add get or set dump status*/
extern int32_t fibo_dm_dump_status(uint8_t mode,int32_t* status);
/*MTC1001-37 end zhaihaojie add get or set dump status*/
//sftp
extern INT32 fibo_sftp_open(char * remoteaddr,  const char *username,const char *password,int32_t uRemotePort);
extern INT32 fibo_sftp_write(const char *loclpath,const char *sftppath);
extern INT32 fibo_sftp_read(const char *sftppath,const char *localfile);
//2024-08-06, haopanpan, MTC0697-603, Begin. Provides sftp interfaces for uploading logs to sftp files.
extern INT32 fibo_sftp_open_file(uint8_t index, char * remoteaddr,  const char *username,const char *password,int32_t uRemotePort, const char *sftppath);
extern INT32 fibo_sftp_write_file(uint8_t index, void* data ,uint32_t size);
extern INT32 fibo_sftp_close_file(uint8_t index);
//2024-08-06, haopanpan, MTC0697-603, end.

//efuse
extern bool fibo_efuse_read(uint32_t index, uint32_t * value);
extern bool fibo_efuse_write(uint32_t index, uint32_t value);
extern INT32 fibo_cus_fw_init();

//poweron npi mode
extern int32_t fibo_usb_get_poweron_npi_mode(uint8_t * mode);
extern int32_t fibo_usb_set_poweron_npi_mode(uint8_t mode);

typedef void (*user_callback_t)(void* param);
extern void fibo_set_user_callback(user_callback_t user_cb);

//hid
extern INT32 fibo_usb_hid_open(uint32_t name, Hidhand * hid);
extern INT32 fibo_usb_hid_send(Hidhand hid, uint8_t * buf, uint32_t size);
extern INT32 fibo_usb_hid_close(Hidhand * hid);
extern INT32 fibo_usb_hid_set_callback(Hidhand hid, Hidcbtype_t type, Hidcallback_t cb, void * ctx);
extern INT32 fibo_usb_hid_update_descriptor(Hidhand hid, Desctype_t type, void * Desc, bool usbRst);

//SMS
extern int fibo_sms_send_message(sms_client_handle_type h_sms, fibo_sms_send_msg_info_t *sms_info);
extern int fibo_sms_addrxmsghandler(sms_client_handle_type h_sms, fibo_sms_rxmsg_handler_func_t handlerPtr);
extern int fibo_sms_getsmscenteraddress( sms_client_handle_type         h_sms, fibo_sms_service_center_cfg_t *service_center_cfg);
extern int fibo_sms_setsmscenteraddress( sms_client_handle_type         h_sms, fibo_sms_service_center_cfg_t *service_center_cfg);
extern int fibo_sms_read_message(sms_client_handle_type h_sms, fibo_sms_read_message_info_t *read_info, fibo_sms_read_message_req_info_t *read_req_info);
extern int fibo_sms_deletefromstorage(sms_client_handle_type h_sms, fibo_sms_storage_info_t *pt_sms_storage);
extern int fibo_sms_getmsglist(sms_client_handle_type h_sms,     fibo_sms_msg_list_req_info_t *req_info,    fibo_sms_msg_list_info_t *resp_info);


extern int32_t fibo_sim_get_phonenumber(sim_client_handle_type h_sim,fibo_sim_app_id_info_t *pt_info,char *phone_num,uint32_t phone_len);
extern int32_t fibo_sim_esim_apdu_send(    char* apdu_cmd,    int apdu_length,    char* response,int resp_length);
extern int32_t fibo_sim_notify_detect(sim_client_handle_type h_sim,uint8_t connect);
extern INT32 fibo_audio_set_data(void * data, uint32_t size);
extern INT32 fibo_audio_stream_start_v2(uint8_t format, uint32_t samplerate, uint8_t *buff, uint32_t *Rp, uint32_t *Wp, uint32_t buffsize, int32_t timeout, auPlayerCallback_t cb_ctx);


int32_t fibo_dm_get_cgmm(dm_client_handle_type h_dm, UINT8 *p_cgmm_info, UINT8 cgmm_info_size);
QR_ENC_CODE_T *Cam_QR_Enc(int length, const unsigned char *data, int scale);

extern INT32 fibo_charge_notice_cb_set(fiboChargeNoticeCB_t cb);

//watchdog
extern INT32 fibo_watchdog_enable(UINT32 sec);
extern INT32 fibo_watchdog_disable(void);
extern INT32 fibo_watchdog_feed(void);

extern INT32 fibo_set_prior_RAT(UINT8 nPreferRat,CFW_SIM_ID nSim);
extern UINT8 fibo_get_curr_prior_RAT(CFW_SIM_ID nSim);

extern INT32 fibo_file_getSize(const char *path);
extern INT32 fibo_set_app_version(char *ver);
extern int   lwip_ioctl(int s, long cmd, void *argp);
//8910 old api: poweroff
extern INT32 fibo_softPowerOff(void);
extern INT32 fibo_clock_gettime_montionic(struct timeval *tv, void *tz);

extern int32_t fibo_pi_get_host_type(uint32_t*host_type);
extern int32_t fibo_pi_set_usb_recovery_report(report_callback r_cb, void * cb_ctx);
extern int32 fibo_usb_register_vbus_cb(f_vbus_callback cb, void * cb_ctx);

extern int32_t fibo_sim_esim_logical_channel_open (char* aid_value,uint8_t *session_id);
extern int32_t fibo_sim_esim_channel_apdu_send (uint8_t session_id,char* apdu_cmd,int apdu_length,char* response,int resp_length);
extern int32_t fibo_sim_esim_logical_channel_close (uint8_t session_id);

