/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                              (c) Copyright 2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : JJL
*                 EHS
*                 Michael Vysotsky
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                            APPLICATION DEFINES
*********************************************************************************************************
*/

#define   WORD    unsigned short
#define   BYTE    unsigned char
#define   DWORD   unsigned long
#define   UINT    unsigned int

/*
*********************************************************************************************************
*                                            BSP CONFIGURATION
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES + IDs
*********************************************************************************************************
*/
//#define  ETHERNET_INPUT_PRIO                              1
#define  APP_TASK_START_PRIO                              2
#define  CTR_TASK_START_PRIO                              8
#define  SENSOR_TASK_PRIO                                 9


#define  APP_HIGH_PRIORITY                                (APP_TASK_START_PRIO+1)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
//#define  ETHERNET_INPUT_STK_SIZE                         1024
#define  APP_TASK_START_STK_SIZE                         128
#define  CTR_TASK_START_STK_SIZE                         128
#define  SENSOR_TASK_STK_SIZE                            128





/*
*********************************************************************************************************
*                                           uC/LIB CONFIGURATION
*********************************************************************************************************
*/

#include <lib_cfg.h>

/*
*********************************************************************************************************
*                                          uC/Probe CONFIGURATION
*********************************************************************************************************
*/

#define  APP_CFG_PROBE_OS_PLUGIN_EN    DEF_DISABLED
#define  APP_CFG_PROBE_COM_EN          DEF_DISABLED

/*
*********************************************************************************************************
*                                    BSP CONFIGURATION: RS-232
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DEBUG                                 2

#define  APP_TRACE_LEVEL                    TRACE_LEVEL_INFO
#define  APP_TRACE                            BSP_Ser_Printf

#define  APP_TRACE_INFO(x)            ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DEBUG(x)           ((APP_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(APP_TRACE x) : (void)0)


#endif
