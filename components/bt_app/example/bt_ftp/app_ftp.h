
#ifndef __APP_FTP_H__
#define __APP_FTP_H__

#define SDP_SRV_FTP      (1 << 8)
#define BT_INQUIRY_MAX_NUM      100

#define SCLASS_UUID_OBEX_FTP 0x1106

//header
#define OBEX_NAME 0x01
#define OBEX_TYPE 0x42
#define OBEX_LENGTH 0xc3
#define OBEX_BODY 0x48
#define OBEX_END_BODY 0x49
#define OBEX_OK 0xa0
#define OBEX_HD_USER_BACKWARD 0x31
#define OBEX_HD_USER_MIDBODY 0x32

//opcode
#define OBEX_PUT 0x02
#define OBEX_GET 0x03
#define OBEX_GET_FINAL 0x83
#define OBEX_SETPATH 0x85
#define OBEX_PUT_FINAL 0x82

//response code
#define OBEX_OK 0xa0
#define OBEX_CONTINUE 0x90
#define OBEX_BAD_REQUEST 0xc0
#define OBEX_FORBIDDEN 0xc3

#if 0
typedef enum
{
    OBEX_PUT_FIRST_DATA,
    OBEX_PUT_MIDDLE_DATA,
    OBEX_PUT_END_DATA,
    OBEX_PUT_NONE,
}OBEX_PUT_FLAG;
#endif
typedef enum
{
    FTP_STATUS_IDLE,
    FTP_STATUS_CONNECTING,
    FTP_STATUS_CONNECTED,//2
    FTP_STATUS_SETPATH,//3
    FTP_STATUS_BROWSING,
    FTP_STATUS_BROWSED,//5
    FTP_STATUS_PUSH,
    FTP_STATUS_PUSHED,//7
    FTP_STATUS_PULL,
    FTP_STATUS_PULLED,//9
    FTP_STATUS_GET,
    FTP_STATUS_NAVIGATE,//b
    FTP_STATUS_DISCONNECT
}AP_FTP_STATUS;

typedef enum
{
    FTP_SERVER,
    FTP_CLIENT
}AP_FTP_DEMO_MODE;

BT_STATUS ftp_scan_mode_set(uint8 discoverable, uint8 connectable);
int bt_ftp_demo_handle(char *argv[], int argc, char *rsp, int *rsp_len);
void app_bt_ftp_sdp_init(void);
void app_bt_ftp_init(void);
bool bt_app_ftp_send_msg(uint16 msg_id, void *data_ptr);
//void app_ftp_response_bowsing(UINT8 res_code);
void app_ftp_get_folder(void);
void app_ftp_set_path(void);
bt_status_t app_ftp_push(char *filename,FTP_FLAG flag);
void app_ftp_pull(char *filename,FTP_FLAG flag);
void app_ftp_response(UINT8 rescode,FTP_FLAG flag);
extern UINT8 ftp_demo_mode;
extern UINT8 ftp_state;
extern char ftp_target_folder[31];
extern char ftp_target_name[31];
extern char ftp_target_file[31];
extern char ftp_target_file_type[31];

#endif //__APP_OPP_H__
