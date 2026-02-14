/**
 * @file     bt_sdp.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020.10.10
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef __BT_SDP_H__
#define __BT_SDP_H__

/**
 * @addtogroup bluestack
 * @{
 * @defgroup sdp
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


typedef struct
{
/**
* @brief srv_discover_cb
* @details service discover callback.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] properties  type of bt_property_t, service detail info with properties->val, it should be force convert to bt_service_record.
* @return None
*/
    void (*srv_discover_cb)(bt_conn_t *p_conn, bt_property_t *properties);

/**
* @brief error_cb
* @details Internel error callback, err_type value of bt_status_t, err_level value of bt_error_level_t.
* @param [out] bt_conn_t * All device info and profile info in this point
* @param [out] UINT32 err_type The error type
* @param [out] UINT8 err_level The error level
* @return None
*/
    void (*sdp_err_cb)(bt_conn_t *, UINT32 err_type, UINT8 err_level);   /**< internel error callback, err_type value of bt_status_t, err_level value of bt_error_level_t*/
} bt_sdp_cb_t;

typedef struct
{
/**
* @brief init
* @details init the sdp callback structure
* @param [in] callbacks callback structure
* @return bt_status_t
*/
    bt_status_t (*init) (bt_sdp_cb_t *callbacks);

/**
* @brief get_srv_by_uuid
* @details discover the service with UUID, the result will reture by srv_discover_cb
* @param [in] bt_conn_t * All device info and profile info in this point
* @param [in] uuid uuid of service,  such as below list
*                       avrcp target: 0x110C
*                       avrcp controller: 0x110F
*                       hfp hf: 0x111E
*                       hfp ag: 0x111F
*                       avdtp source: 0x110A
*                       avdtp sink: 0x110B
*                       serial port: 0x1101
* @return bt_status_t
*/
    bt_status_t (*get_srv_by_uuid)(bt_conn_t *p_conn, UINT32 uuid);

/**
* @brief get_srv_list
* @details get all class of service, the result will reture by srv_discover_cb
* @param [in] bt_conn_t * All device info and profile info in this point
* @return bt_status_t
*/
    bt_status_t (*get_srv_list)(bt_conn_t *p_conn);
} bt_sdp_if_t;

bt_sdp_if_t *btsdp_get_interface(void);
bt_sdp_cb_t *btsdp_get_callback(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

/** @} */
#endif /* __BT_SDP_H__ */
