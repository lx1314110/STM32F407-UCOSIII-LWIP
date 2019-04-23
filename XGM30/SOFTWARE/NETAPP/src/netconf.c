/***************************************************************************** 
* netconf.c - config network
*
*
* portions Copyright (c) 2011 by Michael Vysotsky.
*
******************************************************************************/
#include <includes.h>
#include "netconf.h"
#include <stdio.h>
/*----------------------------------------------------------------------------*/
/*                      DEFINITIONS                                           */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                      VARIABLES                                             */
/*----------------------------------------------------------------------------*/

NET_CONTEXT net_context;



/*----------------------------------------------------------------------------*/
/*                      PROTOTYPES                                            */
/*----------------------------------------------------------------------------*/



bool   CheckNetmask(ip_addr_t * mask)
{
  if(mask == NULL)
    return false;
  DWORD t_mask = 0xFFFFFFFF;
  if(!mask->addr)
    return true;
  int i;
  for(i=0; i<25; ++i){
    if(t_mask == htonl(mask->addr))
      return true;
    t_mask <<=1;
  }
  return false;
}

ip_addr_t * Localhost(void)
{
  if(net_context.local_ip.addr == 0)
    return NULL;
  return &net_context.local_ip;
}
void   SetLocalhost(ip_addr_t * ip)
{
  ip_addr_set(&net_context.local_ip,ip);
}
ip_addr_t * GetPrimaryDNS(void)
{
  if(net_context.dns[0].addr == 0)
    return NULL;  
  return &net_context.dns[0];
}
void SetDNS(BYTE n,ip_addr_t *  ip)
{
  ip_addr_set(&net_context.dns[n],ip);
}
ip_addr_t * GetNetMask(void)
{
  if(net_context.net_mask.addr == 0)
    return NULL;  
  if(!CheckNetmask(&net_context.net_mask))
    net_context.net_mask.addr = htonl(0xFFFFFF00);
  return &net_context.net_mask;
}
void SetNetMask(ip_addr_t *  mask)
{
  ip_addr_set(&net_context.net_mask,mask);
}
ip_addr_t *  GetGateway(void)
{
  if(net_context.gateway_ip.addr == 0)
    return NULL;    
  return &net_context.gateway_ip;
}
void SetGateway(ip_addr_t *  ip)
{
  ip_addr_set(&net_context.gateway_ip,ip);
}
ip_addr_t * GetSecondaryDNS(void)
{
  if(net_context.dns[1].addr == 0)
    return NULL;    
  return &net_context.dns[1];
}

char *GetHostName(void)
{
  return net_context.name;
}
void SetHostName(char *buff)
{
  int length = strlen(buff);
  if(length >=MAX_NAME_LENGTH) length = sizeof(net_context.name)-1;
  memset(net_context.name,0,sizeof(net_context.name));
  while(length--)
    net_context.name[length] = buff[length];
}
char * htoa(struct in_addr * addr)
{
  return inet_ntoa(*addr);
}