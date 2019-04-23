/**************************************************************************
*                                                                         *
*   PROJECT     : TMON (Transparent monitor)                              *
*                                                                         *
*   MODULE      : LWIP.c                                                  *
*                                                                         *
*   AUTHOR      : Michael Anburaj                                         *
*                 URL  : http://geocities.com/michaelanburaj/             *
*                 EMAIL: michaelanburaj@hotmail.com                       *
*                                                                         *
*   PROCESSOR   : Any                                                     *
*                                                                         *
*   Tool-chain  : gcc                                                     *
*                                                                         *
*   DESCRIPTION :                                                         *
*   LwIP master source file.                                              *
*                                                                         *
**************************************************************************/

#include "tcpip.h"
#include "ethernetif.h"
#include "netconf.h"
#include "config.h"

//#define IP_ADDR0   172
//#define IP_ADDR1   18
//#define IP_ADDR2   5
//#define IP_ADDR3   88
///*NETMASK*/
//#define NETMASK_ADDR0   255
//#define NETMASK_ADDR1   255
//#define NETMASK_ADDR2   255
//#define NETMASK_ADDR3   0
//
///*Gateway Address*/
//#define GW_ADDR0   172
//#define GW_ADDR1   18
//#define GW_ADDR2   5
//#define GW_ADDR3   254  


//
#define ETHERNET_INPUT_PRIO 0
/* ********************************************************************* */
/* Global definitions */


/* ********************************************************************* */
/* File local definitions */

netif_t     main_net;
OS_SEM	    lwip_sem;	 //定义一个信号量

/* ********************************************************************* */
/* Local definitions */
static unsigned long g_ulIPMode = IPADDR_USE_STATIC;
////
////! OS_TCB  control defintion.
////
//static   OS_TCB  EthernetInputTaskTCB;
////
////! control task stack.
////
//static   CPU_STK EthernetInputTaskStk[ETHERNET_INPUT_STK_SIZE];
static void LwIP_SendSem(void)
{
  OS_ERR err;
 // sys_sem_signal(&lwip_eth_sem);
//  CPU_SR_ALLOC(); 
//  CPU_CRITICAL_ENTER();                                   
//  OSIntEnter(); 
//  CPU_CRITICAL_EXIT(); 
  OSSemPost(&lwip_sem,OS_OPT_POST_1,&err);	 //发送信号量
  //ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, DISABLE);
//  while(BSP_ETH_IsRxPktValid()) 		 
//    ethernetif_input(&main_net);
//   OSIntExit();  
}


/* ********************************************************************* */
/* Global functions */


/**
  * @brief  LwIP periodic tasks
  * @param  localtime the current LocalTime value
  * @retval None
  */
static int static_ip_timeout=0;
static void LwIP_DHCP_task(void *arg)
{
  static struct ip_addr ip={0};
  bool static_ip = arg? true:false;
  if((!ip_addr_isany(&main_net.ip_addr)&&
     !ip_addr_cmp(&main_net.ip_addr,&ip)) || (++static_ip_timeout > 60 && static_ip)){
       if(!static_ip){   
         ip_addr_set(&ip,&main_net.ip_addr);
         SetLocalhost(&main_net.ip_addr);
         SetNetMask(&main_net.netmask);
         SetGateway(&main_net.gw);
///*** LCD Display DHCP Address ****/ 
//#include "stm32_eval.h" 
//#include "stm322xg_eval_lcd.h"         
//         uint32_t IPaddress = main_net.ip_addr.addr;
//         uint8_t iptab[4];
//         uint8_t iptxt[20];
//         iptab[0] = (uint8_t)(IPaddress >> 24);
//         iptab[1] = (uint8_t)(IPaddress >> 16);
//         iptab[2] = (uint8_t)(IPaddress >> 8);
//         iptab[3] = (uint8_t)(IPaddress);
//          
//         sprintf((char*)iptxt, "  %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);  
//          
//          /* Display the IP address */
//         LCD_DisplayStringLine(Line7, (uint8_t*)"IP address assigned ");
//         LCD_DisplayStringLine(Line8, (uint8_t*)"  by a DHCP server  ");
//         LCD_DisplayStringLine(Line9, iptxt);         
/************************************/           
       }else{
         ip_addr_set(&ip,Localhost());
         netif_set_addr(&main_net, Localhost(),GetNetMask(),GetGateway());
         dhcp_stop(&main_net);
         return;
       }
  }
  tcpip_timeout(1000, LwIP_DHCP_task, arg); 
}  
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
//void LwIP_Init(bool static_ip)
//{
//  struct ip_addr ipaddr;
//  struct ip_addr netmask;
//  struct ip_addr gw;
//  
//  tcpip_init(NULL, NULL);
//
//#if static_ip
//  ipaddr.addr = 0;
//  netmask.addr = 0;
//  gw.addr = 0;
//#else
//  IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
//  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
//  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
//#endif
//  
//  MEMCPY(main_net.hwaddr,BSP_ETH_GetMacAddress(),ETHARP_HWADDR_LEN);
//  main_net.hwaddr_len=ETHARP_HWADDR_LEN;
//  MEMCPY(main_net.name,"eth0",4);
//#if LWIP_NETIF_HOSTNAME  
//  main_net.hostname = GetHostName();
//#endif
//  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
//            struct ip_addr *netmask, struct ip_addr *gw,
//            void *state, err_t (* init)(struct netif *netif),
//            err_t (* input)(struct pbuf *p, struct netif *netif))
//    
//   Adds your network interface to the netif_list. Allocate a struct
//  netif and pass a pointer to this structure as the first argument.
//  Give pointers to cleared ip_addr structures when using DHCP,
//  or fill them with sane numbers otherwise. The state pointer may be NULL.
//
//  The init function pointer must point to a initialization function for
//  your ethernet netif interface. The following code illustrates it's use.*/
//  
//  
//  netif_add(&main_net, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
//  /*  Registers the default network interface.*/
//  netif_set_default(&main_net);
//
//  BSP_ETH_IRQHandler = LwIP_SendSem;
//  /*  Creates a new DHCP client for this interface on the first call.
//  Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
//  the predefined regular intervals after starting the client.
//  You can peek in the netif->dhcp struct for the actual DHCP status.*/
//  //dhcp_start(&main_net);
//  /*  When the netif is fully configured this function must be called.*/
//  netif_set_up(&main_net);
//
////  tcpip_timeout(1000, LwIP_DHCP_task, (void*)static_ip);   
////  SetHostName("STM322xG");
////  SsdpInit(&main_net);
////  LlmnrInit(&main_net);  
//
//}


#if (NO_SYS == 0)
static void  LwIP_Main_Thread(void *parg)
{
	OS_ERR err;

       //DogInit();
		while(1)
		{
                       // CPU_SR_ALLOC();
                       // CPU_CRITICAL_ENTER();                                 // Tell uC/OS-II
			OSSemPend(&lwip_sem,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
                        while(BSP_ETH_IsRxPktValid()) 
			ethernetif_input(&main_net);
                       // ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
                        //CPU_CRITICAL_EXIT(); 
			
		}
}
#endif
	
//*****************************************************************************
//
//! Initializes the lwIP TCP/IP stack.
//!
//! \param pucMAC is a pointer to a six byte array containing the MAC
//! address to be used for the interface.
//! \param ulIPAddr is the IP address to be used (static).
//! \param ulNetMask is the network mask to be used (static).
//! \param ulGWAddr is the Gateway address to be used (static).
//! \param ulIPMode is the IP Address Mode.  \b IPADDR_USE_STATIC will force
//! static IP addressing to be used, \b IPADDR_USE_DHCP will force DHCP with
//! fallback to Link Local (Auto IP), while \b IPADDR_USE_AUTOIP will force
//! Link Local only.
//!
//! This function performs initialization of the lwIP TCP/IP stack for the
//! Stellaris Ethernet MAC, including DHCP and/or AutoIP, as configured.
//!
//! \return None.
//
//*****************************************************************************
void
lwIPInit(const unsigned char *pucMAC, unsigned long ulIPAddr,
         unsigned long ulNetMask, unsigned long ulGWAddr,
         unsigned long ulIPMode)
{
     OS_ERR err;
    struct ip_addr ip_addr;
    struct ip_addr net_mask;
    struct ip_addr gw_addr;
  
    //
    // Check the parameters.
    //
#if LWIP_DHCP && LWIP_AUTOIP
    assert_param((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_DHCP) ||
           (ulIPMode == IPADDR_USE_AUTOIP));
#elif LWIP_DHCP
    assert_param((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_DHCP));
#elif LWIP_AUTOIP
    assert_param((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_AUTOIP));
#else
    assert_param(ulIPMode == IPADDR_USE_STATIC);
#endif

    //
    // Initialize lwIP library modules
    //
    tcpip_init(NULL, NULL);

    //
    // Setup the network address values.
    //
    if(ulIPMode == IPADDR_USE_STATIC)
    {
        ip_addr.addr = htonl(ulIPAddr);
        net_mask.addr = htonl(ulNetMask);
        gw_addr.addr = htonl(ulGWAddr);
    }
#if LWIP_DHCP || LWIP_AUTOIP
    else
    {
        ip_addr.addr = 0;
        net_mask.addr = 0;
        gw_addr.addr = 0;
    }
#endif
    MEMCPY(main_net.hwaddr,BSP_ETH_GetMacAddress(),ETHARP_HWADDR_LEN);
    main_net.hwaddr_len=ETHARP_HWADDR_LEN;
    MEMCPY(main_net.name,"eth0",4);
#if LWIP_NETIF_HOSTNA
    main_net.hostname = GetHostName();
#endif


    //
    // Create, configure and add the Ethernet controller interface with
    // default settings.
    //
    netif_add(&main_net, &ip_addr, &net_mask, &gw_addr, NULL,
              &ethernetif_init, &tcpip_input);
    netif_set_default(&main_net);
    
    BSP_ETH_IRQHandler = LwIP_SendSem;
    
    //
    // Indicate that we are running in static IP mode.
    //
    g_ulIPMode = ulIPMode;

    //
    // Start DHCP, if enabled.
    //
#if LWIP_DHCP
    if(g_ulIPMode == IPADDR_USE_DHCP)
    {
        dhcp_start(&main_net);
    }
#endif

    //
    // Start AutoIP, if enabled and DHCP is not.
    //
#if LWIP_AUTOIP
    if(g_ulIPMode == IPADDR_USE_AUTOIP)
    {
        autoip_start(&main_net);
    }
#endif
    
    //
    //ethernet irq point set.
    //
    BSP_ETH_IRQHandler = LwIP_SendSem;
    
    //
    // Bring the interface up.
    //
    netif_set_up(&main_net);
    
    
    
    if (netif_is_link_up(&main_net))
    {
        /* When the netif is fully configured this function must be called */
	 netif_set_up(&main_net);
    }
    else
    {
       /* When the netif link is down this function must be called */
	 netif_set_down(&main_net);
    }
		
    /* Set the link callback function, this function is called on change of link status*/
       netif_set_link_callback(&main_net, ethernetif_update_config);
    //
    //! create semphore to deal ethernet_input.
    //
    if(NO_SYS == 0)  
           OSSemCreate ((OS_SEM*)&lwip_sem,(CPU_CHAR*)"lwip_sem",(OS_SEM_CTR)0, (OS_ERR*)&err);
      
    #if 1
    sys_thread_new("Ethernet input", LwIP_Main_Thread, NULL, DEFAULT_THREAD_STACKSIZE, ETHERNET_INPUT_PRIO);
//     OSTaskCreate( (OS_TCB        *)	&EthernetInputTaskTCB,
//				  (CPU_CHAR      *)	"Ethernet input",
//				  (OS_TASK_PTR    )	 LwIP_Main_Thread,
//				  (void          *) 0,
//				  (OS_PRIO        )	ETHERNET_INPUT_PRIO,
//				  (CPU_STK       *)	&EthernetInputTaskStk[0],
//				  (CPU_STK   *)	        &EthernetInputTaskStk[ETHERNET_INPUT_STK_SIZE / 10u],
//				  (OS_STK_SIZE   )	ETHERNET_INPUT_STK_SIZE,
//				  (OS_MSG_QTY     ) 0,
//				  (OS_TICK        ) 0,
//				  (void          *) 0,
//				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//				  (OS_ERR        *) &err);
   #endif
		
}

//*****************************************************************************
//
//! Change the configuration of the lwIP network interface.
//!
//! \param ulIPAddr is the new IP address to be used (static).
//! \param ulNetMask is the new network mask to be used (static).
//! \param ulGWAddr is the new Gateway address to be used (static).
//! \param ulIPMode is the IP Address Mode.  \b IPADDR_USE_STATIC 0 will force
//! static IP addressing to be used, \b IPADDR_USE_DHCP will force DHCP with
//! fallback to Link Local (Auto IP), while \b IPADDR_USE_AUTOIP will force
//! Link Local only.
//!
//! This function will evaluate the new configuration data.  If necessary, the
//! interface will be brought down, reconfigured, and then brought back up
//! with the new configuration.
//!
//! \return None.
//
//*****************************************************************************
void
lwIPNetworkConfigChange(unsigned long ulIPAddr, unsigned long ulNetMask,
                        unsigned long ulGWAddr, unsigned long ulIPMode)
{
    struct ip_addr ip_addr;
    struct ip_addr net_mask;
    struct ip_addr gw_addr;

    //
    // Check the parameters.
    //
#if LWIP_DHCP && LWIP_AUTOIP
    assert_param((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_DHCP) ||
           (ulIPMode == IPADDR_USE_AUTOIP));
#elif LWIP_DHCP
    assert_param((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_DHCP));
#elif LWIP_AUTOIP
    assert_param((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_AUTOIP));
#else
    assert_param(ulIPMode == IPADDR_USE_STATIC);
#endif

    //
    // Setup the network address values.
    //
    if(ulIPMode == IPADDR_USE_STATIC)
    {
        ip_addr.addr = htonl(ulIPAddr);
        net_mask.addr = htonl(ulNetMask);
        gw_addr.addr = htonl(ulGWAddr);
    }
#if LWIP_DHCP || LWIP_AUTOIP
    else
    {
        ip_addr.addr = 0;
        net_mask.addr = 0;
        gw_addr.addr = 0;
    }
#endif

    //
    // Switch on the current IP Address Aquisition mode.
    //
    switch(g_ulIPMode)
    {
        //
        // Static IP
        //
        case IPADDR_USE_STATIC:
        {
            //
            // Set the new address parameters.  This will change the address
            // configuration in lwIP, and if necessary, will reset any links
            // that are active.  This is valid for all three modes.
            //
            netif_set_addr(&main_net, &ip_addr, &net_mask, &gw_addr);
            netif_set_up(&main_net); 
            //
            // If we are going to DHCP mode, then start the DHCP server now.
            //
#if LWIP_DHCP
            if(ulIPMode == IPADDR_USE_DHCP)
            {
                dhcp_start(&main_net);
            }
#endif

            //
            // If we are going to AutoIP mode, then start the AutoIP process
            // now.
            //
#if LWIP_AUTOIP
            if(ulIPMode == IPADDR_USE_AUTOIP)
            {
                autoip_start(&main_net);
            }
#endif
            //
            // Save the new mode.
            //
            g_ulIPMode = ulIPMode;

            //
            // And we're done.
            //
            break;
        }

        //
        // DHCP (with AutoIP fallback).
        //
#if LWIP_DHCP
        case IPADDR_USE_DHCP:
        {
            //
            // If we are going to static IP addressing, then disable DHCP and
            // force the new static IP address.
            //
            if(ulIPMode == IPADDR_USE_STATIC)
            {
                dhcp_stop(&main_net);
                netif_set_addr(&main_net, &ip_addr, &net_mask, &gw_addr);
            }

            //
            // If we are going to AUTO IP addressing, then disable DHCP, set
            // the default addresses, and start AutoIP.
            //
#if LWIP_AUTOIP
            else if(ulIPMode == IPADDR_USE_AUTOIP)
            {
                dhcp_stop(&main_net);
                netif_set_addr(&main_net, &ip_addr, &net_mask, &gw_addr);
                autoip_start(&main_net);
            }
#endif
            break;
        }
#endif

        //
        // AUTOIP
        //
#if LWIP_AUTOIP
        case IPADDR_USE_AUTOIP:
        {
            //
            // If we are going to static IP addressing, then disable AutoIP and
            // force the new static IP address.
            //
            if(ulIPMode == IPADDR_USE_STATIC)
            {
                autoip_stop(&main_net);
                netif_set_addr(&main_net, &ip_addr, &net_mask, &gw_addr);
            }

            //
            // If we are going to DHCP addressing, then disable AutoIP, set the
            // default addresses, and start dhcp.
            //
#if LWIP_DHCP
            else if(ulIPMode == IPADDR_USE_AUTOIP)
            {
                autoip_stop(&main_net);
                netif_set_addr(&main_net, &ip_addr, &net_mask, &gw_addr);
                dhcp_start(&main_net);
            }
#endif
            break;
        }
#endif
    }
}
void LwIP_DhcpStart(bool static_ip)
{
  dhcp_stop(&main_net);
  netif_set_addr(&main_net, 0,0,0);
  dhcp_start(&main_net);
  static_ip_timeout=0;
  tcpip_timeout(1000, LwIP_DHCP_task, (void*)static_ip);  
}
void LwIP_DhcpStop(void)
{
  dhcp_stop(&main_net);
}
//*****************************************************************************
//
//! Returns the IP address for this interface.
//!
//! This function will read and return the currently assigned IP address for
//! the Stellaris Ethernet interface.
//!
//! \return Returns the assigned IP address for this interface.
//
//*****************************************************************************
unsigned long
lwIPLocalIPAddrGet(void)
{
    return((unsigned long)main_net.ip_addr.addr);
}

//*****************************************************************************
//
//! Returns the network mask for this interface.
//!
//! This function will read and return the currently assigned network mask for
//! the Stellaris Ethernet interface.
//!
//! \return the assigned network mask for this interface.
//
//*****************************************************************************
unsigned long
lwIPLocalNetMaskGet(void)
{
    return((unsigned long)main_net.netmask.addr);
}

//*****************************************************************************
//
//! Returns the gateway address for this interface.
//!
//! This function will read and return the currently assigned gateway address
//! for the Stellaris Ethernet interface.
//!
//! \return the assigned gateway address for this interface.
//
//*****************************************************************************
unsigned long
lwIPLocalGWAddrGet(void)
{
    return((unsigned long)main_net.gw.addr);
}


/* ********************************************************************* */
