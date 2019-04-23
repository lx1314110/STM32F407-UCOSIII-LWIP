#include <includes.h>
#include "arch/cc.h"
#include "Extent INT.h"
//
//!EXTI INPUT PIN  == GPIO_Pin_5
#define PULSE_IN_PIN      GPIO_Pin_5
#define PULSE_EXTI_CLOCK  RCC_AHB1Periph_GPIOB

static  const u16_t PULSE_INPUT_PIN =
{
   PULSE_IN_PIN
};

//
//!Define EXTI INPUT PORT.
GPIO_TypeDef* ulMap_PulseIn_Port =
{
   GPIOB,
};

//
//!Initial the GPIOB5 AS input setting. 
//
static void Extent_Int5IO_Initial(void)
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
   
   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,GPIO_PinSource5);
   EXTI_DeInit();
   EXTI_InitStructure.EXTI_Line=EXTI_Line5;               //中断线的标号 取值范围为EXTI_Line0~EXTI_Line15
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //中断模式，可选值为中断 EXTI_Mode_Interrupt 和事件 EXTI_Mode_Event。
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//触发方式，可以是下降沿触发 EXTI_Trigger_Falling，上升沿触发 EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发EXTI_Trigger_Rising_Falling
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);                        //根据结构体信息进行初始化
   EXTI_ClearITPendingBit(EXTI_Line5);
}
static void App_IntHandlerEXTI9_5(void)
{
   //OS_ERR  err;
   if(EXTI_GetFlagStatus(EXTI_Line5)==SET)
   {
     EXTI_ClearITPendingBit(EXTI_Line5);               /* Clear the ext5 Update interrupt                    */        
        
     //OSTaskSemPost(&AppTask_EXTI15_10_TCB, OS_OPT_POST_NO_SCHED, &err);
   }        
}
void Extent_Int5_Set(void)
{
  Extent_Int5IO_Initial();                            //initial PB5 as input.
  NVIC_EXTI_Initial();                                //initial EXTI_NVIC.  
  EXTI_SOURCE_Initial();                              //initial Set
  BSP_IntVectSet(BSP_INT_ID_EXTI9_5,App_IntHandlerEXTI9_5);
  BSP_IntPrioSet(BSP_INT_ID_EXTI9_5,2);    
  BSP_IntEn(BSP_INT_ID_EXTI9_5); 
}