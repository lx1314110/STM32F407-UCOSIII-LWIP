/*
***********************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                        (c) Copyright 2009, Micrium, Weston, FL
*                                                  All Rights Reserved
*                                                    www.Micrium.com
*
*                                                    TICK MANAGEMENT
*
* File    : OS_TICK.C
* By      : JJL
* Version : V3.00.4
*
* LICENSING TERMS:
* ---------------
*       uC/OS-III  is provided in source form to registered licensees.  It is illegal to distribute this source
*       code to any third party unless you receive written permission by an authorized Micrium officer.  
*
*       Knowledge of the source code may NOT be used to develop a similar product.
*
*       Please help us continue to provide the  Embedded  community with the  finest software  available.   Your 
*       honesty is greatly appreciated.
*
*       You can contact us at www.micrium.com.
************************************************************************************************************************
*/

#include <os.h>

/*
************************************************************************************************************************
*                                                  LOCAL PROTOTYPES
************************************************************************************************************************
*/


/*
************************************************************************************************************************
*                                                      TICK TASK
*
* Description: This task is internal to uC/OS-III and is triggered by the tick interrupt.
*
* Arguments  : p_arg     is an argument passed to the task when the task is created (unused).
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TickTask (void *p_arg)
{
    OS_ERR   err;
    CPU_TS   ts;


    p_arg = p_arg;                                          /* Prevent compiler warning                               */

    for (;;) {
        (void)OSTaskSemPend((OS_TICK  )0,                                 
                            (OS_OPT   )OS_OPT_PEND_BLOCKING,
                            (CPU_TS  *)&ts,
                            (OS_ERR  *)&err);               /* Wait for signal from tick interrupt                    */
        if (err == OS_ERR_NONE) {
            if (OSRunning == OS_STATE_OS_RUNNING) {
                OS_TickListUpdate();                        /* Update all tasks waiting for time                      */
            }
        }
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 INITIALIZE TICK TASK
*
* Description: This function is called by OSInit() to create the tick task.
*
* Arguments  : p_err   is a pointer to a variable that will hold the value of an error code:
*
*                          OS_ERR_TICK_STK_INVALID   if the pointer to the tick task stack is a NULL pointer 
*                          OS_ERR_TICK_STK_SIZE      indicates that the specified stack size 
*                          OS_ERR_PRIO_INVALID       if the priority you specified in the configuration is invalid
*                                                      (There could be only one task at the Idle Task priority)
*                                                      (Maybe the priority you specified is higher than OS_CFG_PRIO_MAX-1
*                          OS_ERR_??                 other error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TickTaskInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL_RELEASE
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif
                                                           
    OSTickCtr         = (OS_TICK)0;                         /* Clear the tick counter                                 */
 
    OSTickTaskTimeMax = (CPU_TS)0;
       

    OS_TickListInit();                                      /* Initialize the tick list data structures               */

                                                            /* ---------------- CREATE THE TICK TASK ---------------- */
    if (OSCfg_TickTaskPrio >= (OS_CFG_PRIO_MAX - 1u)) {     /* Only one task at the 'Idle Task' priority              */
        *p_err = OS_ERR_PRIO_INVALID;
        return;
    }
    
    OSTaskCreate((OS_TCB    *)&OSTickTaskTCB,
                 (CPU_CHAR  *)"uC/OS-III Tick Task",
                 (OS_TASK_PTR)OS_TickTask,
                 (void      *)0,
                 (OS_PRIO    )OSCfg_TickTaskPrio,
                 (CPU_STK   *)OSCfg_TickTaskStkBasePtr,
                 (CPU_STK   *)OSCfg_TickTaskStkLimitPtr,
                 (OS_STK_SIZE)OSCfg_TickTaskStkSize,
                 (OS_MSG_QTY )0,
                 (OS_TICK    )0,
                 (void      *)0,                                 
                 (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR    *)p_err);                                   
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               INITIALIZE THE TICK LIST
*
* Description: This function initializes the tick handling data structures of uC/OS-III.
*
* Arguments  : none
*
* Returns    : None
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TickListInit (void)
{
    OS_TICK_SPOKE_IX   i;
    OS_TICK_SPOKE     *p_spoke;

    
    
    p_spoke = (OS_TICK_SPOKE *)&OSCfg_TickWheel[0];
    for (i = 0u; i < OSCfg_TickWheelSize; i++) {
        p_spoke->FirstPtr      = (OS_TCB   *)0;
        p_spoke->NbrEntries    = (OS_OBJ_QTY)0;
        p_spoke->NbrEntriesMax = (OS_OBJ_QTY)0;
        p_spoke++;
    }
}

/*
************************************************************************************************************************
*                                                ADD TASK TO TICK LIST
*
* Description: This function is called to place a task in a list of task waiting for either time to expire or waiting to 
*              timeout on a pend call.
*
* Arguments  : p_tcb          is a pointer to the OS_TCB of the task to add to the tick list
*              -----
*
*              time           represents either the 'match' value of OSTickCtr or a relative time from the current
*                             value of OSTickCtr as specified by the 'opt' argument..
*
*                             match     when 'opt' is set to OS_OPT_TIME_MATCH
*                             relative  when 'opt' is set to OS_OPT_TIME_DLY
*                             relative  when 'opt' is set to OS_OPT_TIME_TIMEOUT
*
*              opt            is an option specifying how to calculate time.  The valid values are:
*
*                                 OS_OPT_TIME_MATCH
*                                 OS_OPT_TIME_DLY
*                                 OS_OPT_TIME_TIMEOUT
*
* Returns    : None
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
*
*              2) This function is assumed to be called with interrupts disabled.
************************************************************************************************************************
*/

void  OS_TickListInsert (OS_TCB   *p_tcb, 
                         OS_TICK   time, 
                         OS_OPT    opt)
{
    OS_TICK            match;
    OS_TICK_SPOKE     *p_spoke;
    OS_TCB            *p_tcb0;
    OS_TCB            *p_tcb1;
    OS_TICK_SPOKE_IX   spoke;
    CPU_BOOLEAN        break_loop;



    if (opt == OS_OPT_TIME_MATCH) {
        match               = time;                              /* Task time is absolute                                  */
        p_tcb->TickRemain   = time - OSTickCtr;
        p_tcb->TickCtrMatch = match;
    } else {
        if (time > (OS_TICK)0) {                                 /* Task time relative to current                          */
            match               = OSTickCtr + time;
            p_tcb->TickRemain   = time;
            p_tcb->TickCtrMatch = match;
        } else {          
            p_tcb->TickRemain   = (OS_TICK        )0;
            p_tcb->TickCtrMatch = (OS_TICK        )0;
            p_tcb->TickSpokePtr = (OS_TICK_SPOKE *)0;
            return;
        }
    }

    spoke   = (OS_TICK_SPOKE_IX)(match % OSCfg_TickWheelSize);
    p_spoke = &OSCfg_TickWheel[spoke];

    if (p_spoke->NbrEntries == (OS_OBJ_QTY)0) {                  /* First entry in the spoke                          */
        p_spoke->FirstPtr   = p_tcb;
        p_tcb->TickNextPtr  = (OS_TCB   *)0;
        p_tcb->TickPrevPtr  = (OS_TCB   *)0;
        p_spoke->NbrEntries = (OS_OBJ_QTY)1;
    } else {
        p_tcb1     = p_spoke->FirstPtr;                          /* Point to current first TCB in the list            */
        break_loop = DEF_FALSE;
        while (p_tcb1 != (OS_TCB *)0) {
            p_tcb1->TickRemain = p_tcb1->TickCtrMatch            /* Compute time remaining of current TCB in list     */
                               - OSTickCtr;
            if (p_tcb->TickRemain > p_tcb1->TickRemain) {        /* Do we need to insert AFTER current TCB in list?   */
                if (p_tcb1->TickNextPtr != (OS_TCB *)0) {        /* Yes, are we pointing at the last TCB in the list? */
                    p_tcb1               = p_tcb1->TickNextPtr;  /* No,  Point to next TCB in the list                */
                } else {
                    p_tcb1->TickNextPtr  =  p_tcb;               /* Yes, TCB to add is now new last entry in the list */
                    p_tcb->TickPrevPtr   =  p_tcb1;
                    p_tcb->TickNextPtr   = (OS_TCB *)0;
                    p_spoke->NbrEntries++;
                    break_loop           = DEF_TRUE;             /* No point of continuing through the list           */
                }
            } else {                                             /* Insert before the current TCB                     */
                if (p_tcb1->TickPrevPtr == (OS_TCB *)0) {        /* Are we inserting before the first TCB?            */
                    p_tcb->TickPrevPtr   = (OS_TCB *)0;
                    p_tcb->TickNextPtr   = p_tcb1;
                    p_tcb1->TickPrevPtr  = p_tcb;
                    p_spoke->FirstPtr    = p_tcb;
                } else {                                         /* Insert in between 2 TCBs already in the list      */
                    p_tcb0               = p_tcb1->TickPrevPtr;
                    p_tcb0->TickNextPtr  = p_tcb;
                    p_tcb->TickPrevPtr   = p_tcb0;
                    p_tcb->TickNextPtr   = p_tcb1;
                    p_tcb1->TickPrevPtr  = p_tcb;
                }
                p_spoke->NbrEntries++;
                break_loop               = DEF_TRUE;
            }
            if (break_loop == DEF_TRUE) {
                break;
            }
        }
    }
    if (p_spoke->NbrEntries > p_spoke->NbrEntriesMax) {          /* Keep track of maximum # of entries in each spoke  */
        p_spoke->NbrEntriesMax = p_spoke->NbrEntries;
    }
    p_tcb->TickSpokePtr = p_spoke;                               /* Link back to tick spoke                           */
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                         REMOVE A TASK FROM THE TICK LIST
*
* Description: This function is called to remove a task from the tick list
*
* Arguments  : p_tcb          Is a pointer to the OS_TCB to remove.
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
*
*              2) This function is assumed to be called with interrupts disabled.
************************************************************************************************************************
*/

void  OS_TickListRemove (OS_TCB  *p_tcb) 
{
    OS_TICK_SPOKE  *p_spoke;
    OS_TCB         *p_tcb1;
    OS_TCB         *p_tcb2;


    
    p_spoke = p_tcb->TickSpokePtr;
    if (p_spoke != (OS_TICK_SPOKE *)0) {                              /* Confirm that task is in tick list            */
        p_tcb->TickRemain = (OS_TICK)0;
        if (p_spoke->FirstPtr == p_tcb) {                             /* Is timer to remove at the beginning of list? */
            p_tcb1            = (OS_TCB *)p_tcb->TickNextPtr;         /* Yes                                          */
            p_spoke->FirstPtr = p_tcb1;
            if (p_tcb1 != (OS_TCB *)0) {
                p_tcb1->TickPrevPtr = (void *)0;
            }
        } else {
            p_tcb1              = p_tcb->TickPrevPtr;                 /* No, remove timer from somewhere in the list  */
            p_tcb2              = p_tcb->TickNextPtr;
            p_tcb1->TickNextPtr = p_tcb2;
            if (p_tcb2 != (OS_TCB *)0) {
                p_tcb2->TickPrevPtr = p_tcb1;
            }
        }
        p_tcb->TickNextPtr  = (OS_TCB        *)0;
        p_tcb->TickPrevPtr  = (OS_TCB        *)0;
        p_tcb->TickSpokePtr = (OS_TICK_SPOKE *)0;
        p_tcb->TickCtrMatch = (OS_TICK        )0;
        p_spoke->NbrEntries--;
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                              RESET TICK LIST PEAK DETECTOR
*
* Description: This function is used to reset the peak detector for the number of entries in each spoke.
*
* Arguments  : void
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/


void  OS_TickListResetPeak (void)
{
    OS_TICK_SPOKE_IX   i;
    OS_TICK_SPOKE     *p_spoke;

    
    
    p_spoke = (OS_TICK_SPOKE *)&OSCfg_TickWheel[0];
    for (i = 0u; i < OSCfg_TickWheelSize; i++) {
        p_spoke->NbrEntriesMax = (OS_OBJ_QTY)0;
        p_spoke++;
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                UPDATE THE TICK LIST
*
* Description: This function is called when a tick occurs and determines if the timeout waiting for a kernel object has
*              expired or a delay has expired.
*
* Arguments  : non
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TickListUpdate (void)
{
    CPU_BOOLEAN         done;
    OS_TICK_SPOKE      *p_spoke;
    OS_TCB             *p_tcb;
    OS_TCB             *p_tcb_next;
    OS_TICK_SPOKE_IX    spoke;
    CPU_TS              ts_start;
    CPU_TS              ts_end;
    CPU_SR_ALLOC();


    OS_CRITICAL_ENTER();
    ts_start = OS_TS_GET();
    OSTickCtr++;                                                       /* Keep track of the number of ticks           */
    spoke    = (OS_TICK_SPOKE_IX)(OSTickCtr % OSCfg_TickWheelSize);
    p_spoke  = &OSCfg_TickWheel[spoke];
    p_tcb    = p_spoke->FirstPtr;
    done     = DEF_FALSE;
    while (done == DEF_FALSE) {
        if (p_tcb != (OS_TCB *)0) {
            p_tcb_next = p_tcb->TickNextPtr;                           /* Point to next TCB to update                 */
            switch (p_tcb->TaskState) {
                case OS_TASK_STATE_RDY:
                case OS_TASK_STATE_PEND:
                case OS_TASK_STATE_SUSPENDED:
                case OS_TASK_STATE_PEND_SUSPENDED:          
                     break;

                case OS_TASK_STATE_DLY:
                     if (OSTickCtr == p_tcb->TickCtrMatch) {           /* Process each TCB that expires               */
                         p_tcb->TaskState = OS_TASK_STATE_RDY;     
                         OS_TaskRdy(p_tcb);                            /* Make task ready to run                      */
                     } else {
                         done             = DEF_TRUE;                  /* Don't find a match, we're done!             */
                     } 
                     break;

                case OS_TASK_STATE_PEND_TIMEOUT:
                     if (OSTickCtr == p_tcb->TickCtrMatch) {           /* Process each TCB that expires               */
                         p_tcb->MsgPtr     = (void      *)0;
                         p_tcb->MsgSize    = (OS_MSG_SIZE)0;
                         p_tcb->TS         = OS_TS_GET();
                         OS_PendListRemove(p_tcb);                     /* Remove from wait list                       */
                         OS_TickListRemove(p_tcb);                     /* Remove from current wheel spoke             */
                         OS_TaskRdy(p_tcb);                           
                         p_tcb->TaskState  = OS_TASK_STATE_RDY;
                         p_tcb->PendStatus = OS_STATUS_PEND_TIMEOUT;   /* Indicate pend was aborted                   */
                         p_tcb->PendOn     = OS_TASK_PEND_ON_NOTHING;  /* Indicate no longer pending                  */
                     } else {
                         done              = DEF_TRUE;                 /* Don't find a match, we're done!             */
                     }
                     break;
                
                case OS_TASK_STATE_DLY_SUSPENDED: 
                     if (OSTickCtr == p_tcb->TickCtrMatch) {           /* Process each TCB that expires               */
                         p_tcb->TaskState  = OS_TASK_STATE_SUSPENDED;
                         OS_TickListRemove(p_tcb);                     /* Remove from current wheel spoke             */
                     } else {
                         done              = DEF_TRUE;                 /* Don't find a match, we're done!             */
                     }
                     break;

                case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
                     if (OSTickCtr == p_tcb->TickCtrMatch) {           /* Process each TCB that expires               */
                         p_tcb->MsgPtr     = (void      *)0;
                         p_tcb->MsgSize    = (OS_MSG_SIZE)0;
                         p_tcb->TS         = OS_TS_GET();
                         OS_PendListRemove(p_tcb);                     /* Remove from wait list                       */
                         OS_TickListRemove(p_tcb);                     /* Remove from current wheel spoke             */
                         p_tcb->TaskState  = OS_TASK_STATE_SUSPENDED;
                         p_tcb->PendStatus = OS_STATUS_PEND_TIMEOUT;   /* Indicate pend was aborted                   */
                         p_tcb->PendOn     = OS_TASK_PEND_ON_NOTHING;  /* Indicate no longer pending                  */
                     } else {
                         done              = DEF_TRUE;                 /* Don't find a match, we're done!             */
                     }
                     break;
                     
                default:
                     break;
            }
            p_tcb = p_tcb_next;
        } else {
            done  = DEF_TRUE;
        }
    }
    ts_end = OS_TS_GET() - ts_start;                                   /* Measure execution time of tick task         */
    if (ts_end > OSTickTaskTimeMax) {
        OSTickTaskTimeMax = ts_end;
    }
    OS_CRITICAL_EXIT();
}
