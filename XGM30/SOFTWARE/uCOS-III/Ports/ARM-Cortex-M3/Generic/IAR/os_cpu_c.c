/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                The Real-Time Kernel
*
*
*                                      (c) Copyright 2009, Micrium, Weston, FL
*                                                 All Rights Reserved
*
*                                                  ARM Cortex-M3 Port
*
* File      : OS_CPU_C.C
* Version   : V3.00.4
* By        : Jean J. Labrosse
*             Brian Nagel
*
* For       : ARMv7M Cortex-M3
* Mode      : Thumb2
* Toolchain : IAR EWARM
************************************************************************************************************************
*/

#define  OS_CPU_GLOBALS
#include <os.h>

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do such things as
*              STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OSIdleTaskHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppIdleTaskHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppIdleTaskHookPtr)();
    }
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                OS INITIALIZATION HOOK
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OSInitHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppInitHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppInitHookPtr)();
    }
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-III's statistics task.  This allows your application to add
*              functionality to the statistics task.
*
* Arguments  : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OSStatTaskHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppStatTaskHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppStatTaskHookPtr)();
    }
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : p_tcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OSTaskCreateHook (OS_TCB *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskCreateHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskCreateHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;                                            /* Prevent compiler warning                               */
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : p_tcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OSTaskDelHook (OS_TCB *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskDelHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskDelHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;                                            /* Prevent compiler warning                               */
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   TASK RETURN HOOK
*
* Description: This function is called if a task accidentally returns.  In other words, a task should either be an
*              infinite loop or delete itself when done.
*
* Arguments  : p_tcb   is a pointer to the task control block of the task that is returning.
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OSTaskReturnHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskReturnHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskReturnHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               INITIALIZE A TASK'S STACK
*
* Description: This function is called by OS_Task_Create() to initialize the stack frame of the task being created. This
*              function is highly processor specific.
*
* Arguments  : p_task        is a pointer to the task entry point address.
*
*              p_arg         is a pointer to a user supplied data area that will be passed to the task when the task
*                            first executes.
*
*              p_stk_base    is a pointer to the base address of the stack.
*
*              stk_size      is the size of the stack in number of CPU_STK elements.
*
*              opt           specifies options that can be used to alter the behavior of OS_Task_StkInit().
*                            (see OS.H for OS_TASK_OPT_xxx).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have been placed on the
*              stack in the proper order.
*
* Note(s)    : 1) Interrupts are enabled when your task starts executing.
*              2) All tasks run in Thread mode, using process stack.
************************************************************************************************************************
*/

CPU_STK *OSTaskStkInit (OS_TASK_PTR   p_task,
                        void         *p_arg,
                        CPU_STK      *p_stk_base,
                        CPU_STK      *p_stk_limit,
                        OS_STK_SIZE   stk_size,
                        OS_OPT        opt)
{
    CPU_STK  *p_stk;


    (void)opt;                                              /* 'opt' is not used, prevent warning                     */

     p_stk = &p_stk_base[stk_size - 1u];                    /* Load stack pointer                                     */

                                                            /* Registers stacked as if auto-saved on exception        */
    *p_stk = (CPU_INT32U)0x01000000L;                       /* xPSR                                                   */
     p_stk--;

    *p_stk = (CPU_INT32U)p_task;                            /* Entry Point                                            */
     p_stk--;

    *p_stk = (CPU_INT32U)OS_TaskReturn;                     /* R14 (LR)                                               */
     p_stk--;

    *p_stk = (CPU_INT32U)0x12121212L;                       /* R12                                                    */
     p_stk--;

    *p_stk = (CPU_INT32U)0x03030303L;                       /* R3                                                     */
     p_stk--;

    *p_stk = (CPU_INT32U)0x02020202L;                       /* R2                                                     */
     p_stk--;

    *p_stk = (CPU_INT32U)p_stk_limit;                       /* R1                                                     */
     p_stk--;

    *p_stk = (CPU_INT32U)p_arg;                             /* R0 : argument                                          */
     p_stk--;

                                                            /* Remaining registers saved on process stack             */
    *p_stk = (CPU_INT32U)0x11111111L;                       /* R11                                                    */
     p_stk--;

    *p_stk = (CPU_INT32U)0x10101010L;                       /* R10                                                    */
     p_stk--;

    *p_stk = (CPU_INT32U)0x09090909L;                       /* R9                                                     */
     p_stk--;

    *p_stk = (CPU_INT32U)0x08080808L;                       /* R8                                                     */
     p_stk--;

    *p_stk = (CPU_INT32U)0x07070707L;                       /* R7                                                     */
     p_stk--;

    *p_stk = (CPU_INT32U)0x06060606L;                       /* R6                                                     */
     p_stk--;

    *p_stk = (CPU_INT32U)0x05050505L;                       /* R5                                                     */
     p_stk--;

    *p_stk = (CPU_INT32U)0x04040404L;                       /* R4                                                     */

    return (p_stk);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other operations
*              during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdyPtr' points to the TCB of the task that will be
*                 'switched in' (i.e. the highest priority task) and, 'OSTCBCurPtr' points to the task being switched out
*                 (i.e. the preempted task).
************************************************************************************************************************
*/

void  OSTaskSwHook (void)
{
#if OS_CFG_TASK_PROFILE_EN > 0u && defined(CPU_CFG_INT_DIS_MEAS_EN)
    CPU_TS     ts;
#endif
#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_TS     int_dis_time;
#endif



#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskSwHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppTaskSwHookPtr)();
    }
#endif

#if OS_CFG_TASK_PROFILE_EN > 0u && defined(CPU_CFG_INT_DIS_MEAS_EN)
    ts = OS_TS_GET();
    if (OSTCBCurPtr != OSTCBHighRdyPtr) {
        OSTCBCurPtr->CyclesDelta  = ts - OSTCBCurPtr->CyclesStart;
        OSTCBCurPtr->CyclesTotal += (OS_CYCLES)OSTCBCurPtr->CyclesDelta;
    }

    OSTCBHighRdyPtr->CyclesStart = ts;
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    int_dis_time = CPU_IntDisMeasMaxCurReset();             /* Keep track of per-task interrupt disable time          */
    if (int_dis_time > OSTCBCurPtr->IntDisTimeMax) {
        OSTCBCurPtr->IntDisTimeMax = int_dis_time;
    }
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
    if (OSSchedLockTimeMaxCur > OSTCBCurPtr->SchedLockTimeMax) { /* Keep track of per-task scheduler lock time        */
        OSTCBCurPtr->SchedLockTimeMax = OSSchedLockTimeMaxCur;
        OSSchedLockTimeMaxCur         = (CPU_TS)0;               /* Reset the per-task value                          */
    }
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                      TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) This function is assumed to be called from the Tick ISR.
************************************************************************************************************************
*/

void  OSTimeTickHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTimeTickHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppTimeTickHookPtr)();
    }
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  SYS TICK HANDLER
*
* Description: Handle the system tick (SysTick) interrupt, which is used to generate the uC/OS-II tick
*              interrupt.
*
* Arguments  : none.
*
* Note(s)    : 1) This function MUST be placed on entry 15 of the Cortex-M3 vector table.
************************************************************************************************************************
*/

void  OS_CPU_SysTickHandler (void)
{
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;                                      /* Tell uC/OS-III that we are starting an ISR             */
    CPU_CRITICAL_EXIT();

    OSTimeTick();                                           /* Call uC/OS-III's OSTimeTick()                          */

    OSIntExit();                                            /* Tell uC/OS-III that we are leaving the ISR             */
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                INITIALIZE SYS TICK
*
* Description: Initialize the SysTick.
*
* Arguments  : cnts          is the number of SysTick counts between two OS tick interrupts.
*
* Note(s)    : 1) This function MUST be called after OSStart() & after processor initialization.
************************************************************************************************************************
*/

void  OS_CPU_SysTickInit (CPU_INT32U  cnts)
{
    CPU_REG_NVIC_ST_RELOAD = cnts - 1u;
                                                            /* Set prio of SysTick handler to min prio.               */
    CPU_REG_NVIC_SHPRI3   |= 0xFF000000u;
                                                            /* Enable timer.                                          */
    CPU_REG_NVIC_ST_CTRL  |= CPU_REG_NVIC_ST_CTRL_CLKSOURCE | CPU_REG_NVIC_ST_CTRL_ENABLE;
                                                            /* Enable timer interrupt.                                */
    CPU_REG_NVIC_ST_CTRL  |= CPU_REG_NVIC_ST_CTRL_TICKINT;
}
