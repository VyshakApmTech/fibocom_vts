/* Copyright (C) 2023 RDA Technologies Limited and/or its affiliates("RDA").
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
#ifndef __DHCPSERVER_H__
#define __DHCPSERVER_H__


#include "lwip/ip_addr.h"

#define DHCPSER_COARSE_TIMER_SECS (60)

#define DHCPS_COARSE_TIMER_MSECS (DHCPSER_COARSE_TIMER_SECS * 1000UL)

struct TCPIP_DHCPS_HOST_INFO_T{
    u8_t hostname[256];
    u32_t hostip;
};

void dhcps_start(struct netif *netif, ip4_addr_t ip);
void dhcps_stop(void);

void dhcps_dns_setserver(int cid, int sim);

u8_t dhcps_coarse_tmr(void);

u32_t dhcps_gethostinfo(u8_t *mac, struct TCPIP_DHCPS_HOST_INFO_T *info_ptr);
bool dhcp_search_mac_on_ip(u8_t *mac, ip4_addr_t *ip);

#endif
