#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "config.h"
#include "Sntp/sntp.h"

#define  SECTOHOUR 3600
#define  SECTOMIN  60

extern tConfigParameters g_sParameters;
////
////! OS_TCB  control defintion.
////
//static   OS_TCB  SntpTaskTCB;
////
////! control task stack.
////
//static   CPU_STK SntpTaskStk[CTR_SNTP_START_STK_SIZE];
//
//! extern config var g_sParameters;
//


//
//! seconds to hour minute or second .
//
//void SntpInterInitilize(unsigned short seconds,unsigned char SEC,unsigned char MIN,unsigned char HOUR)
//{
//   unsigned short ulSec;
//   ulSec = seconds;
//   //
//   //seconds /3600 = hours
//   //
//   HOUR = ((ulSec/SECTOHOUR) > 24)? 24:(ulSec/SECTOHOUR);
//   ulSec = ulSec%SECTOHOUR;
//   //
//   //seconds/24 = minutes
//   //
//   MIN = ((ulSec/SECTOMIN) > 60 )? 60 :(ulSec/SECTOMIN);
//   ulSec = ulSec%SECTOMIN;
//   SEC = ulSec;
//   
//}
//void 
//
//! sntp sample time
//
//
 void Sntp_Main_Init(void)
{
  if(g_sParameters.ucFlags&SNTP_EN_FLAG)
  sntp_init();   
}
//! create Sntp task.
//
//void Main_Sntp_Task(void)
//{
//   // OS_ERR err;
//   //sys_thread_new("Sntp", Web_Thread, NULL, DEFAULT_THREAD_STACKSIZE, WEBSERVER_THREAD_PRIO);
////   OSTaskCreate( (OS_TCB        *)	&SntpTaskTCB,
////				  (CPU_CHAR      *)	"task sntp",
////				  (OS_TASK_PTR    )	 Sntp_Main_Task,
////				  (void          *) 0,
////				  (OS_PRIO        )	CTR_SNTP_START_PRIO,
////				  (CPU_STK       *)	&SntpTaskStk[0],
////				  (CPU_STK   *)	        &SntpTaskStk[CTR_SNTP_START_STK_SIZE / 10u],
////				  (OS_STK_SIZE   )	CTR_SNTP_START_STK_SIZE,
////				  (OS_MSG_QTY     ) 0,
////				  (OS_TICK        ) 0,
////				  (void          *) 0,
////				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
////				  (OS_ERR        *) &err);
//}