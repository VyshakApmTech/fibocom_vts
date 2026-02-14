/**
 * @file    bt_a2dp.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */



#ifndef __BT_A2DP_H__
#define __BT_A2DP_H__

/**
 * @addtogroup bluestack
 * @{
 * @defgroup a2dp
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define AVDTP_STATE_SIGNAL_CONN (1 << 0)
#define AVDTP_STATE_MEDIA_CONN  (1 << 1)
#define AVDTP_STATE_REPORT_CONN (1 << 2)

#define AVDTP_MAX_SEPS              6 // count =  (2 sbc+ 1 mp3)*SRC and SNK, as (2+1)*2
#define AVDTP_MAX_SEPS_PEER         16 //peer devices was is powerful of support seps count

#define AVDTP_SRV_CAPS_MAX_SIZE     64 //caps buff max size

typedef enum
{
    A2DP_SDP_STATUS_NONE,
    A2DP_SDP_STATUS_DISCOVRING,
    A2DP_SDP_STATUS_DONE,
    A2DP_SDP_STATUS_FAIL,
}A2DP_SDP_STATUS;


/* AVDTP signals */
typedef enum
{
    A2DP_NULL = (0x00),
    A2DP_DISCOVER,
    A2DP_GET_CAPABILITIES,
    A2DP_SET_CONFIGURATION,
    A2DP_GET_CONFIGURATION,
    A2DP_RECONFIGURE,
    A2DP_OPEN,
    A2DP_START,
    A2DP_CLOSE,
    A2DP_SUSPEND,
    A2DP_ABORT,
    A2DP_SECURITY_CONTROL,
    A2DP_GET_ALL_CAPABILITIES,
    A2DP_DELAYREPORT
}a2dp_signal_id_t;

/* Stream Endpoint (SEP) states based on AVDTP Figure 6.4 */
typedef enum
{
    AVDTP_SEP_STATE_EMPTY,
    AVDTP_SEP_STATE_IDLE,
    AVDTP_SEP_STATE_WAITING_FOR_SIGNAL,
    AVDTP_SEP_STATE_DISCOVERING,
    AVDTP_SEP_STATE_READING_CAPS,
    AVDTP_SEP_STATE_PROCESSING_CAPS,
    AVDTP_SEP_STATE_CONFIGURING,
    AVDTP_SEP_STATE_CONFIGURED,
    AVDTP_SEP_STATE_LOCAL_OPENING,
    AVDTP_SEP_STATE_REMOTE_OPENING,
    AVDTP_SEP_STATE_OPEN,
    AVDTP_SEP_STATE_STREAMING,
    AVDTP_SEP_STATE_LOCAL_SUSPENDING,
    AVDTP_SEP_STATE_REMOTE_SUSPENDING,
    AVDTP_SEP_STATE_LOCAL_CLOSING,
    AVDTP_SEP_STATE_REMOTE_CLOSING,
    AVDTP_SEP_STATE_RECONFIG_READING_CAPS,
    AVDTP_SEP_STATE_RECONFIG_PROCESSING_CAPS,
    AVDTP_SEP_STATE_RECONFIGURING,
    AVDTP_SEP_STATE_RECONFIGURE_FAILURE,
    AVDTP_SEP_STATE_LOCAL_ABORTING,
    AVDTP_SEP_STATE_REMOTE_ABORTING
} sep_state_t;

typedef enum
{
    SEP_MEDIA_TYPE_AUDIO,
    SEP_MEDIA_TYPE_VIDEO,
    SEP_MEDIA_TYPE_MULTIMEDIA
}sep_media_type_t;

typedef enum
{
    SEP_CODEC_SBC = 0,
    SEP_CODEC_MPEG1_2_AUDIO = 1,
    SEP_CODEC_MPEG2_4_AAC = 2,
    SEP_CODEC_ATRAC = 3
}sep_codec_type_t;

typedef enum
{
    avdtp_bad_header_format             = (0x01),   /*!< The request packet header format error that is not specified above ERROR_CODE. */
    avdtp_bad_length                    = (0x11),   /*!< The request packet length is not match the assumed length. */
    avdtp_bad_acp_seid                  = (0x12),   /*!< The requested command indicates an invalid ACP SEID (not addressable). */
    avdtp_sep_in_use                    = (0x13),   /*!< The SEP is in use. */
    avdtp_sep_not_in_use                = (0x14),   /*!< The SEP is not in use. */
    avdtp_bad_serv_category             = (0x17),   /*!< The value of Service Category in the request packet is not defined in AVDTP. */
    avdtp_bad_payload_format            = (0x18),   /*!< The requested command has an incorrect payload format (Format errors not specified in this ERROR_CODE). */
    avdtp_not_supported_command         = (0x19),   /*!< The requested command is not supported by the device. */
    avdtp_invalid_capabilities          = (0x1a),   /*!< The reconfigure command is an attempt to reconfigure a transport service capabilities of the SEP. Reconfigure is only permitted for application service capabilities. */
    avdtp_bad_recovery_type             = (0x22),   /*!< The requested Recovery Type is not defined in AVDTP. */
    avdtp_bad_media_transport_format    = (0x23),   /*!< The format of Media Transport Capability is not correct. */
    avdtp_bad_recovery_format           = (0x25),   /*!< The format of Recovery Service Capability is not correct. */
    avdtp_bad_rohc_format               = (0x26),   /*!< The format of Header Compression Service. */
    avdtp_bad_cp_format                 = (0x27),   /*!< The format of Content Protection Service Capability is not correct. */
    avdtp_bad_multiplexing_format       = (0x28),   /*!< The format of Multiplexing Service Capability is not correct. */
    avdtp_unsupported_configuration     = (0x29),   /*!< Configuration not supported. */
    avdtp_bad_state                     = (0x31)    /*!< Indicates that the ACP state machine is in an invalid state in order to process the signal.*/
} avdtp_err_code_t;


typedef struct
{
    UINT8 min_bit_pool;
    UINT8 max_bit_pool;
    UINT8 block_len; // b0: 16, b1: 12, b2: 8, b3: 4
    UINT8 subband_num; // b0: 8, b1: 4
    UINT8 alloc_method; // b0: loudness, b1: SNR
    UINT8 sample_rate; // b0: 48000, b1: 44100, b2: 32000, b3: 16000
    UINT8 channel_mode; // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
} avdtp_sbc_caps_t;

typedef struct
{
    UINT8 layer; // b0: layerIII, b1: layerII, b0: layerI
    UINT8 CRC;
    UINT8 channel_mode; // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
    UINT8 MPF; // is support MPF-2
    UINT8 sample_rate; // b0: 48000, b1: 44100, b2: 32000, b3: 24000, b4: 22050, b5: 16000
    UINT8 VBR; // is support VBR
    UINT16 bit_rate; // bit-rate index in ISO 11172-3 , b0:0000 ~ b14: 1110
} avdtp_mp3_caps_t; /* all MPEG-1,2 Audio */

typedef struct
{
    UINT8 object_type; // b4: M4-scalable, b5: M4-LTP, b6: M4-LC, b7: M2-LC
    UINT16 sample_rate; // b0~b11: 96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000
    UINT8 channels; // b0: 2, b1: 1
    UINT8 VBR; // is supported VBR
    UINT32 bit_rate; // constant/peak bits per second in 23 bit UiMsbf, 0:unknown
} avdtp_aac_caps_t; /* all MPEG-2,4 AAC */

typedef struct
{
    UINT8 version; // 1:ATRAC, 2:ATRAC2, 3:ATRAC3
    UINT8 channel_mode; // b0: joint stereo, b1: dual, b2: single
    UINT8 sample_rate; // b0: 48000, b1: 44100
    UINT8 VBR; // is supported VBR
    UINT32 bit_rate; // bit-rate index in ATRAC, b0: 0x0012 ~ b18: 0x0000
    UINT16 max_sul; // sound unit length in 16 bits UiMsbf
} avdtp_atrac_caps_t; /* all ATRAC family */

typedef struct
{
    UINT8   codec_type;
    UINT8   content_protection;
    UINT8   acp_seid;//active acp seid
    UINT8   int_seid;//active int seid
    union
    {
        avdtp_sbc_caps_t sbc;
        avdtp_mp3_caps_t mp3;
        avdtp_aac_caps_t aac;
        avdtp_atrac_caps_t atrac;
    }codec;
}avdtp_sep_cfg_t;

typedef struct
{
    UINT8 state;///< sep_state_t
    UINT8 seid; ///< number of seps list, begin with 1
    UINT8 in_use; ///< 0: unused, 1:used
    UINT8 role;  ///< 0:SRC 1:SINK
    UINT8 c_p; ///< content protection

    UINT8 media_type; ///< value of sep_media_type_t
    UINT8 codec_type; ///< sep_codec_type_t
    UINT16 service_caps_size; ///< actual size of service_caps
    UINT8 service_caps[AVDTP_SRV_CAPS_MAX_SIZE];  //all service of caps, such as media transport/media codec/content protection ...
} avdtp_sep_t;

typedef struct
{
    UINT8   state;  ///< connection state, bit0: signal channel,  bit1:media channel
    UINT8   sep_state; ///< status of SEP, type of sep_state_t
    UINT8   signal_label; ///< label of signal data, used to transmit response

    avdtp_sep_t *p_sep; ///< local sep information
    avdtp_sep_t *p_peer_sep; ///< peer device sep information
    UINT8   peer_sep_count; ///< count of peer device sep
    UINT8   sep_count; ///< count of local device sep

    UINT16     signal_cid;///< channel id of signal, used to send signal data
    UINT16     media_cid;///< channel id of media, used to send media data
    UINT16     report_cid;///< channel id of report, used to send report data
    avdtp_sep_cfg_t sep_cfg; ///< configure of sep
} bt_avdtp_t;

typedef struct _BT_A2DP_T
{
    bt_avdtp_t avdtp; ///< data structure of avdtp
    UINT32 a2dp_check_timer; ///< not used, for reserved
    UINT16 profile_ver; ///< a2dp profile version
    UINT8 role; ///< type of bt_a2dp_role_t
    UINT16 sdp_feature; ///< record the a2dp feature of SDP response
    UINT8 sdp_status; ///< indicate the sdp process is finished or not
} bt_a2dp_t;

typedef struct
{
    UINT8 rtp_ver; ///< version of RTP
    UINT8 padding; ///< padding data, default value is 0
    UINT8 header_ext; ///< if the extension bit is set, the fixed header is follwed by exactly one header extension, default is 0
    UINT8 csrc_count;///< contains the number of CSRC identifiers that follow the fixed header
    UINT8 marker; ///< the interpretation of the marker is defined by a profile, default is 0
    UINT8 payload_type;///< type of RTP payload, default is 0x60 means audio, refs for RFC3551 standard
    UINT16 seq_num; ///< sequence number increments by one for each media packet sent
    UINT32 time_stamp; ///< reflects the sampling instant of the first octet in the media packet
    UINT32 sync_src; ///< identifies the synchronization source
    UINT8 pkt_header;//bit0~bit3:frame count; bit5:is last frame; bit6:is start frame; bit7: is fragmented
    UINT16 pkt_len; ///< media data length
    UINT8 *pkt; ///< media data memory
}avdtp_media_data_t;


/**a2dp callback function structure*/
typedef struct
{
/**
* @brief a2dp_signaling_connction_cb
* @details When signaling connection is established in stack, this callback function will be called to send connection message to app.
* @param [out] bt_conn_t * All device info and a2dp info in this point
* @return None
*/
    void (*a2dp_signaling_connction_cb)(bt_conn_t *);
/**
* @brief a2dp_signaling_disconnection_cb
* @details When signaling connection is disconnected in stack, this callback function will be called to send disconnection message to app.
* @param [out] bt_conn_t * All device info and a2dp info in this point
* @return None
*/
    void (*a2dp_signaling_disconnection_cb)(bt_conn_t *);
/**
* @brief a2dp_media_connction_cb
* @details When media connection is established in stack, this callback function will be called to send connection message to app.
* @param [out] bt_conn_t * All device info and a2dp info in this point
* @return None
*/
    void (*a2dp_media_connction_cb)(bt_conn_t *);
/**
* @brief a2dp_signaling_disconnection_cb
* @details When media connection is disconnected in stack, this callback function will be called to send disconnection message to app.
* @param [out] bt_conn_t * All device info and a2dp info in this point
* @return None
*/
    void (*a2dp_media_disconnection_cb)(bt_conn_t *);
/**
* @brief a2dp_media_data_cb
* @details When our role is sink and we received media data from source, this callback will be called to send madia data to app.
* @param [out] bt_conn_t * All device info and a2dp info in this point
* @param [out] avdtp_media_data_t *media The information of media data
* @return None
*/
    void (*a2dp_media_data_cb)(bt_conn_t *, avdtp_media_data_t *media);
/**
* @brief a2dp_signaling_cb
* @details When we received signal id from other device, this callback will be called to send signal id to app.
* @param [out] bt_conn_t * All device info and a2dp info in this point
* @param [out] a2dp_signal_id_t signal_id The signal id, different id represent play status command of media
* @return None
*/
    void (*a2dp_signaling_cb)(bt_conn_t *, a2dp_signal_id_t signal_id);
/**
* @brief error_cb
* @details When we received error message from other device, this callback will be called to send error info to app.
* @param [out] bt_conn_t * All device info and a2dp info in this point
* @param [out] avdtp_err_code_t err_type Error type
* @param [out] UINT8 err_level Error level
* @return None
*/
    void (*error_cb)(bt_conn_t *, avdtp_err_code_t err_type, UINT8 err_level);   /**< internel error callback, err_type value of avdtp_err_code_t, err_level value of bt_error_level_t*/
} bt_a2dp_cb_t;

/**a2dp interface function structure */
typedef struct
{
/**
* @brief init
* @details Init a2dp callback function when start function of a2dp profile by start demo or send at command.
* @param [in] bt_a2dp_cb_t *cb A2dp callback function
* @return None
*/
    bt_status_t (*init)(bt_a2dp_cb_t *);
/**
* @brief set_role
* @details Set role source or sink when bt is start by different at command.
* @param [in] bt_a2dp_role_t role source or sink
* @return None
*/
    bt_status_t (*set_role)(bt_a2dp_role_t role);
/**
* @brief  connect
* @details Requesition of connecting a2dp profile. Always in phone demo or AT command.This function is used to connect signal connect.
* @param [in] bt_conn_t *p_conn All device info and a2dp info in this point
* @return none
*/
    bt_status_t (*connect)(bt_conn_t *);
/**
* @brief  disconnect
* @details Requesition of disconnecting a2dp profile. Always in phone demo or AT command.
* @param [in] bt_conn_t *p_conn All device info and a2dp info in this point
* @return none
*/
    bt_status_t (*disconnect)(bt_conn_t *);
/**
* @brief  a2dp_send_media_data
* @details when our role is source, this function is used to start sending media data to sink.
* @param [in] bt_conn_t *p_conn All device info and a2dp info in this point
* @param [in] avdtp_media_data_t * The information of media data, and before it is send, app will write media data information
* @return none
*/
    bt_status_t (*a2dp_send_media_data)(bt_conn_t *, avdtp_media_data_t *);
/**
* @brief  a2dp_send_signaling
* @details when our role is source, this function is used to start sending signal id to sink to control play status of media.When app
*              send signal id a2dp_open, the media connect req will send to sink in stack, then source can send media data.
* @param [in] bt_conn_t *p_conn All device info and a2dp info in this point
* @param [in] a2dp_signal_id_t signal_id The signal id, different id represent play status command of media
* @return none
*/
    bt_status_t (*a2dp_send_signaling)(bt_conn_t *, a2dp_signal_id_t signal_id);
/**
* @brief  cleanup
* @details When we disconnect a2dp profile or close bt, ues this function to free memory.
* @param [in] bt_conn_t *p_conn All device info and a2dp info in this point
* @return none
*/
    bt_status_t (*cleanup) (bt_conn_t *);
} bt_a2dp_if_t;

bt_a2dp_if_t *bta2dp_get_interface(void);
bt_a2dp_cb_t *bta2dp_get_callback(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

/** @} */


#endif
