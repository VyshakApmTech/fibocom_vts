/* Copyright (C) 2021 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#include <stdint.h>
#include <stddef.h>
#include "lwip_data_event.h"
#include "lwip/priv/data_event_priv.h"
#include "lwip/tcp.h"

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

static data_event_callback sDataEventCallback = NULL;
static void *sDataEventCallbackArg = NULL;

void lwip_setDataEventCallback(data_event_callback callback, void *param)
{
    sDataEventCallback = callback;
    sDataEventCallbackArg = param;
}

err_t lwip_tcp_data_event(ip_addr_t *srcAddress, ip_addr_t *dstAddress, u16_t srcPort, u16_t dstPort, u8_t flags, u16_t payloadLen, u8_t event)
{
    if (sDataEventCallback == NULL)
        return ERR_OK;

    tcp_data_t *tcp_event_data = calloc(1, sizeof(*tcp_event_data));
    if (tcp_event_data == NULL)
    {
        return ERR_MEM;
    }

    tcp_event_data->tcp_flags = flags;
    tcp_event_data->tcp_payload_length = payloadLen;

    switch (event)
    {
    case 1:
        tcp_event_data->tcp_event = TCP_CONNECT;
        break;
    case 2:
        tcp_event_data->tcp_event = TCP_DISCONNECT;
        break;
#ifdef CONFIG_NET_IDS_SUPPORT
#ifdef CONFIG_FIBOCOM_BASE
    case 3:
        tcp_event_data->tcp_event = TCP_BAD_PACKAGE;
        break;
#endif
#endif
    default:
        tcp_event_data->tcp_event = TCP_EVENT_NULL;
        break;
    }

    event_data_t *event_data = (event_data_t *)tcp_event_data;
    event_data->tuple.protocol = TUPLE_PROTOCOL_TCP;
    event_data->tuple.src_port = srcPort;
    event_data->tuple.dst_port = dstPort;
    ip_addr_copy(event_data->tuple.src_address, *srcAddress);
    ip_addr_copy(event_data->tuple.dst_address, *dstAddress);
    sDataEventCallback(sDataEventCallbackArg, event_data);

    return ERR_OK;
}

#ifdef CONFIG_NET_IDS_SUPPORT
#ifdef CONFIG_FIBOCOM_BASE
err_t lwip_ip_data_event(ip_addr_t *srcAddress, ip_addr_t *dstAddress, u16_t payloadLen, u8_t event)
{
    if (sDataEventCallback == NULL)
        return ERR_OK;

    ip_data_t *ip_event_data = calloc(1, sizeof(*ip_event_data));
    if (ip_event_data == NULL)
    {
        return ERR_MEM;
    }

    ip_event_data->ip_payload_length = payloadLen;
    switch (event)
    {
    case 1:
        ip_event_data->invalid_type = IP_INVALID_TYPE;
        break;
    case 2:
        ip_event_data->invalid_type = IP_INVALID_PACKAGE;
        break;
    case 3:
        ip_event_data->invalid_type = IP_INVALID_LEN;
        break;
    case 4:
        ip_event_data->invalid_type = IP_INVALID_OPTION;
        break;
    default:
        ip_event_data->invalid_type = IP_INVALID_NULL;
        break;
    }

    event_data_t *event_data = (event_data_t *)ip_event_data;
    event_data->tuple.protocol = TUPLE_PROTOCOL_IP;
    ip_addr_copy(event_data->tuple.src_address, *srcAddress);
    ip_addr_copy(event_data->tuple.dst_address, *dstAddress);
    sDataEventCallback(sDataEventCallbackArg, event_data);
    return ERR_OK;
}
#endif
#endif

err_t lwip_udp_data_event(ip_addr_t *srcAddress, ip_addr_t *dstAddress, u16_t srcPort, u16_t dstPort, u16_t payloadLen)
{
    if (sDataEventCallback == NULL)
        return ERR_OK;

    udp_data_t *udp_event_data = calloc(1, sizeof(*udp_event_data));
    if (udp_event_data == NULL)
    {
        return ERR_MEM;
    }

    udp_event_data->udp_payload_length = payloadLen;

    event_data_t *event_data = (event_data_t *)udp_event_data;
    event_data->tuple.protocol = TUPLE_PROTOCOL_UDP;
    event_data->tuple.src_port = srcPort;
    event_data->tuple.dst_port = dstPort;
    ip_addr_copy(event_data->tuple.src_address, *srcAddress);
    ip_addr_copy(event_data->tuple.dst_address, *dstAddress);
    sDataEventCallback(sDataEventCallbackArg, event_data);
    return ERR_OK;
}

err_t lwip_icmp_data_event(ip_addr_t *srcAddress, ip_addr_t *dstAddress, u16_t payloadLen)
{
    if (sDataEventCallback == NULL)
        return ERR_OK;

    icmp_data_t *icmp_event_data = calloc(1, sizeof(*icmp_event_data));
    if (icmp_event_data == NULL)
    {
        return ERR_MEM;
    }

    icmp_event_data->icmp_payload_length = payloadLen;

    event_data_t *event_data = (event_data_t *)icmp_event_data;
    event_data->tuple.protocol = TUPLE_PROTOCOL_ICMP;
    ip_addr_copy(event_data->tuple.src_address, *srcAddress);
    ip_addr_copy(event_data->tuple.dst_address, *dstAddress);
    sDataEventCallback(sDataEventCallbackArg, event_data);
    return ERR_OK;
}
