/**
 * @file    bt_avrcp.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_AVRCP_H__
#define __BT_AVRCP_H__

/**
 * @addtogroup bluestack
 * @{
 * @defgroup avrcp
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/** Avrcp connection state enumeration*/
typedef enum
{
    BTAVRCP_CONNECTION_STATE_DISCONNECTED = 0,       /**< Avrcp connection is disconnected */
    BTAVRCP_CONNECTION_STATE_CONNECTING,             /**< Avrcp connection is connecting */
    BTAVRCP_CONNECTION_STATE_CONNECTED,              /**< Avrcp connection is connected */
    BTAVRCP_CONNECTION_STATE_DISCONNECTING,          /**< Avrcp connection is disconnecting */
} btavrcp_connection_state_t;


typedef enum
{
    BTAVRCP_SDP_STATE_SUCCESS = 0,
    BTAVRCP_SDP_STATE_FAILED = 1,
} btavrcp_sdp_state_t;

/** Avrcp play status enumeration*/
typedef enum
{
    AVRCP_PLAYBACK_STATUS_STOPPED,                /**< The play is stopped */
    AVRCP_PLAYBACK_STATUS_PLAYING,                /**< The play is playing */
    AVRCP_PLAYBACK_STATUS_PAUSED,                 /**< The play is paused */
    AVRCP_PLAYBACK_STATUS_FWD_SEEK,               /**< The play is seeking forward */
    AVRCP_PLAYBACK_STATUS_REV_SEEK,               /**< The play is seeking rewind */
    AVRCP_PLAYBACK_STATUS_ERROR = 0xff            /**< Error */
} avrcp_playback_status_t;

/** Avrcp event enumeration*/
typedef enum
{
    AVRCP_EVENT_PLAYBACK_STATUS_CHANGED = (0x01),                   /**< The playback status changed event */
    AVRCP_EVENT_TRACK_CHANGED = (0x02),                             /**< The track changed event */
    AVRCP_EVENT_TRACK_REACHED_END = (0x03),                         /**< The track reaches end event */
    AVRCP_EVENT_TRACK_REACHED_START = (0x04),                       /**< The track reaches start event */
    AVRCP_EVENT_PLAYBACK_POS_CHANGED = (0x05),                      /**< The palyback position changed event */
    AVRCP_EVENT_BATT_STATUS_CHANGED = (0x06),                       /**< The battery status changed event */
    AVRCP_EVENT_SYSTEM_STATUS_CHANGED = (0x07),                     /**< The system status changed event */
    AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = (0x08),        /**< The player application setting changed event */
    AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED = (0x09),               /**< The now playing content changed event */
    AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED = (0x0A),                 /**< The available players changed event */
    AVRCP_EVENT_ADDRESSED_PLAYERS_CHANGED = (0x0B),                 /**< The addressed players changed event */
    AVRCP_EVENT_UIDS_CHANGED = (0x0C),                              /**< The UIDs changed event */
    AVRCP_EVENT_VOLUME_CHANGED = (0x0D),                            /**< The volume changed event */
} avrcp_event_t;


typedef enum
{
    AVRCP_CONN_PSM,
    AVRCP_CONN_PSM_BROWSE,
}bt_avrcp_conn_type_t;

/** AVRCP opcode used in app to parse avrcp command*/
typedef enum
{
    OPID_SELECT             = (0x0),
    OPID_UP,
    OPID_DOWN,
    OPID_LEFT,
    OPID_RIGHT,
    OPID_RIGHT_UP,
    OPID_RIGHT_DOWN,
    OPID_LEFT_UP,
    OPID_LEFT_DOWN,
    OPID_ROOT_MENU,
    OPID_SETUP_MENU,
    OPID_CONTENTS_MENU,
    OPID_FAVOURITE_MENU,
    OPID_EXIT,
    /* 0x0e to 0x1f Reserved */
    OPID_0                  = (0x20),
    OPID_1,
    OPID_2,
    OPID_3,
    OPID_4,
    OPID_5,
    OPID_6,
    OPID_7,
    OPID_8,
    OPID_9,
    OPID_DOT,
    OPID_ENTER,
    OPID_CLEAR,
    /* 0x2d - 0x2f Reserved */
    OPID_CHANNEL_UP         = (0x30),
    OPID_CHANNEL_DOWN,
    OPID_SOUND_SELECT,
    OPID_INPUT_SELECT,
    OPID_DISPLAY_INFORMATION,
    OPID_HELP,
    OPID_PAGE_UP,
    OPID_PAGE_DOWN,
    /* 0x39 - 0x3f Reserved */
    OPID_POWER              = (0x40),
    OPID_VOLUME_UP,
    OPID_VOLUME_DOWN,
    OPID_MUTE,
    OPID_PLAY,
    OPID_STOP,
    OPID_PAUSE,
    OPID_RECORD,
    OPID_REWIND,
    OPID_FAST_FORWARD,
    OPID_EJECT,
    OPID_FORWARD,
    OPID_BACKWARD,
    /* 0x4d - 0x4f Reserved */
    OPID_ANGLE              = (0x50),
    OPID_SUBPICTURE,
    /* 0x52 - 0x70 Reserved */
    OPID_F1                 = (0x71),
    OPID_F2,
    OPID_F3,
    OPID_F4,
    OPID_F5,
    OPID_VENDOR_UNIQUE      = (0x7e)
                              /* Ox7f Reserved */
} avrcp_opcode_id_t;
typedef enum
{
    AVC_PASSTHROUGH_BUTTON_PUSHED = 0,
    AVC_PASSTHROUGH_BUTTON_RELEASED = 0x01,
} avrcp_passthrough_button_status_t;

typedef enum
{
    //for AVC
    AVRCP_PDU_GETCAPABILITIES                                     = 0X10,
    AVRCP_PDU_LISTPLAYERAPPLICATIONSETTINGATTRIBUTES              = 0X11,
    AVRCP_PDU_LISTPLAYERAPPLICATIONSETTINGVALUES                  = 0X12,
    AVRCP_PDU_GETCURRENTPLAYERAPPLICATIONSETTINGVALUE             = 0X13,
    AVRCP_PDU_SETPLAYERAPPLICATIONSETTINGVALUE                    = 0X14,
    AVRCP_PDU_GETPLAYERAPPLICATIONSETTINGATTRIBUTETEXT            = 0X15,
    AVRCP_PDU_GETPLAYERAPPLICATIONSETTINGVALUETEXT                = 0X16,
    AVRCP_PDU_INFORMDISPLAYABLECHARACTERSET                       = 0X17,
    AVRCP_PDU_INFORMBATTERYSTATUSOFCT                             = 0X18,
    AVRCP_PDU_GETELEMENTATTRIBUTES                                = 0X20,
    AVRCP_PDU_GETPLAYSTATUS                                       = 0X30,
    AVRCP_PDU_REGISTERNOTIFICATION                                = 0X31,
    AVRCP_PDU_REQUESTCONTINUINGRESPONSE                           = 0X40,
    AVRCP_PDU_ABORTCONTINUINGRESPONSE                             = 0X41,
    AVRCP_PDU_SETABSOLUTEVOLUME                                   = 0X50,
    AVRCP_PDU_SETADDRESSEDPLAYER                                  = 0x60,

    AVRCP_PDU_SETBROWSEDPLAYER                                    = 0X70,//for browser
    AVRCP_PDU_GETFOLDERITEMS                                      = 0X71,//for browser
    AVRCP_PDU_CHANGEPATH                                          = 0X72,//for browser
    AVRCP_PDU_GETITEMATTRIBUTES                                   = 0X73,//for browser
    AVRCP_PDU_PLAYITEM                                            = 0X74,//for AVC
    AVRCP_PDU_GETTOTALNUMOFITEMS                                  = 0X75,//for browser
    AVRCP_PDU_SEARCH                                              = 0X80,//for browser
    AVRCP_PDU_ADDTONOWPLAY                                        = 0X90,//for AVC
    AVRCP_PDU_GENERALREJECT                                       = 0Xa0,//for browser
} avrcp_pdu_id_t;

/**
* @brief  app_bt_avrcp_support_sonfig
* @details Set avrcp support config. The first param in cfg is support event id.Event id is UINT16, 13 bits from left to
*              right is useful, if the first bit in right is 1, means we only support AVRCP_EVENT_PLAYBACK_STATUS_CHA
*              NGED(0x01), the event id is 0001, our default support is 0x1013, means 0001 0000 0001 0011,so we support
*              0x01,0x02,0x05 and 0x13, as this way, other bit also use this role to define.
*              AVRCP_EVENT_PLAYBACK_STATUS_CHANGED = (0x01),
*              AVRCP_EVENT_TRACK_CHANGED = (0x02),
*              AVRCP_EVENT_TRACK_REACHED_END = (0x03),
*              AVRCP_EVENT_TRACK_REACHED_START = (0x04),
*              AVRCP_EVENT_PLAYBACK_POS_CHANGED = (0x05),
*              AVRCP_EVENT_BATT_STATUS_CHANGED = (0x06),
*              AVRCP_EVENT_SYSTEM_STATUS_CHANGED = (0x07),
*              AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = (0x08),
*              AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED = (0x09),
*              AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED = (0x0A),
*              AVRCP_EVENT_ADDRESSED_PLAYERS_CHANGED = (0x0B),
*              AVRCP_EVENT_UIDS_CHANGED = (0x0C),
*              AVRCP_EVENT_VOLUME_CHANGED = (0x0D)
*              The second param is support absolute volume, if this param is true, means we support volume change notify
*              and absolute volume change when our role is phone.
* @param [in] void
* @return none
*/
typedef struct
{
    UINT16 event_id;
    UINT8  support_absoulte_vol;
}avrcp_config_t;

typedef struct
{
    UINT16 param_len;
    UINT8 capability_count;;
    UINT8 *avrcp_support_event;
}bt_support_event_t;

typedef struct
{
    UINT32 opcode;
    UINT8 *buff;
    UINT32 len;
}bt_avrcp_data_t;

typedef struct
{
    UINT8 state_flag;           //push or release
    UINT8 operation_id;         //operation id
    UINT8 operation_data_len;   //operation data length
}bt_passthrough_data_t;

typedef struct
{
    UINT8 company_id[3];        // value of Bluetooth SIG,Inc {0x00,0x19,0x58}
    UINT8 pdu_id;               //value of t_AVRCP_PDU_ID
    UINT8 pack_type;            //is fragmented
    UINT16 param_len;           //parameters length
    UINT8 *param;               //parameters
}bt_vendor_data_t;

typedef struct _BT_AVRCP_T
{
    avrcp_playback_status_t cur_play_status;    /**< Current play status */
    UINT32 cur_song_pos;                        /**< Current song position */
    UINT8 play_volume;                          /**< Current play volume */
    UINT16 cid;                                 /**< Avrcp channel id */
    UINT8 status;                               /**< value of btavrcp_connection_state_t */
    UINT8 role;                                 /**< role of avrcp, value of bt_avrcp_role_t */
    UINT16 browse_cid;                          /**< Avrcp browser channel id */
    UINT8 browsing_status;                      /**< value of btavrcp_connection_state_t of browsing status */
    UINT8 transaction_label;                    /**< 0x01-0x0f one command has special transcation label */
    UINT8 numberofevents;                       /**< Number of events */
    UINT8 Registerd_numberofevents;             /**< Registered number of events */
    UINT8 supported_events_id[AVRCP_EVENT_VOLUME_CHANGED];  /**< Supported events id */
    UINT8 numberofattribures;                   /**< Number of attributes */
    UINT8 attribures_index;                     /**< Attributes index */
    UINT8 supported_attributes_type[4];         /**< Supported attributes type */
    UINT8 supported_attributes_value_type[4][4];/**< Supported attributes value type */
    UINT8 UID_Counter;                          /**< UID counter */
    UINT8 NumberofItems;                        /**< Number of items */
    UINT8 folder_depth;                         /**< Folder depth */
    UINT8 avrcp_adp_state;                      /**< Avrcp adp state */
    UINT8 volume_changed_event_support;         /**< Volume changed event support, if it is true, headset send volume change response to phone*/
    UINT8 play_back_change_support;             /**< Playback change support, if it is true, TG send playback response to ct */

    UINT16 ver_info;                            /**< local device version */
    UINT16 peer_ver_info;                       /**< peer device version */
    UINT16 local_feature;                       /**< local device sdp feature */
    UINT16 peer_feature;                        /**< peer device sdp feature */
    UINT16 event_bit_val;                       /**< each bit is indicate the event id(avrcp_event_t), like bit1 mean AVRCP_EVENT_PLAYBACK_STATUS_CHANGED was support or not*/
    UINT8 sdp_status;                           /**< Sdp status */
    UINT8 my_profile;                           /**< current profile, value of Controller or Target */
    UINT8 need_connect_after_sdp;               /**< Need connect after sdp */
} bt_avrcp_t;

typedef struct
{
/**
* @brief bt_avrcp_connection_cb
* @details The main feature of this callback function is tell app layer that avrcp has been connected in stack and send point bt_conn_t info include
*              avrcp info in stack to app layer. If our role is ct , this function will be called when avrcp l2cap config confirm, if our role is tg ,this function
*              will be called when avrcp l2cap connect confirm and if two devices support browsing cheannel , both roles will send browsing channel connection
*              callback after send control channel connection.
* @param [out] bt_conn_t * All device info and avrcp info in this point
* @param [out] UINT8 conn_type Connection is control or browsing channel
* @param [out] UINT8 status The connection status
* @return None
*/
    void (*bt_avrcp_connection_cb)(bt_conn_t *, UINT8 conn_type, UINT8 status);
/**
* @brief bt_avrcp_disconnection_cb
* @details The main feature of this callback function is tell app layer that avrcp has been disconnected in stack . If our role is ct , this function will
*              be called when avrcp l2cap disconnect ind, if our role is tg ,this function will be called when avrcp l2cap disconnect cnf. if two devices
*              support browsing cheannelBoth roles will send browsing channel disconnection callback after send control channel disconnection.
* @param [out] bt_conn_t * All device info and avrcp info in this point
* @param [out] UINT8 conn_type Connection is control or browsing channel
* @return None
*/
    void (*bt_avrcp_disconnection_cb)(bt_conn_t *, UINT8 conn_type);
/**
* @brief bt_avrcp_passthrough_data_cb
* @details The main feature of this callback function is tell app layer that the avrcp passthrough data we received in stack. In app layer, we need
*              to do specific operation by different passthrough data.
* @param [out] bt_conn_t * All device info and avrcp info in this point
* @param [out] bt_passthrough_data_t * Push or release, operation id
* @return None
*/
    void (*bt_avrcp_passthrough_data_cb)(bt_conn_t *, bt_passthrough_data_t *);
/**
* @brief bt_avrcp_vendor_data_cb
* @details The main feature of this callback function is tell app layer that the avrcp vendor data we received in stack. In app layer, we need to do
*              specific operation by different vendor data. Main data is pdu id and *param, different pdu id decides specific data in *param and length.
*              App layer must register notification with different status after receive this callback.
* @param [out] bt_conn_t * All device info and avrcp info in this point
* @param [out] bt_vendor_data_t * Pdu id and *param
* @return None
*/
    void (*bt_avrcp_vendor_data_cb)(bt_conn_t *, bt_vendor_data_t *);
} bt_avrcp_cb_t;

typedef struct
{
/**
* @brief init
* @details Init avrcp callback function when start function of avrcp profile by start demo or send at command.
* @param [in] bt_avrcp_cb_t *cb Avrcp callback function
* @return None
*/
    bt_status_t (*init)(bt_avrcp_cb_t *cb);
/**
* @brief set_role
* @details Set role ct or tg when bt is start by different at command.
* @param [in] bt_avrcp_role_t role controller or target
* @return None
*/
    bt_status_t (*set_role)(bt_avrcp_role_t role);
/**
* @brief  avrcp_set_config
* @details Set config when avrcp init ,config contain support_absoulte_vol and support event_id in function using in get capability. In bluetooth spec
*             there are 14 events in UINT16 event_id, from right to left is event id ,from 0x01 to 0x14 ,and it can be set by this function.
* @param [in] avrcp_config_t *cfg Support_absoulte_vol and event_id
* @return none
*/
    bt_status_t (*set_config)(avrcp_config_t *cfg);
/**
* @brief  connect
* @details Requesition of connecting avrcp profile. Always in phone demo or AT command and it is used with connection callback function.
* @param [in] bt_conn_t *p_conn All device info and avrcp info in this point
* @param [in] UINT32 timeout Timeout max should less than 5s
* @param [in] void(*timeout_cb)(bt_conn_t *) Register the callback of timeout
* @return none
*/
    bt_status_t (*connect)(bt_conn_t *p_conn, UINT32 timeout, void(*timeout_cb)(bt_conn_t *));
/**
* @brief  disconnect
* @details Requesition of disconnecting avrcp profile. Always in phone demo or AT command and it is used with disconnection callback function.
* @param [in] bt_conn_t *p_conn All device info and avrcp info in this point
* @return none
*/
    bt_status_t (*disconnect)(bt_conn_t *p_conn);
/**
* @brief  send_notification_resp
* @details Send notification response to other device, before this function was called, we must have received notification request.
* @param [in] bt_conn_t *p_conn All device info and avrcp info in this point
* @param [in] bt_vendor_data_t *vendor_data Vendor date is packaged in app layer
* @return none
*/
    bt_status_t (*send_notification_resp)(bt_conn_t *p_conn, bt_vendor_data_t *vendor_data);
/**
* @brief  send_notification_req
* @details When our role is ct and want to register notifaction, send vendor data with this function. The vendor data need pack up in app layer.
* @param [in] bt_conn_t *p_conn All device info and avrcp info in this point
* @param [in] bt_vendor_data_t *vendor_data pdu id/param length and *param
* @return none
*/
    bt_status_t (*send_notification_req)(bt_conn_t *p_conn, bt_vendor_data_t *vendor_data);
/**
* @brief  send_passthrough
* @details When our role is ct and want to send passthrough req, send passthrough data with this function. The passthrough data need pack up
*              in app layer. Always in headset demo or AT command.
* @param [in] bt_conn_t *p_conn All device info and avrcp info in this point
* @param [in] bt_passthrough_data_t *passthrough_data Push/release, operation id
* @return none
*/
    bt_status_t (*send_passthrough)(bt_conn_t *p_conn, bt_passthrough_data_t *passthrough_data);
/**
* @brief  set_absolute_volume
* @details When our role is tg, we use this function to set absolute volume. Before this function was called, we must have supported
*              absolute volume.
* @param [in] bt_conn_t *p_conn All device info and avrcp info in this point
* @param [in] volume The value of volume
* @return none
*/
    bt_status_t (*set_absolute_volume)(bt_conn_t *p_conn, UINT8 volume);
/**
* @brief  close
* @details When we disconnect avrcp profile or close bt, ues this function to free memory.
* @param [in] bt_conn_t *p_conn All device info and avrcp info in this point
* @param [in] volume The value of volume
* @return none
*/
    bt_status_t (*close)(bt_conn_t *p_conn);
} bt_avrcp_if_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

/** @} */
bt_avrcp_if_t *btavrcp_get_interface(void);
bt_avrcp_cb_t *btavrcp_get_callback(void);

#endif

