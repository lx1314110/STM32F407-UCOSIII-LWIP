
#include <includes.h>
#include "arch/cc.h"
#include "config.h"
#include "control.h"
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
void control_led_io_init(void)
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

/*****************************************************************************************************/
/*Read_ReyStatus:GPIOx as IO port,GPIO_Pin as IO pin,LedState as relay_ON/LED_OFF                    */
/*****************************************************************************************************/
//static State Read_ReyStatus(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
//{
//   
//    State  ucReystatus ; 
//   //!read 4 channel status.
//    if( GPIOx->IDR  & GPIO_Pin)
//         ucReystatus = ON;
//    else
//         ucReystatus = OFF;
//    return ucReystatus;
//}