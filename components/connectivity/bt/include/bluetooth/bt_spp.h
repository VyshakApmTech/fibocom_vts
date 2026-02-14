/**
 * @file    bt_spp.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_SPP_H__
#define __BT_SPP_H__

/**
 * @addtogroup bluestack
 * @{
 * @defgroup spp
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

//#include "bluetooth/bt_types.h"

#define SPP_FLOW_STOP 0
#define SPP_FLOW_GO 1

typedef struct
{
    bdaddr_t addr;
    UINT8 port;
    UINT8 state;
} sppif_device_info_t;


typedef enum
{
    BTSPP_CONNECTION_STATE_DISCONNECTED = 0,
    BTSPP_CONNECTION_STATE_CONNECTING,
    BTSPP_CONNECTION_STATE_CONNECTED,
    BTSPP_CONNECTION_STATE_DISCONNECTING,
} btspp_connection_state_t;

/**spp device information structure*/
typedef struct _BT_SPP_T
{
    bdaddr_t address;                       /* device address*/
    UINT8 schan;                            /* server channel*/
    UINT8 dlci;                             /* data link connection identifier*/
    btspp_connection_state_t spp_state;     /* spp connection state*/
    UINT16 uuid16;                          /* uuid16 */
    UINT8 *uuid128;                         /* point of uuid 128*/
    UINT8 isUsed;                           /* port is used or not*/
    UINT8 isServer;                         /* device is server or not*/
    UINT16 sendCount;                       /* the count of send, judge recount or not*/
    UINT16 full_length;                     /* full send data length*/
    UINT16 full_send_count;                 /* full send data count*/
    UINT16 max_frame_size;                  /* the max frame size*/

    UINT16 profile_ver;                     /* peer device version */
    UINT16 sdp_feature;                     /* sdp feature*/
    UINT8 sdp_status;                       /* sdp status*/
    UINT8 need_connect_after_sdp;           /* need connect after sdp*/
    UINT8 sdp_rf_serverChannel;             /*sdp rf server channel*/
    UINT16 now_port;                        /* the port used now*/
} bt_spp_t;

typedef struct
{
/**
* @brief connection_state_callback
* @details The main feature of this callback function is tell app layer the connection state that spp has been connected
*              in stack and send point bt_conn_t info include spp info in stack to app layer.
* @param [out] bt_conn_t * All device info and spp info in this point
* @param [out] btspp_connection_state_t state
* @return None
*/
    void (*connection_state_callback)(bt_conn_t *, btspp_connection_state_t state);
/**
* @brief data_recv_callback
* @details The main feature of this callback function is tell app layer the data we received, the length of received data
*              and spp port.
* @param [out] UINT16 port The spp port
* @param [out] UINT8* data The data point we received
* @param [out] UINT16 length The length of received data
* @return None
*/
    void (*data_recv_callback) (UINT16 port, UINT8* data, UINT16 length);
/**
* @brief flowctrl_ind_callback
* @details The main feature of this callback function is tell app layer the flow control information in stack by result. When
*              result is 1 means not flow control.
* @param [out] UINT32 result The flow state
* @return None
*/
    void (*flowctrl_ind_callback)(UINT32 result);
/**
* @brief mtu_result_callback
* @details The main feature of this callback function is tell app layer the MTU consulted by slient and server.When app layer
*               wants to send data, it must consider that MTU value.
* @param [out] UINT16 frame_size The max frame size
* @return None
*/
    void (*mtu_result_callback)(UINT16 frame_size);
/**
* @brief resend_callback
* @details The main feature of this callback function is tell app layer the spp data send fail and the number of failed data, we
*               need app layer resend the rest spp data.
* @param [out] UINT16 spp_resend_data_length The spp data length we need to resend
* @return None
*/
    UINT8 (*resend_callback)(UINT16 spp_resend_data_length);
} bt_spp_cb_t;


typedef struct
{
/**
* @brief init
* @details Init spp callback function when start function of spp profile by start demo or send at command.
* @param [in] bt_spp_cb_t *callback Spp callback function
* @return BT_SUCCESS
* @return BT_FAIL
*/
    bt_status_t (*init) (bt_spp_cb_t *callbacks);
/**
* @brief connect
* @details Send connect request to other device by address.
* @param [in] bdaddr_t *addr The address if other device.
* @return BT_SUCCESS
* @return BT_INVALIDPARAM
* @return BT_NORESOURCES
*/
    bt_status_t (*connect) (bdaddr_t *addr);
/**
* @brief send
* @details Send spp data to other device.
* @param [in] bt_conn_t * The point of connection information.
* @param [in] UINT8* data The spp data.
* @param [in] UINT16 length The length of spp data.
* @param [in] UINT16 port The port we used to send spp data.
* @return BT_SUCCESS
* @return BT_INVALIDPARAM
*/
    bt_status_t (*send) (bt_conn_t *, UINT8* data, UINT16 length, UINT16 port);
/**
* @brief send
* @details Send spp data to other device.
* @param [in] bt_conn_t * The point of connection information.
* @param [in] UINT8* data The spp data.
* @param [in] UINT16 length The length of spp data.
* @param [in] UINT16 port The port we used to send spp data.
* @return BT_SUCCESS
* @return BT_INVALIDPARAM
*/
    bt_status_t (*disconnect) (bt_conn_t *);
} bt_spp_if_t;

/**
 * @brief get bt spp interface
 * @return bt spp interface
 */
const bt_spp_if_t *btspp_get_interface(void);
bt_spp_cb_t *btspp_get_callback(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

/** @} */

#endif /* __BT_SPP_H__*/


