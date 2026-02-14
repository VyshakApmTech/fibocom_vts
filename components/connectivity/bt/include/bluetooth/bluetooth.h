/**
 * @file    bluetooth.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

/**
 * @addtogroup bluestack
 * @{
 * @defgroup common
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define NUM_BYTES_LMP_FEATURES          8
#define NUM_BYTES_EX_FEATURES             8

#define BT_PROFILE_GATT_ID "gatt"
#define BT_PROFILE_A2DP_ID "a2dp"
#define BT_PROFILE_AVRCP_ID "avrcp"
#define BT_PROFILE_SPP_ID "spp"
#define BT_PROFILE_HFP_ID "hfp"
#define BT_PROFILE_SDP_ID "sdp"
#define BT_PROFILE_UWS_ID "uws"
#define BT_AT_PTS_ID "pts"

#define BT_BTAT_NPI_ID "npi"

#define BT_INQUIRY_GAP        0x9e8b33

#define HCI_CMD_WRITE_LOCAL_ADDRESS ((0x3f << 10) | 0xA0)

#define BT_MAX_PAIRED_DEVICE_COUNT  10
#define BT_MAX_DEVICE_NAME      32  ///< max length of device name
#define BT_EXTENDED_INQUIRY_SRV_COUNT 20

#define LE_MAX_DEVICE_NAME        22
#define LE_WHITE_NUM_MAX        12

/*
    HCI Status/Error Codes
*/

#define HCI_errNOERROR              0x00
#define HCI_errUNKNOWNCOMMAND       0x01
#define HCI_errNOCONNECTION         0x02
#define HCI_errHWFAILURE            0x03
#define HCI_errPAGETIMEOUT          0x04
#define HCI_errAUTHFAILURE          0x05
#define HCI_errKEYMISSING           0x06
#define HCI_errMEMORYFULL           0x07
#define HCI_errCONNTIMEOUT          0x08
#define HCI_errMAXCONNECTIONS       0x09
#define HCI_errMAXSCOS              0x0A
#define HCI_errACLEXISTS            0x0B
#define HCI_errCMDDISALLOWED        0x0C
#define HCI_errREJRESOURCES         0x0D
#define HCI_errREJSECURITY          0x0E
#define HCI_errREJPERSONALDEV       0x0F
#define HCI_errHOSTTIMEOUT          0x10
#define HCI_errUNSUPPORTED          0x11
#define HCI_errINVALIDPARAM         0x12
#define HCI_errTERMUSER             0x13
#define HCI_errTERMRESOURCES        0x14
#define HCI_errTERMPOWEROFF         0x15
#define HCI_errLOCALTERM            0x16
#define HCI_errREPEATED             0x17
#define HCI_errNOPARING             0x18
#define HCI_errUNKNOWNLMPPDU        0x19
#define HCI_errUNSUPPORTEDREMOTE    0x1A
#define HCI_errSCOOFFSETREJ         0x1B
#define HCI_errSCOINTERVALREJ       0x1C
#define HCI_errSCOMODEREJ           0x1D
#define HCI_errLMPINVALIDPARAM      0x1E
#define HCI_errUNSPECERROR          0x1F
#define HCI_errLMPUNSUPPPARAM       0x20
#define HCI_errNOROLECHANGE         0x21
#define HCI_errLMPTIMEOUT           0x22
#define HCI_errLMPTRANSCOLLISION    0x23
#define HCI_errLMPPDUDISALLOWED     0x24
#define HCI_errBADCRYPTMODE         0x25
#define HCI_errUNITKEYUSED          0x26
#define HCI_errQOSUNSUPPORTED       0x27
#define HCI_errINSTANTPASSED        0x28
#define HCI_errNOUNITPAIRING        0x29

#define CONN_NONE              0
#define CONN_ACTIVE             1
#define CONN_CLOSED             2
#define CONN_PENDING            3
#define CONN_RELEASEPENDING     4
#define CONN_PENDINGHOLDALLACLS 5
#define CONN_REQPENDING    6

typedef enum
{
    MGR_SET_NO_VISIABLE  = 0x00,
    MGR_SET_CONNECTABLE  = 0x10,
    MGR_SET_DISCOVERABLE = 0x01,
    MGR_SET_ALL_VISIABLE = 0x11,
    MGR_SET_LIMITEDDISCOVERY_NOCONNECTABLE = 0x02,
    MGR_SET_LIMITEDDISCOVERY_CONNECTABLE = 0x12,
} bt_visible_t;

typedef struct
{
    UINT16 handle;
    UINT8 tx_phy;
    UINT8 rx_phy;
}le_phy_param_t;

typedef enum
{
    BT_A2DP_ROLE_SOURCE = 0,   /* a2dp source */
    BT_A2DP_ROLE_SINK,     /*a2dp sink */
} bt_a2dp_role_t;

typedef enum
{
    BT_HFP_ROLE_AG = 0,   /* hfp ag */
    BT_HFP_ROLE_HF,     /*hfp hf */
} bt_hfp_role_t;

typedef enum
{
    BT_AVRCP_ROLE_TG = 0,   /* avrcp tg */
    BT_AVRCP_ROLE_CT,     /*avrcp ct */
} bt_avrcp_role_t;

/**
 * @enum bt_state_t
 * @brief Enumeration with possible bt open state.
 *
 */

typedef enum
{
    BT_STATE_OFF,                  /**< Bt is off */
    BT_STATE_TURNING_OFF,  /**< Bt is turning on */
    BT_STATE_TURNING_ON,   /**< Bt is turning off */
    BT_STATE_ON,                   /**< Bt is on */
    BT_STATE_CMD_ERR, /**< HCI send cmd error*/
    BT_STATE_ERR, /**< Bt is ERR */
} bt_state_t;

/** ACL state enumeration*/
typedef enum
{
    BT_ACL_STATE_DISCONNECTED,/**< ACL is disconnected */
    BT_ACL_STATE_CONNECTED,     /**< ACL is connected */
} bt_acl_state_t;

/** Bond state enumeration*/
typedef enum
{
    BT_BOND_STATE_NONE,         /**< There is no bonding */
    BT_BOND_STATE_BONDING,   /**< Bt is bonding */
    BT_BOND_STATE_BONDED,     /**< Bt is bonded */
} bt_bond_state_t;

/**Bt property type enumeration*/
typedef enum
{
    BT_PRO_BDNAME = 0x01,                  /**< Use to set/get local name */
    BT_PRO_BDADDR,                             /**< Use to set/get local addr */
    BT_PRO_UUIDS,                                /**< Use to set/get uuids */
    BT_PRO_CLASSIC_PACKAGES,           /**< They are classic packages */
    BT_PRO_BLE_PACKAGES,                  /**< They are ble packages */
    BT_PRO_INQUIRY_SCAN_ACTIVITY,  /**< It is an inquiry scan activity */
    BT_PRO_PAGE_SCAN_ACTIVITY,       /**< It is an page scan activity */
    BT_PRO_BLE_PHY,                            /**< Use to get ble phy */
    BT_PRO_REMOTE_VERSION,              /**< Use to set/get remote version */
    BT_PRO_STACK_SMP_CONFIG,           /**< Use to set/get smp configuration */
    BT_PRO_BT_STORAGE,               /**< Use to get bt storage */
    BT_PRO_BT_LOCAL_VERSION,
} bt_pro_type_t;

typedef enum
{
    BT_DEVICE_TYPE_BREDR = 1,
    BT_DEVICE_TYPE_BLE,
    BT_DEVICE_TYPE_DUMO,
}bt_device_type_t;

typedef struct
{
    UINT8 device_type; //bt_device_type_t

    UINT8 *raw_data;
    UINT32 raw_data_len;

    UINT32 list_count;

    UINT32 *service_class_list;
    UINT32 *profile_uuid_list;
    UINT16 *feature_list;
    UINT16 *profile_ver_list;

}bt_service_record;
/**Bt property structure*/
typedef struct
{
    bt_pro_type_t type;  /**< The property type */
    void *val;                /**< The value of "type" */
    int len;                   /**<  The lenght of "type" */
} bt_property_t;

/**Bt LMP version enumeration*/
typedef enum
{
    LL_VERSION_RESERVE = 0x05,/**< Reserved */
    LL_VERSION_CORE_4_0,         /**< Link layer version core specification 4.0 */
    LL_VERSION_CORE_4_1,         /**< Link layer version core specification 4.1 */
    LL_VERSION_CORE_4_2,         /**< Link layer version core specification 4.2 */
    LL_VERSION_CORE_5_0,         /**< Link layer version core specification 5.0 */
    LL_VERSION_CORE_5_1,         /**< Link layer version core specification 5.1 */
    LL_VERSION_CORE_5_2,         /**< Link layer version core specification 5.2 */
} bt_lmp_version;

/**Bt enable state enumeration */
typedef enum
{
    BT_STATE_DISABLE = 0x00,  /**< Initial state of bluetooth */
    BT_STATE_ENABLE,         /**< Bt is enabled  */
    BT_STATE_NOUSE,               /**< Bt is not used  */
} bt_enable_state_t;

/**Bt remote device version information structure*/
typedef struct
{
    ///UINT16 handle;
    bdaddr_t addr;                          /**< The address of remote device */
    UINT8 lmp_ver;                         /**< Lmp version number */
    UINT16 manufacturer_name;     /**< Manufacturer name */
    UINT16 lmp_sub_ver;                /**< Lmp sub version number */
} bt_remote_device_version_info;
/**Bt pair mode */
typedef enum
{
    BT_PAIR_MODE = 0x00,
    BT_PAIR_LAGACY_PIN_CODE_MODE,
    BT_PAIR_SSP_AUTO_NUMERIC_MODE,
    BT_PAIR_SSP_USER_NUMERIC_MODE,
    BT_PAIR_SSP_PASSKEY_INPUT_MODE,
    BT_PAIR_SSP_PASSKEY_OUTPUT_MODE,
    BT_PAIR_SSP_PASSKEY_NOTIFI_MODE,
    BT_PAIR_SSP_PASSKEY_OUT_OF_BAND_MODE,
} bt_pair_mode_t;
/** structure of bt SLC connection*/


typedef enum
{
    BT_INQUIRY                     = ((0x1<<10) | 0x01),
    BT_INQUIRY_CANCEL             = ((0x1<<10) | 0x02),
    BT_INQUIRY_PERIOD            = ((0x1<<10) | 0x03),
    BT_INQUIRY_PERIOD_CANCEL     = ((0x1<<10) | 0x04),
    BT_CREATE_CONNECTION        = ((0x1<<10) | 0x05),
    BT_DISCONNECT                = ((0x1<<10) | 0x06),
    BT_ACCEPT_CONNECTION        = ((0x1<<10) | 0x09),
    BT_REJECT_CONNECTION        = ((0x1<<10) | 0x0A),
    BT_BONDING                    = ((0x1<<10) | 0x11),
    BT_CREATE_SCO_CONNECTION    = ((0x1<<10) | 0x28),
    BT_ROLE_SWITCH                = ((0x2<<10) | 0x0B),
    BT_SET_LINK_TIMEOUT            = ((0x3<<10) | 0x37),
}BT_STACK_ERROR_OPCODE;

typedef enum
{
    ROLE_MASTER = 0,
    ROLE_SLAVE = 1,
    ROLE_UNKNOWN = 0xFF,
}bt_acl_role;

typedef struct
{
    bt_acl_role role; ///<  0 = master, 1 = slave
    UINT8 aclState; ///< value of  CONN_OPEN | CONN_CLOSED | CONN_PENDING ...
    UINT16 aclHandle;
    //BD_FEATURES features[HCI_EXT_FEATURES_PAGE_MAX + 1];
    UINT8 num_read_pages;
    UINT8 max_pages;
    UINT8 host_features[NUM_BYTES_LMP_FEATURES]; ///< LMP features of peer device

    UINT8 scoState; ///< value of CONN_OPEN | CONN_CLOSED | CONN_PENDING
    UINT16 scoHandle;
    UINT16 sco_rx_length;

    UINT8 sniff_packets;
    UINT8 sniffTimer;
    UINT16 min_interval;
    UINT16 max_interval;
    UINT16 adjust_interval;
    UINT8 active_sniff;
    UINT8 deviceMode; ///<  value of DEV_SNIFF | DEV_HOLD | DEV_PARK | DEV_ACTIVE
    UINT16 interval;
    UINT8 authMode; ///< MGR_UNTRUSTED | MGR_TRUSTED
    UINT8 encryptionMode; ///< CRPYT_DISABLED | CRYPT_P2PONLY | CRYPT_ALL

    void *access_Tid;
    UINT8 accessProto;  ///< L2CAP_PROTOCOLID | RFCOMM_PROTOCOLID | SDP_PROTOCOLID | TCSBIN_PROTOCOLID
    UINT8 accessDirect;
    UINT8 accessLevel; ///< MGR_NOUSER_INTERACTION | MGR_NOMITM_MIN_USERIF | MGR_NOMITM_ENCRYPTION | MGR_MITM_ENCRYPTION
    UINT8 accessReqState;   ///<  ACCESS_REQ_PENDING | ACCESS_REQ_ENCRYPT_PENDING
    UINT8 bondingState;  ///<  BOND_INACTIVE | BOND_PENDING
    UINT8 bond_role;
    UINT8 bond_type;  ///< DEDICATED_BONDING | GENERAL_BONDING

    /* For setting the Flush Timeout on an ACL */
    UINT8 aclFlushTimeoutFlag;
    UINT16 aclFlushTimeout;
    void *flushTOCallbackTid;

    /* For setting the Link Supervision Timeout on an ACL */
    UINT8 aclLinkSupTimeoutFlag;
    UINT16 aclLinkSupTimeout;
    UINT16 aclLinkSupTimeoutNew;
    void *aclLinkSupTOCallbackTid;

    void *l2_connect_Tid;

    /* For setting the QoS parameters for an ACL */
#if QOS_SUPPORT == 1
    t_L2_ChannelFlowSpec *qosParams;
    t_L2_ChannelFlowSpec *qosParamsNew;
    void *qosSetupCallbackTid;
#endif

    UINT8 aclDisconnectReason;
    UINT8 link_type;
    UINT8 security_want_keytype;
    BOOL is_incoming;
} bt_stack_connection_t;

typedef struct
{
    bdaddr_t bdAddress; ///< addr of device
    UINT8 current_device_type;///< eir or not  HCI_evEXTENDEDINQUIRYRESULT or other
    UINT32 classOfDevice; ///< cod of device
    UINT8 pageScanRepetitionMode;
    UINT8 pageScanPeriodMode;
    UINT8 pageScanMode;
    UINT16 clockOffset;
    UINT8 rssi;
    /*extended inquiry result list below*/
    UINT8 name_length;
    UINT8 name[MGR_MAX_REMOTE_NAME_LEN];
    UINT16 srv_uuid_16bit[BT_EXTENDED_INQUIRY_SRV_COUNT]; ///< srv list of extended inquiry
    UINT8 OOB_R[LINKKEY_SIZE];
    UINT8 OOB_C[LINKKEY_SIZE];

    UINT8 io_capability;
    UINT8 auth_requirements;
    UINT8 bonding;
    UINT8 linkKeyType; ///< value of  MGR_COMBINATION | MGR_LOCALUNITKEY | MGR_REMOTEUNITKEY ...
    UINT8 linkKey[LINKKEY_SIZE];
    //UINT8 get_timers;
    UINT16 deviceState; ///< combined with device record flags, value of _mgr_device_list_flag enum
    UINT8 lmp_ver;
    UINT8 manufacturer_name;
    UINT16 lmp_sub_ver;
    UINT8 lmp_features[2+1][8];///< HCI_EXT_FEATURES_PAGE_MAX
    UINT8 ex_features[NUM_BYTES_EX_FEATURES];
    //UINT32 sysBuildDate;
    INT64 update_date;
} bt_stack_device_info_t;

/** structure of bt connection*/
typedef struct
{
    bt_stack_device_info_t dev;
    bt_stack_connection_t acl;
    struct _BT_HFP_T *hfp;   /**< structure of hfp module all data info*/
    struct _BT_A2DP_T *a2dp; /**< structure of a2dp module all data info*/
    struct _BT_AVRCP_T *avrcp;  /**< structure of avrcp module all data info*/
    struct _BT_SPP_T *spp;  /**< structure of spp module all data info*/
    struct _BT_OBEX_T *obex;  /**< structure of obex module all data info*/
}bt_conn_t;


/**Bt scan acitivity parameter structure*/
typedef struct
{
    UINT16 interval;
    UINT16 window;
} bt_scan_activity_param;



typedef struct
{
    bdaddr_t addr;
    UINT8 current_device_type;///< eir if event code is Extended Inquiry Result(0x2f)
    UINT32 class_of_device;
    UINT8 page_scan_repetition_mode;
    UINT8 page_scan_period_mode;
    UINT8 page_scan_mode;
    UINT16 clock_offset;
    UINT8 rssi;

    /*extended inquiry result list below*/
    UINT8 name_length;
    UINT8 name[MGR_MAX_REMOTE_NAME_LEN];
    UINT16 srv_uuid_16bit[BT_EXTENDED_INQUIRY_SRV_COUNT]; ///< srv list of extended inquiry
    UINT8 OOB_R[LINKKEY_SIZE];
    UINT8 OOB_C[LINKKEY_SIZE];
} bt_stack_inquiry_info_t;

typedef struct _bt_stack_storage_t
{
    UINT32 magic; ///< indicate the db record was invalid or not
    UINT8 status; ///<BT is disable or enable for default
    UINT8 visibility; ///<BT visibility
    UINT32 volume; ///<Volume of HFP/A2DP
    UINT8 local_name[BT_MAX_DEVICE_NAME]; ///< BT classic name
    UINT8 le_name[LE_MAX_DEVICE_NAME];///< name of BLE
    UINT8 le_irk[16];///< IRK of BLE
    UINT8 paired_count; ///< paired device count
    UINT8 le_paired_count;///< le paired device count
    bdaddr_t local_br_addr;
    bdaddr_t local_le_addr; ///< ble public address
    bdaddr_t le_random_addr;    ///< ble random address
    ble_stack_white_list_t white_list[LE_WHITE_NUM_MAX]; ///< white list for BLE

    ble_smp_t ble_smp[BT_MAX_PAIRED_DEVICE_COUNT];  ///< local smp information
    ble_smp_t ble_smp_remote[BT_MAX_PAIRED_DEVICE_COUNT]; ///< peer smp information
    ble_device_t ble_dev[BT_MAX_PAIRED_DEVICE_COUNT]; ///< peer device information
    bt_stack_device_info_t bt_dev[BT_MAX_PAIRED_DEVICE_COUNT];
    UINT32 reserved;
}bt_stack_storage_t;

typedef struct
{
/**
* @brief inquiry_complete_cb
* @details When we start to inquiry devices, we also neeed this callback, when inquiry time is over or
*              number of inquiry devices is bigger than max devices number, this function will be called
*              and inquiry is completed.
* @param [out] UINT8 status Inquiry is success or has error
* @return None
*/
    void (*inquiry_complete_cb)(UINT8 status);
/**
* @brief inquiry_result_cb
* @details When we start to inquiry devices, we also need this callback, when devices are inquiried by
*              us, this function will be called to record and report their information.
* @param [out] bt_stack_inquiry_info_t *p_info Information of devices we inquiry
* @return None
*/
    void (*inquiry_result_cb)(bt_stack_inquiry_info_t *p_info);
/**
* @brief get_remote_name_complete_cb
* @details Report the name and status when get remote name complete.
* @param [out] bdaddr_t *addr Device address
* @param [out] UINT8 status Get remote device name status
* @param [out] UINT8 *name Device name
* @return None
*/
    void (*get_remote_name_complete_cb)(bdaddr_t *addr, UINT8 status, UINT8 *name);
/**
* @brief connection_complete_cb
* @details Report connection status when connection complete.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT8 status Connection complete status
* @return None
*/
    void (*connection_complete_cb)(bt_conn_t *p_conn, UINT8 status);
/**
* @brief sco_connection_complete_cb
* @details Report sco connection status when sco connection complete.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT8 status Scoconnection complete status
* @return None
*/
    void (*sco_connection_complete_cb)(bt_conn_t *p_conn, UINT8 status);
/**
* @brief connection_request_cb
* @details When we accept connection request, the function is called to report link info.
* @param [out] bdaddr_t *addr The address of device which send connection request to us
* @param [out] UINT32 class_of_device The class if device
* @param [out] UINT8 link_type The link type of device
* @return Success or not
*/
    bt_status_t (*connection_request_cb)(bdaddr_t *addr, UINT32 class_of_device, UINT8 link_type);
/**
* @brief acl_disconnection_cb
* @details When acl link is disconnected, this function will be called to tell app.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT8 reason The reason why alc link disconnect
* @param [out] UINT16 handle The handle of acl connection
* @return None
*/
    void (*acl_disconnection_cb)(bt_conn_t *p_conn, UINT8 reason, UINT16 handle);
/**
* @brief sco_disconnection_cb
* @details When sco link is disconnected, this function will be called to tell app.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT8 reason The reason why sco link disconnect
* @param [out] UINT16 handle The handle of sco connection
* @return None
*/
    void (*sco_disconnection_cb)(bt_conn_t *p_conn, UINT8 reason, UINT16 handle);
/**
* @brief calib_clock_cb
* @details If other device send calib clock msg to us, this function will be called to tell app.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] int offset The offset time of clock
* @param [out] UINT8 status The status of calib colock
* @return None
*/
    void (*calib_clock_cb)(bt_conn_t *p_conn, int offset, UINT8 status);
/**
* @brief bonding_complete_cb
* @details When bonding is complete, this function will be called to app.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT8 status The status of bonding
* @return None
*/
    void (*bonding_complete_cb)(bt_conn_t *p_conn, UINT8 status);
/**
* @brief user_confirm_cb
* @details When we receive user comfirm message, this function is called to tell app and reply.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT32 value The value of user confirm
* @return Success or not
*/
    void (*user_confirm_cb)(bt_conn_t *p_conn, bt_pair_mode_t mode, UINT32 value);
/**
* @brief encrypt_change_cb
* @details When we receive encrypt change message, this function is called to tell app and reply.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT8 value The value of status
* @return Success or not
*/
    void (*encrypt_change_cb)(bt_conn_t *p_conn, UINT8 status);
/**
* @brief link_timeout_changed_cb
* @details When the value of link timeout is changed, this function is called to app.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT16 timeout The value of timeout
* @return None
*/
    void (*link_timeout_changed_cb)(bt_conn_t *p_conn, UINT16 timeout);
/**
* @brief role_change_cb
* @details When we change our role, this function is called to tell app.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT8 role The role we change to be
* @param [out] UINT8 status The status of role change
* @return None
*/
    void (*role_change_cb)(bt_conn_t *p_conn, UINT8 role, UINT8 status);
/**
* @brief function_error_cb
* @details When error occur in some function, this function is called to tell app.
* @param [out] UINT16 opcode The error occur in this opcode case
* @param [out] UINT8 err_code The error code in this error
* @return None
*/
    void (*function_error_cb)(UINT16 opcode, UINT8 err_code);
/**
* @brief mode_change_cb
* @details When hci mode is changed, this function is called to tell app.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] UINT8 mode The mode we change to be
* @return None
*/
    void (*mode_change_cb)(bt_conn_t *p_conn, UINT8 mode);
/**
* @brief num_of_complete_cb
* @details Hci number of complete event callback.
* @param [out] UINT8 total_num number
* @return None
*/
    void (*num_of_complete_cb)(UINT8 total_num);
/**
* @brief state_change_cb
* @details When our state is changed, this function is called to tell app.
* @param [out] bt_state_t state The state we change to be
* @return None
*/
    void (*state_change_cb)(bt_state_t state);
/**
* @brief read_rssi_cb
* @details When other device send their rssi to us, this function is called to tell app rssi if device.
* @param [out] bt_conn_t *p_conn All device info are in this point
* @param [out] int rssi The value of rssi
* @return Success or not
*/
    void (*read_rssi_cb)(bt_conn_t *p_conn, int rssi);
} bt_stack_common_callbacks_t;

/**
 * @struct bt_stack_callbacks_t
 * @brief bt stack callback function.
*/
typedef struct
{
    bt_stack_common_callbacks_t comm_cb;
} bt_stack_callbacks_t;

typedef struct
{
/**
* @brief init
* @details Init bt stack interface, set config and storage in stack with this function.
* @param [in] bt_stack_callbacks_t *cb Bluestack callback function
* @param [in] const bt_stack_config_t *config All config information in stack
* @param [in] bt_stack_storage_t *p_storage All storage information in satck
* @return Success or not
*/
    bt_status_t (*init)(bt_stack_callbacks_t *cb,
        const bt_stack_config_t *config, bt_stack_storage_t *p_storage);
/**
* @brief  cleanup
* @details When we disconnect with other device or close bt, ues this function to cleanup and free memory.
* @param [in] bt_conn_t *p_conn All device info and a2dp info in this point
* @return Success or not
*/
    void (*cleanup)(void);
/**
* @brief  enable
* @details Enable bt.
* @param [in] void
* @return void
*/
    bt_status_t (*enable)(void);
/**
* @brief  disable
* @details disable bt.
* @param [in] void
* @return Success or not
*/
    bt_status_t (*disable)(void);
/**
* @brief  get_dev_list
* @details get classic bt device list.
* @param [in] void
* @return Bt device list
*/
    bt_conn_t ** (*get_dev_list)(void);
/**
* @brief  inquiry
* @details Bt start to inquiry other devices by this function.
* @param [in] UINT32 lap BT_INQUIRY_GAP  0x9e8b33
* @param [in] UINT8 length Inquiry timeout
* @param [in] UINT8 num_rsp Max inquiry number
* @return Success or not
*/
    bt_status_t (*inquiry)(UINT32 lap, UINT8 length, UINT8 num_rsp);
/**
* @brief  inquiry_cancel
* @details Bt start to cancel inquiry when device is inquirying.
* @param [in] void
* @return Success or not
*/
    bt_status_t (*inquiry_cancel)(void);
/**
* @brief  inquiry_period
* @details Inquiry devices by period.
* @param [in] UINT16 max_period Max period of inquiry
* @param [in] UINT16 min_period Min period of inquiry
* @param [in] UINT32 lap BT_INQUIRY_GAP  0x9e8b33
* @param [in] UINT8 length Inquiry timeout
* @param [in] UINT8 num_rsp Max inquiry number
* @return Success or not
*/
    bt_status_t (*inquiry_period)(UINT16 max_period, UINT16 min_period, UINT32 lap,
                                    UINT8 length, UINT8 num_rsp);
/**
* @brief  inquiry_period_cancel
* @details Bt start to cancel period inquiry when device is period inquirying.
* @param [in] void
* @return Success or not
*/
    bt_status_t (*inquiry_period_cancel)(void);
/**
* @brief  create_connection
* @details Bt start to create connection with other device.
* @param [in] bt_stack_inquiry_info_t *p_info The information searched
* @param [in] UINT8 allow_role_switch Whether allow role switch, generally it is 0
* @return Success or not
*/
    bt_status_t (*create_connection)(bt_stack_inquiry_info_t *p_info, UINT8 allow_role_switch);
/**
* @brief  disconnect
* @details Bt disconnect with other device.
* @param [in] bt_conn_t *p_conn All device info are in this point
* @param [in] UINT8 reason Disconnect reason
* @return Success or not
*/
    bt_status_t (*disconnect)(bt_conn_t *p_conn, UINT8 reason);
/**
* @brief  disconnect_sco
* @details disconnect sco/esco connection.
* @param [in] bt_conn_t *p_conn All device info are in this point
* @param [in] UINT8 reason Disconnect reason
* @return Success or not
*/
    bt_status_t (*disconnect_sco)(bt_conn_t *p_conn, UINT8 reason);
/**
* @brief  accept_connection
* @details Accept connection.
* @param [in] bdaddr_t *addr The device address which send connection req to us.
* @param [in] UINT8 reason Disconnect reason
* @return Success or not
*/
    bt_status_t (*accept_connection)(bdaddr_t *addr, UINT8 role);
/**
* @brief  reject_connection
* @details Reject connection.
* @param [in] bdaddr_t *addr The device address which send connection req to us.
* @param [in] UINT8 reason Reject reason
* @return Success or not
*/
    bt_status_t (*reject_connection)(bdaddr_t *addr, UINT8 reason);
/**
* @brief  bonding
* @details Bt send bonding request to other device.
* @param [in] bdaddr_t *addr The device address we bond.
* @return Success or not
*/
    bt_status_t (*bonding)(bdaddr_t *addr);
/**
* @brief  delete_bonding
* @details Bt delete the bonding device.
* @param [in] bdaddr_t *addr The device address we have bonded.
* @return Success or not
*/
    bt_status_t (*delete_bonding)(bdaddr_t *addr);
/**
* @brief  set_link_timeout
* @details Bt call this function when link is not set and link time is bigger than timeout set.
* @param [in] bt_conn_t *p_conn All device info are in this point
* @return Success or not
*/
    bt_status_t (*set_link_timeout)(bt_conn_t *p_conn, UINT16 timeout);
/**
* @brief  role_switch
* @details switch our role.
* @param [in] bt_conn_t *p_conn All device info are in this point.
* @param [in] UINT8 role The role we want to switch
* @return Success or not
*/
    bt_status_t (*role_switch)(bt_conn_t *p_conn, UINT8 role);
/**
* @brief  create_sco_connection
* @details Creat sco connection.
* @param [in] bt_conn_t *p_conn All device info are in this point.
* @return Success or not
*/
    bt_status_t (*create_sco_connection)(bt_conn_t *p_conn);

    //bt_status_t (*get_remote_name)(bdaddr_t *addr, UINT8 page_scan_mode, UINT16 clock_offset);
/**
* @brief  set_visible
* @details Set our visible.
* @param [in] UINT8 visiable 0x00 means no visible, 0x01 means discoverable, 0x02 means connectable
*                   0x03 means all visable.
* @return Success or not
*/
    bt_status_t (*set_visible)(UINT8 visiable);
    //UINT8 (*get_host_state)(void);
/**
* @brief  get_host_mode
* @details get host mode, such as BLE only/BR only/Dual mode
* @param void
* @return host mode, 1:BR only 2:BLE only 3:Dual mode
*/
    UINT8 (*get_host_mode)(void);
/**
* @brief  get_visible
* @details Get our visible.
* @param [in] void
* @return Visible 0x00 means no visible, 0x01 means discoverable, 0x02 means connectable 0x03 means
*             all visable.
*/
    UINT8 (*get_visable)(void);
/**
* @brief  set_property
* @details Set our property.
* @param [in] bt_property_t *property Bt property information
* @return Success or not
*/
    bt_status_t (*set_property)(bt_property_t *property);
/**
* @brief  get_property
* @details Get our property.
* @param [in] bt_property_t *property Bt property information
* @return Success or not
*/
    bt_status_t (*get_property)(bt_property_t *property);
/**
* @brief  enable_dut
* @details When we are in signal test during npi test, use this function to set visiblity.
* @param [in] UINT16 transport Always is HCI_PRIMARY_TRANSPORT 0x01
* @return Success or not
*/
    bt_status_t (*enable_dut)(UINT16 transport);
/**
* @brief  write_pagescan_parameters
* @details Write pagescan parameters, includeing interval and window.
* @param [in] UINT16 inter Pagescan interval
* @param [in] UINT16 win Pagescan window
* @return Success or not
*/
    bt_status_t (*write_pagescan_parameters)(UINT16 inter, UINT16 win);
/**
* @brief  get_conn
* @details BT find p_conn and information of it by handle or address.
* @param [in] UINT16 *handle If connection already has handle, p_conn can be found by handle, else
*                   p_conn can be found by address
* @param [in] bdaddr_t *addr Device address
* @return Success or not
*/
    bt_conn_t* (*get_conn)(UINT16 *handle, bdaddr_t *addr);
/**
* @brief passkey_response
* @details BT ssp and lagacy passkey responce.
* @param [in] bdaddr_t *addr Device address
* @param [in] UINT32 passkey Passkey entered by the user
* @param [in] UINT8 result Do users agree to the operation
* @return Success or not
*/
    bt_status_t (*passkey_response)(bdaddr_t addr,UINT8 mode,UINT8 result, UINT8 *passkey);
/**
* @brief  read_rssi
* @details read link rssi.
* @param [in] bt_conn_t *p_conn All device info are in this point.
* @return Success or not
*/
    bt_status_t (*read_rssi)(bt_conn_t *p_conn);
/**
* @brief get_config_param
* @details BT ssp and lagacy paramter set.
* @param [in] manager_config_t *config address
* @return Success or not
*/
    bt_status_t (*get_config_param)(manager_config_t *config);
/**
* @brief set_config_param
* @details BT ssp and lagacy  paramter set.
* @param [in] UINT8 io_cap I/O capability set
* @param [in] UINT8 whether to support SSP
* @return Success or not
*/
    bt_status_t (*set_config_param)(UINT8 io_cap,UINT8 ssp_support);
} bt_stack_interface_t;

bt_stack_interface_t *bt_stack_get_interface(void);
bt_stack_callbacks_t *bt_stack_get_callback(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

/** @} */

#endif
