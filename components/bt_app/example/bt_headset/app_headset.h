#ifndef __APP_HEADSET_H__
#define __APP_HEADSET_H__

#define RECONNECTION_EN 1
#if RECONNECTION_EN
#define MAX_RECONNECT_TIME 5
#endif

void app_bt_hfp_init();

void bt_app_hfp_stop(void);
bool app_bt_hfp_call_reject(void);
bool app_bt_hfp_call_release(void);
bool app_bt_hfp_call_answer_accept(void);
bool app_bt_hfp_dial_req(UINT8 *phone_number);
bool app_bt_hfp_redial_req(void);
bool app_bt_hfp_vr_enable(void);
bool app_bt_hfp_vr_disable(void);
bool app_bt_hfp_three_way_call_ctrl(UINT8 CtrlCmd);
bool app_bt_sco_disconnect(void);

void app_bt_avrcp_init();
bool app_bt_avrcp_start(void);
bool app_bt_avrcp_pause(void);
bool app_bt_avrcp_previ(void);
bool app_bt_avrcp_next(void);

bool app_bt_avrcp_vol_set(UINT8 volume);
int8 app_bt_avrcp_vol_get(void);
void bt_avrcp_msg_handler(void* msg_data);
void bt_app_avrcp_stop(void);


void app_bt_a2dp_init();
void bt_app_a2dp_stop(void);
void bt_a2dp_msg_handle(void* msg_data);

void bt_app_headset_start(void);
void bt_app_headset_stop(void);
void bt_app_headset_ssp_passkey_input(UINT8 mode,UINT8 result, UINT8 *passkey);
void bt_app_headset_pair_device_remove(void);
void bt_app_headset_set_config(UINT8 io_cap,UINT8 ssp_support);
bool bt_app_headset_send_msg(uint16 msg_id, void *data_ptr);

#endif //__APP_HEADSET_H__
