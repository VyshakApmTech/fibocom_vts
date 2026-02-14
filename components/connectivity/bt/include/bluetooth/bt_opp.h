/**
 * @file    bt_opp.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */



#ifndef __BT_OPP_H__
#define __BT_OPP_H__

/**
 * @addtogroup bluestack
 * @{
 * @defgroup obex
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef enum
{
    OPP_PUT_FIRST_DATA,
    OPP_PUT_MIDDLE_DATA,
    OPP_PUT_END_DATA,
    OPP_NONE,
}OPP_FLAG;

typedef struct
{
    void (*opp_connected_cb)(bt_conn_t *,bt_status_t status,UINT16 max_lenght);
    void (*opp_disconnect_cb)(bt_conn_t *,bt_status_t status);
    void (*opp_put_cb)(bt_conn_t *,bt_status_t status,UINT32 remand_length);
    void (*opp_header_cb)(bt_conn_t *,UINT8 oprescode,UINT8 header,UINT8 *data,UINT16 data_lenght);
    void (*error_cb)(bt_conn_t *, bt_status_t err_type, UINT8 err_level);
} bt_opp_cb_t;

typedef struct
{
    bt_status_t (*init)(bt_opp_cb_t *);
    bt_status_t (*connect)(bt_conn_t *);
    bt_status_t (*disconnect)(bt_conn_t *);
    bt_status_t (*set_file_infor)(bt_conn_t *,UINT16 *file_name,UINT16 file_name_len,char *file_type,UINT32 file_length);
    bt_status_t (*put)(bt_conn_t *,UINT8 *file_data,UINT32 remand_length,UINT8 flag);
    bt_status_t (*response)(bt_conn_t *,UINT8 res_code);
    bt_status_t (*autoaccept)(bt_conn_t *,UINT8 accept);
} bt_opp_if_t;

bt_opp_if_t *btopp_get_interface(void);
bt_opp_cb_t *btopp_get_callback(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

/** @} */


#endif
