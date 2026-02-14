#include "osi_api.h"
#include "osi_log.h"
#include "fibo_opencpu.h"


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

void hash_to_hex_string_raw(const uint8_t *hash, size_t len, char *hex_str) 
{
    for (size_t i = 0; i < len; i++) {
        // 高4位转字符
        uint8_t high = (hash[i] >> 4) & 0x0F;
        hex_str[i * 2] = high + (high < 10 ? '0' : 'a' - 10);
        
        // 低4位转字符
        uint8_t low = hash[i] & 0x0F;
        hex_str[i * 2 + 1] = low + (low < 10 ? '0' : 'a' - 10);
    }
    hex_str[len * 2] = '\0';
}

static void hello_world_demo()
{
    fibo_textTrace("application thread enter");
    for (int n = 0; n < 10; n++)
    {
        fibo_textTrace("WOMYDOWNLOAD %d", n);
        fibo_textTrace("app image hello world %d", n);
        fibo_taskSleep(500);
    }
    
    
    uint8_t data[] = "Hello, KFC!";
    uint8_t hash[16];
    uint8_t hex_str[33]={0};
    fibo_textTrace("oo2025");
    //1.fibo_ssl_sign
    int ret = fibo_ssl_sign(FIBO_MD_MD5,false,NULL,0,data,strlen((char*)data),hash);
    fibo_textTrace("haha2025");
    if(ret == 0)
    {
        fibo_textTrace("start print hex md5");
        hash_to_hex_string_raw(hash,16,hex_str);
        fibo_textTrace("end print hex md5");
        fibo_textTrace("md5 hash: %s",hex_str);
    }
    else
    {
        fibo_textTrace("md5 sum fail");
    }

    //2.fibo_base64_encode编码
    const char *original_string = "Hello, KFC!";
    size_t original_len = strlen(original_string);
    fibo_textTrace("original_string is %d",original_len);
    size_t encoded_len = 4 * ( (original_len + 2) / 3) + 16;
    fibo_textTrace("encoded_len is %d",encoded_len);
    unsigned char encoded_buffer[encoded_len + 1];
    size_t actual_encoded_len = 0;
    
    INT32 encode_result = fibo_base64_encode(encoded_buffer, encoded_len, &actual_encoded_len, 
                                       (const unsigned char *)original_string, original_len);
    fibo_textTrace("actual_encoded_len is %d",actual_encoded_len);
    if (encode_result != 0) 
    {
        fibo_textTrace("Encoding failed: %d", encode_result);
        return;
    }
    encoded_buffer[actual_encoded_len] = '\0';
    

    //3.fibo_base64_decode解码

    size_t max_decoded_len = 3 * (actual_encoded_len / 4)+16;
    unsigned char decoded_buffer[max_decoded_len + 1];
    size_t actual_decoded_len = 0;
    fibo_textTrace("max_decoded_len is %d",max_decoded_len);
    INT32 decode_result = fibo_base64_decode(decoded_buffer, max_decoded_len, &actual_decoded_len, 
                                       encoded_buffer, actual_encoded_len);
    
    if (decode_result != 0) 
    {
        fibo_textTrace("Decoding failed: %d", decode_result);
        return;
    }
    decoded_buffer[actual_decoded_len] = '\0';
    
    // 输出结果
    fibo_textTrace("Original: %s", original_string);
    fibo_textTrace("Encoded: %s", encoded_buffer);
    fibo_textTrace("Decoded: %s", decoded_buffer);
    
    // 验证
    if (strcmp(original_string, (char *)decoded_buffer))
    {
        fibo_textTrace("Verification failed!");
        return;
    }
    
    fibo_textTrace("All operations completed successfully!");
}

void *app_image_start(void)
{
    fibo_textTrace("app codes enter");
    fibo_thread_create(hello_world_demo, "mythread", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    return (void *)&user_callback;
}

