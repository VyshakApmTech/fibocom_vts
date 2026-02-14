/**
 * @file     bt_abs.h
 * @brief    bt interface adaptor
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020-09-30
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */
#ifndef BT_ABS_H
#define BT_ABS_H

#include <assert.h>
#include "bluetooth/bluestack.h"
#include "porting/bt_porting.h"
#include "bt_app_config.h"
#include "sci_types.h"

/********************************All structures ***********************/
#define BT_PRODUCT_HEADSET 0
#define BT_PRODUCT_PHONE 1

#define SDP_UUID_SERIAL 0x1101                 //spp
#define SDP_UUID_OPP 0x1105                    //opp
#define SDP_UUID_FTP 0x1106                    //ftp
#define SDP_UUID_AUDIOSOURCE 0x110A            //a2dp src
#define SDP_UUID_AUDIOSINK 0x110B              //a2dp sink
#define SDP_UUID_HANDSFREE 0x111E              //hfp hf
#define SDP_UUID_HANDSFREE_AUDIOGATEWAY 0x111F //hfp ag
#define SDP_UUID_AVRCP_TG 0x110E               //avrcp target
#define SDP_UUID_AVRCP_CT 0x110F               //avrcp controller

#define BT_INQUIRY_TIMEOUT 30 //time=30*1.28s
#define BT_INQUIRY_ACCESS_CODE_LIAC (0x9e8b00)
#define BT_INQUIRY_ACCESS_CODE_DEFAULT (0x9e8b33)

typedef struct _BT_ADDRESS
{
    uint8 addr[SIZE_OF_BDADDR];
} BT_ADDRESS;

/************************All function declarations*******************************/
//Classic
void bt_core_init(bt_stack_callbacks_t *bt_callbacks,
                  const bt_stack_config_t *bt_config, bt_stack_storage_t *p_storage);

bt_status_t bt_set_role(UINT8 is_phone);

bt_status_t bt_start(void);

bt_status_t bt_stop(void);

bt_state_t bt_state_get(void);

bt_status_t bt_device_search_start(uint8 max_result, uint8 is_liac);

bt_status_t bt_device_search_cancel(void);

bt_status_t bt_device_acl_connect(bt_stack_inquiry_info_t *p_inquiry_info);

bt_status_t bt_device_pair(bdaddr_t *addr);

bt_status_t bt_pair_user_input(bdaddr_t addr, UINT8 mode, UINT8 result, UINT8 *passkey);

bt_status_t bt_pair_configpara_set(UINT8 io_cap, UINT8 ssp_support);

bt_status_t bt_pair_configpara_get(manager_config_t *config);

bt_status_t bt_device_pair_cancel(bdaddr_t *addr);

bt_status_t bt_inquiry_scan_param_set(UINT16 interval, UINT16 window);

bt_status_t bt_page_scan_param_set(UINT16 interval, UINT16 window);

bt_status_t bt_read_rssi(bt_conn_t *p_conn);

bt_status_t bt_paired_device_remove(bdaddr_t *addr);

bt_status_t bt_visibility_set(bt_visible_t visible);

bt_status_t bt_visibility_get(bt_visible_t *visible);

//bt_status_t bt_local_name_set(const uint8 *name);

bt_status_t bt_local_name_change(const uint8 *name);

bt_status_t bt_local_name_get(char *name);

bt_status_t bt_public_addr_get(bdaddr_t *addr);

#ifdef CONFIG_FIBOCOM_BASE            //add patch CQ:SPCSS01419072
bt_status_t bt_local_version_get(char* hci_version);
#endif

//HFP
bt_status_t bt_hfp_cb_register(bt_hfp_cb_t *cb);

bt_status_t bt_hfp_connect_req(bdaddr_t *pAddr, UINT32 timeout, void (*cb)(bt_conn_t *));

bt_status_t bt_hfp_disconnect_req(bt_conn_t *p_conn);

bt_status_t bt_hfp_audio_switch(bt_conn_t *p_conn, BOOL isOn);

bt_status_t bt_hfp_call_resp(bt_conn_t *p_conn, UINT32 cmd, UINT8 chld_val);

bt_status_t bt_hfp_call_num(bt_conn_t *p_conn, UINT8 *call_num);

bt_status_t bt_hfp_adjust_vol(bt_conn_t *p_conn, bthfp_volume_type_t type, UINT8 vol);

bt_status_t bt_hfp_send_ciev(bt_conn_t *p_conn, hfp_cind_t ind, UINT8 value);

bt_status_t bt_hfp_send_raw_data(bt_conn_t *p_conn, UINT8 *buff, UINT32 len);

bt_status_t bt_hfp_ring(bt_conn_t *p_conn);

bt_status_t bt_hfp_send_ok(bt_conn_t *p_conn);

bt_status_t bt_hfp_send_error(bt_conn_t *p_conn);

//bt_status_t bt_hfp_set_num(bt_conn_t *hfp_conn, UINT8 *num);
bt_status_t bt_hfp_voice_recognition_enable(bt_conn_t *p_conn);

bt_status_t bt_hfp_voice_recognition_disable(bt_conn_t *p_conn);

bt_status_t bt_hfp_audio_disconnect(bt_conn_t *p_conn);

bt_status_t bt_hfp_close(bt_conn_t *p_conn);

//a2dp
bt_status_t bt_a2dp_cb_register(bt_a2dp_cb_t *callbacks);
bt_status_t bt_a2dp_connect_req(bt_conn_t *p_conn);
bt_status_t bt_a2dp_disconnect_req(bt_conn_t *p_conn);
bt_status_t bt_a2dp_send_data(bt_conn_t *p_conn, avdtp_media_data_t *media_data);
bt_status_t bt_a2dp_send_signaling(bt_conn_t *p_conn, a2dp_signal_id_t signal_id);
//Avrcp
bt_status_t bt_avrcp_cb_register(bt_avrcp_cb_t *callbacks);
bt_status_t bt_avrcp_connect_req(bt_conn_t *p_conn, UINT32 timeout, void (*timeout_cb)(bt_conn_t *));
bt_status_t bt_avrcp_disconnect_req(bt_conn_t *p_conn);
bt_status_t bt_avrcp_set_config(avrcp_config_t *cfg);
bt_status_t bt_avrcp_send_notification_resp(bt_conn_t *p_conn, bt_vendor_data_t *vendor_data);
bt_status_t bt_avrcp_send_notification_req(bt_conn_t *p_conn, bt_vendor_data_t *vendor_data);
bt_status_t bt_avrcp_volume_change_notify_req(bt_conn_t *p_conn, UINT8 volume);
bt_status_t bt_avrcp_send_passthrough(bt_conn_t *p_conn, bt_passthrough_data_t *passthrough_data);
bt_status_t bt_avrcp_set_absolute_volume(bt_conn_t *p_conn, UINT8 volume);
bt_status_t bt_avrcp_close(bt_conn_t *p_conn);
//Spp
bt_status_t bt_spp_cb_register(bt_spp_cb_t *callbacks);

bt_status_t bt_spp_connect_req(bdaddr_t *addr);

bt_status_t bt_spp_disconnect_req(bt_conn_t *);

bt_status_t bt_spp_data_send(bt_conn_t *, uint8 *data, uint16 length, UINT16 port);

//opp
bt_status_t bt_opp_init(bt_opp_cb_t *callbacks);
bt_status_t bt_opp_connect(bt_conn_t *p_conn);
bt_status_t bt_opp_set_file_infor(bt_conn_t *p_conn, UINT16 *file_name, UINT16 file_name_len, char *file_type, UINT32 file_length);
bt_status_t bt_opp_put(bt_conn_t *p_conn, UINT8 *data, UINT32 remand_length, UINT8 flag);
bt_status_t bt_opp_disconnect(bt_conn_t *p_conn);
bt_status_t bt_opp_response(bt_conn_t *p_conn, UINT8 res_code);
bt_status_t bt_opp_autoaccept(bt_conn_t *p_conn, UINT8 accept);

//ftp
bt_status_t bt_ftp_init(bt_ftp_cb_t *callbacks);
bt_status_t bt_ftp_connect(bt_conn_t *p_conn);
bt_status_t bt_ftp_get_folder(bt_conn_t *p_conn);
bt_status_t bt_ftp_set_path(bt_conn_t *p_conn, UINT16 *folder, UINT16 folder_len, UINT8 flags);
//bt_status_t bt_ftp_set_file_infor(bt_conn_t *p_conn,bt_obex_file_inf *infor);
bt_status_t bt_ftp_set_file_infor(bt_conn_t *p_conn, UINT16 *file_name, UINT16 file_name_len, char *file_type, UINT32 file_length);
bt_status_t bt_ftp_pull(bt_conn_t *p_conn, UINT8 flag);
bt_status_t bt_ftp_push(bt_conn_t *p_conn, UINT8 *file_data, UINT32 remand_length, UINT8 flag);
bt_status_t bt_ftp_response(bt_conn_t *p_conn, UINT8 res_code, UINT8 *file_data, UINT32 remand_length, UINT8 flag);
bt_status_t bt_ftp_autoaccept(bt_conn_t *p_conn, UINT8 accept);

//sdp
bt_status_t bt_sdp_init(bt_sdp_cb_t *cb);

bt_status_t bt_sdp_get_srv_list(bt_conn_t *p_conn);

//BLE  common  interface
bt_status_t ble_set_addr(bdaddr_t *addr, UINT8 type);

bt_status_t ble_get_addr(bdaddr_t *addr, UINT8 type);

bt_status_t ble_connect_para_update_req(ble_conn_t *p_conn, ble_conn_update_param_t *param);

bt_status_t ble_set_resolved_addr_period(UINT16 sec);

bt_status_t ble_get_local_buff_size(UINT32 *packets_size, UINT32 *packets_num);

bt_status_t ble_white_list_add(bdaddr_t *addr, UINT8 addr_type);

bt_status_t ble_white_list_remove(bdaddr_t *addr, UINT8 addr_type);

bt_status_t ble_white_list_clear(void);

UINT32 ble_get_white_list_size(void);

bt_status_t ble_add_resolved_list(UINT8 peer_ida_type, bdaddr_t *peer_ida, UINT8 *peer_irk, UINT8 *local_irk);

UINT32 ble_get_resolved_list_size(void);

bt_status_t ble_remove_resolved_list(UINT8 peer_ida_type, bdaddr_t *peer_ida);

bt_status_t ble_clear_resolved_list(void);

bt_status_t ble_enable_resolved_list(UINT8 enable);

bt_status_t ble_set_data_length(ble_conn_t *p_conn, UINT16 tx_oct, UINT16 tx_time);

ble_conn_t **(ble_get_dev_list)(void);

//BLE adv interface
bt_status_t ble_add_srv(ble_gatt_element_t *p_element, UINT32 size);

bt_status_t ble_clear_srv(void);

bt_status_t ble_adv_data_set(uint8 *data, uint32 len);

bt_status_t ble_set_adv_param(ble_adv_param_t *param);

bt_status_t ble_adv_enable(void);

bt_status_t ble_adv_disable(void);

bt_status_t ble_srv_send_notification(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_srv_send_indication(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_scan_rsp_data_set(uint8 *data, uint8 length);

//set iBeacon data
//uuid is 16 bytes, such as 0xb9407f30f5f8466eaff925556b57fe6d;
//major is 2 bytes, such as 0x0001;
//minor is 2 bytes, such as 0x0002;
bt_status_t ble_beacon_data_set(UINT8 uuid[16], UINT8 major[2], UINT8 minor[2]);

//BLE scan interface
bt_status_t ble_set_scan_param(ble_scan_param_t *param);

bt_status_t ble_start_scan(UINT8 filter_enable);

bt_status_t ble_stop_scan(void);

bt_status_t ble_connect_req(ble_conn_param_t *param);

bt_status_t ble_connect_cancel(void);

bt_status_t ble_disconnect_req(ble_conn_t *p_conn, UINT8 reason);

bt_status_t ble_exchange_mtu(ble_conn_t *p_conn, UINT16 mtu);

bt_status_t ble_read_rssi(ble_conn_t *p_conn);

bt_status_t ble_cli_discover_all_primary_service(ble_conn_t *p_conn, ble_gatt_discover_t *param);

bt_status_t ble_cli_discover_primary_service_by_uuid(ble_conn_t *p_conn, ble_gatt_discover_t *param);

bt_status_t ble_cli_find_include_service(ble_conn_t *p_conn, ble_gatt_discover_t *param);

bt_status_t ble_cli_discover_all_characteristic(ble_conn_t *p_conn, ble_gatt_discover_t *param);

bt_status_t ble_cli_discover_characteristic_by_uuid(ble_conn_t *p_conn, ble_gatt_discover_t *param);

bt_status_t ble_cli_discover_all_characteristic_descriptor(ble_conn_t *p_conn, ble_gatt_discover_t *param);

bt_status_t ble_cli_read_char_value_by_handle(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_cli_read_char_value_by_uuid(ble_conn_t *p_conn, ble_gatt_discover_t *param);

bt_status_t ble_cli_read_long_char(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_cli_read_multiple_char_value(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_cli_write_char_value_without_rsp(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_cli_write_char_value(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_cli_write_char_value_reliable(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_cli_write_char_value_signed(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_cli_write_long_char_value(ble_conn_t *p_conn, ble_gatt_rw_t *param);

bt_status_t ble_cli_write_execute_char_value(ble_conn_t *p_conn, UINT8 flag);
//BLE smp interface
bt_status_t ble_smp_start_pair(ble_conn_t *p_conn);

bt_status_t ble_smp_remove_dev_from_resolving_list(ble_conn_t *p_conn);

bt_status_t ble_smp_clear_pair_list(void);

void bt_ssp_display_result_confirm(UINT8 result, bdaddr_t addr);

bt_status_t ble_app_smp_user_confirm(ble_conn_t *p_conn, UINT8 mode, UINT32 passkey);

bt_status_t ble_app_smp_configpara_set(UINT8 io_cap, UINT8 auth_Req, UINT32 pass_key, UINT32 pairing_timeout);

bt_status_t ble_app_smp_configpara_get(smp_config_t *config);
#endif /* BT_ABS_H */
