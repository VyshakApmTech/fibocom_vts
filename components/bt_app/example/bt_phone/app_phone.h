
#ifndef __APP_PHONE_H__
#define __APP_PHONE_H__

#define BT_INQUIRY_MAX_NUM      100
#define SPP_DATA_RECV_MAX_NUM   667
#define A2DP_MEDIA_PKT_CNT      2 // 2 packats for each timeout 40ms, averiage with 20ms send 7frames sbc data
#define A2DP_SEND_DATA_TIMEOUT  40// 40ms for send media data

#define SDP_SRV_HFP_HF      (1 << 0)
#define SDP_SRV_HFP_AG      (1 << 1)
#define SDP_SRV_A2DP_SRC    (1 << 2)
#define SDP_SRV_A2DP_SINK   (1 << 3)
#define SDP_SRV_AVRCP_TG    (1 << 4)
#define SDP_SRV_AVRCP_CT    (1 << 5)
#define SDP_SRV_SPP         (1 << 6)

#define TEST_MUSIC_FILE_PATH CONFIG_FS_SDCARD_MOUNT_POINT"/music/test1.mp3"

#define AUDIO_DEFAULT_VOLUME 50

typedef struct bt_hfp_call_status
{
    UINT8 active_call;
    UINT8 hold_call;
    UINT8 wait_call;
} hfp_call_status;

extern UINT32 sdp_srv_list;
extern UINT8 target_name[31];

bool audevSetPlayVolume(unsigned vol);

extern bool bt_app_phone_send_msg(uint16 msg_id, void *data_ptr);
extern void bt_app_phone_start(void);
extern void bt_app_phone_stop(void);

extern void bt_app_ag_init(void);
extern void bt_app_ag_dial(UINT8 *num);
extern void bt_app_ag_multi_call(UINT8 nCmd);
extern void bt_app_ag_stop(void);

extern void app_bt_sdp_init(void);
extern void app_bt_a2dp_src_init(void);
extern void app_bt_a2dp_src_stop(void);
extern void bt_a2dp_send_start();
extern void bt_a2dp_send_stop();
extern void bt_a2dp_src_msg_handle(void* msg_data);

extern void app_bt_avrcp_tg_init(void);
void bt_app_send_status_notifition_resp(avrcp_playback_status_t status , UINT8 trans_label);
void app_bt_play_state_set(avrcp_playback_status_t status);
avrcp_playback_status_t app_bt_play_state_get(void);
UINT8 app_bt_vendor_trans_label_get(void);
void app_bt_set_play_position(UINT32 pos);
void app_bt_set_play_position_stop(void);
void app_bt_set_play_position_start(void);
bool app_bt_avrcp_set_absolute_vol(UINT8 volume);
UINT8 app_bt_avrcp_get_volume(void);

//audio player
INT32 auPlayer_init(void);
void auPlayer_config(avdtp_sbc_caps_t *p_sbc_cfg);
INT32 auPlayer_start(void);
void auPlayer_pause(void);
void auPlayer_resume(void);
void auPlayer_stop(void);
INT32 auPlayer_get_sbc_pkt(UINT8 *pkt, UINT32 *pkt_len, UINT32 *frame_count);

void app_bt_set_audio_volume();
int bt_phone_demo_handle(char *argv[], int argc, char *rsp, int *rsp_len);

void bt_app_phone_open(void);
void auPlayer_close(void);

#endif //__APP_PHONE_H__
