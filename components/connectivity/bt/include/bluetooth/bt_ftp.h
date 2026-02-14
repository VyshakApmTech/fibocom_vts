/**
 * @file    bt_ftp.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */



#ifndef __BT_FTP_H__
#define __BT_FTP_H__

//#include "bt_stack_include.h"

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
    FTP_PUT_FIRST_DATA,
    FTP_PUT_MIDDLE_DATA,
    FTP_PUT_END_DATA,
    FTP_GET_FIRST_DATA,
    FTP_GET_MIDDLE_DATA,
    FTP_GET_END_DATA,
    FTP_GET_FIRST_FOLDER,
    FTP_GET_MIDDLE_FOLDER,
    FTP_GET_END_FOLDER,
    FTP_NONE,
}FTP_FLAG;

typedef enum
{
    FTP_ROOT,
    FTP_BACKWARD,
    FTP_FORWARD
}AP_FTP_BROWSING;

typedef struct
{
    void (*ftp_connected_cb)(bt_conn_t *,bt_status_t status,UINT16 max_lenght);
    void (*ftp_disconnect_cb)(bt_conn_t *,bt_status_t status);
    void (*ftp_put_cb)(bt_conn_t *,bt_status_t status,UINT32 remand_length);
    void (*ftp_get_cb)(bt_conn_t *,UINT8 header_type,UINT8 *data,UINT16 data_lenght);
    void (*ftp_header_cb)(bt_conn_t *,UINT8 oprescode,UINT8 header,UINT8 *data,UINT16 data_lenght);
    void (*error_cb)(bt_conn_t *, bt_status_t err_type, UINT8 err_level);
} bt_ftp_cb_t;

typedef struct
{
    bt_status_t (*init)(bt_ftp_cb_t *);
    bt_status_t (*connect)(bt_conn_t *);
    bt_status_t (*disconnect)(bt_conn_t *);
    bt_status_t (*set_file_infor)(bt_conn_t *,UINT16 *file_name,UINT16 file_name_len,char *file_type,UINT32 file_length);
    bt_status_t (*get_folder)(bt_conn_t *);
    bt_status_t (*set_path)(bt_conn_t *,UINT16 *folder,UINT16 folder_len,UINT8 flags);
    bt_status_t (*pull_object)(bt_conn_t *,UINT8 flag);
    bt_status_t (*push_object)(bt_conn_t *,UINT8 *file_data,UINT32 remand_length,UINT8 flag);
    bt_status_t (*delete_object)(bt_conn_t *);
    bt_status_t (*creat_folder)(bt_conn_t *);
    bt_status_t (*copy_object)(bt_conn_t *);
    bt_status_t (*move_rename_object)(bt_conn_t *);
    bt_status_t (*set_permission)(bt_conn_t *);
    bt_status_t (*response)(bt_conn_t *p_conn,UINT8 res_code, UINT8 *file_data,UINT32 remand_length,UINT8 flag);
    bt_status_t (*autoaccept)(bt_conn_t *,UINT8 accept);
} bt_ftp_if_t;


bt_ftp_if_t *btftp_get_interface(void);
bt_ftp_cb_t *btftp_get_callback(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

/** @} */


#endif
