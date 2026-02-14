/* Copyright (C) 2022 FIBOCOM Technologies Limited and/or its affiliates("FIBOCOM").
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


#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')

#include "osi_log.h"
#include "osi_api.h"
#include "oc_socket.h"
#include "fibo_opencpu.h"
#define OC_SOC_LOG(format, ...) fibo_textTrace("[oc_sock][fibocom][%s:%d] " format, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "sockets.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/def.h"
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FIBO_DEMO_R_FAILED -1
#define FIBO_DEMO_R_SUCCESS 0

typedef enum fibo_connection_status_s
{
    FIBO_STATUS_IDLE = 0,
    FIBO_STATUS_CONNECTTING,
    FIBO_STATUS_CONNECT_OK,
    FIBO_STATUS_CONNECT_FAIL,
    FIBO_STATUS_CONNECT_DISCONNECTTD
} fibo_connection_status_t;

typedef enum fibo_pdp_status_s
{
    FIBO_PDP_STATUS_DEACTIVED = 0,
    FIBO_PDP_STATUS_ACTIVED,
    FIBO_PDP_STATUS_ACTIVING,
    FIBO_PDP_STATUS_DEACTIVING
} fibo_pdp_status_t;

typedef struct fibo_demo_connection_s
{
    int sock;
    fibo_connection_status_t status;
} fibo_demo_connection_t;

#define CHECK_EX(condition, err_code, format, ...)                                                                                                                                                        \
    do                                                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
        if (condition)                                                                                                                                                                                 \
        {                                                                                                                                                                                              \
            OC_SOC_LOG(format, ##__VA_ARGS__);                                                                                                                                                   \
            ret = err_code;                                                                                                                                                                            \
            goto error;                                                                                                                                                                                \
        }                                                                                                                                                                                              \
    } while (0);


static UINT32 g_sig_sem = 0;
static UINT32 g_sig_lock = 0;
static fibo_pdp_status_t g_pdp_status = FIBO_PDP_STATUS_DEACTIVED;

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    int cid  = 0;

    fibo_mutex_lock(g_sig_lock);
    switch (sig)
    {
        case GAPP_SIG_PDP_ACTIVE_IND:
            cid = va_arg(arg, int);
            OC_SOC_LOG("pdp active ok %d", cid);
            va_end(arg);

            if (g_pdp_status == FIBO_PDP_STATUS_ACTIVING)
            {
                g_pdp_status = FIBO_PDP_STATUS_ACTIVED;
                fibo_sem_signal(g_sig_sem);
            }
            else
            {
                OC_SOC_LOG("pdp status invalid %d", g_pdp_status);
            }
            break;

        case GAPP_SIG_PDP_ACTIVE_OR_DEACTIVE_FAIL_IND:
            cid = va_arg(arg, int);
            va_end(arg);

            if (g_pdp_status == FIBO_PDP_STATUS_ACTIVING)
            {
                OC_SOC_LOG("pdp active fail %d", cid);
                g_pdp_status = FIBO_PDP_STATUS_DEACTIVED;
                fibo_sem_signal(g_sig_sem);
            }
            else if (g_pdp_status == FIBO_PDP_STATUS_DEACTIVING)
            {
                OC_SOC_LOG("pdp deactive fail %d", cid);
                g_pdp_status = FIBO_PDP_STATUS_DEACTIVED;
                fibo_sem_signal(g_sig_sem);
            }
            else 
            {
                OC_SOC_LOG("pdp status invalid %d", g_pdp_status);
            }

            break;

        case GAPP_SIG_PDP_RELEASE_IND:
            cid = va_arg(arg, int);
            va_end(arg);

            if (g_pdp_status == FIBO_PDP_STATUS_DEACTIVING)
            {
                OC_SOC_LOG("pdp deactive ok %d", cid);
                g_pdp_status = FIBO_PDP_STATUS_DEACTIVED;
                fibo_sem_signal(g_sig_sem);
            }
            else
            {
                OC_SOC_LOG("pdp status invalid %d", g_pdp_status);
            }
            break;

        default:
            OC_SOC_LOG("recv sig %d", sig);
            break;
    }

    fibo_mutex_unlock(g_sig_lock);
}


static int connect_timeout(fibo_demo_connection_t *client, struct sockaddr *addr, int addr_len, int timeout)
{
    int ret = FIBO_DEMO_R_SUCCESS;
    struct timeval tm = {0};
    fd_set wset;
    FD_ZERO(&wset);
    FD_SET(client->sock, &wset);
    tm.tv_sec = timeout / 1000;
    tm.tv_usec = (timeout % 1000) * 1000;
    ret = fibo_sock_connect_v2(client->sock, addr, addr_len);

    if (ret != FIBO_DEMO_R_SUCCESS || fibo_get_socket_error() == EINPROGRESS)
    {
        if (fibo_get_socket_error() != EINPROGRESS)
        {
            OC_SOC_LOG("connect failed:%s,%d", strerror(fibo_get_socket_error()), fibo_get_socket_error());
            ret = FIBO_DEMO_R_FAILED;
        }
        else
        {
            ret = fibo_sock_select(client->sock + 1, NULL, &wset, NULL, timeout > 0 ? &tm : NULL);
            if (ret < 0)
            {
                OC_SOC_LOG("select failed:%s,%d", strerror(fibo_get_socket_error()), fibo_get_socket_error());
                ret = FIBO_DEMO_R_FAILED;
            }
            else if (ret == 0)
            {
                OC_SOC_LOG("connect time out");
                ret = FIBO_DEMO_R_FAILED;
            }
            else if (ret > 0)
            {
                if (!FD_ISSET(client->sock, &wset))
                {
                    OC_SOC_LOG("connect error when select:%s,%d", strerror(fibo_get_socket_error()), fibo_get_socket_error());
                    ret = FIBO_DEMO_R_FAILED;
                }
                else
                {
                    ret = FIBO_DEMO_R_SUCCESS;
                }
            }
        }
    }
    else
    {
        OC_SOC_LOG("tcp connect directly");
    }

    return ret;
}

int fibo_demo_send(fibo_demo_connection_t *c, const void *data, int len)
{
    int ret = FIBO_DEMO_R_SUCCESS;
    uint32_t sent = 0;
    struct timeval tm = {0};
    fd_set wset;

    const uint8_t *send_data = (const uint8_t *)data;
    while (sent < len)
    {
        ret = (int)fibo_sock_send_v2(c->sock, &send_data[sent], len - sent, 0);
        if (ret < 0)
        {
            if ((EAGAIN == fibo_get_socket_error()) || (EWOULDBLOCK == fibo_get_socket_error()))
            {
                OC_SOC_LOG("wait send buffer ava");
                FD_ZERO(&wset);
                FD_SET(c->sock, &wset);
                tm.tv_sec = 1;
                tm.tv_usec = 0;
                fibo_sock_select(c->sock + 1, NULL, &wset, NULL, &tm);
            }
            else
            {
                OC_SOC_LOG("send data failed:%s %d send=%d len=%u", strerror(fibo_get_socket_error()), fibo_get_socket_error(), (int)(len - sent), len);
                ret = FIBO_DEMO_R_FAILED;
                break;
            }
        }
        else
        {
            sent += (uint32_t)ret;
            OC_SOC_LOG("try send:send=%ld", sent);
        }
    }

    return ret;
}

int fibo_demo_connect(fibo_demo_connection_t *client, char *hostname, uint16_t port, int family, int proto, int timeout)
{
    int ret = FIBO_DEMO_R_SUCCESS;
    int non_block = 1;
    client->sock = -1;

    fibo_addrinfo_t *peer_addr = NULL;
    fibo_addrinfo_para paras = {hostname, 3000, AF_INET, NULL, NULL, &peer_addr, 20*1000};
    ret = fibo_get_addrinfo(&paras);
    CHECK_EX(ret != FIBO_DEMO_R_SUCCESS, FIBO_DEMO_R_FAILED, "get addr info failed: (%s)", hostname);

    for (fibo_addrinfo_t *cur = peer_addr; cur != NULL; cur = cur->ai_next)
    {
        client->sock = fibo_sock_socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (client->sock < 0)
        {
            OC_SOC_LOG("create socket failed:%s,%d", strerror(fibo_get_socket_error()), fibo_get_socket_error());
            ret = FIBO_DEMO_R_FAILED;
            continue;
        }

        ret = fibo_sock_ioctl(client->sock, FIONBIO, &non_block);
        CHECK_EX(ret != 0, FIBO_DEMO_R_FAILED, "set non block fail %d", ret);

        OC_SOC_LOG("connect with timeout timeout=%d", timeout);
        ret = connect_timeout(client, (struct sockaddr *)cur->ai_addr, cur->ai_addrlen, timeout);

        if (ret == FIBO_DEMO_R_SUCCESS)
        {
            OC_SOC_LOG("racoon connect ok");
            break;
        }
        fibo_sock_close_v2(client->sock);
        client->sock = -1;
        OC_SOC_LOG("try tcp connect failed:%s,%d", strerror(fibo_get_socket_error()), fibo_get_socket_error());
        ret = FIBO_DEMO_R_FAILED;
    }

    CHECK_EX(ret != FIBO_DEMO_R_SUCCESS, FIBO_DEMO_R_FAILED, "connect host(%s) failed: %s,%d", hostname, strerror(fibo_get_socket_error()), fibo_get_socket_error());
    client->status = FIBO_STATUS_CONNECT_OK;
    fibo_sock_freeaddrinfo(peer_addr);

    return ret;

error:
    fibo_sock_freeaddrinfo(peer_addr);
    if (client->sock > 0)
    {
        fibo_sock_close_v2(client->sock);
    }

    client->status = FIBO_STATUS_CONNECT_FAIL;
    return ret;
}

static int init_connect_client(fibo_demo_connection_t *c)
{
    memset(c, 0, sizeof(*c));
    c->status = fibo_sem_new(0);

    return FIBO_DEMO_R_SUCCESS;
}

static void create_connect_task(void *param)
{
    //����һ��TCP���ӣ���ʱʱ��Ϊ5s
    fibo_demo_connection_t *c = (fibo_demo_connection_t *)param;
    c->status = FIBO_STATUS_CONNECTTING;

    int ret = fibo_demo_connect(c, "111.231.250.105", 3000, AF_INET, SOCK_STREAM, 5000);
    CHECK_EX(ret != FIBO_DEMO_R_SUCCESS, ret, "connect fail:%d", ret);

    if(ret == FIBO_DEMO_R_SUCCESS)
    {
        fibo_taskSleep(20);
        fibo_demo_send(c, "3132", 2);
        fibo_taskSleep(20);
    }

error:
    fibo_thread_delete();
    return;
}

static void recv_task(void *param)
{
    int ret = FIBO_DEMO_R_SUCCESS;
    int iret = FIBO_DEMO_R_SUCCESS;
    fibo_demo_connection_t *c = (fibo_demo_connection_t *)param;
    int loop_period = 1000;

    struct timeval tv;
    fd_set read_fds;
    int fd = c->sock;

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    tv.tv_sec = loop_period / 1000;
    tv.tv_usec = (loop_period % 1000) * 1000;
    OC_SOC_LOG("read enter...");

    char recv_buf[1024];

    while (c->status == FIBO_STATUS_CONNECT_OK && ret == FIBO_DEMO_R_SUCCESS)
    {
        iret = fibo_sock_select(fd + 1, &read_fds, NULL, NULL, &tv);
        OC_SOC_LOG("select, iret=%d", iret);
        if (iret > 0)
        {
            iret = fibo_sock_recv_v2(fd, recv_buf, sizeof(recv_buf), 0);
            OC_SOC_LOG("recv, iret=%d", iret);
            if (iret > 0)
            {
                //��������,��Ӧ����
                OC_SOC_LOG("recv handle");
            }
            else if (iret == 0)
            {
                //TCP�Ѿ������رգ��յ���FIN�����߷�����FIN��
                ret = FIBO_DEMO_R_FAILED;
                OC_SOC_LOG("socket have closed noraml");
            }
            else if ((EAGAIN == fibo_get_socket_error()) || (EWOULDBLOCK == fibo_get_socket_error()))
            {
                //���������ݽ�����ϣ������ȴ�����
                OC_SOC_LOG("recv finish fd=%d", fd);
            }
            else
            {
                //�������ر�tcp���ӡ�
                ret = FIBO_DEMO_R_FAILED;
                OC_SOC_LOG("read failed, close tcp connection:%s,%d", strerror(fibo_get_socket_error()), fibo_get_socket_error());
            }
        }
        else if (iret == 0)
        {
            OC_SOC_LOG("wait for network data...");
        }
        else
        {
            OC_SOC_LOG("select failed:%s,%d", strerror(fibo_get_socket_error()), fibo_get_socket_error());
            ret = FIBO_DEMO_R_FAILED;
        }
    }

    c->status = FIBO_STATUS_CONNECT_DISCONNECTTD;
    fibo_sock_close_v2(fd);
    OC_SOC_LOG("tcp connect close");

    fibo_thread_delete();
    return;
}

int tcp_main(void *param)
{
    OC_SOC_LOG("application thread enter, param");

    fibo_demo_connection_t g_client = {0};

    init_connect_client(&g_client);

    fibo_thread_create(create_connect_task, "tcp-connect", 1024 * 4, &g_client, OSI_PRIORITY_NORMAL);

    //����������

    int need_log_once = 1;
    while (g_client.status != FIBO_STATUS_CONNECT_OK)
    {
        if(need_log_once)
        {
            OC_SOC_LOG("tcp connect %s", g_client.status != FIBO_STATUS_CONNECT_OK ? "fail" : "OK");
            need_log_once = 0;
        }
        if (g_client.status == FIBO_STATUS_CONNECT_OK)
        {
            fibo_thread_create(recv_task, "tcp-recv", 1024 * 4, &g_client, OSI_PRIORITY_NORMAL);
        }

        fibo_taskSleep(1);//sleep
    }

    while (true)
    {
        fibo_taskSleep(2000);//sleep
    }

    return 0;
}

void oc_ntp_callback_test(void* param)
{
    fibo_ntp_rsp* p_ntp_rsp = (fibo_ntp_rsp*)param;
    OC_SOC_LOG("result=%d, time=%lld", p_ntp_rsp->result, p_ntp_rsp->time);
}

void oc_ping_callback_test(void* param)
{
    fibo_ping_rsp* p_ping_rsp = (fibo_ping_rsp*)param;
    OC_SOC_LOG("send_count=%ld, recv_count=%ld, rtt_avg=%ld", p_ping_rsp->send_count, p_ping_rsp->recv_count, p_ping_rsp->rtt_avg);
}

void test_sock_tcp()
{
    OC_SOC_LOG("test_sock_tcp");
    //fibo_at_send((const UINT8*)"AT+MIPCALL=1,\"cmnet\"\r\n", strlen("AT+MIPCALL=1,\"cmnet\"\r\n"));
    tcp_main(NULL);
}

void test_connect_domain()
{
    OC_SOC_LOG("test_connect_domain");

    //dns
    int ret = 0;
    int sock = 0;
    fibo_addrinfo_t *peer_addr = NULL;
    fibo_addrinfo_para paras = {"xatest2.fibocom.com", 3000, AF_INET, NULL, NULL, &peer_addr, 20*1000};
    ret = fibo_get_addrinfo(&paras);

    //print & conn
    if(ret==0 && peer_addr && peer_addr->ai_addr)
    {
        sock = fibo_sock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        ret = fibo_sock_connect_v2(sock, (struct sockaddr *)peer_addr->ai_addr, peer_addr->ai_addrlen);
        OC_SOC_LOG("conn, ret=%d", ret);
    }

    //free
    fibo_sock_freeaddrinfo(peer_addr);
    peer_addr = NULL;
    fibo_sock_close(sock);
}

void test_connect_ip()
{
    OC_SOC_LOG("test_connect_ip");

    //dns
    int ret = 0;
    int sock = 0;
    fibo_addrinfo_t *peer_addr = NULL;
    fibo_addrinfo_para paras = {"111.231.250.105", 3000, AF_INET, NULL, NULL, &peer_addr, 20*1000};
    ret = fibo_get_addrinfo(&paras);

    //print & conn
    if(ret==0 && peer_addr && peer_addr->ai_addr)
    {
        sock = fibo_sock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        ret = fibo_sock_connect_v2(sock, (struct sockaddr *)peer_addr->ai_addr, peer_addr->ai_addrlen);
        OC_SOC_LOG("conn, ret=%d", ret);
    }

    fibo_sock_close(sock);
    //free
    fibo_sock_freeaddrinfo(peer_addr);
    peer_addr = NULL;
}

void test_ping_api()
{
    fibo_ping_para paras = {1, "www.baidu.com", 4, 32, 64, 0, 4000, oc_ping_callback_test};
    fibo_mping_v2(&paras);
    fibo_taskSleep(6000);
}

void test_ntp_api()
{
    fibo_sock_ntp("ntp1.aliyun.com", 123, 0, oc_ntp_callback_test);
    fibo_taskSleep(3000);
}

void test_dns_get()
{
    char priv4[46] = {0};
    char secv4[46] = {0};
    char priv6[46] = {0};
    char secv6[46] = {0};
    uint8_t simid = 0, cid = 1;
    fibo_dns_get(simid, cid, priv4, secv4, priv6, secv6);
}

void test_netinfo_api()
{
    uint32_t net_info[6] = {0};
    fibo_get_NetInfo(net_info, sizeof(net_info)/sizeof(uint32_t));
}

static void prvInvokeGlobalCtors(void)
{
    extern void (*__init_array_start[])();
    extern void (*__init_array_end[])();

    size_t count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; ++i)
        __init_array_start[i]();
}

static void app_main(void *param)
{
    OC_SOC_LOG("application thread enter, param 0x%x", param);
    int ret = FIBO_R_SUCCESS;

    /// 初始化
    g_sig_sem = fibo_sem_new(0);
    g_sig_lock = fibo_mutex_create();


    /// 1. 等待驻网成功
    reg_info_t reg_info;
    fibo_pdp_profile_t pdp_profile;
    memset(&pdp_profile, 0, sizeof(fibo_pdp_profile_t));
    pdp_profile.cid = 1;
    memcpy(pdp_profile.nPdpType, "IP", strlen("IP"));
    while (true) 
    {
        fibo_reg_info_get(&reg_info, 0);
        if (reg_info.nStatus != 1) /// 未驻网成功
        {
            fibo_taskSleep(1000);
            OC_SOC_LOG("wait attach succ reg_info.nStatus=%d", reg_info.nStatus);
        }
        else
        {
            break;
        }
    }
    OC_SOC_LOG("attach succ");

    /// 2. 拨号上网
    fibo_mutex_lock(g_sig_lock);
    if (g_pdp_status != FIBO_PDP_STATUS_ACTIVED)
    {
        ret = fibo_pdp_active(1, &pdp_profile, 0);
        if (ret == FIBO_R_SUCCESS)
        {
            g_pdp_status = FIBO_PDP_STATUS_ACTIVING;
            while(g_pdp_status == FIBO_PDP_STATUS_ACTIVING)
            {
                fibo_mutex_unlock(g_sig_lock);  
                fibo_sem_try_wait(g_sig_sem, 1000);
                fibo_mutex_lock(g_sig_lock);
                OC_SOC_LOG("g_pdp_status =%d", g_pdp_status);
            }
        }
        else
        {
            OC_SOC_LOG("pdp active fail =%d", ret);
        }
    }
    else
    {
         OC_SOC_LOG("pdp have actived");
    }
    fibo_mutex_unlock(g_sig_lock);
    OC_SOC_LOG("pdp active succ");

    /// 3. 做业务
    test_connect_domain();
    test_connect_ip();
    fibo_taskSleep(10000);

    fibo_pdp_release(0, pdp_profile.cid, 0);
    fibo_thread_delete();
}

static FIBO_CALLBACK_T user_callback = {
    .fibo_signal = sig_res_callback};

void *appimg_enter(void *param)
{
    OC_SOC_LOG("application image enter, param 0x%x", param);

    prvInvokeGlobalCtors();
    fibo_thread_create(app_main, "app_main", 1024 * 16, NULL, OSI_PRIORITY_NORMAL);
    return &user_callback;
}

void appimg_exit(void)
{
    OC_SOC_LOG("application image exit");
}

