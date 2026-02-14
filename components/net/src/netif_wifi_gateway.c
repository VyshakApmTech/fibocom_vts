/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('N', 'D', 'E', 'V')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "cfw.h"
#include "osi_api.h"
#include "osi_log.h"
#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#if CONFIG_NET_DNS_HIJACK_SUPPORT
#include "lwip/prot/udp.h"
#include "lwip/prot/dns.h"
#include "lwip/inet_chksum.h"
#include "sockets.h"
#endif

#include "drv_ps_path.h"
#include "netdev_interface_imp.h"
#include "netdev_interface_nat_lan.h"
#include "drv_ether.h"
#include "hal_chip.h"
#include "lwip/prot/ip.h"
#include "lwip/prot/icmp6.h"
#include "osi_api.h"
#include "hal_chip.h"
#if IP_NAT
#include "netif_wifi_gateway.h"
#include "dhcpserver.h"

#ifdef CONFIG_NET_NAT_WIFI_SUPPORT
#include "lwip/ip4_nat.h"
#include "netutils.h"

#include "netif/ethernet.h"
#include "wifi.h"
#include "ssv_types.h"
#include "ssv_frame.h"
#include "etharp.h"

struct netif *gNetifWifiGW = NULL;
//extern osiThread_t *netGetTaskID(void);
extern u8_t ip4_wan_forward(struct pbuf *p, struct netif *inp);
#if CONFIG_NET_DNS_HIJACK_SUPPORT
typedef struct ip4_dns_hijack_t
{
    struct ip4_dns_hijack_t *next;
    ip4_addr_t ip;
    uint8_t mac[6];
} ip4_dns_hijack;
static struct ip4_dns_hijack_t *g_ip4_dns_hijack = NULL;
#endif
/*
input pbuf p is ip frame
*/
static err_t wan_to_wifi_lan_datainput(struct pbuf *p, struct netif *nif)
{
    struct eth_addr *ethaddr_ret = NULL;
    const ip4_addr_t *ipaddr_ret = NULL;
    ip4_addr_t dest_addr = {0};
    struct pbuf *r = NULL;
    struct eth_addr ethbroadcast = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
    if (p == NULL || nif == NULL)
        return 0;

    //sys_arch_dump(p->payload, p->tot_len);
    OSI_LOGI(0, "wan_to_wifi_lan_datainput");

    const struct ip_hdr *iphdr;
    /* identify the IP header */
    iphdr = (struct ip_hdr *)p->payload;
    ip4_addr_copy(dest_addr, iphdr->dest);
    ip4_addr_t *current_netif_addr = (ip4_addr_t *)netif_ip4_addr(nif);

    if (current_netif_addr != 0 && (dest_addr.addr == current_netif_addr->addr || dest_addr.addr == PP_HTONL(LWIP_MAKEU32(255, 255, 255, 255)))) // for gateway
    {
        ip_input(p, nif);
    }
    else
    {
        u8_t mac[6] = {0};
        //add eth header
        if (etharp_find_addr(nif, &dest_addr, &ethaddr_ret, &ipaddr_ret) < 0)
        {
            if (ethaddr_ret == NULL)
            {
                if (dhcp_search_mac_on_ip(mac, &dest_addr) == true)
                {
                    OSI_LOGI(0, "find mac for DHCP");
                    memcpy(ethbroadcast.addr, mac, ETH_ALEN);
                }
                OSI_LOGI(0, "ethernet_output dhcp do");
                ethaddr_ret = &ethbroadcast;
            }
        }

        if (pbuf_add_header(p, SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR) == 0)
        {
            pbuf_remove_header(p, SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR);
            ethernet_output(nif, p, (struct eth_addr *)(nif->hwaddr), ethaddr_ret, ETHTYPE_IP);
        }
        else
        {
            r = (struct pbuf *)pbuf_alloc(PBUF_LINK, p->tot_len, PBUF_POOL);
            if (r == NULL)
            {
                return ERR_MEM;
            }
            pbuf_copy(r, p);
            //OSI_LOGXI(OSI_LOGPAR_S, 0, "wan_to_wifi_lan_datainput IP4: %s", ip4addr_ntoa(ipaddr_ret));
            ethernet_output(nif, r, (struct eth_addr *)(nif->hwaddr), ethaddr_ret, ETHTYPE_IP);
            pbuf_free(r);
        }

        pbuf_free(p);
    }
    return 0;
}
static bool prvWifiDataToUL(struct pbuf *p)
{
#if LWIP_TCPIP_CORE_LOCKING
    LOCK_TCPIP_CORE();
#endif

    if (p != NULL)
    {
        //sys_arch_dump(p->payload, p->tot_len);
    }
    gNetifWifiGW->u32RndisULSize += p->tot_len;

    u8_t taken = ip4_wan_forward(p, gNetifWifiGW);
    if (taken == 0)
        taken = ip4_nat_out(p);
#if LWIP_TCPIP_CORE_LOCKING
    UNLOCK_TCPIP_CORE();
#endif
    OSI_LOGI(0, "prvWifiDataToPs %d", taken);
    return true;
}
static err_t nat_wifi_data_output(struct netif *netif, struct pbuf *p,
                                  ip_addr_t *ipaddr)
{
    struct eth_addr *ethaddr_ret = NULL;
    const ip4_addr_t *ipaddr_ret = NULL;
    ip4_addr_t dest_addr = {0};
    struct eth_addr ethbroadcast = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
    struct pbuf *r = NULL;

    if (p == NULL || netif == NULL)
        return 0;

    OSI_LOGI(0, "nat_wifi_data_output");
    //sys_arch_dump(p->payload, p->tot_len);

    if (ip4_wan_forward(p, netif) == 0)
    {
        const struct ip_hdr *iphdr;
        /* identify the IP header */
        iphdr = (struct ip_hdr *)p->payload;
        ip4_addr_copy(dest_addr, iphdr->dest);

        if (p != NULL)
        {
            //sys_arch_dump(p->payload, p->tot_len);

            //add eth header
            etharp_find_addr(netif, &dest_addr, &ethaddr_ret, &ipaddr_ret);
            //OSI_LOGXI(OSI_LOGPAR_S, 0, "wan_to_wifi_lan_datainput IP4: %s", ip4addr_ntoa(ipaddr_ret));

            if (ethaddr_ret == NULL)
            {
                OSI_LOGI(0, "ethernet_output dhcp do");
                ethaddr_ret = &ethbroadcast;
            }
            if (pbuf_add_header(p, SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR) == 0)
                pbuf_remove_header(p, SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR);
            else
            {
                r = (struct pbuf *)pbuf_alloc(PBUF_LINK, p->tot_len, PBUF_POOL);
                if (r == NULL)
                {
                    return ERR_MEM;
                }
                pbuf_copy(r, p);
                p = r;
            }
            ethernet_output(netif, p, (struct eth_addr *)(netif->hwaddr), ethaddr_ret, ETHTYPE_IP);
            netif->u32LwipDLSize += p->tot_len;
            if (r != NULL)
            {
                pbuf_free(r);
            }
        }
    }
    return ERR_OK;
}
err_t nat_wifi_gateway_output(struct netif *netif, struct pbuf *p,
                              const struct eth_addr *src, const struct eth_addr *dst,
                              u16_t eth_type)
{
    if (p != NULL)
    {
        sys_arch_dump(p->payload, p->tot_len);
        ssv_wifi_tx_pkt(p, p->len);
    }
    //pbuf_free(p);
    return 0;
}
static err_t netif_gprs_nat_lan_wifi_init(struct netif *netif)
{
    /* initialize the snmp variables and counters inside the struct netif
   * ifSpeed: no assumption can be made!
   */
    netif->name[0] = 'W';
    netif->name[1] = 'F';
    netif->mtu = GPRS_MTU;
#if LWIP_IPV4
    netif->output = (netif_output_fn)nat_wifi_data_output;
    netif->linkoutput = (netif_linkoutput_fn)nat_wifi_gateway_output;
#endif
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
    netif->hwaddr_len = ETH_HWADDR_LEN;
    return ERR_OK;
}
void nat_wifi_gateway_SetMacAddr(void)
{
    unsigned char ssv_mac_addr[6] = {0};
    ssv_wifi_get_mac_2(ssv_mac_addr, 0);
    if (gNetifWifiGW != NULL)
    {
        memcpy(gNetifWifiGW->hwaddr, ssv_mac_addr, ETH_HWADDR_LEN);
    }
}
extern void dhcps_start(struct netif *netif, ip4_addr_t ip);
void nat_wifi_gateway_create(void)
{
    OSI_LOGI(0, "wifi gate way create");
    uint8_t T_cid = NAT_SIM << 4 | NAT_CID;

    //No Wan
    struct netif *wan_netif = netif_get_by_cid_type(T_cid, NETIF_LINK_MODE_NAT_WAN);
    if (wan_netif != NULL)
    {
        return;
    }

    //create lan
    struct lan_netif *plan = newLan(NAT_SIM, NAT_CID, NAT_LAN_IDX_WIFI);
    if (plan == NULL)
        return;
    if (lan_create(plan, NAT_SIM, NAT_CID, netif_gprs_nat_lan_wifi_init, wan_to_wifi_lan_datainput) != true)
        return;
    gNetifWifiGW = plan->netif;
#if 0
    //set default netif for lwip
    struct lan_netif *plwip_lan = getLan(NAT_SIM, NAT_CID, NAT_LAN_IDX_LWIP);
    if (NULL != plwip_lan && plwip_lan->isUsing == 1)
        lan_setDefaultNetif(plwip_lan);
#endif
    //bind DHCP Server
    dhcps_start(gNetifWifiGW, gNetifWifiGW->ip_addr.u_addr.ip4);

    return;
}
#if CONFIG_NET_DNS_HIJACK_SUPPORT
bool isDnsPkt(struct pbuf *p)
{
    bool isUDPpacket = false;
    bool isDNSpacket = false;
    uint8_t *ipdata = (uint8_t *)p->payload;
    if (IPH_PROTO((struct ip_hdr *)ipdata) == IPPROTO_UDP) //UDP packet
    {
        isUDPpacket = true;
    }
    if (!isUDPpacket)
        return false;
    uint8_t *udpdata = ipdata + IP_HLEN;
    if (udpdata[2] == 0x00 && udpdata[3] == 0x35) //dest port -->53
    {
        isDNSpacket = true;
    }

    uint8_t *dns_hdr = udpdata + 8; //skip UDP header
    uint8_t flags1 = dns_hdr[2];
    uint16_t nquestions = *(uint16_t *)(dns_hdr + 4);
    nquestions = lwip_ntohs(nquestions);
    if (0x01 != flags1)
    {
        isDNSpacket = false;
    }
    if (nquestions != 1)
    {
        isDNSpacket = false;
    }
    if (!isDNSpacket)
        return false;
    return isDNSpacket;
}
struct Answer
{
    u16_t type;
    u16_t cls;
    u32_t ttl;
};
#define DNS_TTL 1
struct pbuf *BuildDnsAnswerPkt(struct pbuf *p, ip_addr_t *ip)
{
    OSI_LOGXI(OSI_LOGPAR_S, 0, "BuildDnsAnswerPkt:enter ip:%s", ipaddr_ntoa(ip));
    struct ip_hdr *in_iphdr;
    in_iphdr = (struct ip_hdr *)p->payload;
    ip_addr_t ip_src = {0};
    ip_addr_t ip_dst = {0};
    ip_addr_copy_from_ip4(ip_src, in_iphdr->dest);
    ip_addr_copy_from_ip4(ip_dst, in_iphdr->src);
    OSI_LOGXI(OSI_LOGPAR_S, 0, "BuildDnsAnswerPkt:ip_src %s", ipaddr_ntoa(&ip_src));
    u16_t iphdr_hlen;
    iphdr_hlen = IPH_HL(in_iphdr);
    iphdr_hlen *= 4;
    pbuf_header(p, -(s16_t)iphdr_hlen);
    struct udp_hdr *in_udphdr;
    in_udphdr = (struct udp_hdr *)p->payload;
    u16_t src_port, dest_port;
    dest_port = lwip_ntohs(in_udphdr->src);
    src_port = lwip_ntohs(in_udphdr->dest);
    OSI_LOGI(0, "BuildDnsAnswerPkt:dest_port %d,src_port %d", dest_port, src_port);
    pbuf_header(p, -UDP_HLEN);
    struct dns_hdr in_hdr;
    u8_t flags1, flags2;
    u16_t txid = 0;
    u16_t nquestions = 0;
    if (pbuf_copy_partial(p, &in_hdr, SIZEOF_DNS_HDR, 0) == SIZEOF_DNS_HDR)
    {
        txid = in_hdr.id;
        nquestions = in_hdr.numquestions;
    }
    if (in_hdr.flags1 & DNS_FLAG1_RD)
    {
        flags1 = DNS_FLAG1_RD | DNS_FLAG1_RESPONSE; //0x81
        flags2 = DNS_FLAG2_RA | DNS_FLAG2_ERR_NONE; //0x80
    }
    else
    {
        flags1 = DNS_FLAG1_RESPONSE;
        flags2 = DNS_FLAG2_ERR_NONE;
    }
    OSI_LOGI(0, "BuildDnsAnswerPkt:txid %x,nquestions %d flags1 %x,flags2 %x", txid, nquestions, flags1, flags2);
    uint8_t *in_qry = NULL;
    in_qry = malloc(p->tot_len - SIZEOF_DNS_HDR);
    memset(in_qry, 0, p->tot_len - SIZEOF_DNS_HDR);
    int res = 0;
    res = pbuf_copy_partial(p, in_qry, p->tot_len - SIZEOF_DNS_HDR, SIZEOF_DNS_HDR);
    OSI_LOGI(0, "BuildDnsAnswerPkt:p->tot_len:%d res %d", p->tot_len, res);
    struct Answer ans;
    memset(&ans, 0, sizeof(ans));
    ans.cls = PP_HTONS(DNS_RRCLASS_IN);
    ans.ttl = PP_HTONS(DNS_TTL);
    u16_t ans_iplen = 0;
    if (IP_IS_V4(ip))
    {
        ans_iplen = lwip_htons(sizeof(ip4_addr_t));
        ans.type = PP_HTONS(DNS_RRTYPE_A);
    }
    if (IP_IS_V6(ip))
    {
        ans_iplen = lwip_htons(16);
        ans.type = PP_HTONS(DNS_RRTYPE_AAAA);
    }
    uint16_t totalLen = SIZEOF_DNS_HDR + res + 2 + sizeof(ans) + sizeof(ans_iplen) + sizeof(ip4_addr_t);
    OSI_LOGI(0, "BuildDnsAnswerPkt:totalLen%d", totalLen);
    struct pbuf *q = pbuf_alloc(PBUF_TRANSPORT, totalLen, PBUF_RAM);
    if (!q)
    {
        OSI_LOGE(0, "BuildDnsAnswerPkt malloc error.\n");
        return NULL;
    }
    struct dns_hdr out_hdr;
    memset(&out_hdr, 0, SIZEOF_DNS_HDR);
    out_hdr.id = txid;
    out_hdr.flags1 = flags1;
    out_hdr.flags2 = flags2;
    out_hdr.numquestions = nquestions;
    out_hdr.numanswers = lwip_htons(1);
    out_hdr.numauthrr = lwip_htons(0);
    out_hdr.numextrarr = lwip_htons(0);
    int idx = 0;
    pbuf_take(q, &out_hdr, SIZEOF_DNS_HDR);
    idx += SIZEOF_DNS_HDR;
    pbuf_take_at(q, in_qry, res, idx);
    idx += res;
    free(in_qry);
    in_qry = NULL;
    OSI_LOGI(0, "BuildDnsAnswerPkt:header+qry idx %d", idx);
    u16_t domain_pos = 0;
    domain_pos = lwip_htons(0xC00C);
    pbuf_take_at(q, &domain_pos, 2, idx);
    idx += 2;
    pbuf_take_at(q, &ans, sizeof(ans), idx);
    idx += sizeof(ans);
    OSI_LOGI(0, "BuildDnsAnswerPkt:ans idx1 %d", idx);
    pbuf_take_at(q, &ans_iplen, sizeof(ans_iplen), idx);
    idx += sizeof(ans_iplen);
    if (IP_IS_V4(ip))
    {
        pbuf_take_at(q, &(ip->u_addr.ip4), sizeof(ip4_addr_t), idx);
        idx += sizeof(ip4_addr_t);
    }
    OSI_LOGI(0, "BuildDnsAnswerPkt:ans ip+iplen idx2 %d", idx);
    pbuf_header(q, UDP_HLEN);
    struct udp_hdr *udphdr = (struct udp_hdr *)q->payload;
    udphdr->src = lwip_htons(src_port);
    udphdr->dest = lwip_htons(dest_port);
    udphdr->len = lwip_htons(q->tot_len);
    udphdr->chksum = 0;
    uint16_t udpchksum = ip_chksum_pseudo(q, IP_PROTO_UDP, q->tot_len, &(ip_src), &(ip_dst));
    if (udpchksum == 0x0000)
    {
        udpchksum = 0xffff;
    }
    udphdr->chksum = udpchksum;
    OSI_LOGI(0, "BuildDnsAnswerPkt:udpchksum %d", udpchksum);
    pbuf_header(q, IP_HLEN);
    struct ip_hdr *iphdr = (struct ip_hdr *)q->payload;
    IPH_TTL_SET(iphdr, 255);
    IPH_PROTO_SET(iphdr, IP_PROTO_UDP);
    ip4_addr_copy(iphdr->dest, *ip_2_ip4(&(ip_dst)));
    ip4_addr_copy(iphdr->src, *ip_2_ip4(&(ip_src)));
    u16_t ip_hlen = IP_HLEN;
    IPH_VHL_SET(iphdr, 4, ip_hlen / 4);
    IPH_TOS_SET(iphdr, 0);
    IPH_LEN_SET(iphdr, lwip_htons(q->tot_len));
    IPH_OFFSET_SET(iphdr, 0);
    IPH_ID_SET(iphdr, 0);
    IPH_CHKSUM_SET(iphdr, 0);
    IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, ip_hlen));
    struct pbuf *q1 = NULL;
    uint16_t offset = 0;
    uint8_t *pData = malloc(q->tot_len);
    if (pData == NULL)
    {
        return NULL;
    }
    for (q1 = q; q1 != NULL; q1 = q1->next)
    {
        memcpy(&pData[offset], q1->payload, q1->len);
        offset += q1->len;
    }
    sys_arch_dump(pData, q->tot_len);
    free(pData);
    pData = NULL;
    return q;
}
int ip4_dns_hijack_add(uint8_t *mac, ip4_addr_t ip)
{
    ip4_dns_hijack *cur = g_ip4_dns_hijack;
    ip4_dns_hijack *new = NULL;
    int ret = 0;

    if (NULL == mac)
    {
        OSI_LOGE(0, "ip4_dns_hijack_add param error\n");
        return -1;
    }

    OSI_LOGI(0, "ip4_dns_hijack_add mac:%x-%x-%x-%x-%x-%x, ip:%#x\n",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ip.addr);

    while (cur != NULL)
    {
        if (0 == memcmp(cur->mac, mac, sizeof(cur->mac)))
        {
            cur->ip.addr = ip.addr;
            break;
        }
        cur = cur->next;
    }

    if (NULL == cur)
    {
        new = (ip4_dns_hijack *)calloc(1, sizeof(ip4_dns_hijack));
        if (NULL == new)
        {
            OSI_LOGE(0, "ip4_dns_hijack_add malloc failed\n");
            ret = -1;
        }

        new->ip.addr = ip.addr;
        memcpy(new->mac, mac, sizeof(new->mac));
        if (NULL == g_ip4_dns_hijack)
        {
            g_ip4_dns_hijack = new;
        }
        else
        {
            cur = g_ip4_dns_hijack;
            while (cur->next != NULL)
            {
                cur = cur->next;
            }
            cur->next = new;
        }
    }

    return ret;
}

void ip4_dns_hijack_del(uint8_t *mac)
{
    ip4_dns_hijack *cur = g_ip4_dns_hijack;
    ip4_dns_hijack *next;
    ip4_dns_hijack *previous = NULL;

    if (NULL == mac)
    {
        OSI_LOGE(0, "ip4_dns_hijack_del param error\n");
        return;
    }

    OSI_LOGI(0, "ip4_dns_hijack_del mac:%x-%x-%x-%x-%x-%x\n",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    while (cur != NULL)
    {
        if (0 == memcmp(cur->mac, mac, sizeof(cur->mac)))
        {
            next = cur->next;
            if (cur == g_ip4_dns_hijack)
            {
                g_ip4_dns_hijack = next;
            }
            else
            {
                LWIP_ASSERT("NULL != previous", NULL != previous);
                previous->next = next;
            }
            /* free 'cur' or there will be a memory leak */
            free(cur);
            break;
        }
        else
        {
            previous = cur;
            cur = cur->next;
        }
    }
}

static int ip4_dns_hijack_get_ip(uint8_t *mac, ip4_addr_t *ip)
{
    ip4_dns_hijack *cur = g_ip4_dns_hijack;
    int ret = -1;

    if (NULL == mac)
    {
        OSI_LOGE(0, "ip4_dns_hijack_get_ip param error\n");
        return -1;
    }

    while (cur != NULL)
    {
        if (0 == memcmp(cur->mac, mac, sizeof(cur->mac)))
        {
            ip->addr = cur->ip.addr;
            ret = 0;
            break;
        }
        cur = cur->next;
    }

    OSI_LOGI(0, "ip4_dns_hijack_get_ip mac:%x-%x-%x-%x-%x-%x, ret:%d\n",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ret);

    return ret;
}
#endif
static int g_netstack_drop_pkt_count = 0;

int netstack_input(void *data, int32_t len, uint8_t vif_idx)
{

    ssv_type_u8 *data_buff = 0;
    //TCPIP_PACKET_INFO_T  pkt = {0};
    //TCPIP_ERROR_E ret = 0;
    //static int count=0;
    struct eth_hdr *eth;

    //LOG_PRINTF("netstack_input tcpip prt=%x, len %d, calc ptr %x, calc len=%d, wifi_netid=%x\n", data, len, OS_FRAME_GET_DATA(data), OS_FRAME_GET_DATA_LEN(data), wifi_netid);
    if (!data || len == 0) // || (vif_idx >= SSV_NETIF_MAX))
    {
        return -1;
    }

    //if (wifi_netid[vif_idx] == 0xffffffff)
    //{
    //    os_frame_free(data);
    //    return 0;
    //}

    osiThreadSetPriority(osiThreadCurrent(), 40);

    data_buff = OS_FRAME_GET_DATA(data);

    //filter IPV6 packet
    eth = (struct eth_hdr *)data_buff;
    if ((eth->type != htons(0x0806U)) && (eth->type != htons(0x0800U)) &&
        (eth->type != htons(0x86ddU)))
    {
        //LOG_PRINTF("%s, eth->type=%02x, len=%d\r\n",__func__, eth->type, len);
        os_frame_free(data);
        return 0;
    }

    OSI_LOGI(0, "ip4_nat netstack_input: get wifi data, len:%d, eth->type = %d\n", len, eth->type);
    sys_arch_dump(data_buff, len);

    if (eth->type != htons(0x0806U)) //ARP
    {
        data_buff += ETH_HLEN;
        len -= ETH_HLEN;
    }

    struct pbuf *p, *q = NULL;
    int offset = 0;
    uint8_t *pData = (uint8_t *)data_buff;
    p = (struct pbuf *)pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

    /* todo sgy test code */
    if (NULL == p)
    {
        g_netstack_drop_pkt_count++;
        OSI_LOGE(0, "ip4_nat netstack_input: alloc failed, drop count = %d\n", g_netstack_drop_pkt_count);
        goto LEAVE;
    }

    if (p != NULL && gNetifWifiGW != NULL)
    {
        if (len > p->len)
        {
            for (q = p; len > q->len; q = q->next)
            {
                memcpy(q->payload, &(pData[offset]), q->len);
                len -= q->len;
                offset += q->len;
            }
            if (len != 0)
            {
                memcpy(q->payload, &(pData[offset]), len);
            }
        }
        else
        {
            memcpy(p->payload, &(pData[offset]), len);
        }
    }

    if (eth->type == htons(0x0806U)) //ARP
    {
        if (p != NULL)
        {
            //sys_arch_dump(p->payload + ETH_HLEN, p->tot_len - ETH_HLEN);
        }
        OSI_LOGI(0, "ip4_nat netstack_input: get wifi data, ARP len:%d\n", len);
        tcpip_input(p, gNetifWifiGW);
        goto LEAVE;
    }
    else if (0) //DHCP
    {
    }
#if CONFIG_NET_DNS_HIJACK_SUPPORT
    else if (true == isDnsPkt(p)) //DNS
    {
        ip_addr_t ip = {0};
        ip.type = IPADDR_TYPE_V4;
        if (0 == ip4_dns_hijack_get_ip(eth->src.addr, &ip.u_addr.ip4))
        {
            /* pack dns packet */
            struct pbuf *p_rep = BuildDnsAnswerPkt(p, &ip);
            if (NULL == p_rep)
            {
                OSI_LOGE(0, "ip4_nat netstack_input: dns hijack alloc pbuf failed\n");
            }
            else
            {
                pbuf_header(p_rep, PBUF_LINK_HLEN);
                struct eth_hdr *ethhdr = (struct eth_hdr *)p_rep->payload;
                ethhdr->type = eth->type;
                memcpy(ethhdr->dest.addr, eth->src.addr, ETH_ALEN);
                memcpy(ethhdr->src.addr, eth->dest.addr, ETH_ALEN);
                sys_arch_dump(p_rep->payload, p_rep->tot_len);
                ssv_wifi_tx_pkt(p_rep, p_rep->len);
                pbuf_free(p_rep);
            }
        }
        else
        {
            prvWifiDataToUL(p);
        }
    }
#endif
    else
    {
        prvWifiDataToUL(p);
    }
    pbuf_free(p);
LEAVE:
    os_frame_free(data);
    return 0;
}
enum ssv6xxx_data_priority
{
    ssv6xxx_data_priority_0,
    ssv6xxx_data_priority_1,
    ssv6xxx_data_priority_2,
    ssv6xxx_data_priority_3,
    ssv6xxx_data_priority_4,
    ssv6xxx_data_priority_5,
    ssv6xxx_data_priority_6,
    ssv6xxx_data_priority_7
};
extern ssv_type_s32 ssv6xxx_wifi_send_ethernet(ssv_type_u8 vif_idx, void *frame, ssv_type_s32 len, enum ssv6xxx_data_priority priority);
int netstack_output(void *net_interface, void *data, ssv_type_u32 len)
{
    struct pbuf *q;
    static unsigned char *ucBuffer;
    unsigned char *pucBuffer = ucBuffer;
    unsigned char *pucChar;
    ssv_type_u8 vif_index = 1;
    ssv_type_u8 *frame;
    ssv_type_u8 retry_cnt = 5;

    struct pbuf *p = (struct pbuf *)data;
    //LOG_PRINTF("netstack_output len = %d \r\n", p->tot_len);

    halSysWdtStart();
retry:
    frame = os_frame_alloc(p->tot_len * sizeof(unsigned char), FALSE);
    if (NULL == frame)
    {
        retry_cnt--;
#if CONFIG_STATUS_CHECK
        g_tx_allocate_fail++;
#endif //don't xmit this frame. no enough frame.
        if (0 == retry_cnt)
        {
            return -1;
        }
        //OS_TickDelay(1);
        goto retry;
    }
    ucBuffer = OS_FRAME_GET_DATA(frame);
    /* Initiate transfer. */
    if (p->len == p->tot_len)
    {
        /* No pbuf chain, don't have to copy -> faster. */
        pucBuffer = &((unsigned char *)p->payload)[ETH_PAD_SIZE];
        memcpy(ucBuffer, pucBuffer, p->tot_len * sizeof(unsigned char));
    }
    else
    {
        pucChar = ucBuffer;
        for (q = p; q != NULL; q = q->next)
        {
            /* Send the data from the pbuf to the interface, one pbuf at a
			time. The size of the data in each pbuf is kept in the ->len
			variable. */
            /* send data from(q->payload, q->len); */
            //LWIP_DEBUGF(NETIF_DEBUG, ("NETIF: send pucChar %p q->payload %p q->len %i q->next %p\r\n", pucChar, q->payload, (int)q->len, (void *)q->next));
            if (q == p)
            {
                memcpy(pucChar, &((char *)q->payload)[ETH_PAD_SIZE], q->len - ETH_PAD_SIZE);
                pucChar += q->len - ETH_PAD_SIZE;
            }
            else
            {
                memcpy(pucChar, q->payload, q->len);
                pucChar += q->len;
            }
        }
    }
    //print_data(__func__, OS_FRAME_GET_DATA(frame), OS_FRAME_GET_DATA_LEN(frame));
    ssv6xxx_wifi_send_ethernet(vif_index, frame, OS_FRAME_GET_DATA_LEN(frame), 0);
    //LOG_PRINTF("netstack_output len = %d end\r\n", p->tot_len);
    return 0;
}
struct netstack_ip_addr
{
    ssv_type_u32 addr;
};
int weak_netstack_find_ip_in_arp_table(uint8_t *mac, struct netstack_ip_addr *ipaddr)
{
#undef MAC2STR
#undef MACSTR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

    int ret = 0;
    struct TCPIP_DHCPS_HOST_INFO_T info_ptr = {0};

    if (!mac || !ipaddr)
        return 0;
    ret = dhcps_gethostinfo(mac, &info_ptr);
    if (ret)
    {
        OSI_LOGI(0, "dhcps_gethostinfo(" MACSTR ") failed\n", MAC2STR(mac));
    }
    else
    {
        OSI_LOGXI(OSI_LOGPAR_SI, 0, "station hostname:%s ip:%08x\n", (char *)&info_ptr.hostname, (info_ptr.hostip));
    }
    ipaddr->addr = (info_ptr.hostip);
    return 1;
}

void test_host()
{
    uint8_t mac[] = {0x70, 0x18, 0x8b, 0xf7, 0xe2, 0x86};
    struct netstack_ip_addr ipaddr = {0};
    weak_netstack_find_ip_in_arp_table(mac, &ipaddr);
}
#endif
#endif
