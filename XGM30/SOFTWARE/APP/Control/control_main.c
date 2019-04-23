
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "config.h"
#include "Control/control.h"

//
//! OS_TCB  control defintion.
//
static   OS_TCB  ControlTaskTCB;
static   CPU_STK ControlTaskStk[CTR_TASK_START_STK_SIZE];

//
//! 
//
extern u32_t EthInitStatus;
//
//!private fuction.
//
static void main_task_hanlder(void);
static void main_control_init(void);

//
//static void DogInit(void)
//{
//
//	RCC_LSICmd(ENABLE);
//	while(RCC_GetFlagStatus( RCC_FLAG_LSIRDY)==RESET);
//	
//	/* Enable write access to IWDG_PR and IWDG_RLR registers */
//	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//	
//	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */  // /64 =625
//  	IWDG_SetPrescaler(IWDG_Prescaler_256);//IWDG_Prescaler_64
//
// 	/* Set counter reload value to 3 (3+1)*1/1.25=5ms,¨°????¨²??1ms *//*625 1s*/
//  	IWDG_SetReload(625);//625
//
//	/* Reload IWDG counter */
//	IWDG_ReloadCounter();
//
//	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
//	IWDG_Enable();
//};


//static void FeedDog(void)
//{
//	IWDG_ReloadCounter();
//};

//static void Pre_Sec_Feeddog(void)
//{
//     FeedDog();
//     //
//    // sys_timeout get_time per 1seconds.
//    //
//    sys_timeout(100000,Pre_Sec_Feeddog,NULL);
//}

//
//!main control init.
//
static void main_control_init(void)
{
   control_led_io_init();
}

//
//! main handle
//
//extern void SnmpDataRefresh(void);
static void main_task_hanlder(void)
{
     //! led indicated.
       Led_indicate();
}

//
//! main control handler.
//
void main_control(void)
{ 
  /*OS Error varibe*/
  OS_ERR err;
  
  /*LED Run initiales*/
  main_control_init();
  
  /*main task loop*/
  for(;;)
  {  
    
     /*Run led flash*/
     main_task_hanlder();
     
     /*check the 100M phy initiale status*/
     if(!EthInitStatus)
     {
       OSTimeDlyHMSM(0, 0,5, 0, OS_OPT_TIME_HMSM_STRICT, &err);
       reConnect_config();
     }
     
     OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err); 
  }
}

//
//! create main control task.
//
void main_control_Task(void)
{
    OS_ERR err;
   //!create the main task of led ¡¢dogweed ¡¢SPI
    OSTaskCreate( (OS_TCB        *)	&ControlTaskTCB,
				  (CPU_CHAR      *)	"task control",
				  (OS_TASK_PTR    )	 main_control,
				  (void          *) 0,
				  (OS_PRIO        )	CTR_TASK_START_PRIO,
				  (CPU_STK       *)	&ControlTaskStk[0],
				  (CPU_STK   *)	        &ControlTaskStk[CTR_TASK_START_STK_SIZE / 10u],
				  (OS_STK_SIZE   )	CTR_TASK_START_STK_SIZE,
				  (OS_MSG_QTY     ) 0,
				  (OS_TICK        ) 0,
				  (void          *) 0,
				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				  (OS_ERR        *) &err);
}