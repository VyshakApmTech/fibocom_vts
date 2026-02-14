#ifndef LIB_INCLUDE_FFW_PAL_SMS
#define LIB_INCLUDE_FFW_PAL_SMS


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "oc_sms.h"


#ifdef CONFIG_FIBOCOM_SMS_FEATURE
typedef unsigned long ffw_uint32;
typedef unsigned short ffw_uint16;
typedef unsigned char ffw_uint8;
typedef signed long ffw_int32;

#define FFW_TEL_NUMBER_MAX_LEN (21 + 1)
#define FFW_PAL_SMS_CENTER_ADDRESS_LEN_MAX             (32)
#define FFW_PAL_SMS_CENTER_ADDRESS_TYPE_LEN_MAX        (4)

#define AT_SMS_STATUS_UNREAD 0
#define AT_SMS_STATUS_READ 1
#define AT_SMS_STATUS_UNSENT 2
#define AT_SMS_STATUS_SENT 3
#define AT_SMS_STATUS_ALL 4
#define FFW_PAL_SMS_EXTRA_DA_LEN_MAX                   (20 * 15)
#define FFW_PAL_SMS_EXTRA_TIME_LEN_MAX                 (40)
#define FFW_PAL_SMS_CONT_LEN_MAX                       (1024)
#define FFW_PAL_SMS_MSG_LIST_NULL -2

typedef enum
{
    ffw_sms_msg_format_PDU = -1,    //PDU format sms
    ffw_sms_msg_format_GSM = 0,
    ffw_sms_msg_format_HEX = 1,
    ffw_sms_msg_format_UCS2 = 2,
    ffw_sms_msg_format_IRA = 4,
}ffw_pal_sms_format;

typedef enum
{
    ffw_sms_msg_class_0 = 0, //class 0
    ffw_sms_msg_class_1 = 1, //class 1
    ffw_sms_msg_class_2 = 2, //class 2
    ffw_sms_msg_class_3 = 3, //class 3
    ffw_sms_msg_class_4 = 4, //None
}ffw_pal_sms_msg_class;


typedef enum SmsFormatModeTag
{
    SMS_FORMAT_PDU_MODE = 0,
    SMS_FORMAT_TXT_MODE = 1
}SmsFormatMode;

typedef enum
{
    FFW_PAL_SMS_SEND_NO_SAVE = 0,
    FFW_PAL_SMS_SEND_AND_SAVE,
} ffw_pal_sms_send_flag_e;  


typedef enum
{
    FFW_PAL_SMS_DEL_BY_INDEX = 0,
    FFW_PAL_SMS_DEL_MT_READ,
    FFW_PAL_SMS_DEL_MT_NOT_READ,
    FFW_PAL_SMS_DEL_MO_SENT,
    FFW_PAL_SMS_DEL_MO_NOT_SENT,
    FFW_PAL_SMS_DEL_ALL,
} ffw_pal_sms_delete_flag_e;

typedef enum
{
    FFW_PAL_SMS_STORAGE_TYPE_INVALID = -1,
    FFW_PAL_SMS_STORAGE_TYPE_UIM = 0,
    FFW_PAL_SMS_STORAGE_TYPE_NV,
    FFW_PAL_SMS_STORAGE_TYPE_AP,
    FFW_PAL_SMS_STORAGE_TYPE_MAX,
}ffw_pal_sms_storage_type_e;


typedef enum
{
    FFW_PAL_SMS_MSG_FORMAT_INVALID = -1,
    FFW_PAL_SMS_MSG_FORMAT_CDMA,
    FFW_PAL_SMS_MSG_FORMAT_GW_PP,
    FFW_PAL_SMS_MSG_FORMAT_GW_BC,
    FFW_PAL_SMS_MSG_FORMAT_MWI,
    FFW_PAL_SMS_MSG_FORMAT_MAX
}ffw_pal_sms_msg_format_e;

typedef enum
{
    FFW_PAL_SMS_MSG_STATUS_INVALID = -1,
    FFW_PAL_SMS_MSG_STATUS_UNREAD = 0,
    FFW_PAL_SMS_MSG_STATUS_READ,
    FFW_PAL_SMS_MSG_STATUS_UNSENT,
    FFW_PAL_SMS_MSG_STATUS_SENT,
} ffw_pal_sms_msg_status_flag_e;

typedef struct
{
    ffw_int32 used_cnt;
    ffw_int32 max_cnt;
}ffw_pal_sms_loc_store_info_t;

typedef enum
{
    FFW_PAL_SMS_MSG_CFG_INVALID = -1,
    FFW_PAL_SMS_MSG_CFG_STORAGE_TYPE = 0,

} ffw_pal_sms_cfg_e;


typedef struct sms_context_s
{
    int result;
    bool pending;
    ffw_uint32 sem;
    ffw_uint32 lock;
    bool init;
    ffw_uint32 return_message_id;
} sms_context_t;


typedef struct
{
    ffw_pal_sms_format format;
    uint8_t msg_class_valid;
    ffw_pal_sms_msg_class msg_class;
    uint32_t length;
}ffw_pal_sms_cfg;


typedef struct sms_center_address
{
    uint8_t uti;
    void *async_ctx;
} sms_center_address;

typedef struct
{
    int result;
    uint8_t simid;
    bool origin_volte;
    uint8_t sms_origin_format;
    uint16_t uti;
}sms_context;


typedef struct
{
    uint8_t total_segments;     /**<     The number of long  short message*/
    uint8_t seg_number;         /**<     Current number.*/
    uint8_t reference_number;   /**< reference_number.*/
}ffw_sms_pal_udh_t;



typedef struct
{
    ffw_pal_sms_msg_format_e   format;
    ffw_pal_sms_msg_status_flag_e status;
    int pdu_data_len;
    uint8_t pdu_data[FFW_PAL_SMS_CONT_LEN_MAX];
    char     extra_da[FFW_PAL_SMS_EXTRA_DA_LEN_MAX];          //reserved for AP storerage
    fibo_sms_timestamp extra_time;   //reserved for AP storerage
    uint8_t data_head_valid;
    ffw_sms_pal_udh_t data_head;
}ffw_pal_sms_msg_t;


typedef struct
{
    int result;
    uint8_t simid;
    bool origin_volte;
    uint8_t sms_origin_format;
    uint16_t uti;
    ffw_pal_sms_msg_t* sms_msg_t;
}sms_read_context;



typedef struct {
    bool flag;
    uint8_t *old_data;
    size_t old_size;
}sms_data_exsit_zero_t;

extern int ffw_pal_sms_param_set(uint8_t sim_id, ffw_pal_sms_cfg *cfg);
extern void fibo_sms_register_cb(uint32_t event_id, void* cb_data);
extern int ffw_pal_sms_event_register(fibo_sms_rxmsg_handler_func_t callback, void *cb_param);
extern int ffw_pal_sms_center_address_get(ffw_uint8 sim_id, char center_address[FFW_PAL_SMS_CENTER_ADDRESS_LEN_MAX], char center_address_type[FFW_PAL_SMS_CENTER_ADDRESS_TYPE_LEN_MAX]);
extern int ffw_pal_sms_center_address_set(ffw_uint8 sim_id, char center_address[FFW_PAL_SMS_CENTER_ADDRESS_LEN_MAX], char center_address_type[FFW_PAL_SMS_CENTER_ADDRESS_TYPE_LEN_MAX]);
extern int ffw_pal_sms_msg_send_text(ffw_uint8 sim_id, char *phone_num, char *data, ffw_pal_sms_cfg *cfg);
extern int ffw_pal_sms_msg_send_pdu(ffw_uint8 sim_id, unsigned char *pdu, ffw_int32 pdu_len, ffw_pal_sms_send_flag_e send_flag, ffw_uint32 *msg_id_ptr, ffw_int32 *storage_index_ptr);
extern int ffw_pal_sms_msg_read(ffw_uint8 sim_id, ffw_pal_sms_storage_type_e storage_type, ffw_uint32 index, ffw_pal_sms_msg_t *sms_msg_ptr, ffw_int32 sms_msg_num);
extern int ffw_pal_sms_msg_list_read(ffw_uint8 sim_id, fibo_sms_msg_list_req_info_t *req_info, fibo_sms_msg_list_info_t *resp_info);
extern int ffw_pal_sms_msg_delete(ffw_uint8 sim_id, ffw_pal_sms_storage_type_e storage_type, ffw_int32 index, ffw_pal_sms_delete_flag_e delete_flag);
extern void ffw_pal_sms_read_param_set(uint8_t sim_id, int read_mode);
extern int ffw_pal_sms_read_param_get(uint8_t sim_id);
extern ffw_int32 ffw_pal_sms_storage_info_get(    ffw_pal_sms_storage_type_e storage_type,     ffw_pal_sms_loc_store_info_t * sms_store_info_ptr);
extern int ffw_pal_sms_cfg_set(uint8_t simid, ffw_pal_sms_cfg_e sms_cfg, void*cfg_data);
extern int ffw_pal_sms_cfg_get(uint8_t simid, ffw_pal_sms_cfg_e sms_cfg, void*cfg_data);
extern int ffw_pal_set_sms_cscs(void *ffw_cscs);
extern int ffw_pal_get_sms_cscs();


#endif
#endif
