#include "osi_api.h"
#include "osi_log.h"

char *app_ver = "APP_0001";

#include "app_image.h"
#include "fibo_opencpu.h"

#define APP_LOG_INFO(format, ...) fibo_textTrace("[app][info][%s:%d]-" format " \n", __FUNCTION__, __LINE__, ##__VA_ARGS__)


typedef enum _AT_MSG_ALPHABET
{
    AT_MSG_GSM_7 = 0,
    AT_MSG_8_BIT_DATA,
    AT_MSG_UCS2,
    AT_MSG_RESERVED,
} AT_MSG_ALPHABET;


#define TESTNUM "15002931597"

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

static void hello_world_demo()
{
    fibo_textTrace("application thread enter");
    set_app_ver(app_ver);
    for (int n = 0; n < 10; n++)
    {
        fibo_textTrace("app image hello world %d", n);
        fibo_taskSleep(500);
    }
}

static UINT32 g_sig_sms_sem = 0;
static uint16_t g_sms_index = 0;



static char ucs2_to_ascii(unsigned short ucs2_char)
{
    fibo_textTrace("[%s-%d] %X", __FUNCTION__, __LINE__, ucs2_char);
    if ((ucs2_char & 0xff) == 0x00) {
        return (char)((ucs2_char >> 8) & 0xFF);
    } else {
        return '?';  
    }
}



static void ucs2_to_ascii_string(const unsigned short *ucs2_str, char *ascii_str, size_t length)
{
    for (size_t i = 0; i < length; ++i) {
        ascii_str[i] = ucs2_to_ascii(ucs2_str[i]);
    }
    ascii_str[length] = '\0';  
}





static void sms_callback(uint32_t ind_flag, void *ind_msg_buf, uint32_t ind_msg_len)
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
             g_sms_index =  sms_msg->nIndex;
            fibo_textTrace("[%s-%d] sms_msg->dcs:%d msglen %d", __FUNCTION__, __LINE__, sms_msg->dcs, sms_msg->msglen);
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
            g_sms_index =  sms_msg->nIndex;
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
            e_fibo_sms_storage_type_t *storage  = (e_fibo_sms_storage_type_t *)ind_msg_buf;
            fibo_textTrace("sms mem full : %d", *storage);
            break;
        }
        case FIBO_SMS_REPORT_IND:
        {
            break;
        }
        default :
            break;
    }
    if((FIBO_SMS_NEW_PDU_MSG_IND == ind_flag)||(FIBO_SMS_NEW_MSG_IND == ind_flag))
    {
        if(g_sig_sms_sem)
        {
            fibo_sem_signal(g_sig_sms_sem);
        }

    }
}


void send_and_rev_sms_demo()
{
    fibo_textTrace("application thread enter");

    for (int n = 0; n < 20; n++)
    {
        fibo_textTrace("app image hello world %d", n);
        fibo_taskSleep(500);
    }

    int ret = 0;
    g_sig_sms_sem = fibo_sem_new(0);
    fibo_textTrace("fibo_sms_send_message ret:%d", ret);
    fibo_sms_addrxmsghandler(0, sms_callback);

    fibo_sms_storage_info_t pt_sms_storage = {0};
    pt_sms_storage.storage_idx = 0xFFFFFFFF;
    pt_sms_storage.e_storage = 0;
    ret = fibo_sms_deletefromstorage(0, &pt_sms_storage);
    APP_LOG_INFO("fibo_sms_deletefromstorage ret = %d", ret);
    


        //SEND PDU SMS
    fibo_textTrace("PDU SMS:fibo_sms_send_smspdu");
  
    int str_len;
    fibo_sms_send_raw_message_data_t raw_message_data = {0};
    fibo_sms_raw_send_resp_t raw_resp = {0};
    char *msg_buf = "0001000D91688107921167F100088C0031003200330034003500360037003800390030003100320033003400350036003700380039003000310032003300340035003600370038003900300031003200330034003500360037003800390030003100320033003400350036003700380039003000310032003300340035003600370038003900300031003200330034003500360037003800390030";    //UCS2 format PDU SMS
    //char *msg_buf = "0891683108705505F001000D91685128799155F200088C0031003200330034003500360037003800390030003100320033003400350036003700380039003000310032003300340035003600370038003900300031003200330034003500360037003800390030003100320033003400350036003700380039003000310032003300340035003600370038003900300031003200330034003500360037003800390030";    //UCS2 format PDU SMS
    raw_message_data.raw_message_len = 154;    //not str len.Refer to [3GPP TS 27.007] CMGS->length
    str_len = strlen(msg_buf);
    memcpy(&raw_message_data.raw_message[0],msg_buf,str_len);
    ret = fibo_sms_send_smspdu( 0, raw_message_data, &raw_resp);
    fibo_textTrace("ret: %d , message_id: %d", ret, raw_resp.message_id);

    fibo_sem_wait(g_sig_sms_sem);
    fibo_textTrace("rev sms");

    fibo_sms_read_message_info_t  read_info = {0};
    fibo_sms_read_message_req_info_t read_req_info = {0};

    read_req_info.storage_index = g_sms_index;
    read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
    read_req_info.message_mode_valid = 1;
    read_req_info.message_mode = E_FIBO_SMS_MESSAGE_MODE_PDU;
    ret = fibo_sms_read_message(0, &read_info, &read_req_info);
    APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
        ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
    if(read_info.data_head_valid)
    {
        APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
    }

    if(read_info.timestamp_valid)
    {
        APP_LOG_INFO("timestamp %d/%02d/%02d,%02d:%02d:%02d%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);
    
    }

    read_req_info.storage_index = g_sms_index;
    read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
    read_req_info.message_mode_valid = 0;
    ret = fibo_sms_read_message(0, &read_info, &read_req_info);
    APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
        ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
    if(read_info.data_head_valid)
    {
        APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
    }
    fibo_textTrace("sms timestamp_valid:%d", read_info.timestamp_valid);
    if(read_info.timestamp_valid)
    {
         APP_LOG_INFO("timestamp %d/%02d/%02d,%02d:%02d:%02d%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);
    
    }

     //delete sms
    pt_sms_storage.storage_idx = g_sms_index;
    pt_sms_storage.e_storage = E_FIBO_SMS_STORAGE_TYPE_UIM;
    ret = fibo_sms_deletefromstorage(0, &pt_sms_storage);
    APP_LOG_INFO("fibo_sms_deletefromstorage ret = %d", ret);

    //send text message
    fibo_sms_send_msg_info_t sms_info = {0};
    //char sms_text[] = "123";
    char sms_text[] = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";

    sms_info.format = E_FIBO_SMS_FORMAT_GSM_7BIT;
    sms_info.mode = FIBO_SMS_MSG_FORMAT_GW_PP_V01;
    sms_info.message_class_valid = false;
    sms_info.message_class = 0;
    strcpy((char *)sms_info.mobile_number ,"18702911761");//xxx... is phone number
    strcpy((char *)sms_info.message_text,sms_text);
    sms_info.length = strlen(sms_text);

    ret = fibo_sms_send_message(0, &sms_info);
    APP_LOG_INFO("fibo_sms_send_message ret = %d", ret);

    fibo_sem_wait(g_sig_sms_sem);
    APP_LOG_INFO("rev txt sms");
    read_req_info.storage_index = g_sms_index;
    read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
    read_req_info.message_mode_valid = 1;
    read_req_info.message_mode = E_FIBO_SMS_MESSAGE_MODE_PDU;
    ret = fibo_sms_read_message(0, &read_info, &read_req_info);
    APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
        ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
    if(read_info.data_head_valid)
    {
        APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
    }

    if(read_info.timestamp_valid)
    {
        APP_LOG_INFO("timestamp %d/%02d/%02d,%02d:%02d:%02d%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);
    
    }

    read_req_info.storage_index = g_sms_index;
    read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
    read_req_info.message_mode_valid = 0;
    ret = fibo_sms_read_message(0, &read_info, &read_req_info);
    APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
        ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
    if(read_info.data_head_valid)
    {
        APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
    }
    fibo_textTrace("sms timestamp_valid:%d", read_info.timestamp_valid);
    if(read_info.timestamp_valid)
    {
         APP_LOG_INFO("timestamp %d/%02d/%02d,%02d:%02d:%02d%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);
    
    }
  
}


static void sms_send_sms_text_demo() 
{
    int ret = 0;

    fibo_sms_addrxmsghandler(0, sms_callback);

   //send text msg  
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        fibo_textTrace("Hello world %d", i);
        fibo_taskSleep(1000);
    }
    g_sig_sms_sem = fibo_sem_new(0);

    fibo_sms_send_msg_info_t sms_info = {0};
    //char sms_text1[] = "0031003200330034003500360037003800390030";
   // char sms_text[] = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
  
     fibo_sms_storage_info_t pt_sms_storage = {0};
     pt_sms_storage.storage_idx = 0xFFFFFFFF;
     pt_sms_storage.e_storage = 0;
     ret = fibo_sms_deletefromstorage(0, &pt_sms_storage);
     APP_LOG_INFO("fibo_sms_deletefromstorage ret = %d", ret);


     
     sms_info.format = E_FIBO_SMS_FORMAT_UCS2;
     sms_info.mode = FIBO_SMS_MSG_FORMAT_GW_PP_V01;
     sms_info.message_class_valid = true;
     sms_info.message_class = FIBO_SMS_MESSAGE_CLASS_1_V01;
     strcpy((char *)sms_info.mobile_number ,TESTNUM);//
     strcpy((char *)sms_info.message_text, "00400040");//@@
     sms_info.length = strlen((char *)sms_info.message_text);
     ret = fibo_sms_send_message(0, &sms_info);
     fibo_textTrace("fibo_sms_send_message ret = %d", ret);

    fibo_sem_wait(g_sig_sms_sem);
    APP_LOG_INFO("rev txt sms");


    fibo_sms_read_message_info_t  read_info = {0};
    fibo_sms_read_message_req_info_t read_req_info = {0};

    read_req_info.storage_index = g_sms_index;
    read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
    read_req_info.message_mode_valid = 0;
    ret = fibo_sms_read_message(0, &read_info, &read_req_info);
    APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
    ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
    if(read_info.data_head_valid)
    {
        APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
    }

    if(read_info.timestamp_valid)
    {
        APP_LOG_INFO("timestamp %d%02d%02d-%02d%02d%02d-%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);
    
    }


     sms_info.format = E_FIBO_SMS_FORMAT_UCS2;
     sms_info.mode = FIBO_SMS_MSG_FORMAT_GW_PP_V01;
     sms_info.message_class_valid = true;
     sms_info.message_class = FIBO_SMS_MESSAGE_CLASS_1_V01;
     strcpy((char *)sms_info.mobile_number,TESTNUM);//
     strcpy((char *)sms_info.message_text, "4F60597D");
     sms_info.length = strlen((char *)sms_info.message_text);
     ret = fibo_sms_send_message(0, &sms_info);
     fibo_textTrace("fibo_sms_send_message ret = %d", ret);

    fibo_sem_wait(g_sig_sms_sem);
    APP_LOG_INFO("rev txt sms");

   int cscs =  E_FIBO_SMS_FORMAT_UCS2;

  
    ret = fibo_sms_set_config(0,FIBO_READ_TEXT_SMS_FORMAT,&cscs,NULL);

    read_req_info.storage_index = g_sms_index;
    read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
    read_req_info.message_mode_valid = 0;
    ret = fibo_sms_read_message(0, &read_info, &read_req_info);
    APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
    ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);

    if(read_info.data_head_valid)
    {
        APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
    }

    if(read_info.timestamp_valid)
    {
        APP_LOG_INFO("timestamp %d%02d%02d-%02d%02d%02d-%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);
    
    }


    read_req_info.storage_index = g_sms_index;
    read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
    read_req_info.message_mode_valid = 1;
    read_req_info.message_mode = E_FIBO_SMS_MESSAGE_MODE_PDU;
    ret = fibo_sms_read_message(0, &read_info, &read_req_info);
    APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
    ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
    if(read_info.data_head_valid)
    {
        APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
    }

    if(read_info.timestamp_valid)
    {
        APP_LOG_INFO("timestamp %d%02d%02d-%02d%02d%02d-%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);
    
    }

    int j = 0;
    fibo_sms_msg_list_req_info_t req_info = {0};
    fibo_sms_msg_list_info_t resp_info = {0};
        
    req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
    req_info.tag_type = E_FIBO_SMS_TAG_TYPE_MT_READ_V01;
    ret = fibo_sms_getmsglist(0, &req_info, &resp_info);
    if(ret == 0)
    {
            APP_LOG_INFO("list count = %d", resp_info.message_list_length);
            for(j = 0;j<resp_info.message_list_length;j++)
            {
                APP_LOG_INFO("tag_type:%d index = %d", resp_info.message_list[j].tag_type,resp_info.message_list[j].message_index);
            }
    }
        //APP_LOG_INFO("fibo_sms_getmsglist type %d,ret = %d", i,ret);
        
        
        //delete sms
        pt_sms_storage.storage_idx = g_sms_index;
        pt_sms_storage.e_storage = E_FIBO_SMS_STORAGE_TYPE_UIM;
        ret = fibo_sms_deletefromstorage(0, &pt_sms_storage);
        APP_LOG_INFO("fibo_sms_deletefromstorage ret = %d", ret);

        sms_info.format = E_FIBO_SMS_FORMAT_GSM_7BIT;
        sms_info.mode = FIBO_SMS_MSG_FORMAT_GW_PP_V01;
        sms_info.message_class_valid = true;
        sms_info.message_class = FIBO_SMS_MESSAGE_CLASS_1_V01;
        strcpy((char *)sms_info.mobile_number ,TESTNUM);//
        strcpy((char *)sms_info.message_text,"@@@@");
        sms_info.length = strlen((char *)sms_info.message_text);
        ret = fibo_sms_send_message(0, &sms_info);
        APP_LOG_INFO("fibo_sms_send_message ret = %d", ret);

        fibo_sem_wait(g_sig_sms_sem);
        APP_LOG_INFO("rev txt sms");

        read_req_info.storage_index = g_sms_index;
        read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
        read_req_info.message_mode_valid = 1;
        read_req_info.message_mode = E_FIBO_SMS_MESSAGE_MODE_PDU;
        ret = fibo_sms_read_message(0, &read_info, &read_req_info);
        APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
        ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
        if(read_info.data_head_valid)
        {
            APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
        }

        if(read_info.timestamp_valid)
        {
            APP_LOG_INFO("timestamp %d%02d%02d-%02d%02d%02d-%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);
    
        }
        cscs =  E_FIBO_SMS_FORMAT_GSM_7BIT;
        
          
        ret = fibo_sms_set_config(0,FIBO_READ_TEXT_SMS_FORMAT,&cscs,NULL);


        read_req_info.storage_index = g_sms_index;
        read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
        read_req_info.message_mode_valid = 0;
        ret = fibo_sms_read_message(0, &read_info, &read_req_info);
        APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
        ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
        if(read_info.data_head_valid)
        {
            APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
        }

        if(read_info.timestamp_valid)
        {
            APP_LOG_INFO("timestamp %d%02d%02d-%02d%02d%02d-%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);

        }


  
}


void receive_long_sms_demo()
{
    int i = 0;
    for (i = 0; i < 5; i++)
    {
        APP_LOG_INFO("Hello world %d", i);
        fibo_taskSleep(1000);
    }

    int ret = 0;

     g_sig_sms_sem = fibo_sem_new(0);
     //del all message
     //send pdu message
    ret = fibo_sms_addrxmsghandler(0, sms_callback);
    APP_LOG_INFO("fibo_sms_addrxmsghandler ret = %d", ret);

    while (1)
    {
        fibo_sms_read_message_info_t  read_info = {0};
        fibo_sms_read_message_req_info_t read_req_info = {0};
        fibo_sem_wait(g_sig_sms_sem);
        APP_LOG_INFO("rev sms");
        read_req_info.storage_index = g_sms_index;
        read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
        read_req_info.message_mode_valid = 0;
        ret = fibo_sms_read_message(0, &read_info, &read_req_info);
        APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
            ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
        if(read_info.data_head_valid)
        {
            APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
        }

        if(read_info.timestamp_valid)
        {
            APP_LOG_INFO("timestamp %d%02d%02d-%02d%02d%02d-%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);

        }

        read_req_info.storage_index = g_sms_index;
        read_req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
        read_req_info.message_mode_valid = 1;
        read_req_info.message_mode = E_FIBO_SMS_MESSAGE_MODE_PDU;
        ret = fibo_sms_read_message(0, &read_info, &read_req_info);
        APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
            ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
        if(read_info.data_head_valid)
        {
            APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
        }

        if(read_info.timestamp_valid)
        {
            APP_LOG_INFO("timestamp %d%02d%02d-%02d%02d%02d-%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);

        }


      }
    return ;
}


static void sms_storage_demo()
{
	int i = 0;
    for (i = 0; i < 15; i++)
    {
        APP_LOG_INFO("Hello world %d", i);
        fibo_taskSleep(1000);
    }

	int ret = 0;
    e_fibo_sms_storage_type_t sms_storage = E_FIBO_SMS_STORAGE_TYPE_NV;
    ret = fibo_sms_set_config(0,FIBO_SMS_STORAGETYPE,&sms_storage,NULL);
    APP_LOG_INFO("ret = %d", ret);
	sms_storage = E_FIBO_SMS_STORAGE_TYPE_NONE;
    ret = fibo_sms_get_config(0,FIBO_SMS_STORAGETYPE,NULL,&sms_storage);
    APP_LOG_INFO("ret = %d", ret);
    APP_LOG_INFO("sms_storage = %d", sms_storage);

    int j = 0;
    fibo_sms_msg_list_req_info_t req_info = {0};
    fibo_sms_msg_list_info_t resp_info = {0};
    ret = fibo_sms_getmsglist(0, NULL, &resp_info);
    if(ret == 0)
    {
        APP_LOG_INFO("list count = %d", resp_info.message_list_length);
        for(j = 0;j<resp_info.message_list_length;j++)
        {
            APP_LOG_INFO("tag_type:%d index = %d", resp_info.message_list[j].tag_type,resp_info.message_list[j].message_index);
        }
    }
    
    sms_storage = E_FIBO_SMS_STORAGE_TYPE_UIM;
  
    ret = fibo_sms_set_config(0,FIBO_SMS_STORAGETYPE,&sms_storage,NULL);
    APP_LOG_INFO("ret = %d", ret);
	sms_storage = E_FIBO_SMS_STORAGE_TYPE_NONE;
    ret = fibo_sms_get_config(0,FIBO_SMS_STORAGETYPE,NULL,&sms_storage);
    APP_LOG_INFO("ret = %d", ret);
    APP_LOG_INFO("sms_storage = %d", sms_storage);

    j = 0;
    memset(&resp_info,0x00,sizeof(resp_info));
    ret = fibo_sms_getmsglist(0, NULL, &resp_info);
    if(ret == 0)
    {
        APP_LOG_INFO("list count = %d", resp_info.message_list_length);
        for(j = 0;j<resp_info.message_list_length;j++)
        {
            APP_LOG_INFO("tag_type:%d index = %d", resp_info.message_list[j].tag_type,resp_info.message_list[j].message_index);
        }
    }



}


void receive_sms_demo()
{
    int i = 0;
    for (i = 0; i < 5; i++)
    {
        APP_LOG_INFO("Hello world %d", i);
        fibo_taskSleep(1000);
    }

    int ret = 0;

     g_sig_sms_sem = fibo_sem_new(0);
     //del all message
     //send pdu message
    ret = fibo_sms_addrxmsghandler(0, sms_callback);
    APP_LOG_INFO("fibo_sms_addrxmsghandler ret = %d", ret);
    while (1)
    {
        ret = 0;
        e_fibo_sms_storage_type_t sms_storage = E_FIBO_SMS_STORAGE_TYPE_NONE;
        ret = fibo_sms_get_config(0,FIBO_SMS_STORAGETYPE,NULL,&sms_storage);
        APP_LOG_INFO("ret = %d", ret);
        APP_LOG_INFO("sms_storage = %d", sms_storage);

        fibo_sms_read_message_info_t  read_info = {0};
        fibo_sms_read_message_req_info_t read_req_info = {0};
        fibo_sem_wait(g_sig_sms_sem);
        APP_LOG_INFO("rev sms");
        read_req_info.storage_index = g_sms_index;
        read_req_info.storage_type = sms_storage;
        read_req_info.message_mode_valid = 0;
        ret = fibo_sms_read_message(0, &read_info, &read_req_info);
        APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
            ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
        if(read_info.data_head_valid)
        {
            APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
        }

        if(read_info.timestamp_valid)
        {
            APP_LOG_INFO("timestamp %d%02d%02d-%02d%02d%02d-%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);

        }

        read_req_info.storage_index = g_sms_index;
        read_req_info.storage_type = sms_storage;
        read_req_info.message_mode_valid = 1;
        read_req_info.message_mode = E_FIBO_SMS_MESSAGE_MODE_PDU;
        ret = fibo_sms_read_message(0, &read_info, &read_req_info);
        APP_LOG_INFO("fibo_sms_read_message ret = %d,message_len = %d,message_body = %s,extra_da = %s,type = %d", 
            ret,read_info.length,read_info.message_text,read_info.mobile_number,read_info.tag_type);
        if(read_info.data_head_valid)
        {
            APP_LOG_INFO("total_segments %d,segment_index %d,ref_number %d",read_info.data_head.total_segments,read_info.data_head.seg_number,read_info.data_head.reference_number);
        }

        if(read_info.timestamp_valid)
        {
            APP_LOG_INFO("timestamp %d%02d%02d-%02d%02d%02d-%+d",read_info.timestamp.year,read_info.timestamp.month,read_info.timestamp.day,read_info.timestamp.hours,read_info.timestamp.minutes,read_info.timestamp.seconds,read_info.timestamp.timezone);

        }

        if(sms_storage == E_FIBO_SMS_STORAGE_TYPE_UIM)
        {
            sms_storage = E_FIBO_SMS_STORAGE_TYPE_NV;
        }
        else
        {
            sms_storage = E_FIBO_SMS_STORAGE_TYPE_UIM;
        }
        ret = fibo_sms_set_config(0,FIBO_SMS_STORAGETYPE,&sms_storage,NULL);
        APP_LOG_INFO("ret = %d", ret);

      }
    return ;
}



void *appimg_enter(void *param)
{
    fibo_textTrace("app image enter");

    fibo_thread_create(send_and_rev_sms_demo, "mythread", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    return (void *)&user_callback;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}
