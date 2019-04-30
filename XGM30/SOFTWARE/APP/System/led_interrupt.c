
#include <includes.h>
#include "arch/cc.h"
#include "config.h"
#include "led_interrupt.h"

#define PULSE_IN_PIN      GPIO_Pin_9
#define PULSE_EXTI_CLOCK  RCC_AHB1Periph_GPIOC

static  const u16_t PULSE_INPUT_PIN =
{
   PULSE_IN_PIN
};

//
//!Define EXTI INPUT PORT.
GPIO_TypeDef* ulMap_PulseIn_Port =
{
   GPIOC,
};

static void Extent_Int5_Set(void);
/*****************************************************************************************************/
/*Delay define in main.h,used in stm32f4x7_eth.c                                                      */
/*****************************************************************************************************/
void delay_us(void) 
{  
	__NOP();__NOP();__NOP();__NOP();__NOP();
 	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
} 
void Delay(uint32_t t) 
{  
	uint32_t i; 
	for(i=0;i<t;i++) 
		delay_us();
} 
/*****************************************************************************************************/
/*LED ON OFF:GPIOx as IO port,GPIO_Pin as IO pin,LedState as LED_ON/LED_OFF                          */
/*****************************************************************************************************/
static void LED_ON_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, u8_t LedState)
{
     //!if led on ,pin od seted as close to ground.
     if(LedState != OFF)
     {
         GPIOx->BSRRH = GPIO_Pin;
         
     }
     else
     {
         GPIOx->BSRRL = GPIO_Pin;
     }
}


/*****************************************************************************************************/
/*                                  controlledn io initiliztion                                      */
/*****************************************************************************************************/
void led_gpio_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  /* LED CONTROL IO */
  //
  //!PE4 led control gpio clock enable -------sys_run.
  //
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  //
  //pe4  initilize as od type.
  //
  GPIO_InitStructure.GPIO_Pin =  LED_RUN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(LED_RUN_PORT, &GPIO_InitStructure);
  
  //
  //! 1pps interrupte initialize.
  //
  Extent_Int5_Set();
}
/*****************************************************************************************************/
/*                                    controlled indicate                                            */
/*****************************************************************************************************/
void Led_indicate(void)
{
     static State  run_status = ON;
     
     if(run_status == ON) 
     {
          LED_ON_OFF(LED_RUN_PORT,LED_RUN_PIN,ON);
          
          run_status = OFF;
     }
     else
     {
          LED_ON_OFF(LED_RUN_PORT,LED_RUN_PIN,OFF);
          
          run_status = ON; 
     }
}

//
//!Initial the GPIOC9 AS input setting. 
//
static void extInt9_gpio_initial(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;                 //定义结构体
   
   
   RCC_AHB1PeriphClockCmd(PULSE_EXTI_CLOCK, ENABLE);    //使能时钟
   GPIO_InitStructure.GPIO_Pin  = PULSE_INPUT_PIN;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;         //设置成上拉输入
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
   GPIO_Init(ulMap_PulseIn_Port, &GPIO_InitStructure);  
}

//
//!SET EXTI9_5 Priority as 1:0
//
static void NVIC_EXTI_Initial(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;                 //定义结构体
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
   NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;   //使能外部中断所在的通道
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //抢占优先级 2， 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级 2
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;      //使能外部中断通道 
   NVIC_Init(&NVIC_InitStructure);                      //根据结构体信息进行优先级初始化
}

//
//!EXTI SOURCE SET.
//
static void EXTI_SOURCE_Initial(void)
{
   EXTI_InitTypeDef EXTI_InitStructure;                   //定义初始化结构体
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
   
   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,GPIO_PinSource9);
   EXTI_DeInit();
   EXTI_InitStructure.EXTI_Line=EXTI_Line9;               //中断线的标号 取值范围为EXTI_Line0~EXTI_Line15
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //中断模式，可选值为中断 EXTI_Mode_Interrupt 和事件 EXTI_Mode_Event。
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//触发方式，可以是下降沿触发 EXTI_Trigger_Falling，上升沿触发 EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发EXTI_Trigger_Rising_Falling
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);                        //根据结构体信息进行初始化
   EXTI_ClearITPendingBit(EXTI_Line9);
}


static void App_IntHandlerEXTI9_5(void)
{
   //OS_ERR  err;
   if(EXTI_GetFlagStatus(EXTI_Line9)==SET)
   {
     //! 1pps run led indicate.
     Led_indicate();
     EXTI_ClearITPendingBit(EXTI_Line9);               /* Clear the ext5 Update interrupt                    */        
        
     //OSTaskSemPost(&AppTask_EXTI15_10_TCB, OS_OPT_POST_NO_SCHED, &err);
   }        
}

//
//! 1pps interrupt initialize.
//
static void Extent_Int5_Set(void)
{
  extInt9_gpio_initial();                            //initial PB5 as input.
  NVIC_EXTI_Initial();                                //initial EXTI_NVIC.  
  EXTI_SOURCE_Initial();                              //initial Set
  BSP_IntVectSet(BSP_INT_ID_EXTI9_5,App_IntHandlerEXTI9_5);
  BSP_IntPrioSet(BSP_INT_ID_EXTI9_5,2);    
  BSP_IntEn(BSP_INT_ID_EXTI9_5); 
}