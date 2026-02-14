/* Copyright (C) 2024 FIBOCOM Technologies Limited and/or its affiliates("FIBOCOM").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * FIBOCOM assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. FIBOCOM reserves the right to make changes in the
 * software without notification.  FIBOCOM also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#include "fibo_opencpu.h"
#include "osi_log.h"
#include "osi_api.h"
#include "string.h"

/*
  * set the value to execute different process
  * 1 : whole app upgrade download by http
  * 2 : delta firmware upgrade download by ftp
  * 3 : delta app upgrade
  * 4 : delta app and delta firmware merge download and update
  */
#define TEST_TYPE                4

//Set callback for get download status and process
static void test_dl_callback(int32 status, uint32 len, uint32 total_len)
{
    fibo_textTrace("[OTA_V2] Download status: %d, Total_len: %d, Download len: %d", status, total_len, len);
    return;
}

static void prvInvokeGlobalCtors(void)
{
    extern void (*__init_array_start[])();
    extern void (*__init_array_end[])();

    size_t count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; ++i)
        __init_array_start[i]();
}


void whole_app_http_download_test()
{
    //Only download whole app upgrade package, don't upgrade
    int32 ret = 0;
    fibo_ota_download_msg_t dl_msg =
    {
        .dl_type = 0, //http
        .url = "http://xxx/test.img",//url
        .filename = NULL,
        .username = NULL,
        .password = NULL,
        .dl_callback = test_dl_callback //Set by customer, currently function is to get download status
    };

    fibo_ota_msg_t fibo_ota_msg =
    {
        .dir          = NULL,    /* 1. "/fota" by default, upgrade package will be restored to "/fota/whole_app.pack"
                                    2. Please note that the upgrade directory can be configured, but the upgrade
                                       package name cannot be configured! */
        .reboot       = true,                   //For download interface, this parameter does not take effect
        .fota_type    = FIBO_UPGRADE_WHOLE_APP, //Upgrade or download package type, refer to oc_fota_v2.h
        .ota_dl_msg_p = &dl_msg                 //Download parameters, NULL when upgrade
    };

    for (int n = 0; n < 5; n++)
    {
        fibo_textTrace("[OTA_V2] hello world %d", n);
        fibo_taskSleep(1000);   //Add sleep to wait for resource initialization complete
    }

    ret = fibo_dm_ota_download(&fibo_ota_msg);
    if(ret < 0)
    {
        //return value of fibo_dm_ota_download is error code
        fibo_textTrace("[OTA_V2] fibo_dm_ota_download whole app package failed: %d", ret);
    } else
    {
        fibo_textTrace("[OTA_V2] fibo_dm_ota_download whole app package success: %d", ret);
    }

    return;
}

void delta_firmware_ftp_download_test()
{
    //Only download delta firmware upgrade package, don't upgrade
    int32 ret = 0;
    fibo_ota_download_msg_t dl_msg =
    {
        .dl_type = 1, //ftp
        .url = "ftp://xxx.xxx.xxx.xxx",
        .filename = "/xxx/test.bin",
        .username = "test",
        .password = "test",
        .dl_callback = test_dl_callback
    };

    fibo_ota_msg_t fibo_ota_msg =
    {
        .dir = "/custfota", /* 1. Set directory, "dir" is parent directory, can't content package name;
                               2. fibo_dm_ota_download and fibo_dm_ota_upgrade interfaces will split joint 
                                  complete upgrade package path based on "dir" automatically;
                               3. upgrade package will be restored to "/custfota/fota.pack" */
        .reboot = false,
        .fota_type = FIBO_UPGRADE_DELTA_FW,
        .ota_dl_msg_p = &dl_msg
    };

    for (int n = 0; n < 5; n++)
    {
        fibo_textTrace("[OTA_V2] hello world %d", n);
        fibo_taskSleep(1000);
    }

    ret = fibo_dm_ota_download(&fibo_ota_msg);
    if(ret < 0)
    {
        fibo_textTrace("[OTA_V2] fibo_dm_ota_download delta firmware package failed: %d", ret);
    }else
    {
        fibo_textTrace("[OTA_V2] fibo_dm_ota_download delta firmware package success: %d", ret);
    }

    return;
}

void delta_app_upgrade_test()
{
    //Only upgrade delta app, don't download
    int32 ret = 0;
    fibo_ota_msg_t fibo_ota_msg =
    {
        .dir = "/custfota", /*1. Ensure that the upgrade package has been placed in "/custfota/delta_app.pack" before upgrade;
                              2. Package names corresponding to upgrade type as follows:
                                 delta firmware upgrade package name:                 fota.pack
                                 whole app upgrade package name:                      whole_app.pack
                                 delta app upgrade package name:                      delta_app.pack
                                 delta firmware and delta app upgrade package name:   df_da_merge.pack */
        .reboot = false,     //Won't reboot module after verify upgrade package successfully
        .fota_type = FIBO_UPGRADE_DELTA_APP,
        .ota_dl_msg_p = NULL
    };

    for (int n = 0; n < 5; n++)
    {
        fibo_textTrace("hello world %d", n);
        fibo_taskSleep(1000);
    }

    ret = fibo_dm_ota_upgrade(&fibo_ota_msg);
    if(ret < 0)
    {
        //return value of fibo_dm_ota_upgrade is error code
        fibo_textTrace("fibo_dm_ota_upgrade delta app failed: %d", ret);
    }else
    {
        fibo_textTrace("fibo_dm_ota_upgrade delta app success: %d", ret);
    }

    return;
}

void delta_firmware_delta_app_download_and_upgrade_test()
{
    //Download delta firmware delta app upgrade package, and upgrade
    int32 ret = 0;
    fibo_ota_download_msg_t dl_msg =
    {
        .dl_type = 0, //http
        .url = "http://xxx/test.img",
        .filename = NULL,
        .username = NULL,
        .password = NULL,
        .dl_callback = test_dl_callback
    };

    fibo_ota_msg_t fibo_ota_msg =
    {
        .dir = "/ext/fota", /* 1. Upgrade package will be restored to external flash "/ext/fota/df_da_merge.pack"
                               2. The external flash directory must be same as ext mount point in partinfo_xxx.json;
                               3. Recommend mounting external flash to "/ext" */
        .reboot = true,     //The module will reboot automatically
        .fota_type = FIBO_UPGRADE_DF_DA_MERGE,
        .ota_dl_msg_p = &dl_msg
    };

    for (int n = 0; n < 5; n++)
    {
        fibo_textTrace("[OTA_V2] hello world %d", n);
        fibo_taskSleep(1000);
    }

    ret = fibo_dm_ota_download(&fibo_ota_msg);
    if(ret < 0)
    {
        fibo_textTrace("[OTA_V2] fibo_dm_ota_download df_da_merge failed: %d", ret);
    }
    else
    {
        fibo_textTrace("[OTA_V2] fibo_dm_ota_download df_da_merge success: %d", ret);
        ret = fibo_dm_ota_upgrade(&fibo_ota_msg);
        if(ret < 0)
        {
            fibo_textTrace("[OTA_V2] fibo_dm_ota_upgrade df_da_merge failed: %d", ret);
        } else
        {
            fibo_textTrace("[OTA_V2] fibo_dm_ota_upgrade df_da_merge success: %d", ret);
        }
    }

    return;
}

void otaThreadEntry_ota(void *param)
{
    if(TEST_TYPE == 1)
        whole_app_http_download_test();
    else if(TEST_TYPE == 2)
        delta_firmware_ftp_download_test();
    else if(TEST_TYPE == 3)
        delta_app_upgrade_test();
    else if(TEST_TYPE == 4)
        delta_firmware_delta_app_download_and_upgrade_test();
    else
        fibo_textTrace("[OTA_V2] TEST_TYPE value is setting error, please set again!");

    fibo_thread_delete();
}


static void prvThreadEntry(void *param)
{
    reg_info_t reg_info;
    fibo_pdp_profile_t pdp_profile;

    memset(&pdp_profile, 0, sizeof(fibo_pdp_profile_t));
    pdp_profile.cid = 1;
    memcpy(pdp_profile.nPdpType, "IP", strlen("IP"));

    while(1)
    {
        fibo_reg_info_get(&reg_info, 0);
        fibo_taskSleep(1000);
        fibo_textTrace("[%s-%d]", __FUNCTION__, __LINE__);
        if (reg_info.nStatus == 1)
        {
            //ipv4
            fibo_pdp_active(1, &pdp_profile, 0);
            fibo_taskSleep(3000);
            break;
        }
    }
    fibo_taskSleep(1000);
    fibo_thread_create(otaThreadEntry_ota, "ota-thread", 1024 * 16, NULL, OSI_PRIORITY_NORMAL);

    fibo_thread_delete();
}


int appimg_enter(void *param)
{
    fibo_textTrace("[OTA_V2] application image enter, param 0x%x", param);

    prvInvokeGlobalCtors();

    fibo_thread_create(prvThreadEntry, "mythread", 1024 * 10, NULL, OSI_PRIORITY_NORMAL);
    return 0;
}


void appimg_exit(void)
{
    fibo_textTrace("application image exit");
}
