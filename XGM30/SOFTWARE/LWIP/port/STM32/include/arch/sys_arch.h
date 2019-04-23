/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include <includes.h>
#include "arch/cc.h"




#ifdef SYS_ARCH_GLOBALS
#define SYS_ARCH_EXT
#else
#define SYS_ARCH_EXT extern
#endif



/*-----------------macros-----------------------------------------------------*/
#define LWIP_STK_SIZE	TASK_STK_SIZE
#define LWIP_TASK_MAX	    5
/* The user can change this priority level. 
 * It is important that there was no crossing with other levels.
 */
#define LWIP_TSK_PRIO         APP_HIGH_PRIORITY 
#define LWIP_TASK_START_PRIO  LWIP_TSK_PRIO
#define LWIP_TASK_END_PRIO    LWIP_TSK_PRIO +LWIP_TASK_MAX

/* the max size of each mailbox */
#define MAX_QUEUE_ENTRIES  20	 

#define SYS_MBOX_NULL (void *)0
#define SYS_SEM_NULL  (void *)0

#define sys_arch_mbox_tryfetch(mbox,msg) \
      sys_arch_mbox_fetch(mbox,msg,1)

/*-----------------type define------------------------------------------------*/

/** struct of LwIP mailbox */

typedef OS_SEM     sys_sem_t; // type of semiphores
typedef OS_MUTEX   sys_mutex_t; // type of mutex
typedef OS_Q       sys_mbox_t; // type of mailboxes
typedef CPU_INT08U sys_thread_t; // type of id of the new thread

typedef CPU_INT08U sys_prot_t;

#define mem_malloc BSP_mem_malloc
#define mem_free   BSP_mem_free
#define mem_trim   BSP_mem_trim


/*-----------------global variables-------------------------------------------*/
/**
 * MEMCPY: override this if you have a faster implementation at hand than the
 * one included in your C library
 */
#ifndef MEMCPY
#define MEMCPY(dst,src,len)             Mem_Copy(dst,src,len)
#endif
#ifndef SMEMCPY
#define SMEMCPY(dst,src,len)            OS_MemCopy(dst,src,len)
#endif


#endif /* __SYS_RTXC_H__ */

