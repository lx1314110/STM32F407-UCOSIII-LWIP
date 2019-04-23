/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                        (c) Copyright 2009, Micrium, Weston, FL
*                                                  All Rights Reserved
*                                                    www.Micrium.com
*
* File    : OS.H
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

#ifndef   OS_H
#define   OS_H


/*
************************************************************************************************************************
*                                               uC/OS-III VERSION NUMBER
************************************************************************************************************************
*/

#define  OS_VERSION  3004u                        /* Version of uC/OS-III (Vx.yy mult. by 1000)                       */

/*
************************************************************************************************************************
*                                                 INCLUDE HEADER FILES
************************************************************************************************************************
*/

#ifdef __cplusplus
extern "C" {
#endif



#include <os_cfg.h>
#include <cpu.h>
#include <cpu_core.h>
#include <os_type.h>
#include <os_cpu.h>

/*
************************************************************************************************************************
*                                               CRITICAL SECTION HANDLING
************************************************************************************************************************
*/

  
#if      OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u && defined(CPU_CFG_INT_DIS_MEAS_EN)
#define  OS_SCHED_LOCK_TIME_MEAS_START()  {OS_SchedLockTimeMeasStart();}
#else
#define  OS_SCHED_LOCK_TIME_MEAS_START()
#endif
 
  
#if      OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u && defined(CPU_CFG_INT_DIS_MEAS_EN) 
#define  OS_SCHED_LOCK_TIME_MEAS_STOP()   {OS_SchedLockTimeMeasStop();}
#else
#define  OS_SCHED_LOCK_TIME_MEAS_STOP()
#endif                                      
  
#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                             /* Deferred ISR Posts ------------------------------ */
                                                                 /* Lock the scheduler                                */
#define  OS_CRITICAL_ENTER() {                                     \
             CPU_CRITICAL_ENTER();                                 \
             if (OSSchedLockNestingCtr == 0) {                     \
                 OS_SCHED_LOCK_TIME_MEAS_START();                  \
             }                                                     \
             OSSchedLockNestingCtr++;                              \
             CPU_CRITICAL_EXIT();                                  \
         }
                                                                 /* Lock the scheduler but re-enable interrupts       */
#define  OS_CRITICAL_ENTER_CPU_CRITICAL_EXIT() {                   \
             if (OSSchedLockNestingCtr == 0) {                     \
                 OS_SCHED_LOCK_TIME_MEAS_START();                  \
             }                                                     \
             OSSchedLockNestingCtr++;                              \
             CPU_CRITICAL_EXIT();                                  \
         }

                                                                 /* Scheduling occurs only if an interrupt occurs     */
#define  OS_CRITICAL_EXIT() {                                      \
             CPU_CRITICAL_ENTER();                                 \
             OSSchedLockNestingCtr--;                              \
             if (OSSchedLockNestingCtr == (OS_NESTING_CTR)0) {     \
                 OS_SCHED_LOCK_TIME_MEAS_STOP();                   \
                 if (OSIntQNbrEntries > (OS_OBJ_QTY)0) {           \
                     CPU_CRITICAL_EXIT();                          \
                     OS_Sched0();                                  \
                 } else {                                          \
                     CPU_CRITICAL_EXIT();                          \
                 }                                                 \
             } else {                                              \
                 CPU_CRITICAL_EXIT();                              \
             }                                                     \
         }

#define  OS_CRITICAL_EXIT_NO_SCHED() {                             \
             CPU_CRITICAL_ENTER();                                 \
             OSSchedLockNestingCtr--;                              \
             if (OSSchedLockNestingCtr == (OS_NESTING_CTR)0) {     \
                 OS_SCHED_LOCK_TIME_MEAS_STOP();                   \
             }                                                     \
             CPU_CRITICAL_EXIT();                                  \
         }


#else                                                            /* Direct ISR Posts -------------------------------- */


#define  OS_CRITICAL_ENTER() {                                     \
             CPU_CRITICAL_ENTER();                                 \
         }

#define  OS_CRITICAL_ENTER_CPU_CRITICAL_EXIT() {                   \
         }

#define  OS_CRITICAL_EXIT() {                                      \
             CPU_CRITICAL_EXIT();                                  \
         }

#define  OS_CRITICAL_EXIT_NO_SCHED() {                             \
             CPU_CRITICAL_EXIT();                                  \
         }
#endif
  
/*
************************************************************************************************************************
*                                                     MISCELLANEOUS
************************************************************************************************************************
*/

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif


#define  OS_PRIO_TBL_SIZE           ((OS_CFG_PRIO_MAX - 1u) / (sizeof(CPU_DATA) * 8u) + 1u)

#define  OS_MSG_EN                 (((OS_CFG_TASK_Q_EN > 0u) || (OS_CFG_Q_EN > 0u)) ? 1u : 0u)

/*$PAGE*/
/*
************************************************************************************************************************
************************************************************************************************************************
*                                                   # D E F I N E S
************************************************************************************************************************
************************************************************************************************************************
*/

/*
========================================================================================================================
*                                                     ERROR CODES
========================================================================================================================
*/

#define  OS_ERR_NONE                          (OS_ERR)(    0u)

#define  OS_ERR_A                             (OS_ERR)(10000u)
#define  OS_ERR_ACCEPT_ISR                    (OS_ERR)(10001u)

#define  OS_ERR_B                             (OS_ERR)(11000u)

#define  OS_ERR_C                             (OS_ERR)(12000u)
#define  OS_ERR_CREATE_ISR                    (OS_ERR)(12001u)

#define  OS_ERR_D                             (OS_ERR)(13000u)
#define  OS_ERR_DEL_ISR                       (OS_ERR)(13001u)

#define  OS_ERR_E                             (OS_ERR)(14000u)

#define  OS_ERR_F                             (OS_ERR)(15000u)
#define  OS_ERR_FATAL_RETURN                  (OS_ERR)(15001u)
  
#define  OS_ERR_FLAG_GRP_DEPLETED             (OS_ERR)(15101u)
#define  OS_ERR_FLAG_NOT_RDY                  (OS_ERR)(15102u)
#define  OS_ERR_FLAG_PEND_OPT                 (OS_ERR)(15103u)
#define  OS_ERR_FLUSH_ISR                     (OS_ERR)(15104u)

#define  OS_ERR_G                             (OS_ERR)(16000u)

#define  OS_ERR_H                             (OS_ERR)(17000u)

#define  OS_ERR_I                             (OS_ERR)(18000u)
#define  OS_ERR_INT_Q                         (OS_ERR)(18001u)
#define  OS_ERR_INT_Q_FULL                    (OS_ERR)(18002u)
#define  OS_ERR_INT_Q_SIZE                    (OS_ERR)(18003u)

#define  OS_ERR_J                             (OS_ERR)(19000u)

#define  OS_ERR_K                             (OS_ERR)(20000u)

#define  OS_ERR_L                             (OS_ERR)(21000u)
#define  OS_ERR_LOCK_NESTING_OVF              (OS_ERR)(21001u)
  
#define  OS_ERR_M                             (OS_ERR)(22000u)
  
#define  OS_ERR_MEM_CREATE_ISR                (OS_ERR)(22201u)
#define  OS_ERR_MEM_FULL                      (OS_ERR)(22202u)
#define  OS_ERR_MEM_INVALID_P_ADDR            (OS_ERR)(22203u)
#define  OS_ERR_MEM_INVALID_BLKS              (OS_ERR)(22204u)
#define  OS_ERR_MEM_INVALID_PART              (OS_ERR)(22205u)
#define  OS_ERR_MEM_INVALID_P_BLK             (OS_ERR)(22206u)
#define  OS_ERR_MEM_INVALID_P_MEM             (OS_ERR)(22207u)
#define  OS_ERR_MEM_INVALID_P_DATA            (OS_ERR)(22208u)
#define  OS_ERR_MEM_INVALID_SIZE              (OS_ERR)(22209u)
#define  OS_ERR_MEM_NO_FREE_BLKS              (OS_ERR)(22210u)

#define  OS_ERR_MSG_POOL_EMPTY                (OS_ERR)(22301u)
#define  OS_ERR_MSG_POOL_NULL_PTR             (OS_ERR)(22302u)
  
#define  OS_ERR_MUTEX_NOT_OWNER               (OS_ERR)(22401u)
#define  OS_ERR_MUTEX_OWNER                   (OS_ERR)(22402u)
#define  OS_ERR_MUTEX_NESTING                 (OS_ERR)(22403u)

#define  OS_ERR_N                             (OS_ERR)(23000u)
#define  OS_ERR_NAME                          (OS_ERR)(23001u)

#define  OS_ERR_O                             (OS_ERR)(24000u)
#define  OS_ERR_OBJ_CREATED                   (OS_ERR)(24001u)
#define  OS_ERR_OBJ_DEL                       (OS_ERR)(24002u)
#define  OS_ERR_OBJ_PTR_NULL                  (OS_ERR)(24003u)
#define  OS_ERR_OBJ_TYPE                      (OS_ERR)(24004u)

#define  OS_ERR_OPT_INVALID                   (OS_ERR)(24101u)

#define  OS_ERR_OS_NOT_RUNNING                (OS_ERR)(24201u)
#define  OS_ERR_OS_RUNNING                    (OS_ERR)(24202u)
  
#define  OS_ERR_P                             (OS_ERR)(25000u)
#define  OS_ERR_PEND_ABORT                    (OS_ERR)(25001u)
#define  OS_ERR_PEND_ABORT_ISR                (OS_ERR)(25002u)
#define  OS_ERR_PEND_ABORT_NONE               (OS_ERR)(25003u)
#define  OS_ERR_PEND_ABORT_SELF               (OS_ERR)(25004u)
#define  OS_ERR_PEND_DEL                      (OS_ERR)(25005u)
#define  OS_ERR_PEND_ISR                      (OS_ERR)(25006u)
#define  OS_ERR_PEND_LOCKED                   (OS_ERR)(25007u)
#define  OS_ERR_PEND_WOULD_BLOCK              (OS_ERR)(25008u)

#define  OS_ERR_POST_NULL_PTR                 (OS_ERR)(25101u)
#define  OS_ERR_POST_ISR                      (OS_ERR)(25102u)

#define  OS_ERR_PRIO_EXIST                    (OS_ERR)(25201u)
#define  OS_ERR_PRIO                          (OS_ERR)(25202u)
#define  OS_ERR_PRIO_INVALID                  (OS_ERR)(25203u)

#define  OS_ERR_PTR_INVALID                   (OS_ERR)(25301u)

#define  OS_ERR_Q                             (OS_ERR)(26000u)
#define  OS_ERR_Q_FULL                        (OS_ERR)(26001u)
#define  OS_ERR_Q_EMPTY                       (OS_ERR)(26002u)
#define  OS_ERR_Q_MAX                         (OS_ERR)(26003u)
#define  OS_ERR_Q_SIZE                        (OS_ERR)(26004u)
  
#define  OS_ERR_R                             (OS_ERR)(27000u)
#define  OS_ERR_REG_ID_INVALID                (OS_ERR)(27001u)
#define  OS_ERR_ROUND_ROBIN_1                 (OS_ERR)(27002u)
#define  OS_ERR_ROUND_ROBIN_DISABLED          (OS_ERR)(27003u)

#define  OS_ERR_S                             (OS_ERR)(28000u)
#define  OS_ERR_SCHED_INVALID_TIME_SLICE      (OS_ERR)(28001u)
#define  OS_ERR_SCHED_LOCKED                  (OS_ERR)(28002u)
#define  OS_ERR_SCHED_NOT_LOCKED              (OS_ERR)(28003u)
  
#define  OS_ERR_SEM_OVF                       (OS_ERR)(28101u)
#define  OS_ERR_SET_ISR                       (OS_ERR)(28102u)

#define  OS_ERR_STAT_RESET_ISR                (OS_ERR)(28201u)
#define  OS_ERR_STATE_INVALID                 (OS_ERR)(28202u)
#define  OS_ERR_STATUS_INVALID                (OS_ERR)(28203u)
#define  OS_ERR_STK_INVALID                   (OS_ERR)(28204u)
#define  OS_ERR_STK_SIZE_INVALID              (OS_ERR)(28205u)

#define  OS_ERR_T                             (OS_ERR)(29000u)
#define  OS_ERR_TASK_CHANGE_PRIO_ISR          (OS_ERR)(29001u)
#define  OS_ERR_TASK_CREATE_ISR               (OS_ERR)(29002u)
#define  OS_ERR_TASK_DEL                      (OS_ERR)(29003u)
#define  OS_ERR_TASK_DEL_IDLE                 (OS_ERR)(29004u)
#define  OS_ERR_TASK_DEL_INVALID              (OS_ERR)(29005u)
#define  OS_ERR_TASK_DEL_ISR                  (OS_ERR)(29006u)
#define  OS_ERR_TASK_INVALID                  (OS_ERR)(29007u)
#define  OS_ERR_TASK_NO_MORE_TCB              (OS_ERR)(29008u)
#define  OS_ERR_TASK_NOT_DLY                  (OS_ERR)(29009u)
#define  OS_ERR_TASK_NOT_EXIST                (OS_ERR)(29010u)
#define  OS_ERR_TASK_NOT_SUSPENDED            (OS_ERR)(29011u)
#define  OS_ERR_TASK_OPT                      (OS_ERR)(29012u)
#define  OS_ERR_TASK_RESUME_ISR               (OS_ERR)(29013u)
#define  OS_ERR_TASK_RESUME_PRIO              (OS_ERR)(29014u)
#define  OS_ERR_TASK_RESUME_SELF              (OS_ERR)(29015u)
#define  OS_ERR_TASK_STK_CHK_ISR              (OS_ERR)(29016u)
#define  OS_ERR_TASK_SUSPENDED                (OS_ERR)(29017u)
#define  OS_ERR_TASK_SUSPEND_IDLE             (OS_ERR)(29018u)
#define  OS_ERR_TASK_SUSPEND_INT_HANDLER      (OS_ERR)(29019u)
#define  OS_ERR_TASK_SUSPEND_ISR              (OS_ERR)(29020u)
#define  OS_ERR_TASK_SUSPEND_PRIO             (OS_ERR)(29021u)
#define  OS_ERR_TASK_WAITING                  (OS_ERR)(29022u)

#define  OS_ERR_TCB_INVALID                   (OS_ERR)(29101u)

#define  OS_ERR_TICK_WHEEL_SIZE               (OS_ERR)(29201u)

#define  OS_ERR_TIME_DLY_ISR                  (OS_ERR)(29301u)
#define  OS_ERR_TIME_DLY_RESUME_ISR           (OS_ERR)(29302u)
#define  OS_ERR_TIME_GET_ISR                  (OS_ERR)(29303u)
#define  OS_ERR_TIME_INVALID_HOURS            (OS_ERR)(29304u)
#define  OS_ERR_TIME_INVALID_MINUTES          (OS_ERR)(29305u)
#define  OS_ERR_TIME_INVALID_SECONDS          (OS_ERR)(29306u)
#define  OS_ERR_TIME_INVALID_MILLISECONDS     (OS_ERR)(29307u)
#define  OS_ERR_TIME_NOT_DLY                  (OS_ERR)(29308u)
#define  OS_ERR_TIME_SET_ISR                  (OS_ERR)(29309u)
#define  OS_ERR_TIME_ZERO_DLY                 (OS_ERR)(29310u)

#define  OS_ERR_TIMEOUT                       (OS_ERR)(29401u)

#define  OS_ERR_TMR_INACTIVE                  (OS_ERR)(29501u)
#define  OS_ERR_TMR_INVALID_DEST              (OS_ERR)(29502u)
#define  OS_ERR_TMR_INVALID_DLY               (OS_ERR)(29503u)
#define  OS_ERR_TMR_INVALID_PERIOD            (OS_ERR)(29504u)
#define  OS_ERR_TMR_INVALID_STATE             (OS_ERR)(29505u)
#define  OS_ERR_TMR_INVALID                   (OS_ERR)(29506u)
#define  OS_ERR_TMR_ISR                       (OS_ERR)(29507u)
#define  OS_ERR_TMR_NO_CALLBACK               (OS_ERR)(29508u)
#define  OS_ERR_TMR_NON_AVAIL                 (OS_ERR)(29509u)
#define  OS_ERR_TMR_STK_INVALID               (OS_ERR)(29510u)
#define  OS_ERR_TMR_STOPPED                   (OS_ERR)(29511u)

#define  OS_ERR_U                             (OS_ERR)(30000u)

#define  OS_ERR_V                             (OS_ERR)(31000u)

#define  OS_ERR_W                             (OS_ERR)(32000u)

#define  OS_ERR_X                             (OS_ERR)(33000u)

#define  OS_ERR_Y                             (OS_ERR)(34000u)
#define  OS_ERR_YIELD_ISR                     (OS_ERR)(34001u)

#define  OS_ERR_Z                             (OS_ERR)(35000u)

/*
========================================================================================================================
*                                                      TASK STATUS 
========================================================================================================================
*/

#define  OS_STATE_OS_STOPPED                     (OS_STATE)(0u)
#define  OS_STATE_OS_RUNNING                     (OS_STATE)(1u)

#define  OS_STATE_NOT_RDY                     (CPU_BOOLEAN)(0u)
#define  OS_STATE_RDY                         (CPU_BOOLEAN)(1u)


                                                                /* ------------------- TASK STATES ------------------ */
#define  OS_TASK_STATE_BIT_DLY                (OS_STATE)(0x01u) /*   /-------- SUSPENDED bit                          */
                                                                /*   |                                                */
#define  OS_TASK_STATE_BIT_PEND               (OS_STATE)(0x02u) /*   | /-----  PEND      bit                          */
                                                                /*   | |                                              */
#define  OS_TASK_STATE_BIT_SUSPENDED          (OS_STATE)(0x04u) /*   | | /---  Delayed/Timeout bit                    */
                                                                /*   | | |                                            */
                                                                /*   V V V                                            */

#define  OS_TASK_STATE_RDY                    (OS_STATE)(  0u)  /*   0 0 0     Ready                                  */
#define  OS_TASK_STATE_DLY                    (OS_STATE)(  1u)  /*   0 0 1     Delayed or Timeout                     */
#define  OS_TASK_STATE_PEND                   (OS_STATE)(  2u)  /*   0 1 0     Pend                                   */
#define  OS_TASK_STATE_PEND_TIMEOUT           (OS_STATE)(  3u)  /*   0 1 1     Pend + Timeout                         */
#define  OS_TASK_STATE_SUSPENDED              (OS_STATE)(  4u)  /*   1 0 0     Suspended                              */
#define  OS_TASK_STATE_DLY_SUSPENDED          (OS_STATE)(  5u)  /*   1 0 1     Suspended + Delayed or Timeout         */
#define  OS_TASK_STATE_PEND_SUSPENDED         (OS_STATE)(  6u)  /*   1 1 0     Suspended + Pend                       */
#define  OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED (OS_STATE)(  7u)  /*   1 1 1     Suspended + Pend + Timeout             */
#define  OS_TASK_STATE_DEL                    (OS_STATE)(255u)

                                                                /* ----------------- PENDING ON ... ----------------- */
#define  OS_TASK_PEND_ON_NOTHING              (OS_STATE)(  0u)  /* Pending on nothing                                 */
#define  OS_TASK_PEND_ON_FLAG                 (OS_STATE)(  1u)  /* Pending on event flag group                        */
#define  OS_TASK_PEND_ON_TASK_Q               (OS_STATE)(  2u)  /* Pending on message to be sent to task              */
#define  OS_TASK_PEND_ON_MULTI                (OS_STATE)(  3u)  /* Pending on multiple semaphores and/or queues       */
#define  OS_TASK_PEND_ON_MUTEX                (OS_STATE)(  4u)  /* Pending on mutual exclusion semaphore              */
#define  OS_TASK_PEND_ON_Q                    (OS_STATE)(  5u)  /* Pending on queue                                   */
#define  OS_TASK_PEND_ON_SEM                  (OS_STATE)(  6u)  /* Pending on semaphore                               */
#define  OS_TASK_PEND_ON_TASK_SEM             (OS_STATE)(  7u)  /* Pending on signal  to be sent to task              */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                    TASK PEND STATUS 
*                                      (Status codes for OS_TCB's field .PendStatus)
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_STATUS_PEND_OK                   (OS_STATUS)(  0u)  /* Pending status OK, !pending, or pending complete   */
#define  OS_STATUS_PEND_ABORT                (OS_STATUS)(  1u)  /* Pending aborted                                    */
#define  OS_STATUS_PEND_DEL                  (OS_STATUS)(  2u)  /* Pending object deleted                             */
#define  OS_STATUS_PEND_TIMEOUT              (OS_STATUS)(  3u)  /* Pending timed out                                  */

/*
========================================================================================================================
*                                                   OS OBJECT TYPES
========================================================================================================================
*/

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  OS_OBJ_TYPE_NONE           (OS_OBJ_TYPE)(0x4E4F4E45u)  /* "NONE" in ASCII.                                   */
#define  OS_OBJ_TYPE_SEM            (OS_OBJ_TYPE)(0x53454D41u)  /* "SEMA" in ASCII.                                   */
#define  OS_OBJ_TYPE_Q              (OS_OBJ_TYPE)(0x51554555u)  /* "QUEU" in ASCII.                                   */
#define  OS_OBJ_TYPE_MUTEX          (OS_OBJ_TYPE)(0x4D555458u)  /* "MUTX" in ASCII.                                   */
#define  OS_OBJ_TYPE_FLAG           (OS_OBJ_TYPE)(0x464C4147u)  /* "FLAG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TMR            (OS_OBJ_TYPE)(0x544D5220u)  /* "TMR " in ASCII.                                   */
#define  OS_OBJ_TYPE_TASK_MSG       (OS_OBJ_TYPE)(0x544D5347u)  /* "TMSG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TASK_SIGNAL    (OS_OBJ_TYPE)(0x54534947u)  /* "TSIG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TICK           (OS_OBJ_TYPE)(0x5449434Bu)  /* "TICK" in ASCII.                                   */
#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  OS_OBJ_TYPE_NONE           (OS_OBJ_TYPE)(0x454E4F4Eu)  /* "NONE" in ASCII.                                   */
#define  OS_OBJ_TYPE_SEM            (OS_OBJ_TYPE)(0x414D4553u)  /* "SEMA" in ASCII.                                   */
#define  OS_OBJ_TYPE_Q              (OS_OBJ_TYPE)(0x55455551u)  /* "QUEU" in ASCII.                                   */
#define  OS_OBJ_TYPE_MUTEX          (OS_OBJ_TYPE)(0x5854554Du)  /* "MUTX" in ASCII.                                   */
#define  OS_OBJ_TYPE_FLAG           (OS_OBJ_TYPE)(0x47414C46u)  /* "FLAG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TMR            (OS_OBJ_TYPE)(0x524D2054u)  /* "TMR " in ASCII.                                   */
#define  OS_OBJ_TYPE_TASK_MSG       (OS_OBJ_TYPE)(0x47534D54u)  /* "TMSG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TASK_SIGNAL    (OS_OBJ_TYPE)(0x47495354u)  /* "TSIG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TICK           (OS_OBJ_TYPE)(0x4B434954u)  /* "TICK" in ASCII.                                   */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  OS_OBJ_TYPE_NONE           (OS_OBJ_TYPE)(0x4F4E454Eu)  /* "NONE" in ASCII.                                   */
#define  OS_OBJ_TYPE_SEM            (OS_OBJ_TYPE)(0x4553414Du)  /* "SEMA" in ASCII.                                   */
#define  OS_OBJ_TYPE_Q              (OS_OBJ_TYPE)(0x55515545u)  /* "QUEU" in ASCII.                                   */
#define  OS_OBJ_TYPE_MUTEX          (OS_OBJ_TYPE)(0x554D5854u)  /* "MUTX" in ASCII.                                   */
#define  OS_OBJ_TYPE_FLAG           (OS_OBJ_TYPE)(0x4C464741u)  /* "FLAG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TMR            (OS_OBJ_TYPE)(0x4D545220u)  /* "TMR " in ASCII.                                   */
#define  OS_OBJ_TYPE_TASK_MSG       (OS_OBJ_TYPE)(0x4D544753u)  /* "TMSG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TASK_SIGNAL    (OS_OBJ_TYPE)(0x53544749u)  /* "TSIG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TICK           (OS_OBJ_TYPE)(0x49544B43u)  /* "TICK" in ASCII.                                   */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                             */
#define  OS_OBJ_TYPE_NONE           (OS_OBJ_TYPE)(0x4E4F4E45u)  /* "NONE" in ASCII.                                   */
#define  OS_OBJ_TYPE_SEM            (OS_OBJ_TYPE)(0x53454D41u)  /* "SEMA" in ASCII.                                   */
#define  OS_OBJ_TYPE_Q              (OS_OBJ_TYPE)(0x51554555u)  /* "QUEU" in ASCII.                                   */
#define  OS_OBJ_TYPE_MUTEX          (OS_OBJ_TYPE)(0x4D555458u)  /* "MUTX" in ASCII.                                   */
#define  OS_OBJ_TYPE_FLAG           (OS_OBJ_TYPE)(0x464C4147u)  /* "FLAG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TMR            (OS_OBJ_TYPE)(0x544D5220u)  /* "TMR " in ASCII.                                   */
#define  OS_OBJ_TYPE_TASK_MSG       (OS_OBJ_TYPE)(0x544D5347u)  /* "TMSG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TASK_SIGNAL    (OS_OBJ_TYPE)(0x54534947u)  /* "TSIG" in ASCII.                                   */
#define  OS_OBJ_TYPE_TICK           (OS_OBJ_TYPE)(0x5449434Bu)  /* "TICK" in ASCII.                                   */

#endif
#endif

/*
========================================================================================================================
*                                           Possible values for 'opt' argument
========================================================================================================================
*/

#define  OS_OPT_NONE                         (OS_OPT)(0x0000u)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                    DELETE OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_DEL_NO_PEND                  (OS_OPT)(0x0000u)
#define  OS_OPT_DEL_ALWAYS                   (OS_OPT)(0x0001u)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     PEND OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_PEND_FLAG_MASK               (OS_OPT)(0x000Fu)
#define  OS_OPT_PEND_FLAG_CLR_ALL            (OS_OPT)(0x0001u)  /* Wait for ALL    the bits specified to be CLR       */
#define  OS_OPT_PEND_FLAG_CLR_AND            (OS_OPT)(0x0001u)    

#define  OS_OPT_PEND_FLAG_CLR_ANY            (OS_OPT)(0x0002u)  /* Wait for ANY of the bits specified to be CLR       */
#define  OS_OPT_PEND_FLAG_CLR_OR             (OS_OPT)(0x0002u)    

#define  OS_OPT_PEND_FLAG_SET_ALL            (OS_OPT)(0x0004u)  /* Wait for ALL    the bits specified to be SET       */
#define  OS_OPT_PEND_FLAG_SET_AND            (OS_OPT)(0x0004u)   

#define  OS_OPT_PEND_FLAG_SET_ANY            (OS_OPT)(0x0008u)  /* Wait for ANY of the bits specified to be SET       */
#define  OS_OPT_PEND_FLAG_SET_OR             (OS_OPT)(0x0008u)    

#define  OS_OPT_PEND_FLAG_CONSUME            (OS_OPT)(0x0100u)  /* Consume the flags if condition(s) satisfied        */


#define  OS_OPT_PEND_BLOCKING                (OS_OPT)(0x0000u)
#define  OS_OPT_PEND_NON_BLOCKING            (OS_OPT)(0x8000u)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                  PEND ABORT OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_PEND_ABORT_1                 (OS_OPT)(0x0000u)  /* Pend abort a single waiting task                   */
#define  OS_OPT_PEND_ABORT_ALL               (OS_OPT)(0x0100u)  /* Pend abort ALL tasks waiting                       */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     POST OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/


#define  OS_OPT_POST_NONE                    (OS_OPT)(0x0000u)

#define  OS_OPT_POST_FLAG_SET                (OS_OPT)(0x0000u)    
#define  OS_OPT_POST_FLAG_CLR                (OS_OPT)(0x0001u)    

#define  OS_OPT_POST_FIFO                    (OS_OPT)(0x0000u)  /* Default is to post FIFO                            */
#define  OS_OPT_POST_LIFO                    (OS_OPT)(0x0010u)  /* Post to highest priority task waiting              */
#define  OS_OPT_POST_1                       (OS_OPT)(0x0000u)  /* Post message to highest priority task waiting      */
#define  OS_OPT_POST_ALL                     (OS_OPT)(0x0200u)  /* Broadcast message to ALL tasks waiting             */

#define  OS_OPT_POST_NO_SCHED                (OS_OPT)(0x8000u)  /* Do not call the scheduler if this is selected      */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     TASK OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_TASK_NONE                    (OS_OPT)(0x0000u)  /* NO option selected                                 */
#define  OS_OPT_TASK_STK_CHK                 (OS_OPT)(0x0001u)  /* Enable stack checking for the task                 */
#define  OS_OPT_TASK_STK_CLR                 (OS_OPT)(0x0002u)  /* Clear the stack when the task is create            */
#define  OS_OPT_TASK_SAVE_FP                 (OS_OPT)(0x0004u)  /* Save the contents of any floating-point registers  */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     TIME OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_TIME_DLY                     (OS_OPT)(0u)
#define  OS_OPT_TIME_TIMEOUT                 (OS_OPT)(1u)
#define  OS_OPT_TIME_MATCH                   (OS_OPT)(2u)

#define  OS_OPT_TIME_HMSM_STRICT             (OS_OPT)(0u)
#define  OS_OPT_TIME_HMSM_NON_STRICT         (OS_OPT)(8u)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                    TIMER OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_TMR_NONE                     (OS_OPT)(0u)       /* No option selected                                 */

#define  OS_OPT_TMR_ONE_SHOT                 (OS_OPT)(1u)       /* Timer will not auto restart when it expires        */
#define  OS_OPT_TMR_PERIODIC                 (OS_OPT)(2u)       /* Timer will     auto restart when it expires        */

#define  OS_OPT_TMR_CALLBACK                 (OS_OPT)(3u)       /* OSTmrStop() option to call 'callback' w/ timer arg */
#define  OS_OPT_TMR_CALLBACK_ARG             (OS_OPT)(4u)       /* OSTmrStop() option to call 'callback' w/ new   arg */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                      TIMER STATES
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_TMR_STATE_UNUSED               (OS_STATE)(0u)
#define  OS_TMR_STATE_STOPPED              (OS_STATE)(1u)
#define  OS_TMR_STATE_RUNNING              (OS_STATE)(2u)
#define  OS_TMR_STATE_COMPLETED            (OS_STATE)(3u)

/*$PAGE*/
/*
************************************************************************************************************************
************************************************************************************************************************
*                                                  D A T A   T Y P E S
************************************************************************************************************************
************************************************************************************************************************
*/

#if  OS_CFG_FLAG_EN > 0u
typedef  struct  os_flag_grp         OS_FLAG_GRP;
#endif


#if  OS_CFG_MEM_EN > 0u
typedef  struct  os_mem              OS_MEM;
#endif


typedef  struct  os_msg              OS_MSG;     
typedef  struct  os_msg_pool         OS_MSG_POOL;
typedef  struct  os_msg_q            OS_MSG_Q;


#if  OS_CFG_MUTEX_EN > 0u
typedef  struct  os_mutex            OS_MUTEX;
#endif


typedef  struct  os_int_q            OS_INT_Q;


#if  OS_CFG_Q_EN > 0u
typedef  struct  os_q                OS_Q;
#endif


#if  OS_CFG_SEM_EN > 0u
typedef  struct  os_sem              OS_SEM;
#endif


typedef  void                      (*OS_TASK_PTR)(void *p_arg);

typedef  struct  os_tcb              OS_TCB;

typedef  struct  os_rdy_list         OS_RDY_LIST;

typedef  struct  os_tick_spoke       OS_TICK_SPOKE;


#if  OS_CFG_TMR_EN > 0u
typedef  void                      (*OS_TMR_CALLBACK_PTR)(void *p_tmr, void *p_arg);
typedef  struct  os_tmr              OS_TMR;
typedef  struct  os_tmr_spoke        OS_TMR_SPOKE;
#endif


typedef  struct  os_pend_data        OS_PEND_DATA;
typedef  struct  os_pend_list        OS_PEND_LIST;
typedef  struct  os_pend_obj         OS_PEND_OBJ;

#if OS_CFG_APP_HOOKS_EN > 0u
typedef  void                      (*OS_APP_HOOK_VOID)(void); 
typedef  void                      (*OS_APP_HOOK_TCB)(OS_TCB *p_tcb);
#endif

/*$PAGE*/
/*
************************************************************************************************************************
************************************************************************************************************************
*                                          D A T A   S T R U C T U R E S
************************************************************************************************************************
************************************************************************************************************************
*/

/*
------------------------------------------------------------------------------------------------------------------------
*                                                    ISR POST DATA
------------------------------------------------------------------------------------------------------------------------
*/

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
struct  os_int_q {
    OS_OBJ_TYPE          Type;
    OS_INT_Q            *NextPtr;
    void                *ObjPtr;
    void                *MsgPtr;
    OS_MSG_SIZE          MsgSize;
    OS_FLAGS             Flags;
    OS_OPT               Opt;
    CPU_TS               TS;
};
#endif

/*
------------------------------------------------------------------------------------------------------------------------
*                                                      READY LIST
------------------------------------------------------------------------------------------------------------------------
*/

struct  os_rdy_list {
    OS_TCB              *HeadPtr;
    OS_TCB              *TailPtr;
    OS_OBJ_QTY           NbrEntries;
};


/*
------------------------------------------------------------------------------------------------------------------------
*                                           PEND DATA, PEND LIST and PEND OBJ
------------------------------------------------------------------------------------------------------------------------
*/

struct  os_pend_data {
    OS_PEND_DATA        *PrevPtr;
    OS_PEND_DATA        *NextPtr;
    OS_TCB              *TCBPtr;
    OS_PEND_OBJ         *PendObjPtr;
    OS_PEND_OBJ         *RdyObjPtr;
    void                *RdyMsgPtr;
    OS_MSG_SIZE          RdyMsgSize;
    CPU_TS               RdyTS;
};



struct  os_pend_list {
    OS_PEND_DATA        *HeadPtr;
    OS_PEND_DATA        *TailPtr;
    OS_OBJ_QTY           NbrEntries;
};



struct  os_pend_obj {
    OS_OBJ_TYPE          Type;
    CPU_CHAR            *NamePtr;
    OS_PEND_LIST         PendList;                          /* List of tasks pending on object                        */
};

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     EVENT FLAGS
------------------------------------------------------------------------------------------------------------------------
*/


#if OS_CFG_FLAG_EN > 0u
struct  os_flag_grp {                                       /* Event Flag Group                                       */
    OS_OBJ_TYPE          Type;                              /* Should be set to OS_EVENT_TYPE_FLAG                    */
    CPU_CHAR            *NamePtr;
    OS_PEND_LIST         PendList;                          /* List of tasks waiting on event flag group              */
    OS_FLAGS             Flags;                             /* 8, 16 or 32 bit flags                                  */
    CPU_TS               TS;                                /* Timestamp of when last post occurred                   */
#if OS_CFG_DBG_EN > 0u
    OS_OBJ_QTY           DbgIx;
#endif
    int                  Prio;                              /* Revision Michael Vysotsky correct bug */
};
#endif

/*$PAGE*/
/*
------------------------------------------------------------------------------------------------------------------------
*                                                   MEMORY PARTITIONS
------------------------------------------------------------------------------------------------------------------------
*/


#if OS_CFG_MEM_EN > 0u
struct os_mem {                                             /* MEMORY CONTROL BLOCK                                   */
    void                *AddrPtr;                           /* Pointer to beginning of memory partition               */
    CPU_CHAR            *NamePtr;
    OS_MEM              *FreeListPtr;                       /* Pointer to list of free memory blocks                  */
    OS_MEM_SIZE          BlkSize;                           /* Size (in bytes) of each block of memory                */
    OS_MEM_QTY           NbrMax;                            /* Total number of blocks in this partition               */
    OS_MEM_QTY           NbrFree;                           /* Number of memory blocks remaining in this partition    */
#if OS_CFG_DBG_EN > 0u
    OS_OBJ_QTY           DbgIx;
#endif
};
#endif

/*$PAGE*/
/*
------------------------------------------------------------------------------------------------------------------------
*                                                       MESSAGES
------------------------------------------------------------------------------------------------------------------------
*/

#if (OS_MSG_EN) > 0u
struct  os_msg {                                            /* MESSAGE CONTROL BLOCK                                  */
    OS_MSG              *NextPtr;                           /* Pointer to next message                                */
    void                *MsgPtr;                            /* Actual message                                         */
    OS_MSG_SIZE          MsgSize;                           /* Size of the message (in # bytes)                       */
    CPU_TS               MsgTS;                             /* Time stamp of when message was sent                    */
};




struct  os_msg_pool {                                       /* OS_MSG POOL                                            */
    OS_MSG              *NextPtr;                           /* Pointer to next message                                */
    OS_MSG_QTY           NbrFree;                           /* Number of messages available from this pool            */
    OS_MSG_QTY           NbrUsed;                           /* Number of messages used                                */
};



struct  os_msg_q {
    OS_MSG              *InPtr;
    OS_MSG              *OutPtr;
    OS_MSG_QTY           NbrEntriesSize;
    OS_MSG_QTY           NbrEntries;
    OS_MSG_QTY           NbrEntriesMax;
};
#endif

/*$PAGE*/
/*
------------------------------------------------------------------------------------------------------------------------
*                                              MUTUAL EXCLUSION SEMAPHORES
------------------------------------------------------------------------------------------------------------------------
*/

#if OS_CFG_MUTEX_EN > 0u
struct  os_mutex {
    OS_OBJ_TYPE          Type;
    CPU_CHAR            *NamePtr;
    OS_PEND_LIST         PendList;
    OS_TCB              *OwnerTCBPtr;
    OS_PRIO              OwnerOriginalPrio;
    OS_NESTING_CTR       OwnerNestingCtr;                   /* Mutex is available when the counter is 0               */
    CPU_TS               TS;
#if OS_CFG_DBG_EN > 0u
    OS_OBJ_QTY           DbgIx;
#endif
};
#endif

/*$PAGE*/
/*
------------------------------------------------------------------------------------------------------------------------
*                                                    MESSAGE QUEUES
------------------------------------------------------------------------------------------------------------------------
*/

#if OS_CFG_Q_EN > 0u
struct  os_q {
    OS_OBJ_TYPE          Type;
    CPU_CHAR            *NamePtr;
    OS_PEND_LIST         PendList;
    OS_MSG_Q             MsgQ;
#if OS_CFG_DBG_EN > 0u
    OS_OBJ_QTY           DbgIx;
#endif
};
#endif

/*$PAGE*/
/*
------------------------------------------------------------------------------------------------------------------------
*                                                      SEMAPHORES
------------------------------------------------------------------------------------------------------------------------
*/

#if OS_CFG_SEM_EN > 0u
struct  os_sem {
    OS_OBJ_TYPE          Type;
    CPU_CHAR            *NamePtr;
    OS_PEND_LIST         PendList;
    OS_SEM_CTR           Ctr;
    CPU_TS               TS;
#if OS_CFG_DBG_EN > 0u
    OS_OBJ_QTY           DbgIx;
#endif
};
#endif

/*$PAGE*/
/*
------------------------------------------------------------------------------------------------------------------------
*                                                  TASK CONTROL BLOCK
------------------------------------------------------------------------------------------------------------------------
*/

struct os_tcb {
    CPU_STK             *StkPtr;                            /* Pointer to current top of stack                        */
    
    void                *ExtPtr;                            /* Pointer to user definable data for TCB extension       */

    CPU_STK             *StkLimitPtr;                       /* Pointer used to set stack 'watermark' limit            */

    OS_TCB              *NextPtr;                           /* Pointer to next     TCB in the TCB list                */
    OS_TCB              *PrevPtr;                           /* Pointer to previous TCB in the TCB list                */

    OS_TCB              *TickNextPtr;
    OS_TCB              *TickPrevPtr;                       

    OS_TICK_SPOKE       *TickSpokePtr;                      /* Pointer to tick spoke if task is in the tick list      */

    CPU_CHAR            *NamePtr;                           /* Pointer to task name                                   */

    CPU_STK             *StkBasePtr;                        /* Pointer to base address of stack                       */

    OS_TASK_PTR          TaskEntryAddr;                     /* Pointer to task entry point address                    */
    void                *TaskEntryArg;                      /* Argument passed to task when it was created            */
        
    OS_PEND_DATA        *PendDataTblPtr;                    /* Pointer to list containing objects pended on           */
    OS_OBJ_QTY           PendDataTblEntries;                /* Size of array of objects to pend on                    */

    CPU_TS               TS;                                /* Timestamp                                              */

#if (OS_CFG_Q_EN > 0u) || (OS_CFG_TASK_Q_EN > 0u)
    void                *MsgPtr;                            /* Message received                                       */
    OS_MSG_SIZE          MsgSize;
#endif

#if OS_CFG_TASK_Q_EN > 0u
    OS_MSG_Q             MsgQ;                              /* Message queue associated with task                     */
#if OS_CFG_TASK_PROFILE_EN > 0u
    CPU_TS               MsgQPendTime;                      /* Time it took for signal to be received                 */
    CPU_TS               MsgQPendTimeMax;                   /* Max amount of time it took for signal to be received   */
#endif
#endif

#if OS_CFG_FLAG_EN > 0u
    OS_FLAGS             FlagsPend;                         /* Event flag(s) to wait on                               */
    OS_OPT               FlagsOpt;                          /* Options (See OS_OPT_FLAG_xxx)                          */
    OS_FLAGS             FlagsRdy;                          /* Event flags that made task ready to run                */
#endif

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    OS_REG               RegTbl[OS_CFG_TASK_REG_TBL_SIZE];  /* Task specific registers                                */
#endif

    OS_SEM_CTR           SemCtr;                            /* Task specific semaphore counter                        */
#if OS_CFG_TASK_PROFILE_EN > 0u
    CPU_TS               SemPendTime;                       /* Time it took for signal to be received                 */
    CPU_TS               SemPendTimeMax;                    /* Max amount of time it took for signal to be received   */
#endif

#if OS_CFG_TASK_SUSPEND_EN > 0u
    OS_NESTING_CTR       SuspendCtr;                        /* Nesting counter for OSTaskSuspend()                    */
#endif    

    OS_STK_SIZE          StkSize;                           /* Size of task stack (in number of stack elements)       */
#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
    OS_STK_SIZE          StkUsed;                           /* Number of bytes used from the stack                    */
    OS_STK_SIZE          StkFree;                           /* Number of bytes free on   the stack                    */
#endif    
    OS_OPT               Opt;                               /* Task options as passed by OSTaskCreate()               */

                                                            /* DELAY / TIMEOUT                                        */
    OS_TICK              TickCtrMatch;                      /* Absolute time when task is going to be ready           */
    OS_TICK              TickRemain;                        /* Number of ticks remaining for a match (updated at ...  */
                                                            /* ... run-time by OS_StatTask()                          */
    OS_TICK              TimeQuanta;
    OS_TICK              TimeQuantaCtr;

#if OS_CFG_TASK_PROFILE_EN > 0u
    OS_CPU_USAGE         CPUUsage;                          /* CPU Usage of task (0-100%)                             */
    OS_CTX_SW_CTR        CtxSwCtr;                          /* Number of time the task was switched in                */
    CPU_TS               CyclesDelta;                       /* value of OS_TS_GET() - .CyclesStart                    */
    CPU_TS               CyclesStart;                       /* Snapshot of cycle counter at start of task resumption  */
    OS_CYCLES            CyclesTotal;                       /* Total number of # of cycles the task has been running  */
#endif
    
#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_TS               IntDisTimeMax;                     /* Maximum interrupt disable time                         */
#endif
#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u                                           
    CPU_TS               SchedLockTimeMax;                  /* Maximum scheduler lock time                            */
#endif

    OS_STATE             PendOn;                            /* Indicates what task is pending on                      */
    OS_STATUS            PendStatus;                        /* Pend status                                            */
    OS_STATE             TaskState;                         /* See OS_TASK_STATE_xxx                                  */
     
    OS_PRIO              Prio;                              /* Task priority (0 == highest)                           */

#if OS_CFG_DBG_EN > 0u
    OS_OBJ_QTY           DbgIx;
#endif
};

/*$PAGE*/
/*
------------------------------------------------------------------------------------------------------------------------
*                                                    TICK DATA TYPE 
------------------------------------------------------------------------------------------------------------------------
*/

struct  os_tick_spoke {
    OS_TCB              *FirstPtr;
    OS_OBJ_QTY           NbrEntries;
    OS_OBJ_QTY           NbrEntriesMax;
};


/*
------------------------------------------------------------------------------------------------------------------------
*                                                   TIMER DATA TYPES
------------------------------------------------------------------------------------------------------------------------
*/

#if OS_CFG_TMR_EN > 0u
struct  os_tmr {
    OS_OBJ_TYPE          Type;
    CPU_CHAR            *NamePtr;                           /* Name to give the timer                                 */
    OS_TMR_CALLBACK_PTR  CallbackPtr;                       /* Function to call when timer expires                    */
    void                *CallbackPtrArg;                    /* Argument to pass to function when timer expires        */
    OS_TMR              *NextPtr;                           /* Double link list pointers                              */
    OS_TMR              *PrevPtr;
    OS_TICK              Match;                             /* Timer expires when OSTmrTickCtr matches this value     */
    OS_TICK              Remain;                            /* Amount of time remaining before timer expires          */
    OS_TICK              Dly;                               /* Delay before start of repeat                           */
    OS_TICK              Period;                            /* Period to repeat timer                                 */
    OS_OPT               Opt;                               /* Options (see OS_OPT_TMR_xxx)                           */
    OS_STATE             State;
#if OS_CFG_DBG_EN > 0u
    OS_OBJ_QTY           DbgIx;
#endif
};



struct  os_tmr_spoke {
    OS_TMR              *FirstPtr;                          /* Pointer to first timer in linked list                  */
    OS_OBJ_QTY           NbrEntries;
    OS_OBJ_QTY           NbrEntriesMax;
};
#endif

/*$PAGE*/
/*
************************************************************************************************************************
************************************************************************************************************************
*                                           G L O B A L   V A R I A B L E S
************************************************************************************************************************
************************************************************************************************************************
*/

#if OS_CFG_APP_HOOKS_EN > 0u
OS_EXT           OS_APP_HOOK_TCB         OS_AppTaskCreateHookPtr;     /* Application hooks                            */
OS_EXT           OS_APP_HOOK_TCB         OS_AppTaskDelHookPtr;
OS_EXT           OS_APP_HOOK_TCB         OS_AppTaskReturnHookPtr;

OS_EXT           OS_APP_HOOK_VOID        OS_AppIdleTaskHookPtr;
OS_EXT           OS_APP_HOOK_VOID        OS_AppInitHookPtr;
OS_EXT           OS_APP_HOOK_VOID        OS_AppStatTaskHookPtr;
OS_EXT           OS_APP_HOOK_VOID        OS_AppTaskSwHookPtr;
OS_EXT           OS_APP_HOOK_VOID        OS_AppTimeTickHookPtr;
#endif

                                                                      /* IDLE TASK ---------------------------------- */
OS_EXT            OS_IDLE_CTR            OSIdleTaskCtr;    
OS_EXT            OS_TCB                 OSIdleTaskTCB;

                                                                      /* MISCELLANEOUS ------------------------------ */
OS_EXT            OS_NESTING_CTR         OSIntNestingCtr;             /* Interrupt nesting level                      */
#ifdef CPU_CFG_INT_DIS_MEAS_EN
OS_EXT            CPU_TS                 OSIntDisTimeMax;             /* Overall interrupt disable time               */
#endif

OS_EXT            OS_STATE               OSRunning;                   /* Flag indicating that kernel is running       */


                                                                      /* ISR HANDLER TASK --------------------------- */
#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
OS_EXT            OS_INT_Q              *OSIntQInPtr;
OS_EXT            OS_INT_Q              *OSIntQOutPtr;
OS_EXT            OS_OBJ_QTY             OSIntQNbrEntries;
OS_EXT            OS_OBJ_QTY             OSIntQOvfCtr;
OS_EXT            OS_TCB                 OSIntQTaskTCB;
OS_EXT            CPU_TS                 OSIntQTaskTimeMax;
#endif

                                                                      /* FLAGS -------------------------------------- */
#if OS_CFG_FLAG_EN > 0u
OS_EXT            OS_OBJ_QTY             OSFlagQty;
#endif

                                                                      /* MEMORY MANAGEMENT -------------------------- */
#if OS_CFG_MEM_EN > 0u
OS_EXT            OS_OBJ_QTY             OSMemQty;                    /* Number of memory partitions created          */
#endif

                                                                      /* OS_MSG POOL -------------------------------- */
OS_EXT            OS_MSG_POOL            OSMsgPool;                   /* Pool of OS_MSG                               */

                                                                      /* MUTEX MANAGEMENT --------------------------- */
#if OS_CFG_MUTEX_EN > 0u
OS_EXT            OS_OBJ_QTY             OSMutexQty;                  /* Number of mutexes created                    */
#endif

                                                                      /* PRIORITIES --------------------------------- */
OS_EXT            OS_PRIO                OSPrioCur;                   /* Priority of current task                     */
OS_EXT            OS_PRIO                OSPrioHighRdy;               /* Priority of highest priority task            */
OS_EXT            OS_PRIO                OSPrioSaved;                 /* Saved priority level when Post Deferred      */
extern            CPU_DATA               OSPrioTbl[OS_PRIO_TBL_SIZE];

                                                                      /* QUEUES ------------------------------------- */
#if OS_CFG_Q_EN > 0u
OS_EXT            OS_OBJ_QTY             OSQQty;                      /* Number of message queues created             */
#endif



                                                                      /* READY LIST --------------------------------- */
OS_EXT            OS_RDY_LIST            OSRdyList[OS_CFG_PRIO_MAX];  /* Table of tasks ready to run                  */


                                                                      /* SCHEDULER ---------------------------------- */
#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
OS_EXT            CPU_TS                 OSSchedLockTimeBegin;        /* Scheduler lock time measurement              */
OS_EXT            CPU_TS                 OSSchedLockTimeDelta;
OS_EXT            CPU_TS                 OSSchedLockTimeEnd;
OS_EXT            CPU_TS                 OSSchedLockTimeOvrhd;
OS_EXT            CPU_TS                 OSSchedLockTimeMax;
OS_EXT            CPU_TS                 OSSchedLockTimeMaxCur;
#endif

OS_EXT            OS_NESTING_CTR         OSSchedLockNestingCtr;       /* Lock nesting level                           */
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
OS_EXT            OS_TICK                OSSchedRoundRobinDfltTimeQuanta;
OS_EXT            CPU_BOOLEAN            OSSchedRoundRobinEn;         /* Enable/Disable round-robin scheduling        */
#endif
                                                                      /* SEMAPHORES --------------------------------- */
#if OS_CFG_SEM_EN > 0u
OS_EXT            OS_OBJ_QTY             OSSemQty;                    /* Number of semaphores created                 */
#endif

                                                                      /* STATISTICS --------------------------------- */
#if OS_CFG_STAT_TASK_EN > 0u
OS_EXT            OS_CPU_USAGE           OSStatTaskCPUUsage;          /* CPU Usage in %                               */
OS_EXT            OS_TICK                OSStatTaskCtr;    
OS_EXT            OS_TICK                OSStatTaskCtrMax;    
OS_EXT            OS_TICK                OSStatTaskCtrRun;    
OS_EXT            CPU_BOOLEAN            OSStatTaskRdy;
OS_EXT            OS_TCB                 OSStatTaskTCB;
OS_EXT            CPU_TS                 OSStatTaskTimeMax;
#endif

                                                                      /* TASKS -------------------------------------- */
OS_EXT            OS_CTX_SW_CTR          OSTaskCtxSwCtr;              /* Number of context switches                   */
OS_EXT            OS_OBJ_QTY             OSTaskQty;                   /* Number of tasks created                      */


                                                                      /* TICK TASK ---------------------------------- */
OS_EXT            OS_TICK                OSTickCtr;                   /* Counts the #ticks since startup or last set  */
OS_EXT            OS_TCB                 OSTickTaskTCB;
OS_EXT            CPU_TS                 OSTickTaskTimeCur;
OS_EXT            CPU_TS                 OSTickTaskTimeMax;


#if OS_CFG_TMR_EN > 0u                                                /* TIMERS ------------------------------------- */
OS_EXT            OS_OBJ_QTY             OSTmrQty;                    /* Number of timers created                     */
OS_EXT            OS_TCB                 OSTmrTaskTCB;                /* TCB of timer task                            */
OS_EXT            CPU_TS                 OSTmrTaskTimeMax;
OS_EXT            OS_TMR_TICK            OSTmrTickCtr;                /* Current time for the timers                  */
OS_EXT            OS_CTR                 OSTmrUpdateCnt;              /* Counter for updating timers                  */
OS_EXT            OS_CTR                 OSTmrUpdateCtr;
#endif

                                                                      /* TCBs --------------------------------------- */
OS_EXT            OS_TCB                *OSTCBCurPtr;                 /* Pointer to currently running TCB             */
OS_EXT            OS_TCB                *OSTCBHighRdyPtr;             /* Pointer to highest priority  TCB             */

/*$PAGE*/
/*
************************************************************************************************************************
************************************************************************************************************************
*                                                   E X T E R N A L S
************************************************************************************************************************
************************************************************************************************************************
*/

extern  OS_OBJ_QTY    const OSCfg_FlagDbgTblSize;      

extern  CPU_STK     * const OSCfg_IdleTaskStkBasePtr;  
extern  CPU_STK     * const OSCfg_IdleTaskStkLimitPtr; 
extern  OS_STK_SIZE   const OSCfg_IdleTaskStkSize;     
extern  CPU_INT32U    const OSCfg_IdleTaskStkSizeRAM;

extern  OS_INT_Q    * const OSCfg_IntQBasePtr;            
extern  OS_OBJ_QTY    const OSCfg_IntQSize;            
extern  CPU_INT32U    const OSCfg_IntQSizeRAM;            
extern  CPU_STK     * const OSCfg_IntQTaskStkBasePtr;  
extern  CPU_STK     * const OSCfg_IntQTaskStkLimitPtr; 
extern  OS_STK_SIZE   const OSCfg_IntQTaskStkSize;     
extern  CPU_INT32U    const OSCfg_IntQTaskStkSizeRAM;

extern  CPU_STK     * const OSCfg_ISRStkBasePtr;
extern  OS_STK_SIZE   const OSCfg_ISRStkSize;          
extern  CPU_INT32U    const OSCfg_ISRStkSizeRAM;

extern  OS_OBJ_QTY    const OSCfg_MemDbgTblSize;       

extern  OS_MSG_SIZE   const OSCfg_MsgPoolSize;              
extern  CPU_INT32U    const OSCfg_MsgPoolSizeRAM;            
extern  OS_MSG      * const OSCfg_MsgPoolBasePtr;

extern  OS_OBJ_QTY    const OSCfg_MutexDbgTblSize;     

extern  OS_OBJ_QTY    const OSCfg_QDbgTblSize;         

extern  OS_OBJ_QTY    const OSCfg_SemDbgTblSize;      

extern  OS_PRIO       const OSCfg_StatTaskPrio;        
extern  OS_RATE_HZ    const OSCfg_StatTaskRate_Hz;        
extern  CPU_STK     * const OSCfg_StatTaskStkBasePtr;  
extern  CPU_STK     * const OSCfg_StatTaskStkLimitPtr; 
extern  OS_STK_SIZE   const OSCfg_StatTaskStkSize;     
extern  CPU_INT32U    const OSCfg_StatTaskStkSizeRAM;

extern  OS_STK_SIZE   const OSCfg_StkSizeMin;          

extern  OS_OBJ_QTY    const OSCfg_TaskDbgTblSize;      

extern  OS_RATE_HZ    const OSCfg_TickRate_Hz;            
extern  OS_PRIO       const OSCfg_TickTaskPrio;        
extern  CPU_STK     * const OSCfg_TickTaskStkBasePtr;  
extern  CPU_STK     * const OSCfg_TickTaskStkLimitPtr; 
extern  OS_STK_SIZE   const OSCfg_TickTaskStkSize;     
extern  CPU_INT32U    const OSCfg_TickTaskStkSizeRAM;
extern  OS_OBJ_QTY    const OSCfg_TickWheelSize;       
extern  CPU_INT32U    const OSCfg_TickWheelSizeRAM;            

extern  OS_OBJ_QTY    const OSCfg_TmrDbgTblSize;       
extern  OS_PRIO       const OSCfg_TmrTaskPrio;         
extern  OS_RATE_HZ    const OSCfg_TmrTaskRate_Hz;         
extern  CPU_STK     * const OSCfg_TmrTaskStkBasePtr;   
extern  CPU_STK     * const OSCfg_TmrTaskStkLimitPtr;  
extern  OS_STK_SIZE   const OSCfg_TmrTaskStkSize;      
extern  CPU_INT32U    const OSCfg_TmrTaskStkSizeRAM;
extern  OS_OBJ_QTY    const OSCfg_TmrWheelSize;        
extern  CPU_INT32U    const OSCfg_TmrSizeRAM;            


#if (OS_CFG_DBG_EN > 0u) && (OS_CFG_FLAG_EN > 0u)    
extern  OS_FLAG_GRP   *OSCfg_FlagDbgTbl[];
#endif

extern  CPU_STK        OSCfg_IdleTaskStk[];

#if (OS_CFG_ISR_POST_DEFERRED_EN > 0u)
extern  CPU_STK        OSCfg_IntQTaskStk[];
extern  OS_INT_Q       OSCfg_IntQ[];
#endif

extern  CPU_STK        OSCfg_ISRStk[];

#if (OS_CFG_DBG_EN > 0u) && (OS_CFG_MEM_EN > 0u)    
extern  OS_MEM        *OSCfg_MemDbgTbl[];
#endif

#if (OS_MSG_EN > 0u)
extern  OS_MSG         OSCfg_MsgPool[];
#endif

#if (OS_CFG_DBG_EN > 0u) && (OS_CFG_MUTEX_EN > 0u)    
extern  OS_MUTEX      *OSCfg_MutexDbgTbl[];
#endif

#if (OS_CFG_DBG_EN > 0u) && (OS_CFG_Q_EN > 0u)    
extern  OS_Q          *OSCfg_QDbgTbl[];
#endif

#if (OS_CFG_DBG_EN > 0u) && (OS_CFG_SEM_EN > 0u)      
extern  OS_SEM        *OSCfg_SemDbgTbl[];
#endif

#if (OS_CFG_STAT_TASK_EN > 0u)
extern  CPU_STK        OSCfg_StatTaskStk[];
#endif

#if (OS_CFG_DBG_EN > 0u)
extern  OS_TCB        *OSCfg_TaskDbgTbl[];
#endif

extern  CPU_STK        OSCfg_TickTaskStk[];
extern  OS_TICK_SPOKE  OSCfg_TickWheel[];

#if (OS_CFG_TMR_EN > 0u)
#if (OS_CFG_DBG_EN > 0u)
extern  OS_TMR        *OSCfg_TmrDbgTbl[];
#endif
extern  CPU_STK        OSCfg_TmrTaskStk[];
extern  OS_TMR_SPOKE   OSCfg_TmrWheel[];
#endif

/*
************************************************************************************************************************
************************************************************************************************************************
*                                        F U N C T I O N   P R O T O T Y P E S
************************************************************************************************************************
************************************************************************************************************************
*/

/* ================================================================================================================== */
/*                                                    EVENT FLAGS                                                     */
/* ================================================================================================================== */

#if OS_CFG_FLAG_EN > 0u  

void          OSFlagCreate              (OS_FLAG_GRP           *p_grp,
                                         CPU_CHAR              *p_name,
                                         OS_FLAGS               flags, 
                                         OS_ERR                *p_err);

#if OS_CFG_FLAG_DEL_EN > 0u
OS_OBJ_QTY    OSFlagDel                 (OS_FLAG_GRP           *p_grp, 
                                         OS_OPT                 opt, 
                                         OS_ERR                *p_err);
#endif

OS_FLAGS      OSFlagPend                (OS_FLAG_GRP           *p_grp,
                                         OS_FLAGS               flags, 
                                         OS_TICK                timeout,
                                         OS_OPT                 opt, 
                                         CPU_TS                *p_ts, 
                                         OS_ERR                *p_err);

#if OS_CFG_FLAG_PEND_ABORT_EN > 0u
OS_OBJ_QTY    OSFlagPendAbort           (OS_FLAG_GRP           *p_grp, 
                                         OS_OPT                 opt, 
                                         OS_ERR                *p_err);
#endif

OS_FLAGS      OSFlagPendGetFlagsRdy     (OS_ERR                *p_err);

OS_FLAGS      OSFlagPost                (OS_FLAG_GRP           *p_grp, 
                                         OS_FLAGS               flags, 
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_FlagClr                (OS_FLAG_GRP           *p_grp);

void          OS_FlagBlock              (OS_PEND_DATA          *p_pend_data,
                                         OS_FLAG_GRP           *p_grp, 
                                         OS_FLAGS               flags, 
                                         OS_OPT                 opt, 
                                         OS_TICK                timeout);

#if OS_CFG_DBG_EN > 0u
void          OS_FlagDbgTblAdd          (OS_FLAG_GRP           *p_grp);

void          OS_FlagDbgTblRemove       (OS_FLAG_GRP           *p_grp);
#endif

void          OS_FlagInit               (OS_ERR                *p_err);

OS_FLAGS      OS_FlagPost               (OS_FLAG_GRP           *p_grp, 
                                         OS_FLAGS               flags, 
                                         OS_OPT                 opt,
                                         CPU_TS                 ts,
                                         OS_ERR                *p_err);

void          OS_FlagTaskRdy            (OS_TCB                *p_tcb, 
                                         OS_FLAGS               flags_rdy,
                                         CPU_TS                 ts);
#endif

/*$PAGE*/
/* ================================================================================================================== */
/*                                          FIXED SIZE MEMORY BLOCK MANAGEMENT                                        */
/* ================================================================================================================== */

#if OS_CFG_MEM_EN > 0u  

void          OSMemCreate               (OS_MEM                *p_mem,
                                         CPU_CHAR              *p_name,
                                         void                  *p_addr, 
                                         OS_MEM_QTY             n_blks, 
                                         OS_MEM_SIZE            blk_size, 
                                         OS_ERR                *p_err);

void         *OSMemGet                  (OS_MEM                *p_mem, 
                                         OS_ERR                *p_err);

void          OSMemPut                  (OS_MEM                *p_mem, 
                                         void                  *p_blk,
                                         OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

#if OS_CFG_DBG_EN > 0u
void          OS_MemDbgTblAdd           (OS_MEM                *p_mem);
#endif

void          OS_MemInit                (OS_ERR                *p_err);

#endif

/*$PAGE*/
/* ================================================================================================================== */
/*                                             MUTUAL EXCLUSION SEMAPHORES                                            */
/* ================================================================================================================== */

#if OS_CFG_MUTEX_EN > 0u                                         

void          OSMutexCreate             (OS_MUTEX              *p_mutex,
                                         CPU_CHAR              *p_name,
                                         OS_ERR                *p_err);

#if OS_CFG_MUTEX_DEL_EN > 0u
OS_OBJ_QTY    OSMutexDel                (OS_MUTEX              *p_mutex, 
                                         OS_OPT                 opt, 
                                         OS_ERR                *p_err);
#endif

void          OSMutexPend               (OS_MUTEX              *p_mutex,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt, 
                                         CPU_TS                *p_ts, 
                                         OS_ERR                *p_err);

#if OS_CFG_MUTEX_PEND_ABORT_EN > 0u
OS_OBJ_QTY    OSMutexPendAbort          (OS_MUTEX              *p_mutex, 
                                         OS_OPT                 opt, 
                                         OS_ERR                *p_err);
#endif

void          OSMutexPost               (OS_MUTEX              *p_mutex,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);


/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_MutexClr               (OS_MUTEX              *p_mutex);

#if OS_CFG_DBG_EN > 0u
void          OS_MutexDbgTblAdd         (OS_MUTEX              *p_mutex);

void          OS_MutexDbgTblRemove      (OS_MUTEX              *p_mutex);
#endif

void          OS_MutexInit              (OS_ERR                *p_err);
#endif

/*$PAGE*/
/* ================================================================================================================== */
/*                                                   MESSAGE QUEUES                                                   */
/* ================================================================================================================== */

#if OS_CFG_PEND_MULTI_EN > 0u

OS_OBJ_QTY    OSPendMulti               (OS_PEND_DATA          *p_pend_data_tbl, 
                                         OS_OBJ_QTY             tbl_size,
                                         OS_TICK                timeout, 
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

OS_OBJ_QTY    OS_PendMultiGetRdy        (OS_PEND_DATA          *p_pend_data_tbl,
                                         OS_OBJ_QTY             tbl_size);

CPU_BOOLEAN   OS_PendMultiValidate      (OS_PEND_DATA          *p_pend_data_tbl,
                                         OS_OBJ_QTY             tbl_size);

void          OS_PendMultiWait          (OS_PEND_DATA          *p_pend_data_tbl,
                                         OS_OBJ_QTY             tbl_size,
                                         OS_TICK                timeout);
#endif

/* ================================================================================================================== */
/*                                                   MESSAGE QUEUES                                                   */
/* ================================================================================================================== */

#if OS_CFG_Q_EN > 0u

void          OSQCreate                 (OS_Q                  *p_q,
                                         CPU_CHAR              *p_name,
                                         OS_MSG_QTY             max_qty,
                                         OS_ERR                *p_err);

#if OS_CFG_Q_DEL_EN > 0u
OS_OBJ_QTY    OSQDel                    (OS_Q                  *p_q, 
                                         OS_OPT                 opt, 
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_Q_FLUSH_EN > 0u
OS_MSG_QTY    OSQFlush                  (OS_Q                  *p_q,
                                         OS_ERR                *p_err);
#endif

void         *OSQPend                   (OS_Q                  *p_q,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt, 
                                         OS_MSG_SIZE           *p_msg_size,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

#if OS_CFG_Q_PEND_ABORT_EN > 0u
OS_OBJ_QTY    OSQPendAbort              (OS_Q                  *p_q, 
                                         OS_OPT                 opt, 
                                         OS_ERR                *p_err);
#endif

void          OSQPost                   (OS_Q                  *p_q, 
                                         void                  *p_void,
                                         OS_MSG_SIZE            msg_size, 
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_QClr                   (OS_Q                  *p_q);

#if OS_CFG_DBG_EN > 0u
void          OS_QDbgTblAdd             (OS_Q                  *p_q);

void          OS_QDbgTblRemove          (OS_Q                  *p_q);
#endif

void          OS_QInit                  (OS_ERR                *p_err);

void          OS_QPost                  (OS_Q                  *p_q, 
                                         void                  *p_void,
                                         OS_MSG_SIZE            msg_size, 
                                         OS_OPT                 opt,
                                         CPU_TS                 ts,
                                         OS_ERR                *p_err);
#endif

/*$PAGE*/
/* ================================================================================================================== */
/*                                                     SEMAPHORES                                                     */
/* ================================================================================================================== */

#if OS_CFG_SEM_EN > 0u

void          OSSemCreate               (OS_SEM                *p_sem,
                                         CPU_CHAR              *p_name,
                                         OS_SEM_CTR             cnt,
                                         OS_ERR                *p_err);

OS_OBJ_QTY    OSSemDel                  (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

OS_SEM_CTR    OSSemPend                 (OS_SEM                *p_sem,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

#if OS_CFG_SEM_PEND_ABORT_EN > 0u
OS_OBJ_QTY    OSSemPendAbort            (OS_SEM                *p_sem, 
                                         OS_OPT                 opt, 
                                         OS_ERR                *p_err);
#endif

OS_SEM_CTR    OSSemPost                 (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#if OS_CFG_SEM_SET_EN > 0u
void          OSSemSet                  (OS_SEM                *p_sem,
                                         OS_SEM_CTR             cnt,
                                         OS_ERR                *p_err);
#endif

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_SemClr                 (OS_SEM                *p_sem);

#if OS_CFG_DBG_EN > 0u
void          OS_SemDbgTblAdd           (OS_SEM                *p_sem);

void          OS_SemDbgTblRemove        (OS_SEM                *p_sem);
#endif

void          OS_SemInit                (OS_ERR                *p_err);

OS_SEM_CTR    OS_SemPost                (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         CPU_TS                 ts, 
                                         OS_ERR                *p_err);
#endif

/*$PAGE*/
/* ================================================================================================================== */
/*                                                 TASK MANAGEMENT                                                    */
/* ================================================================================================================== */

#if OS_CFG_TASK_CHANGE_PRIO_EN > 0u
void          OSTaskChangePrio          (OS_TCB                *p_tcb,
                                         OS_PRIO                prio_new,
                                         OS_ERR                *p_err);
#endif

void          OSTaskCreate              (OS_TCB                *p_tcb,
                                         CPU_CHAR              *p_name,
                                         OS_TASK_PTR            p_task,
                                         void                  *p_arg,
                                         OS_PRIO                prio,
                                         CPU_STK               *p_stk_base,
                                         CPU_STK               *p_stk_limit,
                                         OS_STK_SIZE            stk_size,
                                         OS_MSG_QTY             q_size,
                                         OS_TICK                time_quanta,
                                         void                  *p_ext,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#if OS_CFG_TASK_DEL_EN > 0u
void          OSTaskDel                 (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_TASK_Q_EN > 0u
OS_MSG_QTY    OSTaskQFlush              (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);

void         *OSTaskQPend               (OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         OS_MSG_SIZE           *p_msg_size,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

CPU_BOOLEAN   OSTaskQPendAbort          (OS_TCB                *p_tcb, 
                                         OS_OPT                 opt, 
                                         OS_ERR                *p_err);

void          OSTaskQPost               (OS_TCB                *p_tcb, 
                                         void                  *p_void,
                                         OS_MSG_SIZE            msg_size,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#endif

#if OS_CFG_TASK_SUSPEND_EN > 0u
void          OSTaskResume              (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);

void          OSTaskSuspend             (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
OS_REG        OSTaskRegGet              (OS_TCB                *p_tcb,
                                         OS_REG_ID              id,
                                         OS_ERR                *p_err);

void          OSTaskRegSet              (OS_TCB                *p_tcb,
                                         OS_REG_ID              id,
                                         OS_REG                 value,
                                         OS_ERR                *p_err);
#endif

OS_SEM_CTR    OSTaskSemPend             (OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

#if (OS_CFG_TASK_SEM_PEND_ABORT_EN > 0u)
CPU_BOOLEAN   OSTaskSemPendAbort        (OS_TCB                *p_tcb, 
                                         OS_OPT                 opt, 
                                         OS_ERR                *p_err);
#endif

OS_SEM_CTR    OSTaskSemPost             (OS_TCB                *p_tcb,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

OS_SEM_CTR    OSTaskSemSet              (OS_TCB                *p_tcb,
                                         OS_SEM_CTR             cnt, 
                                         OS_ERR                *p_err);

#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
void          OSTaskStkChk              (OS_TCB                *p_tcb,
                                         OS_STK_SIZE           *p_free,
                                         OS_STK_SIZE           *p_used,
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void          OSTaskTimeQuantaSet        (OS_TCB                *p_tcb, 
                                         OS_TICK                time_quanta,
                                         OS_ERR                *p_err);
#endif

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_TaskBlock              (OS_TCB                *p_tcb,
                                         OS_TICK                timeout);

void          OS_TaskInit               (OS_ERR                *p_err);

void          OS_TaskInitTCB            (OS_TCB                *p_tcb);

void          OS_TaskQPost              (OS_TCB                *p_tcb, 
                                         void                  *p_void,
                                         OS_MSG_SIZE            msg_size,
                                         OS_OPT                 opt,
                                         CPU_TS                 ts, 
                                         OS_ERR                *p_err);

void          OS_TaskRdy                (OS_TCB                *p_tcb);

void          OS_TaskReturn             (void);

OS_SEM_CTR    OS_TaskSemPost            (OS_TCB                *p_tcb,
                                         OS_OPT                 opt,
                                         CPU_TS                 ts, 
                                         OS_ERR                *p_err);

/*$PAGE*/
/* ================================================================================================================== */
/*                                                 TIME MANAGEMENT                                                    */
/* ================================================================================================================== */

void          OSTimeDly                 (OS_TICK                dly,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#if OS_CFG_TIME_DLY_HMSM_EN > 0u
void          OSTimeDlyHMSM             (CPU_INT16U             hours, 
                                         CPU_INT16U             minutes, 
                                         CPU_INT16U             seconds, 
                                         CPU_INT32U             milli,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_TIME_DLY_RESUME_EN > 0u
void          OSTimeDlyResume           (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);
#endif

OS_TICK       OSTimeGet                 (OS_ERR                *p_err);

void          OSTimeSet                 (OS_TICK                ticks,
                                         OS_ERR                *p_err);

void          OSTimeTick                (void);

/*$PAGE*/
/* ================================================================================================================== */
/*                                                 TIMER MANAGEMENT                                                   */
/* ================================================================================================================== */

#if OS_CFG_TMR_EN > 0u
void          OSTmrCreate               (OS_TMR                *p_tmr,
                                         CPU_CHAR              *p_name,
                                         OS_TICK                dly,
                                         OS_TICK                period,
                                         OS_OPT                 opt,
                                         OS_TMR_CALLBACK_PTR    p_callback,
                                         void                  *p_callback_arg,
                                         OS_ERR                *p_err);

CPU_BOOLEAN   OSTmrDel                  (OS_TMR                *p_tmr,
                                         OS_ERR                *p_err);

OS_TICK       OSTmrRemainGet            (OS_TMR                *p_tmr,
                                         OS_ERR                *p_err);

CPU_BOOLEAN   OSTmrStart                (OS_TMR                *p_tmr,
                                         OS_ERR                *p_err);

OS_STATE      OSTmrStateGet             (OS_TMR                *p_tmr,
                                         OS_ERR                *p_err);

CPU_BOOLEAN   OSTmrStop                 (OS_TMR                *p_tmr,
                                         OS_OPT                 opt,
                                         void                  *p_callback_arg,
                                         OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_TmrClr                 (OS_TMR                *p_tmr);

#if OS_CFG_DBG_EN > 0u
void          OS_TmrDbgTblAdd           (OS_TMR                *p_tmr);

void          OS_TmrDbgTblRemove        (OS_TMR                *p_tmr);
#endif

void          OS_TmrInit                (OS_ERR                *p_err);

void          OS_TmrLink                (OS_TMR                *p_tmr, 
                                         OS_OPT                 opt);

void          OS_TmrResetPeak           (void);

void          OS_TmrUnlink              (OS_TMR                *p_tmr);

void          OS_TmrTask                (void                  *p_arg);

#endif

/*$PAGE*/
/* ================================================================================================================== */
/*                                                    MISCELLANEOUS                                                   */
/* ================================================================================================================== */

void          OSInit                    (OS_ERR                *p_err);

void          OSIntEnter                (void);
void          OSIntExit                 (void);

void          OSMsgPoolExtend           (OS_MSG                *p_msg,
                                         OS_MSG_QTY             size,
                                         OS_ERR                *p_err);

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void          OSSchedRoundRobinCfg      (CPU_BOOLEAN            en, 
                                         OS_TICK                dflt_time_quanta,
                                         OS_ERR                *p_err);

void          OSSchedRoundRobinYield    (OS_ERR                *p_err);

#endif

void          OSSched                   (void);

void          OSSchedLock               (OS_ERR                *p_err);
void          OSSchedUnlock             (OS_ERR                *p_err);

void          OSStart                   (OS_ERR                *p_err);

#if OS_CFG_STAT_TASK_EN > 0u
void          OSStatReset               (OS_ERR                *p_err);

void          OSStatTaskCPUUsageInit    (OS_ERR                *p_err);
#endif

CPU_INT16U    OSVersion                 (OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_IdleTask               (void                  *p_arg);

void          OS_IdleTaskInit           (OS_ERR                *p_err);

#if OS_CFG_STAT_TASK_EN > 0u
void          OS_StatTask               (void                  *p_arg);
#endif

void          OS_StatTaskInit           (OS_ERR                *p_err);

void          OS_TickTask               (void                  *p_arg);
void          OS_TickTaskInit           (OS_ERR                *p_err);

/*$PAGE*/
/*
************************************************************************************************************************
************************************************************************************************************************
*                                    T A R G E T   S P E C I F I C   F U N C T I O N S
************************************************************************************************************************
************************************************************************************************************************
*/

void          OSInitHook                (void);

void          OSTaskCreateHook          (OS_TCB                *p_tcb);
void          OSTaskDelHook             (OS_TCB                *p_tcb);

void          OSIdleTaskHook            (void);

void          OSTaskReturnHook          (OS_TCB                *p_tcb);

void          OSStatTaskHook            (void);

CPU_STK      *OSTaskStkInit             (OS_TASK_PTR            p_task, 
                                         void                  *p_arg, 
                                         CPU_STK               *p_stk_base,
                                         CPU_STK               *p_stk_limit,
                                         OS_STK_SIZE            stk_size,
                                         OS_OPT                 opt);

void          OSTaskSwHook              (void);

void          OSTimeTickHook            (void);

/*$PAGE*/
/*
************************************************************************************************************************
************************************************************************************************************************
*                   u C / O S - I I I   I N T E R N A L   F U N C T I O N   P R O T O T Y P E S
************************************************************************************************************************
************************************************************************************************************************
*/

void          OSCfg_Init                (void);

#if OS_CFG_DBG_EN > 0u
void          OS_Dbg_Init               (void);
#endif


#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
void          OS_IntQTaskInit           (OS_ERR                *p_err);

void          OS_IntQPost               (OS_OBJ_TYPE            type,
                                         void                  *p_obj,
                                         void                  *p_void,
                                         OS_MSG_SIZE            msg_size,
                                         OS_FLAGS               flags,
                                         OS_OPT                 opt,
                                         CPU_TS                 ts,
                                         OS_ERR                *p_err);
                                         
void          OS_IntQTask               (void                  *p_arg);
#endif


/* ----------------------------------------------- MESSAGE MANAGEMENT ----------------------------------------------- */

void          OS_MsgPoolCreate          (OS_MSG                *p_msg,
                                         OS_MSG_QTY             size);

void          OS_MsgPoolInit            (OS_ERR                *p_err);

void          OS_MsgQEntriesPeakReset   (OS_MSG_Q              *p_msg_q);

OS_MSG_QTY    OS_MsgQFreeAll            (OS_MSG_Q              *p_msg_q);

void         *OS_MsgQGet                (OS_MSG_Q              *p_msg_q,
                                         OS_MSG_SIZE           *p_msg_size,
                                         CPU_TS                *p_ts, 
                                         OS_ERR                *p_err);

void          OS_MsgQInit               (OS_MSG_Q              *p_msg_q, 
                                         OS_MSG_QTY             size);

void          OS_MsgQPut                (OS_MSG_Q              *p_msg_q, 
                                         void                  *p_void,
                                         OS_MSG_SIZE            msg_size,
                                         OS_OPT                 opt,
                                         CPU_TS                 ts, 
                                         OS_ERR                *p_err);

/* ---------------------------------------------- PEND/POST MANAGEMENT ---------------------------------------------- */

void          OS_Pend                   (OS_PEND_DATA          *p_pend_data,
                                         OS_PEND_OBJ           *p_obj,
                                         OS_STATE               pending_on,
                                         OS_TICK                timeout);

void          OS_PendAbort              (OS_TCB                *p_tcb, 
                                         CPU_TS                 ts);

void          OS_PendObjDel             (OS_TCB                *p_tcb,
                                         CPU_TS                 ts);

void          OS_Post                   (OS_PEND_OBJ           *p_obj,
                                         OS_TCB                *p_tcb, 
                                         void                  *p_void, 
                                         OS_MSG_SIZE            msg_size, 
                                         CPU_TS                 ts);

void          OS_Post1                  (OS_PEND_OBJ           *p_obj,
                                         OS_TCB                *p_tcb,
                                         void                  *p_void, 
                                         OS_MSG_SIZE            msg_size, 
                                         CPU_TS                 ts);

/* ----------------------------------------------- PRIORITY MANAGEMENT ---------------------------------------------- */

void          OS_PrioInit               (void);

void          OS_PrioInsert             (OS_PRIO                prio);

void          OS_PrioRemove             (OS_PRIO                prio);

OS_PRIO       OS_PrioGetHighest         (void);

/* --------------------------------------------------- SCHEDULING --------------------------------------------------- */

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
void          OS_Sched0                 (void);
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
void          OS_SchedLockTimeMeasStart (void);
void          OS_SchedLockTimeMeasStop   (void);
#endif

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void          OS_SchedRoundRobin        (OS_RDY_LIST           *p_rdy_list);
#endif

/* --------------------------------------------- READY LIST MANAGEMENT ---------------------------------------------- */

void          OS_RdyListInit            (void);

void          OS_RdyListInsert          (OS_TCB                *p_tcb);

void          OS_RdyListInsertHead      (OS_TCB                *p_tcb);

void          OS_RdyListInsertTail      (OS_TCB                *p_tcb);

void          OS_RdyListMoveHeadToTail  (OS_RDY_LIST           *p_rdy_list);

void          OS_RdyListRemove          (OS_TCB                *p_tcb);

/* ---------------------------------------------- PEND LIST MANAGEMENT ---------------------------------------------- */

void          OS_PendDataInit           (OS_TCB                *p_tcb,
                                         OS_PEND_DATA          *p_pend_data_tbl,
                                         OS_OBJ_QTY             tbl_size);
                                         
OS_PEND_LIST *OS_PendListGetPtr         (OS_PEND_OBJ           *p_obj);

void          OS_PendListInit           (OS_PEND_LIST          *p_pend_list);

void          OS_PendListInsertHead     (OS_PEND_LIST          *p_pend_list, 
                                         OS_PEND_DATA          *p_pend_data);
                                         
void          OS_PendListInsertPrio     (OS_PEND_LIST          *p_pend_list, 
                                         OS_PEND_DATA          *p_pend_data);
                                         
void          OS_PendListChangePrio     (OS_TCB                *p_tcb,
                                         OS_PRIO                prio_new); 

void          OS_PendListRemove         (OS_TCB                *p_tcb);

void          OS_PendListRemove1        (OS_PEND_LIST          *p_pend_list,
                                         OS_PEND_DATA          *p_pend_data);

/* ---------------------------------------------- TICK LIST MANAGEMENT ---------------------------------------------- */

void          OS_TickListInit           (void);

void          OS_TickListInsert         (OS_TCB                *p_tcb, 
                                         OS_TICK                time, 
                                         OS_OPT                 opt);

void          OS_TickListRemove         (OS_TCB                *p_tcb); 

void          OS_TickListResetPeak      (void);

void          OS_TickListUpdate         (void);

/*$PAGE*/
/*
************************************************************************************************************************
*                                          LOOK FOR MISSING #define CONSTANTS
*
* This section is used to generate ERROR messages at compile time if certain #define constants are
* MISSING in OS_CFG.H.  This allows you to quickly determine the source of the error.
*
* You SHOULD NOT change this section UNLESS you would like to add more comments as to the source of the
* compile time error.
************************************************************************************************************************
*/

/*
************************************************************************************************************************
*                                                     MISCELLANEOUS
************************************************************************************************************************
*/

#ifndef OS_CFG_APP_HOOKS_EN
#error  "OS_CFG.H, Missing OS_CFG_APP_HOOKS_EN: Enable (1) or Disable (0) application specific hook functions"
#endif


#ifndef OS_CFG_ARG_CHK_EN
#error  "OS_CFG.H, Missing OS_CFG_ARG_CHK_EN: Enable (1) or Disable (0) argument checking"
#endif


#ifndef OS_CFG_DBG_EN
#error  "OS_CFG.H, Missing OS_CFG_DBG_EN: Allows you to include variables for debugging or not"
#endif


#ifndef OS_CFG_CALLED_FROM_ISR_CHK_EN
#error  "OS_CFG.H, Missing OS_CFG_CALLED_FROM_ISR_CHK_EN: Enable (1) or Disable (0) checking whether in an ISR in kernel services"
#endif


#ifndef OS_CFG_OBJ_TYPE_CHK_EN
#error  "OS_CFG.H, Missing OS_CFG_OBJ_TYPE_CHK_EN: Enable (1) or Disable (0) checking for proper object types in kernel services"
#endif


#ifndef OS_CFG_PEND_MULTI_EN
#error  "OS_CFG.H, Missing OS_CFG_PEND_MULTI_EN: Enable (1) or Disable (0) multi-pend feature"
#endif


#if     OS_CFG_PRIO_MAX < 8u
#error  "OS_CFG.H,         OS_CFG_PRIO_MAX must be >= 8"
#endif


#ifndef OS_CFG_SCHED_LOCK_TIME_MEAS_EN
#error  "OS_CFG.H, Missing OS_CFG_SCHED_LOCK_TIME_MEAS_EN: Include code to measure scheduler lock time"
#endif


#ifndef OS_CFG_SCHED_ROUND_ROBIN_EN
#error  "OS_CFG.H, Missing OS_CFG_SCHED_ROUND_ROBIN_EN: Include code for Round Robin Scheduling"
#endif


#ifndef OS_CFG_STK_SIZE_MIN
#error  "OS_CFG.H, Missing OS_CFG_STK_SIZE_MIN: Determines the minimum size for a task stack"
#endif

/*
************************************************************************************************************************
*                                                     EVENT FLAGS
************************************************************************************************************************
*/

#ifndef OS_CFG_FLAG_EN
#error  "OS_CFG.H, Missing OS_CFG_FLAG_EN: Enable (1) or Disable (0) code generation for Event Flags"
#else
    #ifndef OS_CFG_FLAG_DEL_EN
    #error  "OS_CFG.H, Missing OS_CFG_FLAG_DEL_EN: Include code for OSFlagDel()"
    #endif

    #ifndef OS_CFG_FLAG_MODE_CLR_EN
    #error  "OS_CFG.H, Missing OS_CFG_FLAG_MODE_CLR_EN: Include code for Wait on Clear EVENT FLAGS"
    #endif

    #ifndef OS_CFG_FLAG_PEND_ABORT_EN
    #error  "OS_CFG.H, Missing OS_CFG_FLAG_PEND_ABORT_EN: Include code for aborting pends from another task"
    #endif
#endif

/*
************************************************************************************************************************
*                                                  MEMORY MANAGEMENT
************************************************************************************************************************
*/

#ifndef OS_CFG_MEM_EN
#error  "OS_CFG.H, Missing OS_CFG_MEM_EN: Enable (1) or Disable (0) code generation for MEMORY MANAGER"
#endif

/*
************************************************************************************************************************
*                                              MUTUAL EXCLUSION SEMAPHORES
************************************************************************************************************************
*/

#ifndef OS_CFG_MUTEX_EN
#error  "OS_CFG.H, Missing OS_CFG_MUTEX_EN: Enable (1) or Disable (0) code generation for MUTEX"
#else
    #ifndef OS_CFG_MUTEX_DEL_EN
    #error  "OS_CFG.H, Missing OS_CFG_MUTEX_DEL_EN: Include code for OSMutexDel()"
    #endif

    #ifndef OS_CFG_MUTEX_PEND_ABORT_EN
    #error  "OS_CFG.H, Missing OS_CFG_MUTEX_PEND_ABORT_EN: Include code for OSMutexPendAbort()"
    #endif
#endif

/*
************************************************************************************************************************
*                                                    MESSAGE QUEUES
************************************************************************************************************************
*/

#ifndef OS_CFG_Q_EN
#error  "OS_CFG.H, Missing OS_CFG_Q_EN: Enable (1) or Disable (0) code generation for QUEUES"
#else
    #ifndef OS_CFG_Q_DEL_EN
    #error  "OS_CFG.H, Missing OS_CFG_Q_DEL_EN: Include code for OSQDel()"
    #endif

    #ifndef OS_CFG_Q_FLUSH_EN
    #error  "OS_CFG.H, Missing OS_CFG_Q_FLUSH_EN: Include code for OSQFlush()"
    #endif

    #ifndef OS_CFG_Q_PEND_ABORT_EN
    #error  "OS_CFG.H, Missing OS_CFG_Q_PEND_ABORT_EN: Include code for OSQPendAbort()"
    #endif
#endif

/*
************************************************************************************************************************
*                                                      SEMAPHORES
************************************************************************************************************************
*/

#ifndef OS_CFG_SEM_EN
#error  "OS_CFG.H, Missing OS_CFG_SEM_EN: Enable (1) or Disable (0) code generation for SEMAPHORES"
#else
    #ifndef OS_CFG_SEM_DEL_EN
    #error  "OS_CFG.H, Missing OS_CFG_SEM_DEL_EN: Include code for OSSemDel()"
    #endif

    #ifndef OS_CFG_SEM_PEND_ABORT_EN
    #error  "OS_CFG.H, Missing OS_CFG_SEM_PEND_ABORT_EN: Include code for OSSemPendAbort()"
    #endif

    #ifndef OS_CFG_SEM_SET_EN
    #error  "OS_CFG.H, Missing OS_CFG_SEM_SET_EN: Include code for OSSemSet()"
    #endif
#endif

/*
************************************************************************************************************************
*                                                   TASK MANAGEMENT
************************************************************************************************************************
*/

#ifndef OS_CFG_STAT_TASK_EN
#error  "OS_CFG.H, Missing OS_CFG_STAT_TASK_EN: Enable (1) or Disable(0) the statistics task"
#endif

#ifndef OS_CFG_STAT_TASK_STK_CHK_EN
#error  "OS_CFG.H, Missing OS_CFG_STAT_TASK_STK_CHK_EN: Check task stacks from statistics task"
#endif

#ifndef OS_CFG_TASK_CHANGE_PRIO_EN
#error  "OS_CFG.H, Missing OS_CFG_TASK_CHANGE_PRIO_EN: Include code for OSTaskChangePrio()"
#endif

#ifndef OS_CFG_TASK_DEL_EN
#error  "OS_CFG.H, Missing OS_CFG_TASK_DEL_EN: Include code for OSTaskDel()"
#endif

#ifndef OS_CFG_TASK_Q_EN
#error  "OS_CFG.H, Missing OS_CFG_TASK_Q_EN: Include code for OSTaskQxxx()"
#endif

#ifndef OS_CFG_TASK_Q_PEND_ABORT_EN
#error  "OS_CFG.H, Missing OS_CFG_TASK_Q_PEND_ABORT_EN: Include code for OSTaskQPendAbort()"
#endif

#ifndef OS_CFG_TASK_PROFILE_EN
#error  "OS_CFG.H, Missing OS_CFG_TASK_PROFILE_EN: Include code for task profiling"
#endif

#ifndef OS_CFG_TASK_REG_TBL_SIZE
#error  "OS_CFG.H, Missing OS_CFG_TASK_REG_TBL_SIZE: Include support for task specific registers"
#endif

#ifndef OS_CFG_TASK_SEM_PEND_ABORT_EN
#error  "OS_CFG.H, Missing OS_CFG_TASK_SEM_PEND_ABORT_EN: Include code for OSTaskSemPendAbort()"
#endif

#ifndef OS_CFG_TASK_SUSPEND_EN
#error  "OS_CFG.H, Missing OS_CFG_TASK_SUSPEND_EN: Include code for OSTaskSuspend() and OSTaskResume()"
#endif

/*
************************************************************************************************************************
*                                                  TIME MANAGEMENT
************************************************************************************************************************
*/

#ifndef OS_CFG_TIME_DLY_HMSM_EN
#error  "OS_CFG.H, Missing OS_CFG_TIME_DLY_HMSM_EN: Include code for OSTimeDlyHMSM()"
#endif

#ifndef OS_CFG_TIME_DLY_RESUME_EN
#error  "OS_CFG.H, Missing OS_CFG_TIME_DLY_RESUME_EN: Include code for OSTimeDlyResume()"
#endif

/*
************************************************************************************************************************
*                                                  TIMER MANAGEMENT
************************************************************************************************************************
*/

#ifndef OS_CFG_TMR_EN
#error  "OS_CFG.H, Missing OS_CFG_TMR_EN: When (1) enables code generation for Timer Management"
#else
    #ifndef OS_CFG_TMR_DEL_EN
    #error  "OS_CFG.H, Missing OS_CFG_TMR_DEL_EN: Enables (1) or Disables (0) code for OSTmrDel()"
    #endif
#endif


#ifdef __cplusplus
}
#endif
#endif
