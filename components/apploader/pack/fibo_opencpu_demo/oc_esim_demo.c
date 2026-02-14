#include "osi_api.h"
#include "osi_log.h"

char *app_ver = "APP_0001";

#include "app_image.h"
#include "fibo_opencpu.h"


#define fibo_textTrace(format, ...)                                                              \
    do                                                                                           \
    {                                                                                            \
        fibo_textTrace("[%s:%d]" format "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0);


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
    for (int n = 0; n < 30; n++)
    {
        fibo_textTrace("app image hello world %d", n);
        fibo_taskSleep(500);
    }
    UINT8 imsi[20] = {0};
    int32_t ret = fibo_get_imsi_by_simid(imsi,0);
    if(ret == 0)
    {
        fibo_textTrace("imsi %s",imsi);
    }
    char* aid_value = "A0000000871002FF86FF0389938F4FE1";
    uint8_t session_id = 0;
    ret = fibo_sim_esim_logical_channel_open(aid_value,&session_id);
    fibo_textTrace("fibo_sim_esim_logical_channel_open ret %d  session_id %d", ret,session_id);
    
     char* apdu_cmd = "0053000000";
     int apdu_length = 10;
     char response[522]={0}; 
     int resp_length = 522;
     ret  = fibo_sim_esim_channel_apdu_send(session_id,apdu_cmd,apdu_length,response,resp_length);
     fibo_textTrace("fibo_sim_esim_logical_channel_open ret %d  response %s", ret,response);

     for (int n = 0; n < 10; n++)
     {
         fibo_textTrace("app image hello world %d", n);
         fibo_taskSleep(500);
     }
     ret = fibo_get_imsi_by_simid(imsi,0);
     if(ret == 0)
     {
        fibo_textTrace("imsi %s",imsi);
     }

     ret = fibo_sim_esim_logical_channel_open(aid_value,&session_id);
     fibo_textTrace("fibo_sim_esim_logical_channel_open 2 ret %d  session_id %d", ret,session_id);
     ret = fibo_sim_esim_logical_channel_close(session_id);
     fibo_textTrace("fibo_sim_esim_logical_channel_close ret %d",ret);
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
