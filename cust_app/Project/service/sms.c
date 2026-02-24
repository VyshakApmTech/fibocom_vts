#include "sms.h"
#include <string.h>
#include "osi_log.h"
#include "fibo_opencpu.h"
#include "../common/log.h"

#define TAG "SMS"

//============================================================================
// Global Variables (EXACT match to your sms.h)
//============================================================================
char SMSSender[30] = {0};
uint16_t IsSMS = 0;
char SMSData[400] = {0};
char SimData[400] = {0};

#if defined(PROTO_MAHARASHTRA1) || defined(PROTO_OG)
uint8_t IsSRCMD = 0;
char CMD_Buff[400] = {0};
#endif

//============================================================================
// SMS Callback Function - Matches fibo_sms_rxmsg_handler_func_t
//============================================================================
static void sms_rx_callback(uint32_t ind_flag, void *ind_msg_buf, uint32_t ind_msg_len)
{
    // Check what type of indication this is
    switch (ind_flag)
    {
        case 2: // FIBO_SMS_NEW_MSG_IND - New text message
        {
            fibo_sms_msg_t *msg = (fibo_sms_msg_t*)ind_msg_buf;
            
            if (msg)
            {
                LOGI(TAG, "New SMS received at index: %d", msg->nIndex);
                
                // Store the index for later reading
                // For now, just set flag - main thread will read all unread
                IsSMS = 1;
            }
            break;
        }
        
        case 3: // FIBO_SMS_NEW_PDU_MSG_IND - New PDU message
        {
            fibo_sms_pdu_msg_t *pdu_msg = (fibo_sms_pdu_msg_t*)ind_msg_buf;
            LOGI(TAG, "New PDU SMS received");
            IsSMS = 1;
            break;
        }
        
        case 6: // FIBO_SMS_MEM_FULL_IND - Storage full
        {
            LOGW(TAG, "SMS storage full");
            break;
        }
        
        default:
            LOGD(TAG, "SMS indication: %ld", ind_flag);
            break;
    }
}

//============================================================================
// SendSMS - Using correct Fibocom API
//============================================================================
void SendSMS(char* ph, char* msg)
{
    LOGI(TAG, "SMS Sending! To: %s", ph);

    if (ph == NULL || msg == NULL || strlen(ph) == 0 || strlen(msg) == 0)
    {
        LOGE(TAG, "Invalid phone number or message.");
        return;
    }

    // Prepare SMS info structure
    fibo_sms_send_msg_info_t sms_info = {0};
    
    // Set format (0 = GSM 7-bit default alphabet)
    sms_info.format = E_FIBO_SMS_FORMAT_GSM_7BIT;
    
    // Set mode
    sms_info.mode = FIBO_SMS_MSG_FORMAT_GW_PP_V01;
    
    // Set phone number
    strncpy((char*)sms_info.mobile_number, ph, FIBO_SMS_MAX_ADDR_LENGTH_V01);
    
    // Set message content
    strncpy((char*)sms_info.message_text, msg, FIBO_SMS_MAX_MO_MSG_LENGTH_V01);
    sms_info.length = strlen(msg);
    
    // Send using SIM 0
    int ret = fibo_sms_send_message(0, &sms_info);
    
    if (ret == 0)
    {
        LOGI(TAG, "SMS sent successfully");
    }
    else
    {
        LOGE(TAG, "SMS send failed, ret=%d", ret);
    }
}

//============================================================================
// Delete All Read Messages (Simple Version)
//============================================================================
void delete_read_messages(void)
{
    fibo_sms_msg_list_info_t msg_list = {0};
    uint32_t deleted = 0;
    
    // Get all messages
    if (fibo_sms_getmsglist(0, NULL, &msg_list) != 0)
    {
        LOGE(TAG, "Failed to get message list");
        return;
    }
    
    LOGI(TAG, "Found %ld total messages", msg_list.message_list_length);
    
    // Delete READ messages
    for (uint32_t i = 0; i < msg_list.message_list_length; i++)
    {
        if (msg_list.message_list[i].tag_type == 0) // READ
        {
            uint32_t index = msg_list.message_list[i].message_index;
            
            fibo_sms_storage_info_t delete_info = {0};
            delete_info.e_storage = E_FIBO_SMS_STORAGE_TYPE_UIM;
            delete_info.storage_idx_valid = 1;
            delete_info.storage_idx = index;
            
            if (fibo_sms_deletefromstorage(0, &delete_info) == 0)
            {
                deleted++;
                LOGI(TAG, "Deleted READ message at index %ld", index);
            }
        }
    }
    
    LOGI(TAG, "Deleted %ld read messages", deleted);
}

//============================================================================
// Display All Messages in SIM Storage
//============================================================================
void check_all_messages(void)
{
    fibo_sms_msg_list_req_info_t req_info = {0};
    fibo_sms_msg_list_info_t msg_list = {0};
    
    // Set storage type to SIM (UIM)
    req_info.storage_type = E_FIBO_SMS_STORAGE_TYPE_UIM;
    // NO tag_type filter = get ALL messages
    
    LOGI(TAG, "Getting all messages from SIM storage...");
    
    int ret = fibo_sms_getmsglist(0, &req_info, &msg_list);
    if (ret != 0)
    {
        LOGE(TAG, "Failed to get message list, ret=%d", ret);
        return;
    }
    
    // Display total number of messages
    LOGI(TAG, "===================================");
    LOGI(TAG, "TOTAL MESSAGES IN SIM: %ld", msg_list.message_list_length);
    LOGI(TAG, "===================================");
    
    // Display each message index and tag type
    for (uint32_t i = 0; i < msg_list.message_list_length; i++)
    {
        uint32_t index = msg_list.message_list[i].message_index;
        uint32_t tag = msg_list.message_list[i].tag_type;
        
        const char* tag_str = "";
        switch(tag) {
            case 0: tag_str = "READ"; break;
            case 1: tag_str = "UNREAD"; break;
            case 2: tag_str = "SENT"; break;
            case 3: tag_str = "UNSENT"; break;
            default: tag_str = "UNKNOWN";
        }
        
        LOGI(TAG, "Message %ld: Index=%ld, Type=%s", i+1, index, tag_str);
    }
    
    LOGI(TAG, "===================================");
}

//============================================================================
// SMS Initialization
//============================================================================
void sms_init(void)
{
    LOGI(TAG, "SMS Service Initializing");
    
    // Register SMS callback
    int ret = fibo_sms_addrxmsghandler(0, sms_rx_callback);
    if (ret != 0)
    {
        LOGE(TAG, "Failed to register SMS callback, ret=%d", ret);
        return;
    }
    
    LOGI(TAG, "SMS Service Initialized");
    
    // Check for any unread messages at boot
    check_all_messages();
}

//============================================================================
// Keep Your Existing Functions (from Quectel code)
//============================================================================

// Your existing DecodeSMS from Quectel - will be linked
uint8_t DecodeSMS(char* msg, uint8_t IsServer)
{
    // This comes from your existing code
    return 0;
}

void SendSOSSMS(uint8_t isFall)
{
    LOGI(TAG, "SendSOSSMS: %d", isFall);
}

// GetValueFromData - Keep your existing implementation
int GetValueFromData(char* data, char* cmd, char delim1, int delimPos, char delim2, char* value)
{
    // Your existing code here
    return 0;
}

// Other functions as needed...