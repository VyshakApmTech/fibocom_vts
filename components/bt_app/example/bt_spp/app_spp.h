
#ifndef __APP_SPP_H__
#define __APP_SPP_H__

#define BT_INQUIRY_MAX_NUM 100
#define SPP_DATA_RECV_MAX_NUM 667
#define SDP_SRV_SPP         (1 << 6)

extern bdaddr_t spp_peer_addr;

extern UINT32 sdp_srv_list;

void bt_app_spp_init(void);
void app_bt_spp_sdp_init(void);

bool bt_app_spp_connect_req(bdaddr_t *addr);
bool bt_app_spp_disconnect_req(void);

bool bt_app_spp_data_send_req(uint8 *src_buf, uint32 data_len);

int bt_app_spp_demo_handle(char *argv[], int argc, char *rsp, int *rsp_len);
bool bt_send_msg_spp_task(uint16 msg_id, void *data_ptr);
bool bt_app_spp_data_resend_req(void);
void bt_app_spp_stop(void);

#endif //__APP_SPP_H__
