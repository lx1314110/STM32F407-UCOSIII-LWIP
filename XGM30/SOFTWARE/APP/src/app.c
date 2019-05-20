/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            MTFS30 CODE
*
*                                    
*
* Filename      : app.c
* Version       : v1.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include "Eeprom/eeprom.h"
#include "config.h"
#include "lwip.h"
#include "Gnss/gnss.h"
#include "Gnss/gnss_handler.h"
#include "Gnss/gnss_task.h"
#include "Roll/roll_task.h"
#include "System/watchdog_task.h"
#include "Debug/mtfs30_debug.h"

/*
*********************************************************************************************************
*                                             GLOBAL VARIABLES
*********************************************************************************************************
*/
OS_SEM  g_usart_debug_sem;   /* 串口调试用信号量   */
OS_SEM  g_ut4b0_reply_sem;   /* 命令回复信号量     */
OS_Q    g_gnss_msgq;         /* 卫星信息消息队列   */
/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB; 
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);
/*
*********************************************************************************************************
*                                         extern global variable
*********************************************************************************************************
*/
extern u8_t      g_debug_switch;        /* 串口调试开关变量   */
extern u8_t      g_roll_print_switch;   /* 循环打印开关变量   */

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
  OS_ERR  err;
  
  /* Setup STM32 system (clock, PLL and Flash configuration) */
  SystemInit();
  
  //
  //! Disable all interrupts.
  //
  BSP_IntDisAll(); 
  
  //
  //! Init uC/OS-III.
  //
  OSInit(&err);  
  
  //
  //! Create the start task.
  //
  OSTaskCreate((OS_TCB    *)&AppTaskStartTCB,                 
               (CPU_CHAR  *)"App Task Start",
               (OS_TASK_PTR)AppTaskStart, 
               (void      *)0,
               (OS_PRIO    )APP_TASK_START_PRIO,
               (CPU_STK   *)&AppTaskStartStk[0],
               (CPU_STK   *)&AppTaskStartStk[APP_TASK_START_STK_SIZE / 10],
               (OS_STK_SIZE)APP_TASK_START_STK_SIZE,
               (OS_MSG_QTY )0,
               (OS_TICK    )0,
               (void      *)0,
               (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR    *)&err);
  //
  //!Start multitasking (i.e. give control to uC/OS-III).
  //
  OSStart(&err);                                                
  return (0);    
}

/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
extern void Snmp_Trap_Task(void);
extern void Web_Server_Task(void);
extern void main_control_Task(void);
extern void Sntp_Main_Init(void);
extern void Shell_Server_Task(void);
extern void Main_Sensor_Task(void);
static void sem_init(void);
static void Global_variable_init(void);

static  void  AppTaskStart (void *p_arg)
{
  
  CPU_INT32U  cpu_clk_freq; 
  CPU_INT32U  cnts;
  OS_ERR      err;
  
  (void)p_arg;
  
  /*!Initialize BSP functions*/
  BSP_Init();  
 
  /*!Initialize the uC/CPU services*/
  CPU_Init();   
  
  /*!Determine SysTick reference freq*/
  cpu_clk_freq = BSP_CPU_ClkFreq();  
  
  /*!Determine nbr SysTick increments*/
  cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;  
 
  /*!Init uC/OS periodic time src (SysTick)*/
  OS_CPU_SysTickInit(cnts);                                     
 
  /*!Compute CPU capacity with no task running.*/
#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);                                 
#endif
  
#if 0
  CPU_IntDisMeasMaxCurReset(); 
#endif
  /*! semaphore                             */
  /* debug serial sem                       */
  sem_init();
  
  /*! Flash EEPROM                             */
  /* Unlock the Flash Program Erase controller */
  FLASH_Unlock();
  
  /* EEPROM Init */
  EE_Initialize();
  
  /*config initalize*/
  ConfigInit();
  
  /*Global variable initialize */
  Global_variable_init();
  
  /*wait 5 seconds util rt8041 reset over*/ 
  OSTimeDlyHMSM(0, 0,5, 0, OS_OPT_TIME_HMSM_STRICT, &err);
  
  /*initialize the hardware*/ 
  BSP_InitEth();
  

  /*Initialize Lwip stack*/
  lwIPInit(g_sParameters.ulMACAddr, g_sParameters.ulStaticIP, g_sParameters.ulSubnetMask ,
           g_sParameters.ulGatewayIP, IPADDR_USE_STATIC);
  

  // Wait for an IP address to be assigned to the board before we try to
  // initialize any connections.
  while(!lwIPLocalIPAddrGet())
  {
    //
    // Do nothing until we get our IP address assigned.
    //
    
  }
  
 
  
  //
  //!gnss initialize configure.
  //
  //  gnss_init();
  
  
  //
  //Create the watchdog task.
  //
   watchdog_task_create();
  
  //
  //Create the Http server thread.
  //
   Web_Server_Task();
   
  //
  //Create the gnss thread.
  //
   gnss_task_create();
  
   //
  //Create the debug thread.
  //
   debug_task_create();
    
  //
  //Create the Roll thread.
  //
   roll_task_create();
   
   
  //
  //Create the snmp server thread.
  //
  //    Snmp_Trap_Task();
#if 0   
  //
  //Create the sntp server thread.
  //
  //    Sntp_Main_Init();
  //
  //!Create the shell server thread. 
  //
  //    Shell_Server_Task();
#endif
  //
  //!Create the sensor thread.
  //
  //Main_Sensor_Task();
  //
  //delete self-task
  //
  OSTaskDel((OS_TCB *)&AppTaskStartTCB,(OS_ERR *)&err);
  
  while (DEF_TRUE) 
  {                                            
    // BSP_Sleep(1000);      
  }    
  
}


static void sem_init(void)
{
    OS_ERR      err;
    /* create debug serial semaphore */
    OSSemCreate ((OS_SEM*   ) &g_usart_debug_sem,
                 (CPU_CHAR* ) "DEBUG SEM",
                 (OS_SEM_CTR) 0,
                 (OS_ERR*   ) &err);
    
     /* 创建命令回复信号量 */
    OSSemCreate ((OS_SEM*   ) &g_ut4b0_reply_sem,
                 (CPU_CHAR* ) "UT4B0 REPLY SEM",
                 (OS_SEM_CTR) 0,
                 (OS_ERR*   ) &err); 
    
    /* 创建卫星信息消息队列 */
    OSQCreate ((OS_Q     *) &g_gnss_msgq,
               (CPU_CHAR *) "GNSS MSGQ",
               (OS_MSG_QTY) GNSS_MSG_MAX_LEN,
               (OS_ERR*   ) &err);
}

/*****************************************************************************
 * fuction:     Other_variable_init                                                          
 * description:    initialize the Ohter variable.                                                               
 * parameters:    null                                                                             
 * return:        null                                                                                       
 ****************************************************************************/
static void Global_variable_init(void)
{

    /* serial debug switch,data for 0 off,1 on */
    g_debug_switch = 0; 
    
    /* roll printf switch,data for 0 off,1 on  */
    g_roll_print_switch = 0; 
    
    /* initialize the temp gnss information variable */
    gnss_global_variable_init(); 

}
