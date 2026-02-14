/**
 * @file     bt_gatt_server_demo.h
 * @brief    bt gatt server demo
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020-09-30
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */
#ifndef _BT_GATT_SERVER_DEMO_H
#define _BT_GATT_SERVER_DEMO_H

#define BT_8910_TP_UUID 0x18FE
#define ATT_UUID_HID_PROTOCOL_MODE  0x2A4E
#define ATT_UUID_HID_REPORT         0x2A4D
#define ATT_UUID_HID_REPORT_MAP     0x2A4B
#define ATT_UUID_HID_KEY_INPUT      0x2A22
#define ATT_UUID_HID_KEY_OUTPUT     0x2A32
#define ATT_UUID_HID_MOUSE_INPUT    0x2A33
#define ATT_UUID_HID_INFORMATION    0x2A4A
#define ATT_UUID_HID_CONTROL        0x2A4C
#define BT_8910_FEEDBACK_CHAR       0x8910
#define BT_8910_TP_UUID_CHAR 0xFFFE
#define BT_8910_FEEDBACK_CHAR 0x8910
#define UNISOC_CID 0x073F
#define HID_PROTOCOL_MODE_BOOT      0x00
#define HID_PROTOCOL_MODE_REPORT    0x01
#define HID_REPORT_TYPE_INPUT       0x01
#define HID_REPORT_TYPE_OUTPUT      0x02
#define HID_REPORT_TYPE_FEATURE     0x03

//message id to upper layer apps
#define GATT_CONNECT_CNF 0x11
#define GATT_DISCONNECT_IND 0x12
#define GATT_NOTIFICATION       0x14
#define GATT_INDICATION         0x15
#define GATT_WRITE_NOTIFY 0x17
#define GATT_INDICATION_CONF 0x18
#define GATT_NOTIFICATION_CNF 0x19

/**@brief attribute type*/


#define GAP_APPEARANCE_CHAR_GENERIC_WATCH               3//192
#define GAP_APPEARANCE_CHAR_SUB_SPORTS_WATCH            1//193

//Central Address Resolution
#define GAP_ADDRESS_RESOLUTION_NOT_SUPPORT       0
#define GAP_ADDRESS_RESOLUTION_SUPPORT           1

typedef struct
{
    UINT16 min_conn_interval;
    UINT16 max_conn_interval;
    UINT16 slave_latency;
    UINT16 conn_timeout;
} gatt_chara_pcp_t;

typedef struct
{
    UINT8 handle[4];
} gatt_chara_sc_t;
typedef struct
{
    UINT8 value[5];
} gatt_chara_def_short_t;
typedef struct
{
    UINT8 value[19];
} gatt_chara_value_long_t;
typedef union {
    UINT16 uuid_s;
    UINT8 *uuid_l;
} gatt_uuid_t;
typedef struct
{
    UINT16 att_handle; /**< The att handle */
    gatt_uuid_t uuid;  /**< Uuid */
    UINT8 uuid_type;   /**< The type of uuid */
    UINT16 length;     /**< The length of  */
    UINT16 acl_handle; /**< The acl handle */
    UINT8 *data;       /**< The data information */
} gatt_le_data_info_t;
int bt_8910_gatt_process_msg(UINT16 msg_id, void *data);
void bt_8910_save_smp_key(uint8_t *p_data, uint32_t size);

void Gatt_server_demo_function(void);
void bt_8910_ble_SendTpData(uint16 datalen, uint8 *data);
void example_ble_server_common_info_get(ble_stack_common_callbacks_t **p_server_callbacks);
void example_ble_server_smp_info_get(ble_stack_smp_callbacks_t **p_server_callbacks);
bool bt_send_msg_server_task(uint16 msg_id, void *data_ptr);
void bt_app_server_task_create(void);
#ifdef CONFIG_FIBOCOM_BASE            //add patch CQ:SPCSS01419072
void bt_8910_ble_Send_Notify(uint16 handle, uint8 *data, uint16 datalen);
void bt_8910_ble_Send_Indication(uint16 handle, uint8 *data, uint16 datalen);
#endif
#endif
