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

#ifndef _LWIP_DATA_EVENT_H_
#define _LWIP_DATA_EVENT_H_

#include <stdint.h>
#include <stddef.h>
#include "lwip/opt.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TUPLE_PROTOCOL_TCP  0
#define TUPLE_PROTOCOL_UDP  1
#define TUPLE_PROTOCOL_ICMP 2
#ifdef CONFIG_NET_IDS_SUPPORT
#ifdef CONFIG_FIBOCOM_BASE
#define TUPLE_PROTOCOL_IP   3
#endif
#endif

#define TCP_FIN 0x01U
#define TCP_SYN 0x02U
#define TCP_RST 0x04U
#define TCP_PSH 0x08U
#define TCP_ACK 0x10U
#define TCP_URG 0x20U
#define TCP_ECE 0x40U
#define TCP_CWR 0x80U

typedef struct _five_tuple_t{
    uint8_t protocol;
    ip_addr_t src_address;
    ip_addr_t dst_address;
    uint16_t src_port;
    uint16_t dst_port;
}five_tuple_t;

typedef struct _event_data_t{
    five_tuple_t tuple;
    uint32_t reserved;
}event_data_t;

typedef enum _tcp_event_e{
    TCP_EVENT_NULL = 0,
    TCP_CONNECT,
    TCP_DISCONNECT,
#ifdef CONFIG_NET_IDS_SUPPORT
#ifdef CONFIG_FIBOCOM_BASE
    TCP_BAD_PACKAGE     // all drop
#endif
#endif
}tcp_event_e;

#ifdef CONFIG_NET_IDS_SUPPORT
#ifdef CONFIG_FIBOCOM_BASE
typedef enum _ip_invalid_e{
    IP_INVALID_NULL = 0,
    IP_INVALID_TYPE,   // invalid protocol
    IP_INVALID_LEN,    // invalid length
    IP_INVALID_OPTION, // ipv6 option
    IP_INVALID_PACKAGE // other drop
}ip_invalid_e;
#endif
#endif

typedef struct _tcp_data_t{
    event_data_t tuple;
    uint8_t tcp_flags;
    uint32_t tcp_payload_length;
    tcp_event_e tcp_event;
}tcp_data_t;

typedef struct _udp_data_t{
    event_data_t tuple;
    uint32_t udp_payload_length;
}udp_data_t;

typedef struct _icmp_data_t{
    event_data_t tuple;
    uint32_t icmp_payload_length;
    uint8_t invalid_type;
}icmp_data_t;

#ifdef CONFIG_NET_IDS_SUPPORT
#ifdef CONFIG_FIBOCOM_BASE
typedef struct _ip_data_t{
    event_data_t tuple;
    uint32_t ip_payload_length;
    ip_invalid_e invalid_type;
}ip_data_t;
#endif
#endif

typedef void (* data_event_callback)(void *param, event_data_t *event_data);

void lwip_setDataEventCallback(data_event_callback callback, void *param);

#ifdef __cplusplus
}
#endif
#endif // _LWIP_DATA_EVENT_H_