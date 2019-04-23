#ifndef _NETCONF_H
#define _NETCONF_H

#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/igmp.h"
#include "ethernetif.h"
#include "lwip.h"


#include "Llmnr.h"


#define MAX_NAME_LENGTH     64
typedef struct _net_context{
  char      name[MAX_NAME_LENGTH];
  ip_addr_t local_ip;
  ip_addr_t net_mask;
  ip_addr_t gateway_ip;
  ip_addr_t dns[2];
  ip_addr_t rem_ip;
}NET_CONTEXT;

//typedef struct ip_addr    IP_ADDR;
//typedef struct netif      NETIF;

/************************************************************* */
/* Interface macro & data definition */

/* Interface function definition */


#ifndef atoh
#define atoh(c)   inet_addr(c)
#endif

char *    htoa(struct in_addr * addr);
/* ********************************************************************* */


ip_addr_t *  Localhost(void);
void       SetLocalhost(ip_addr_t * ip);
ip_addr_t *  GetPrimaryDNS(void);
bool       CheckNetmask(ip_addr_t *  mask);
void       SetDNS(BYTE n,ip_addr_t *  ip);
ip_addr_t *  GetNetMask(void);
void       SetNetMask(ip_addr_t *  mask);
ip_addr_t *  GetGateway(void);
void       SetGateway(ip_addr_t *  ip);
ip_addr_t *  GetSecondaryDNS(void);
char *     GetHostName(void);
void       SetHostName(char *buff);
bool       CheckNetmask(ip_addr_t * mask);

#endif