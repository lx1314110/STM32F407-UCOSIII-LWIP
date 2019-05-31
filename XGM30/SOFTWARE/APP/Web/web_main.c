#include <includes.h>
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "fs.h"
#include "string.h"
#include "httpd.h"
#include "config.h"
#include "web_main.h"
#include "Sntp/sntp.h"

#define WEBSERVER_THREAD_PRIO    0
#define TWOSECONDS_DELAY_CNT     4
//*****************************************************************************
//
//! A flag to the main loop indicating that it should update the IP address
//! after a short delay (to allow us to send a suitable page back to the web
//! browser telling it the address has changed).
//
//*****************************************************************************
extern tBoolean g_bChangeIPAddress;
extern tBoolean ugSntp_Changed;
extern tBoolean bReset;
//! phy init status
extern u32_t EthInitStatus;

//
//! ip change delya cnt.
//
static u16_t IPCHANGE_DELAY_CNT = 0;     
//static   OS_TCB  WebTaskTCB;
//static   CPU_STK WebTaskStk[WEB_TASK_START_STK_SIZE];
//
//web_Thread processed the http cgi and ssi.
//
static void Web_Thread(void *arg)
{ 
  OS_ERR err;
  //
  // Parameters of Http and SNMP Initialize .
  //
  //ConfigInit();
  //
  //Intialize the http port and received callback.
  //
  httpd_init();
  
  //
  // CGI_HANDLE and SSI_HANDLE .
  //
  ConfigWebInit();
  
  
  while(1)
  {
    if(!EthInitStatus)
    {
       OSTimeDlyHMSM(0, 0,5, 0, OS_OPT_TIME_HMSM_STRICT, &err);
       reConnect_config();
  }
    if(g_bChangeIPAddress)
    {
           IPCHANGE_DELAY_CNT++;
           if(IPCHANGE_DELAY_CNT >= TWOSECONDS_DELAY_CNT)
           {  
            //
            // Actually update the IP address.
            //
                IPCHANGE_DELAY_CNT = 0 ;
                g_bChangeIPAddress = false;
                ConfigUpdateIPAddress();
           }
    }
    
    if(ugSntp_Changed)
    {
           IPCHANGE_DELAY_CNT++;
           if(IPCHANGE_DELAY_CNT >= TWOSECONDS_DELAY_CNT)
           {  
            //
            // Actually update the IP address.
            //
                IPCHANGE_DELAY_CNT = 0 ;
                ugSntp_Changed = false;
                //sntp_init();
           }  
    }
    if(bReset)
    {
       // disable all interrupt.
       //IntNVCDisable();
       //bReset = false;
       NVIC_SystemReset();
       while(1);
    }
    //
    //!snmp data refresh
    //
    //SnmpDataRefresh();
    OSTimeDlyHMSM(0, 0, 0,20, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}

/**
  * @brief  Initialize the HTTP server (start its thread) 
  * @param  none
  * @retval None
  */
void Web_Server_Task(void)
{
  //OS_ERR err;
  
  sys_thread_new("HTTP", Web_Thread, NULL, DEFAULT_THREAD_STACKSIZE, WEBSERVER_THREAD_PRIO);
//  OSTaskCreate( (OS_TCB        *)	&WebTaskTCB,
//				  (CPU_CHAR      *)	"web task",
//				  (OS_TASK_PTR    )	Web_Thread,
//				  (void          *) 0,
//				  (OS_PRIO        )	WEB_TASK_START_PRIO,
//				  (CPU_STK       *)	&WebTaskStk[0],
//				  (CPU_STK   *)	        &WebTaskStk[WEB_TASK_START_STK_SIZE / 10u],
//				  (OS_STK_SIZE   )	WEB_TASK_START_STK_SIZE,
//				  (OS_MSG_QTY     ) 0,
//				  (OS_TICK        ) 0,
//				  (void          *) 0,
//				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//				  (OS_ERR        *) &err);
//
}