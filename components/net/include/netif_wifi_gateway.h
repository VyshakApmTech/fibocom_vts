/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _NETDEV_INTERFACE_NAT_LAN_H_
#define _NETDEV_INTERFACE_NAT_LAN_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include "drv_ether.h"

#define SSV_WIFI 1

#if CONFIG_NET_DNS_HIJACK_SUPPORT
int ip4_dns_hijack_add(uint8_t *mac, ip4_addr_t ip);
void ip4_dns_hijack_del(uint8_t *mac);
#endif
typedef struct netstack_ip_addr netstack_ip_addr;
#if CONFIG_NET_DNS_HIJACK_SUPPORT
int ip4_dns_hijack_add(uint8_t *mac, ip4_addr_t ip);
void ip4_dns_hijack_del(uint8_t *mac);
#endif
void nat_wifi_gateway_create(void);
void nat_wifi_gateway_SetMacAddr(void);
int weak_netstack_find_ip_in_arp_table(uint8_t *mac, struct netstack_ip_addr *ipaddr);

OSI_EXTERN_C_END

#endif
