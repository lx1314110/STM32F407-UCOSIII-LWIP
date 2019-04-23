/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2007-2008; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM32-SK Evaluation Board
*
* Filename      : bsp_periph.c
* Version       : V2.00
* Programmer(s) : BAN
*                 Michael Vysotstsky
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_PERIPH_MODULE
#include <includes.h>
#include "core_cm3.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define  BSP_PERIPH_REG_RCC_BASE            0x40023800
#define  BSP_PERIPH_REG_RCC_RST             BSP_PERIPH_REG_RCC_BASE + 0x010
#define  BSP_PERIPH_REG_RCC_ENR             BSP_PERIPH_REG_RCC_BASE + 0x030
#define  BSP_PERIPH_REG_RCC_LPENR           BSP_PERIPH_REG_RCC_BASE + 0x050



/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                             BSP_PeriphEn()
*
* Description : Enable clock for peripheral.
*
* Argument(s) : pwr_clk_id      Power/clock ID.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_PeriphEn (CPU_DATA  pwr_clk_id)
{
  CPU_INT32U  reg_off = (pwr_clk_id/32)<<2;
  CPU_INT32U  reg_bit = 1<<(pwr_clk_id%32);
  (*(volatile  CPU_INT32U *)(BSP_PERIPH_REG_RCC_ENR + reg_off)) |= reg_bit;
}


/*
*********************************************************************************************************
*                                             BSP_PeriphDis()
*
* Description : Disable clock for peripheral.
*
* Argument(s) : pwr_clk_id      Power/clock ID.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_PeriphDis (CPU_DATA  pwr_clk_id)
{
  CPU_INT32U  reg_off = (pwr_clk_id/32)<<2;
  CPU_INT32U  reg_bit = 1<<(pwr_clk_id%32);
  (*(volatile  CPU_INT32U *)(BSP_PERIPH_REG_RCC_ENR + reg_off)) &= ~(reg_bit);
}
/*
*********************************************************************************************************
*                                             BSP_PeriphLpEn()
*
* Description : Low Power Enable clock for peripheral.
*
* Argument(s) : pwr_clk_id      Power/clock ID.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_PeriphLpEn (CPU_DATA  pwr_clk_id)
{
  CPU_INT32U  reg_off = (pwr_clk_id/32)<<2;
  CPU_INT32U  reg_bit = 1<<(pwr_clk_id%32);
  (*(volatile  CPU_INT32U *)(BSP_PERIPH_REG_RCC_LPENR + reg_off)) |= reg_bit;
}


/*
*********************************************************************************************************
*                                             BSP_PeriphLpDis()
*
* Description : Low Power Disable clock for peripheral.
*
* Argument(s) : pwr_clk_id      Power/clock ID.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_PeriphLpDis (CPU_DATA  pwr_clk_id)
{
  CPU_INT32U  reg_off = (pwr_clk_id/32)<<2;
  CPU_INT32U  reg_bit = 1<<(pwr_clk_id%32);
  (*(volatile  CPU_INT32U *)(BSP_PERIPH_REG_RCC_LPENR + reg_off)) &= ~(reg_bit);
}
/*
*********************************************************************************************************
*                                             BSP_PeriphRst()
*
* Description : Peripheral Reset.
*
* Argument(s) : pwr_clk_id      Power/clock ID.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_PeriphRst (CPU_DATA  pwr_clk_id)
{
  if(pwr_clk_id >BSP_PERIPH_ID_ADC1 && pwr_clk_id <= BSP_PERIPH_ID_ADC3)
    pwr_clk_id = BSP_PERIPH_ID_ADC1;
  CPU_INT32U  reg_off = (pwr_clk_id/32)<<2;
  CPU_INT32U  reg_bit = 1<<(pwr_clk_id%32);
  (*(volatile  CPU_INT32U *)(BSP_PERIPH_REG_RCC_RST + reg_off)) |= reg_bit;
   __ASM("nop");
   __ASM("nop");  
  (*(volatile  CPU_INT32U *)(BSP_PERIPH_REG_RCC_RST + reg_off)) &= ~(reg_bit);
}