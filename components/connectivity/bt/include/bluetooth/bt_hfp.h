/**
 * @file    bt_hfp.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_HFP_H__
#define __BT_HFP_H__

/**
 * @addtogroup bluestack
 * @{
 * @defgroup hfp
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define HFP_VOLUME_MAX_VALUE    15
#define MAX_PHONE_NUMBER        16

#define HFP_OP_NAME_MAX                 16
#define HFP_OP_SHORT_NAME_MAX           8

/*****************************************************************************/
/*                        Handsfree Feature Definitions                      */
/*****************************************************************************/
//AG supported features bit masks .
#define AG_FEATURE_THREE_WAY_CALLING                (1)
#define AG_FEATURE_NREC_FUNCTION                    (1<<1)
#define AG_FEATURE_VOICE_RECOGNITION                (1<<2)
#define AG_FEATURE_IN_BAND_RING                     (1<<3)
#define AG_FEATURE_VOICE_TAG                        (1<<4)
#define AG_FEATURE_REJECT_CALL                      (1<<5)
#define AG_FEATURE_ENHANCED_CALL_STATUS             (1<<6)
#define AG_FEATURE_ENHANCED_CALL_CONTROL            (1<<7)
#define AG_FEATURE_EXTENDED_ERROR_CODES             (1<<8)
#define AG_FEATURE_CODEC_NEGOTIATION                (1<<9)
#define AG_FEATURE_HF_INDICATORS                    (1<<10)
#define AG_FEATURE_ESCO_S4_SETTING                  (1<<11)
#define AG_FEATURE_ENHANCED_VR_STATUS               (1<<12)
#define AG_FEATURE_VR_TEXT                          (1<<13)


/*Bits for HF_Modify_Remote_Supplementary_Feature(). */
//HF supported features bit masks .
#define HF_FEATURE_NREC_FUNCTION                    (1)
#define HF_FEATURE_CALL_WAIT                        (1<<1)
#define HF_FEATURE_CLIP                             (1<<2)
#define HF_FEATURE_VOICE_RECOGNITION                (1<<3)
#define HF_FEATURE_REMOTE_VOL                       (1<<4)
#define HF_FEATURE_ENHANCED_CALL_STATUS             (1<<5)
#define HF_FEATURE_ENHANCED_CALL_CONTROL            (1<<6)
#define HF_FEATURE_CODEC_NEGOTIATION                (1<<7)
#define HF_FEATURE_HF_INDICATORS                    (1<<8)
#define HF_FEATURE_ESCO_S4                          (1<<9)

//flag of clip/ciev/call wait enable status
#define HFP_FLAG_CLIP_ENABLE            (1<<0)
#define HFP_FLAG_CIEV_ENABLE            (1<<1)
#define HFP_FLAG_CALL_WAIT_ENABLE       (1<<2)

/** hfp status*/
typedef enum
{
    HF_STATUS_NONE=0,
    HF_STATUS_INCOMING_CALL_IN_PROGRESS,
    HF_STATUS_OUTGOING_CALL_IN_PROGRESS,
    HF_STATUS_LOCAL_RING,
    HF_STATUS_REMOTE_RING,
}bthfp_status_t;

/** hfp call command*/
typedef enum
{
    HF_CMD_ANSWER=0,
    HF_CMD_REJECT,
    HF_CMD_RELEASE,
    HF_CMD_MUTI_CALL,
    AG_CMD_RING,
}hfp_call_cmd_t;

/* second argument of hf_call_request */
typedef enum
{
    /* use by hf_call_request */
    HF_CALL_INCOMMING,
    HF_CALL_OUTGOING,
    HF_CALL_START,
    HF_CALL_ALERT,
    HF_CALL_RELEASE,
    HF_CALL_NUM,
    HF_CALL_MEM,
    HF_CALL_REDIAL,
    HF_CALL_SWAP,
    HF_CALL_HOLD,

    /* use by hf_call_answer */
    HF_CALL_ACCEPT,
    HF_CALL_REJECT,

    /* use by hf_call_hold */
    HF_CALL_HELD,
    HF_CALL_WAIT,
    HF_CALL_ACTIVE,
    /* use by hf_call_status */
    HF_SERVICE_INDICATOR,
    HF_CALL_INDICATOR,
    HF_CALLSETUP_INDICATOR,
    HF_CALLHOLD_INDICATOR,
    HF_BATTCHG_INDICATOR,
    HF_SIGNAL_INDICATOR,
    HF_ROAM_INDICATOR,

    /* use by hf_voice_recognition */
    HF_VR_START,
    HF_VR_STOP
}bt_call_type_t;

/**Hfp connection state enumeration*/
typedef enum
{
    BTHFP_CONNECTION_STATE_DISCONNECTED = 0,   /**< Hfp connection is disconnected */
    BTHFP_CONNECTION_STATE_CONNECTING,            /**< Hfp connection is connecting */
    BTHFP_CONNECTION_STATE_CONNECTED,              /**< Hfp connection is connected */
    BTHFP_CONNECTION_STATE_DISCONNECTING,       /**< Hfp connection is disconnecting */
} bthfp_connection_state_t;

/**Hfp audio state enumeration*/
typedef enum
{
    BTHFP_AUDIO_STATE_DISCONNECTED = 0,              /**< Audio is disconnected */
    BTHFP_AUDIO_STATE_CONNECTING,                       /**< Audio is connecting */
    BTHFP_AUDIO_STATE_CONNECTED,                         /**< Audio is connected */
    BTHFP_AUDIO_STATE_DISCONNECTING                   /**< Audio is disconnecting */
} bthfp_audio_state_t;

/**Hfp volume type enumeration*/
typedef enum
{
    BTHFP_VOLUME_TYPE_SPK = 0,                               /**< Volume type is speaker */
    BTHFP_VOLUME_TYPE_MIC                                      /**< Volume type is microphone */
} bthfp_volume_type_t;

/**Hfp codec type enumeration*/
typedef enum
{
    BTHFP_CODEC_TYPE_CVSD = 1,                           /**< Codec type is CVSD */
    BTHFP_CODEC_TYPE_MSBC                                 /**< Codec type is mSBC */
} bthfp_codec_type_t;

/**Hfp service type enumeration*/
typedef enum
{
    BTHFP_SERVICE_TYPE_HOME = 0,                    /**< Service type is home */
    BTHFP_SERVICE_TYPE_ROAMING,                    /**< Service type is roaming */
} bthfp_service_type_t;

/**Hfp network state enumeration*/
typedef enum
{
    BTHFP_NETWORK_STATE_NOT_AVAILABLE = 0,/**< Network is not available */
    BTHFP_NETWORK_STATE_AVAILABLE,              /**< Network is available */
} bthfp_network_state_t;

/**Hfp call state enumeration*/
typedef enum
{
    BTHFP_CALL_NO_CALL_IN_PROGRESS = 0,/**<  No calls is in progress */
    BTHFP_CALL_CALL_IN_PROGRESS,            /**<  At least one call is in progress */
} bthfp_call_state_t;

/**Hfp callsetup state enumeration*/
typedef enum
{
    BTHFP_CALLSETUP_NONE = 0,      /**<  Not currently in call set up */
    BTHFP_CALLSETUP_INCOMING,    /**<  An incoming call process ongoing */
    BTHFP_CALLSETUP_OUTGOING,   /**<  An outcoming call set up is ongoing */
    BTHFP_CALLSETUP_ALERTING     /**<  Remote party being alerted in an outgoing call */
} bthfp_callsetup_state_t;

/**Hfp callheld state enumeration*/
typedef enum
{
    BTHFP_CALLHELD_NONE = 0,                /**<  No call held */
    BTHFP_CALLHELD_HOLD_AND_ACTIVE, /**<  Call is placed on held or active/held calls swapped */
    BTHFP_CALLHELD_HOLD,                      /**<  Call on hold,no active call */
} bthfp_callheld_state_t;

/**Hfp ciev type enumeration*/
/**CIND is define with internal as below:*/
/** "\r\n+CIND:(\"service\",(0,1)),(\"call\",(0,1)),(\"callsetup\",(0,3)),(\"callheld\",(0,2)),(\"battchg\",(0,5)),(\"signal\",(0,5)),(\"roam\",(0,1))\r\n";*/
typedef enum
{
    SERVICE_IND = 1, /**<  indicator of network service */
    CALL_IND,        /**<  indicator of call */
    CALLSETUP_IND,   /**<  indicator of callsetup */
    CALLHELD_IND,    /**<  indicator of call hold */
    BATTCHG_IND,     /**<  indicator of battery information */
    SIGNAL_IND,      /**<  indicator of signal with modem */
    ROAM_IND,        /**<  indicator of roam with network service */

    MAX_IND,
}hfp_cind_t;

typedef struct _HFP_INDICATOR_T
{
    UINT8 index;
    UINT8 value;
}hfp_indicator_t;

/**Hfp device information structure*/
typedef struct _BT_HFP_T
{
    UINT32 local_feature; //local feature, used to send CMD of AT+BRSF
    UINT32 peer_feature;   // peer device supported feature£¬get with +BRSF
    UINT8 role; //role of hfp, value of bt_hfp_role_t
    UINT8 slc_status; //current profile slc status, value of bthfp_connection_state_t
    UINT8 active_profile; //current profile, as HANDSET/HANDFREE/AG
    UINT8 peer_profile;  //peer device active profile HANDSET/HANDFREE/AG
    UINT8 *pending_data;//UINT8* pending_data; was used to save the AT command, so 50 bytes maybe enough. zhujianguo.
    UINT8 pending_data_len;
    UINT8 cmd_pending_index;   // handled in OK responsed by AG
    UINT8 call_state;  // state of calling, value of HFP_CALL_CLOSED/ HFP_CALL_ACTIVE
    UINT8 hfp_sdp_status; //HFP sdp status
    hfp_indicator_t indicator[7];// index of indicator refs for defined by +CIND as AG role, if role was HF, index of indicator set with AT+CIND from AG device;

    UINT8 slc_timer; //timer of slc connection timeout
    UINT8 ring_timer; //timer of ring timeout
    UINT8 at_cmd_timer; //timer for hfp init cmd, start after RFCOMM connected
    UINT8 phone_num[MAX_PHONE_NUMBER]; //phone number of dialing with HF request
    UINT8 num_length;
    UINT8 flags; //status of CIEV/CLIP/CCWA was enable or not

    UINT8 current_pbap; //type of pbab£¬value of PhoneBook_Type
    UINT8 current_pbap_which; //source of phone book£¬value of PhoneBook_Place
    UINT8 set_code; // pbap opcode£¬ 1: get code, 2: set code; 3: quiry code

    UINT16 ver_info; //version of hfp profile
    UINT16 peer_ver_info; //peer device version of hfp profile
    UINT8 scm_state;   // Current State of the Service link to the peer
    UINT8 srv_ch; // server channel used to save the RFCOMM
    UINT8 hfp_port[4]; //save rfcomm port of hfp profile,  HS/HS_AG/HF/HF_AG
    UINT8 rf_role; //role of rfcomm, 2:rfcomm client, 3: rfcomm server
    UINT16 dlci;  //indication of data link, 0: transmit control info !0: transmit data
    UINT8 *rx_buf; //buffer of rfcomm received, malloc/free with open/close BT, buffer length is 148£¬replace variable of AssemblyFrameData
    UINT32 rx_buf_len; //length of rfcomm received, used to packed/unpacked, replace variable of AssemblyFrameDatalen
    BOOL is_assemble;//flag of assembly frame of not, replace variable of AssemblyFrame
} bt_hfp_t;

typedef struct
{
/**
* @brief slc_connection_cb
* @details Hfp connected callback function.
* @param [out] bt_conn_t * All device info and profile info in this point
* @return None
*/
    void (*slc_connection_cb)(bt_conn_t *);
/**
* @brief slc_disconnection_cb
* @details Hfp disconnected callback function.
* @param [out] bt_conn_t * All device info and profile info in this point
* @return None
*/
    void (*slc_disconnection_cb)(bt_conn_t *);
/**
* @brief ciev_cb
* @details CIEV state callback function, used to HS&HF process CIEV msg of AG.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] hfp_cind_t ind Hfp CIEV type indicator
* @param [out] UINT32 value The value of indicator
* @return None
*/
    void (*ciev_cb)(bt_conn_t *, hfp_cind_t ind, UINT32 value);
/**
* @brief ring_cb
* @details Ring callback function, used to HS&HF notified app layer call in.
* @param [out] bt_conn_t * All device info and profile info in this point
* @return None
*/
    void (*ring_cb)(bt_conn_t *);
/**
* @brief sco_connection_cb
* @details Hfp sco connected callback function.
* @param [out] bt_conn_t * All device info and profile info in this point
* @return None
*/
    void (*sco_connection_cb)(bt_conn_t *);
/**
* @brief sco_disconnection_cb
* @details Hfp sco disconnected callback function.
* @param [out] bt_conn_t * All device info and profile info in this point
* @return None
*/
    void (*sco_disconnection_cb)(bt_conn_t *);
/**
* @brief sco_disconnection_cb
* @details Call numer notify callback, used to HS&HF notified app layer call number.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] UINT8 *buff Call number in this buff
* @param [out] UINT32 type The type of call number
* @return None
*/
    void (*call_identification_cb)(bt_conn_t *, UINT8 *buff, UINT32 type);
/**
* @brief current_call_list_cb
* @details call back for clcc cmd.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] bt_hfp_role_t role - BT_HFP_ROLE_AG/BT_HFP_ROLE_HF
* @param [out] UINT8 idx - index
* @param [out] UINT8 dir - direct
* @param [out] UINT8 status - status
* @param [out] UINT8 mode - mode
* @param [out] UINT8 mpty - multi-party
* @param [out] UINT8 *numstr Call number in this numstr
* @param [out] UINT8 type The type of call number
* @return None
*/
    void (*current_call_list_cb)(bt_conn_t *, bt_hfp_role_t role, UINT8 idx, UINT8 dir, UINT8 status, UINT8 mode, UINT8 mpty, UINT8 *numstr, UINT8 type);

/**
* @brief sco_disconnection_cb
* @details Hfp volume adjust callback function.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] bthfp_volume_type_t type Hfp volume type
* @param [out] UINT32 volume The value of volume
* @return None
*/
    void (*volume_cb)(bt_conn_t *, bthfp_volume_type_t type, UINT32 volume);
/**
* @brief subscriber_cb
* @details Pbap callback function, used to HS&HF notified app layer with pbap update.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] UINT8 *buff Subscriber value
* @param [out] UINT32 len The length of subscriber
* @return None
*/
    void (*subscriber_cb)(bt_conn_t *, UINT8 *buff, UINT32 len);
/**
* @brief at_cb
* @details Hfp custmom AT callback function.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] UINT8 *buff AT command handle value
* @param [out] UINT32 len The length of AT command handle
* @return None
*/
    void (*at_cb)(bt_conn_t *, UINT8 *buff, UINT32 len);
/**
* @brief dial_cb
* @details Dial callback function, used to AG dial phone number, if call_num is NULL, then redial last call record.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] bt_call_type_t type Call type
* @param [out] UINT8 *call_num Call number, if it is NULL, redial last call record
* @return None
*/
    void (*dial_cb)(bt_conn_t *, bt_call_type_t type, UINT8 *call_num);
/**
* @brief call_status_cb
* @details Call state change callback function, used to AG notified app layer call state, like answer/reject/release with HS&HF.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] bt_call_type_t type Call type
* @return None
*/
    void (*call_status_cb)(bt_conn_t *, bt_call_type_t type);
/**
* @brief error_cb
* @details Internel error callback, err_type value of bt_status_t, err_level value of bt_error_level_t.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] UINT32 err_type The error type
* @param [out] UINT8 err_level The error level
* @return None
*/
    void (*error_cb)(bt_conn_t *, UINT32 err_type, UINT8 err_level);   /**< internel error callback, err_type value of bt_status_t, err_level value of bt_error_level_t*/
}bt_hfp_cb_t;

typedef struct
{
/**
* @brief init
* @details Init hfp callback function when start function of hfp profile by start demo or send at command.
* @param [in] bt_hfp_cb_t *cb hfp callback function
* @return None
*/
    bt_status_t (*init)(bt_hfp_cb_t *);
/**
* @brief set_role
* @details Set role hands free or audio gate when bt is start by different at command.
* @param [in] bt_hfp_role_t role hands free or audio gate
* @return None
*/
    bt_status_t (*set_role)(bt_hfp_role_t role);
/**
* @brief  connect
* @details Requesition of connecting hfp profile. Always in phone demo or AT command.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @param [in] UINT32 timeout Timeout max should less than 5s
* @param [in] void(*timeout_cb)(bt_conn_t *) Register the callback of timeout
* @return none
*/
    bt_status_t (*connect)(bdaddr_t *, UINT32 timeout, void(*)(bt_conn_t *));
/**
* @brief  disconnect
* @details Requesition of disconnecting hfp profile. Always in phone demo or AT command.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @return none
*/
    bt_status_t (*disconnect)(bt_conn_t *);
/**
* @brief  audio_switch
* @details Hfp switch audio function, used to SCO connect.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @param [in] isOn - true: for creat sco/esco connection;
* @param           - false: for disconnect sco/esco connection;
* @return none
*/
    bt_status_t (*audio_switch)(bt_conn_t *, BOOL isOn);
/**
* @brief  call_answer
* @details Hfp call control function, used to HS&HF, and cmd is answer/reject/release.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @param [in] UINT32 cmd Call answer command, including answer/reject/release and multi call
* @param [in] UINT8 chld_val Param of multi call
* @return none
*/
    bt_status_t (*call_answer)(bt_conn_t *, UINT32 cmd, UINT8 chld_val);
/**
* @brief  call_answer
* @details Hfp call dail number, used to HS&HF.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @param [in] INT8 *call_num Phone number, if it is NULL, means call redial last number
* @return none
*/
    bt_status_t (*call_outgoing)(bt_conn_t *, INT8 *call_num);
/**
* @brief  adjust_volume
* @details Hfp adjust volume with mic and speaker of AG and HS&HF.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @param [in] UINT8 type Type of volume, enum is bthfp_volume_type_t, mic or spearker
* @param [in] UINT8 vol Volume of hfp max value is 15
* @return none
*/
    bt_status_t (*adjust_volume)(bt_conn_t *, UINT8 type, UINT8 vol);
/**
* @brief  send_raw_data
* @details Hfp send raw data with customization function.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @param [in] UINT8 *buff Raw data
* @param [in] UINT32 len Data length
* @return none
*/
    bt_status_t (*send_raw_data)(bt_conn_t *, UINT8 *buff, UINT32 len);
/**
* @brief  send_ciev
* @details Hfp send CIEV msg to HS&HF function.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @param [in] hfp_cind_t ind Indicator of different CIND
* @param [in] UINT8 value Value of indicator
* @return none
*/
    bt_status_t (*send_ciev)(bt_conn_t *, hfp_cind_t ind, UINT8 value);
/**
* @brief  send_ring
* @details Start ring when receive incoming call.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @return none
*/
    bt_status_t (*send_ring)(bt_conn_t *);
/**
* @brief  set voice recognition
* @details Hfp recognition enable or disable.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @param [in] UINT8 status Enable or disable
* @return none
*/
    bt_status_t (*set_voice_recognition)(bt_conn_t *, UINT8 status);
/**
* @brief  audio disconnect
* @details Disconnect sco connection in hfp.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @return none
*/
    bt_status_t (*audio_disconnect)(bt_conn_t *);
/*
* @brief  send_ok
* @details send OK to HF.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @return none
*/
    bt_status_t (*send_ok)(bt_conn_t *);
/**
* @brief  send_error
* @details send error to HF..
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @return none
*/
    bt_status_t (*send_error)(bt_conn_t *);
/**
* @brief  close
* @details Hfp close all connection and uninit hfp stack.
* @param [in] bt_conn_t *p_conn All device info and hfp info in this point
* @return none
*/
    bt_status_t (*close)(bt_conn_t *);
} bt_hfp_if_t;

/**
* @brief   bthfp_get_interface
* @details get hfp interface
* @param void
* @return bt_hfp_if_t structure of HFP interface
*/
bt_hfp_if_t *bthfp_get_interface(void);

/**
* @brief   bthfp_get_callback
* @details get hfp callback
* @param void
* @return bt_hfp_cb_t structure of HFP callback
*/
bt_hfp_cb_t *bthfp_get_callback(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

/** @} */

#endif
