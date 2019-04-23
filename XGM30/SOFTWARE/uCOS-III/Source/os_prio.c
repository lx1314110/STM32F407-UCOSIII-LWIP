/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                The Real-Time Kernel
*
*                                        (c) Copyright 2009, Micrium, Weston, FL
*                                                  All Rights Reserved
*                                                    www.Micrium.com
*
*                                                 PRIORITY MANAGEMENT
*
* File    : OS_PRIO.C
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

#include  <os.h>

CPU_DATA   OSPrioTbl[OS_PRIO_TBL_SIZE];               /* Declare the array local to this file to allow for  ... */
                                                            /* ... optimization.  In other words, this allows the ... */
                                                            /* ... table to be located in fast memory                 */

/*
************************************************************************************************************************
*                                               INITIALIZE THE PRIORITY LIST
*
* Description: This function is called by uC/OS-III to initialize the list of ready priorities.
*
* Arguments  : none
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_PrioInit (void)
{
    CPU_DATA   i;
    CPU_DATA  *p_tbl;
    
 
    
    p_tbl = &OSPrioTbl[0];                                  /* Clear the bitmap table ... no task is ready            */
    for (i = 0u; i < OS_PRIO_TBL_SIZE; i++) {
        *p_tbl = (CPU_DATA)0;
        p_tbl++;
    }
}

/*
************************************************************************************************************************
*                                           GET HIGHEST PRIORITY TASK WAITING
*
* Description: This function is called by other uC/OS-III services to determine the highest priority task
*              waiting on the event.
*
* Arguments  : ptasklist     is a pointer to the Highest Priority task waiting 
*
* Returns    : The priority of the Highest Priority Task (HPT) waiting for the event
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
*
*              2) The compiler should fold 'sizeof(CPU_DATA) * 8' into a constant and thus not require this 
*                 operation to be done at run-time.
************************************************************************************************************************
*/

OS_PRIO  OS_PrioGetHighest (void)
{
    CPU_DATA  *p_tbl;
    OS_PRIO    prio;
    

    
    prio  = (OS_PRIO)0;
    p_tbl = &OSPrioTbl[0];
    while (*p_tbl == (CPU_DATA)0) {                         /* Search the bitmap table for the highest priority       */
        prio += sizeof(CPU_DATA) * 8u;                      /* Compute the step of each CPU_DATA entry                */
        p_tbl++;
    }
    prio += (OS_PRIO)CPU_CntLeadZeros(*p_tbl);              /* Find the position of the first bit set at the entry    */
    return (prio);
}

/*
************************************************************************************************************************
*                                                  INSERT PRIORITY
*
* Description: This function is called to insert a priority in the priority table
*
* Arguments  : prio     is the priority to insert
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_PrioInsert (OS_PRIO  prio)
{
    CPU_DATA  bit;
    OS_QTY    bit_nbr;
    OS_QTY    ix;
    

    
    ix             = prio / (sizeof(CPU_DATA) * 8u);
    bit_nbr        = prio & (sizeof(CPU_DATA) * 8u - 1u);
    bit            = (CPU_DATA)((CPU_DATA)1u << ((sizeof(CPU_DATA) * 8u - 1u) - (CPU_DATA)bit_nbr));
    OSPrioTbl[ix] |= bit;
}

/*
************************************************************************************************************************
*                                                   REMOVE PRIORITY
*
* Description: This function is called to remove a priority in the priority table
*
* Arguments  : prio     is the priority to insert
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_PrioRemove (OS_PRIO  prio)
{
    CPU_DATA  bit;
    OS_QTY    bit_nbr;
    OS_QTY    ix;
    

    
    ix             = prio / (sizeof(CPU_DATA) * 8u);
    bit_nbr        = prio & (sizeof(CPU_DATA) * 8u - 1u);
    bit            = (CPU_DATA)((CPU_DATA)1u << ((sizeof(CPU_DATA) * 8u - 1u) - (CPU_DATA)bit_nbr));
    OSPrioTbl[ix] &= ~bit;
}
