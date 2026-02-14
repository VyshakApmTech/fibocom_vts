/**
 * @file    bt_gatt_client_demo.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020-6-13
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef _BT_GATT_CLIENT_DEMO_H
#define MAX_DEVICE 6
#define MAX_SERVER_NUM  10
#define MAX_CHARACTERISTIC 10
#define MAX_DESCRIPTOR 2
#define FIND_INFORMATION_RSP_S  0x04
#define FIND_INFORMATION_RSP_L  0x18

#define ATT_UUID_CLIENT 0x2902

#define _BT_GATT_CLIENT_DEMO_H

#define GATT_CLIENT_DEMO_ENABLE 1

#if GATT_CLIENT_DEMO_ENABLE
#define MAX_DEVICE 6
#define MAX_SERVER_NUM  10
#define MAX_CHARACTERISTIC 10
#define MAX_DESCRIPTOR 2
#define FIND_INFORMATION_RSP_S  0x04
#define FIND_INFORMATION_RSP_L  0x18

typedef enum
{
    GATT_UUID_SHORT = 1,
    GATT_UUID_LONG,
} GATT_UUID_STATE;

typedef struct
{
    UINT16 acl_handle;
    UINT16 opcode;
    UINT16 start_handle;
    UINT16 end_handle;
    UINT16 length;
    UINT16 offset;
    UINT8 char_index;
    UINT8 char_desc_index;
    UINT8 read_value_index;
    UINT8 write_value_index;
    UINT8 islong;
    UINT8 uuid_l[16];
    UINT16 uuid_s;
    void *value;
    UINT16 value_len;
 } dis_info_t;

typedef struct
{
     UINT16 desc_handle;
     UINT16 desc_uuid_s;
     UINT16 desc_uuid_l[16];
 } desc_info_t;

typedef struct
{
    UINT8 des_cnt;
    UINT8 des_index;
    UINT16 char_uuid_s;
    UINT16 att_handle;
    UINT16 value_handle;
    UINT8 properties;
    UINT8 char_uuid_l[16];
    desc_info_t desc_info[MAX_DESCRIPTOR];
} char_info_t;

typedef struct
{
    UINT8 char_cnt;
    UINT8 char_index;
    UINT8 uuid_type;
    UINT16 start_handle;
    UINT16 end_handle;
    UINT16 srv_uuid_s;
    UINT8 type;
    UINT8 srv_uuid_l[16];
    char_info_t char_info[MAX_CHARACTERISTIC];
} server_info_t;

#define SMP_AUTHREQ_LEGACY_JUSTWORK       0X01
#define SMP_AUTHREQ_LEGACY_PASSKEY        0X05
#define SMP_AUTHREQ_SECURITY_JUSTWORK     0X09
#define SMP_AUTHREQ_SECURITY_PASSKEY      0X0D

typedef enum
{
    GATT_CLIENT_LINK_DISCONNECT,
    GATT_CLIENT_LINK_CONNECTING,
    GATT_CLIENT_LINK_CONNECTED,
    GATT_CLIENT_DISCOVER_PRME_SER,
    GATT_CLIENT_DISCOVER_PRME_SER_DONE,
    GATT_CLIENT_READ_INCLUDE,
    GATT_CLIENT_READ_INCLUDE_DONE,
    GATT_CLIENT_READ_CHARA_DEC,
    GATT_CLIENT_READ_CHARA_DEC_DONE,
    GATT_CLIENT_FIND_INFO,
    GATT_CLIENT_FIND_INFO_DONE,
    GATT_CLIENT_SET_CONFIG,
    GATT_CLIENT_SET_CONFIG_DONE,
} GATT_CLIENT_STATE;

/* module id */

#endif
extern INT8 cli_target_name[31];

void ble_client_demo_start_discover_handle(void *argv);
void ble_cli_discover_srv(UINT16 start_handle, UINT16 end_handle);
void ble_cli_discover_char(UINT16 start_handle, UINT16 end_handle);
void ble_cli_discover_include_srv(UINT16 start_handle, UINT16 end_handle);
void Gatt_client_demo_function(void);
void Gatt_client_demo_get_srv_list(void);
void Gatt_client_demo_read_char(UINT16 char_uuid);
void Gatt_client_demo_write_char(UINT16 char_uuid, UINT8 *data, UINT8 length);

void Gatt_client_demo_stop(void);
void Gatt_client_demo_run_statemachine(GATT_CLIENT_STATE next_state);

void example_ble_client_info_get(ble_stack_client_callbacks_t **p_client_callbacks);
void example_ble_client_common_info_get(ble_stack_common_callbacks_t **p_common_callbacks);
void example_ble_client_smp_info_get(ble_stack_smp_callbacks_t **p_smp_callbacks);
bool bt_send_msg_client_task(uint16 msg_id, void *data_ptr);
void bt_app_client_task_create(void);

#endif
