/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM32-SK Evaluation Board
*
* Filename      : bsp.c
* Version       : V2.00
* Programmer(s) : BAN
*                 Michael Vysotsky
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define  DWT_CR      *(CPU_REG32 *)0xE0001000
#define  DWT_CYCCNT  *(CPU_REG32 *)0xE0001004

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


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
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{

  EXTI_DeInit();
  /* Check if the system has resumed from WWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET){
    /* Clear reset flags */
    RCC_ClearFlag();
  } 
  
}
bool  BSP_ResetFlag=false;

void BSP_WatchdogDisable(void)
{
  BSP_PeriphDis(BSP_PERIPH_ID_WWDG);
}


void  BSP_WatchdogInit(void)
{
  /* WWDG configuration */
  /* Enable WWDG clock */
  BSP_PeriphEn(BSP_PERIPH_ID_WWDG);

  /* WWDG clock counter = (PCLK1/4096)/8 = 1099 Hz (~910 µs)  */
  WWDG_SetPrescaler(WWDG_Prescaler_8);

  /* Set Window value to 127 */
  WWDG_SetWindowValue(127);

  /* Enable WWDG and set counter value to 127, WWDG timeout = ~910 µs * 64 = 58.25 ms */
  WWDG_Enable(127);  
}
bool  BSP_IsReset()
{
  return BSP_ResetFlag;
}
void  BSP_Restart(void)
{
  BSP_ResetFlag=true;
}
/*
*********************************************************************************************************
*   Set System timestamp
*********************************************************************************************************
*/
DWORD           BSP_Timestamp[2];
void BSP_SetTimestamp(DWORD * timestamp)
{
  BSP_Timestamp[0] = timestamp[0];
  BSP_Timestamp[1] = timestamp[1];
}
DWORD * BSP_GetTimestamp(void)
{
  return BSP_Timestamp;
}
void BSP_UpdateTemestamp(void)
{
  if(++BSP_Timestamp[1] >= 1000){
    BSP_Timestamp[1] = 0;
    ++BSP_Timestamp[0];
  }
}
bool  BSP_GetTime(struct tm *inTime)
{
  DWORD timestamp[2];
  Mem_Copy((BYTE*)timestamp,(BYTE*)BSP_GetTimestamp(),8);
  time_t local = timestamp[0];  
  Mem_Copy((BYTE*)inTime,(BYTE*)gmtime(&local),sizeof(struct tm));
  return true;
}
void BSP_GetSerialNumber(char* buf)
{
  Mem_Copy((BYTE*)buf,(BYTE*)UUID_DEVICE,4);
  int i;
  for(i=0; i<4; ++i){
    buf[i] &=0x0F;
    buf[i] += 'A';
  }
  sprintf(&buf[4],"%08X",*(DWORD*)(UUID_DEVICE + 8));  
}
/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}

/*
*********************************************************************************************************
*                                            BSP_Sleep(OS_TICK dly)
*
* Description: This function is called to delay execution of the currently running task until the specified number of 
*              system ticks expires.  This, of course, directly equates to delaying the current task for some time to 
*              expire.  No delay will result if the specified delay is 0.  If the specified delay is greater than 0 
*              then, a context switch will result.
*
* Argument(s) : dly - is a value in 'clock ticks' that the task will either delay for or, the target match value
*                        of the tick counter (OSTickCtr).  Note that specifying 0 means the task is not to delay.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  BSP_Sleep(OS_TICK dly)
{
  OS_ERR   err;
  OSTimeDly(dly,OS_OPT_TIME_DLY,&err);
}

/*
*********************************************************************************************************
*                                          CPU_TS_TmrInit()
*
* Description : Initialize & start CPU timestamp timer.
*
* Argument(s) : none.
*
* Return(s)   : Number of left-shifts to scale & return timer as (32-bit) 'CPU_TS' data type
*                 (see Note #1a1), if necessary.
*
*               0 (see Note #1a2), otherwise.
*
* Caller(s)   : CPU_TS_Init().
*
* Note(s)     : (1) (a) Timer count values MUST be scaled & returned via (32-bit) 'CPU_TS' data type.
*
*                       (1) If timer used has less bits, left-shift timer values until the most
*                           significant bit of the timer value is shifted into         the most
*                           significant bit of the return timestamp data type.
*                       (2) If timer used has more bits, truncate   timer values' higher-order
*                           bits greater than  the return timestamp data type.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                   (c) When applicable, timer period SHOULD be less than the typical measured time
*                       but MUST be less than the maximum measured time; otherwise, timer resolution
*                       inadequate to measure desired times.
*
*                   See also 'CPU_TS_TmrRd()  Note #1'.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_INT16U  CPU_TS_TmrInit (void)
{
    DWT_CR |= (CPU_INT32U)0x00000001;                           /* Enable Cortex-M3's DWT CYCCNT reg.                   */
    return ((CPU_INT16U)0);
}
#endif


/*
*********************************************************************************************************
*                                           CPU_TS_TmrRd()
*
* Description : Get current CPU timestamp timer count value.
*
* Argument(s) : none.
*
* Return(s)   : (32-bit) Timestamp timer count (see Notes #1a & #1b).
*
* Caller(s)   : CPU_TS_Init(),
*               CPU_TS_UpdateHandler(),
*               CPU_IntDisMeasStart(),
*               CPU_IntDisMeasStop().
*
* Note(s)     : (1) (a) Timer count values MUST be returned via (32-bit) 'CPU_TS' data type.
*
*                       (1) If timer used has less bits, left-shift timer values until the most
*                           significant bit of the timer value is shifted into         the most
*                           significant bit of the return timestamp data type.
*                       (2) If timer used has more bits, truncate   timer values' higher-order
*                           bits greater than  the return timestamp data type.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                       (1) If timer is a 'down' counter whose values decrease with each time count,
*                           then the returned timer value MUST be ones-complemented.
*
*                   (c) (1) When applicable, the amount of time measured by CPU timestamps is
*                           calculated by the following equation :
*
*                               Time measured  =  Timer period  *  Number timer counts
*
*                                   where
*
*                                       Timer period            Timer's period in some units of
*                                                                   (fractional) seconds
*                                       Number timer counts     Number of timer counts measured
*                                       Time measured           Amount of time measured, in same
*                                                                   units of (fractional) seconds
*                                                                   as the Timer period
*
*                       (2) Timer period SHOULD be less than the typical measured time but MUST be less
*                           than the maximum measured time; otherwise, timer resolution inadequate to
*                           measure desired times.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS  CPU_TS_TmrRd (void)
{
    return ((CPU_TS)DWT_CYCCNT);
}
#endif


