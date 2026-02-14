
#ifndef __APP_OPP_H__
#define __APP_OPP_H__

#define BT_INQUIRY_MAX_NUM      100
#define SPP_DATA_RECV_MAX_NUM   667
#define SDP_SRV_OPP         (1 << 7)
#if 1
//header
#define OPP_OBEX_NAME 0x01
#define OPP_OBEX_TYPE 0x42
#define OPP_OBEX_LENGTH 0xc3
#define OPP_OBEX_BODY 0x48
#define OPP_OBEX_END_BODY 0x49
#define OPP_OBEX_HD_USER_MIDBODY 0x32

//opcode
#define OPP_OBEX_PUT 0X02
#define OPP_OBEX_FIRST_DATA 0x01
#define OPP_OBEX_MIDDLE_DATA 0x02
#define OPP_OBEX_END_DATA 0x03

//response code
#define OPP_OBEX_OK 0xa0
#define OPP_OBEX_CONTINUE 0x90
#endif
typedef enum
{
    OPP_SERVER,
    OPP_CLIENT
}AP_OPP_DEMO_MODE;

typedef enum
{
    OPP_STATUS_IDLE,
    OPP_STATUS_CONNECTING,
    OPP_STATUS_CONNECTED,//2
    OPP_STATUS_PUT,//3
    OPP_STATUS_PUTED,//4
    OPP_STATUS_DISCONNECT
}AP_OPP_STATUS;

extern UINT32 opp_sdp_srv_list;
extern char opp_target_name[31];
extern char opp_file_name[31];
extern char opp_target_file_type[31];
extern UINT8 opp_state;
extern UINT8 opp_demo_mode;
extern UINT32 opp_file_length;
extern bool bt_app_opp_send_msg(uint16 msg_id, void *data_ptr);
extern void app_opp_init(void);
extern bt_status_t app_opp_put_data(OPP_FLAG flag);
extern void app_opp_sdp_init(void);
int bt_opp_demo_handle(char *argv[], int argc, char *rsp, int *rsp_len);
bt_status_t app_opp_response(UINT8 res_code);
bt_status_t app_opp_autoaccept(UINT8 accept);
#endif //__APP_OPP_H__