#include "osi_log.h"
#include "osi_api.h"
#include "fibo_opencpu.h"
#include "oc_sms.h"

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    fibo_textTrace("sig_res_callback  sig = %d", sig);
}

static void at_res_callback(UINT8 *buf, UINT16 len)
{
    fibo_textTrace("FIBO <--%s", buf);
}

static FIBO_CALLBACK_T user_callback = {
    .fibo_signal = sig_res_callback,
    .at_resp = at_res_callback};

typedef enum _AT_MSG_ALPHABET
{
    AT_MSG_GSM_7 = 0,
    AT_MSG_8_BIT_DATA,
    AT_MSG_UCS2,
    AT_MSG_RESERVED,
} AT_MSG_ALPHABET;


// ��UCS-2�ַ�ת��ΪASCII�ַ�
static char ucs2_to_ascii(unsigned short ucs2_char)
{
    fibo_textTrace("[%s-%d] %X", __FUNCTION__, __LINE__, ucs2_char);
    if ((ucs2_char & 0xff) == 0x00) {
        return (char)((ucs2_char >> 8) & 0xFF);
    } else {
        return '?';  // ����ַ�
    }
}

// ��UCS-2�ַ���ת��ΪASCII�ַ���
static void ucs2_to_ascii_string(const unsigned short *ucs2_str, char *ascii_str, size_t length)
{
    for (size_t i = 0; i < length; ++i) {
        ascii_str[i] = ucs2_to_ascii(ucs2_str[i]);
    }
    ascii_str[length] = '\0';  // ȷ���ַ�����null��β
}

static void user_sms_event_callback(uint32_t ind_flag, void *ind_msg_buf, uint32_t ind_msg_len)
{
    switch(ind_flag)
    {
        case FIBO_SMS_INIT_OK_IND:
        {
            break;
        }
        case FIBO_SMS_NEW_MSG_IND:
        {
            fibo_sms_msg_t *sms_msg = (fibo_sms_msg_t *)ind_msg_buf;

            fibo_textTrace("[%s-%d] sms_msg->dcs:%d", __FUNCTION__, __LINE__, sms_msg->dcs);
            fibo_textTrace("[%s-%d] sms_msg->phoneNum:%s", __FUNCTION__, __LINE__, sms_msg->phoneNum);
            if(sms_msg->dcs == AT_MSG_UCS2)
            {
                char ascii_str[176] = {0};
                ucs2_to_ascii_string((const unsigned short *)sms_msg->message, ascii_str, sms_msg->msglen);
                fibo_textTrace("[%s-%d] sms_msg->message:%s", __FUNCTION__, __LINE__, ascii_str);
            }
            else
            {
                fibo_textTrace("[%s-%d] sms_msg->message:%s", __FUNCTION__, __LINE__, sms_msg->message);
            }
            break;
        }
        case FIBO_SMS_NEW_PDU_MSG_IND:
        {
            fibo_sms_pdu_msg_t *sms_msg = (fibo_sms_pdu_msg_t *)ind_msg_buf;
            fibo_textTrace("[%s-%d]sms_msg->nIndex: %d sms_msg->msglen: %d", __FUNCTION__, __LINE__,sms_msg->nIndex , sms_msg->msglen);
            fibo_textTrace("pdu message: %s", sms_msg->message);
            break;
        }
        case FIBO_SMS_LIST_IND:
        {
            break;
        }
        case FIBO_SMS_LIST_EX_IND:
        {
            break;
        }
        case FIBO_SMS_LIST_END_IND:
        {
            break;
        }
        case FIBO_SMS_MEM_FULL_IND:
        {
            break;
        }
        case FIBO_SMS_REPORT_IND:
        {
            break;
        }
        default :
            break;
    }
}


static void hello_world_demo()
{
    fibo_textTrace("application thread enter");
    fibo_taskSleep(30*1000);

    fibo_sms_service_center_cfg_t service_center;
    memset(&service_center, 0, sizeof(fibo_sms_service_center_cfg_t));
    memcpy(service_center.service_center_addr,"+8613010815500",strlen("+8618064310809"));
    memcpy(service_center.service_center_addr_type,"145",strlen("129"));

    fibo_sms_setsmscenteraddress(0, &service_center);

    memset(&service_center, 0, sizeof(fibo_sms_service_center_cfg_t));
    fibo_sms_getsmscenteraddress(0, &service_center);

    OSI_PRINTFI("fibo_sms_getsmscenteraddress1: %s", service_center.service_center_addr);
    OSI_PRINTFI("fibo_sms_getsmscenteraddress2: %s", service_center.service_center_addr_type);
    fibo_sms_addrxmsghandler(0, user_sms_event_callback);

    //get message storage info
    fibo_taskSleep(2000);
    fibo_sms_store_max_size_info_t pt_info = {0};
    fibo_sms_get_store_max_size_info_t pt_sms_storage = {0};
    pt_sms_storage.e_storage = E_FIBO_SMS_STORAGE_TYPE_UIM;
    ret = fibo_sms_getmaxstoresize(0, &pt_sms_storage, &pt_info);
    OSI_PRINTFI("fibo_sms_getmaxstoresize SM ret = %d,max:%d,free:%d", ret,pt_info.store_max_size,pt_info.free_slots);
    
    memset(&pt_info, 0, sizeof(fibo_sms_store_max_size_info_t));
    pt_sms_storage.e_storage = E_FIBO_SMS_STORAGE_TYPE_NV;
    ret = fibo_sms_getmaxstoresize(0, &pt_sms_storage, &pt_info);
    OSI_PRINTFI("fibo_sms_getmaxstoresize ME ret = %d,max:%d,free:%d", ret,pt_info.store_max_size,pt_info.free_slots);

    //SEND PDU SMS
    OSI_PRINTFI("PDU SMS:fibo_sms_send_smspdu");
    int ret = 0;
    int str_len;
    fibo_sms_send_raw_message_data_t raw_message_data = {0};
    fibo_sms_raw_send_resp_t raw_resp = {0};
    char *msg_buf = "0001000D91688160340108F9000007E6B4F83D7EB701";
    raw_message_data.raw_message_len = 21;    //not str len.Refer to [3GPP TS 27.007] CMGS->length
    str_len = strlen(msg_buf);
    memcpy(&raw_message_data.raw_message[0],msg_buf,str_len);
    ret = fibo_sms_send_smspdu( 0, raw_message_data, &raw_resp);
    OSI_LOGI(0, "ret: %d , message_id: %d", ret, raw_resp.message_id);

    //SEND TEXT SMS
    OSI_PRINTFI("TEXT SMS:fibo_sms_send_message");
    fibo_sms_send_msg_info_t *sms_info = malloc(sizeof(fibo_sms_send_msg_info_t));
    memset(sms_info, 0, sizeof(fibo_sms_send_msg_info_t));
    sms_info->format = 0;
    sms_info->message_class_valid = 0;
    memcpy(sms_info->mobile_number, "18702911761", strlen("18702911761"));
    memcpy(sms_info->message_text, "fibocom", strlen("fibocom"));
    sms_info->length = strlen("fibocom");
    ret = fibo_sms_send_message(0, sms_info);
    OSI_PRINTFI("fibo_sms_send_message ret:%d", ret);


    //READ SMS LIST
    fibo_sms_msg_list_info_t resp_info = {0};
    ret = fibo_sms_getmsglist(0, NULL, &resp_info);
    fibo_textTrace("resp_info.message_list_length = %d", resp_info.message_list_length);
    for(int i = 0; i < resp_info.message_list_length; i++)
    {
        fibo_textTrace("message_list[%d]: index(%d),tag_type(%d)", i, resp_info.message_list[i].message_index, resp_info.message_list[i].tag_type);
    }

    //READ SMS code demo
    //read text sms
    fibo_sms_read_message_info_t read_info;
    fibo_sms_read_message_req_info_t read_req_info;
    memset(&read_info, 0, sizeof(fibo_sms_read_message_info_t));
    memset(&read_req_info, 0, sizeof(fibo_sms_read_message_req_info_t));
    read_req_info.storage_index = 1;
    read_req_info.storage_type = 0;
    fibo_sms_read_message(0, &read_info, &read_req_info);
    OSI_PRINTFE("fibo_sms_read_message,format:%d",read_info.format);
    OSI_PRINTFE("fibo_sms_read_message,read_info.message_text:%s",read_info.message_text);
    OSI_PRINTFE("fibo_sms_read_message,read_info.mobile_number:%s",read_info.mobile_number);
    OSI_PRINTFE("fibo_sms_read_message,index:%d",read_req_info.storage_index);
    OSI_PRINTFE("fibo_sms_read_message,time:%d:%d:%d:%d:%d:%d:%d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);

    //read pdu sms
    memset(&read_info, 0, sizeof(fibo_sms_read_message_info_t));
    memset(&read_req_info, 0, sizeof(fibo_sms_read_message_req_info_t));
    read_req_info.storage_index = 1;
    read_req_info.message_mode_valid = 1;
    read_req_info.message_mode = E_FIBO_SMS_MESSAGE_MODE_PDU;
    fibo_sms_read_message(0, &read_info, &read_req_info);
    OSI_PRINTFE("fibo_sms_read_message,format:%d",read_info.format);    //E_FIBO_SMS_FORMAT_PDU
    OSI_PRINTFE("fibo_sms_read_message,PDU DATA:%s",read_info.message_text);
    OSI_PRINTFE("fibo_sms_read_message,index:%d",read_req_info.storage_index);

    //DEL SMS code demo
    fibo_sms_storage_info_t sms_storage;
    //del a sms 
    sms_storage.e_storage=E_FIBO_SMS_STORAGE_TYPE_UIM;
    sms_storage.storage_idx = 1;
    ret = fibo_sms_deletefromstorage(0, &sms_storage);
    OSI_PRINTFE("fibo_sms_deletefromstorage,ret:%d",ret);

    //del all sms 
    sms_storage.e_storage=E_FIBO_SMS_STORAGE_TYPE_UIM;
    sms_storage.storage_idx = FIBO_SMS_DEL_ALL_STORAGE;
    ret = fibo_sms_deletefromstorage(0, &sms_storage);
    OSI_PRINTFE("fibo_sms_deletefromstorage,ret:%d",ret);

}


void *appimg_enter(void *param)
{
    fibo_textTrace("app image enter");

    fibo_thread_create(hello_world_demo, "mythread", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    return (void *)&user_callback;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}

