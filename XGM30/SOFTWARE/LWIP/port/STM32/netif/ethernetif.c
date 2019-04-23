/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */
#include <includes.h>
#include "lwip/opt.h"

#if 1 /* don't build, this is only a skeleton, see previous comment */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>

#include "netif/etharp.h"

#include "lwip/igmp.h"
#include "lwip/icmp.h"
#include "lwip/dhcp.h"
#include "stm32f4x7_eth.h"
#include <string.h>
#include "ethernetif.h"


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int l = 0;
  u8 *buffer = NULL;

  SYS_ARCH_DECL_PROTECT(sr);
  /* Interrupts are disabled through this whole thing to support multi-threading
	   transmit calls. Also this function might be called from an ISR. */
  SYS_ARCH_PROTECT(sr);
  buffer =  BSP_ETH_GetCurrentTxBuffer();
  
  for(q = p; q != NULL; q = q->next){
    MEMCPY((u8_t*)&buffer[l], q->payload, q->len);
	l = l + q->len;
  }
  ETH_Prepare_Transmit_Descriptors(l);
  SYS_ARCH_UNPROTECT(sr);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
pbuf_t *
low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len;
  int l =0;
  FrameTypeDef frame;
  u8 *buffer;
 
    
  p = NULL;
  frame = ETH_Get_Received_Frame_interrupt();
  /* check that frame has no error */
  if (BSP_ETH_GetDescriptorStatus(&frame)){  
  /* Obtain the size of the packet and put it into the "len"
     variable. */
    len = frame.length;
    
    buffer = (u8 *)frame.buffer;
  
    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  
    if (p != NULL){
      for (q = p; q != NULL; q = q->next){
            MEMCPY((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
        l = l + q->len;
      }    
    }
  }

  BSP_ETH_ReleaseDescriptor(&frame);
  return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
err_t
ethernetif_input(struct netif *inp)
{
  err_t err;
  struct pbuf *p;
  SYS_ARCH_DECL_PROTECT(sr);
  
  SYS_ARCH_PROTECT(sr);
  /* move received packet into a new pbuf */
  p = low_level_input(inp);
  SYS_ARCH_UNPROTECT(sr);

  /* no packet could be read, silently ignore this */
  if (p == NULL) return ERR_MEM;

  err = inp->input(p, inp);
  if (err != ERR_OK){
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;
  }

  return err;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *net)
{
  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  //NETIF_INIT_SNMP(net, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  net->output = etharp_output;
  net->linkoutput = low_level_output;
  /* set MAC hardware address length */
  net->hwaddr_len = ETHARP_HWADDR_LEN;
  /* maximum transfer unit */
  net->mtu = 1500;
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  net->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
  
  return ERR_OK;
}

/**
  * @brief  Link callback function, this function is called on change of link status
  *         to update low level driver configuration.
  * @param  netif: The network interface
  * @retval None
  */
void ethernetif_update_config(struct netif *netif)
{
   if(netif_is_link_up(netif))
   {
     reConnect_config();
   }
   else
   {
     ETH_DMATransmissionCmd(DISABLE);
     ETH_DMAReceptionCmd(DISABLE);
     ETH_MACReceptionCmd(DISABLE);
     /* Flush Transmit FIFO */
     ETH_FlushTransmitFIFO();
     /* Disable transmit state machine of the MAC for transmission on the MII */
     ETH_MACTransmissionCmd(DISABLE); 
   }
   ethernetif_notify_conn_changed(netif);
}
#endif /* 0 */
