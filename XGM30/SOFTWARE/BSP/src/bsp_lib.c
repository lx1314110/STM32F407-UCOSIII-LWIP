/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM322xG Evaluation Board
*
* Filename      : bsp_lib.c
* Version       : V2.00
* Programmer(s) : Michael Vysotsky
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/
const GPIO_TypeDef* BSP_GPIO_PORT[] = {GPIOA,GPIOB,GPIOC,GPIOD,GPIOE,GPIOF,GPIOG,GPIOH,GPIOI};
/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/
typedef struct mem_leaks{
  void *location;
  CPU_INT32U size;
  struct mem_leaks *next;  
}MEM_LEAKS;


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
struct mem_leaks *MemoryLeaks = NULL;
int               MaxUserMemory=0;
int               CurrUserMemory=0;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL GLOBAL FUNCTION
*********************************************************************************************************
*/
/**
 * Allocate memory: determine the smallest pool that is big enough
 * to contain an element of 'size' and get an element from that pool.
 *
 * @param size the size in bytes of the memory needed
 * @return a pointer to the allocated memory or NULL if the pool is empty
 */
void  * BSP_mem_malloc(OS_MEM_SIZE length)
{
  if(!length){
    BSP_ASSERT("BSP_mem_malloc: Illegal Length\n", length);
    return NULL;
  }
  CPU_SR_ALLOC();  
  CPU_CRITICAL_ENTER();
  CPU_INT08U *p = NULL;
/* Create leaks entry */
  struct mem_leaks *mem_leak = malloc(sizeof(MEM_LEAKS));
  BSP_ASSERT("BSP_mem_malloc: no free memory for allocation leaks", mem_leak);
  if(!mem_leak){
    CPU_CRITICAL_EXIT();
    return NULL;
  }
  mem_leak->next = NULL;
/* allocation block memory */
  p = malloc(length);
  BSP_ASSERT("BSP_mem_malloc: no free memory for allocation data", p);
  if(!p){
    CPU_CRITICAL_EXIT();
    return NULL;
  }
  memset(p,0,length);
  mem_leak->location = p;
  mem_leak->size=length;
  CurrUserMemory += length;
  if(CurrUserMemory > MaxUserMemory)
    MaxUserMemory = CurrUserMemory;
/* Create next leaks entry */
  struct mem_leaks *EndMemoryLeaks=NULL;
  if(MemoryLeaks == NULL)
    MemoryLeaks = mem_leak;
  else{
    for(EndMemoryLeaks=MemoryLeaks; EndMemoryLeaks->next; EndMemoryLeaks = EndMemoryLeaks->next);
    EndMemoryLeaks->next = mem_leak; 
  }
  CPU_CRITICAL_EXIT();  
  return (void*)p;  
}
/**
 * Free memory previously allocated by mem_malloc. Loads the pool number
 * and calls memp_free with that pool number to put the element back into
 * its pool
 *
 * @param rmem the memory element to free
 */
void BSP_mem_free(void *rmem)
{
  if(!rmem || MemoryLeaks == NULL) return;
  CPU_SR_ALLOC();  
  CPU_CRITICAL_ENTER();
  struct mem_leaks *EndMemoryLeaks=NULL, *PrevMemoryLeaks =NULL;  
  for(EndMemoryLeaks=MemoryLeaks; EndMemoryLeaks; EndMemoryLeaks = EndMemoryLeaks->next){
    if(EndMemoryLeaks->location == rmem){
      CurrUserMemory -= EndMemoryLeaks->size;
      if(CurrUserMemory <0)
        CurrUserMemory = 0;      
      free(rmem);
      if(PrevMemoryLeaks)
        PrevMemoryLeaks->next = EndMemoryLeaks->next;
      else
        MemoryLeaks = EndMemoryLeaks->next;
      free(EndMemoryLeaks);
      break;     
    }
    PrevMemoryLeaks = EndMemoryLeaks;
  }
  CPU_CRITICAL_EXIT();    
}
/**
  * @brief  gpio port enable
  * @param  GPIO_TypeDef* GPIOx - port structure pointer
  * @retval None
  */
void BSP_GpioPortEn(const GPIO_TypeDef* GPIOx)
{
  uint8_t id=0;
  for(id=0; id<=BSP_PERIPH_ID_IOPI; ++id)
    if(GPIOx == BSP_GPIO_PORT[id]){
      BSP_PeriphEn(id);
      break;
    }
}
/**
 * Shrink memory returned by mem_malloc().
 *
 * @param rmem pointer to memory allocated by mem_malloc the is to be shrinked
 * @param newsize required size after shrinking (needs to be smaller than or
 *                equal to the previous size)
 * @return for compatibility reasons: is always == rmem, at the moment
 *         or NULL if newsize is > old size, in which case rmem is NOT touched
 *         or freed!
 */
void * BSP_mem_trim(void *rmem, OS_MEM_SIZE newsize)
{
  if(!newsize){
    BSP_ASSERT("mem_trim: Illegal Length\n", newsize);
    return NULL;
  }
  CPU_SR_ALLOC();  
  CPU_CRITICAL_ENTER();
  CPU_INT08U *p = rmem;
/* search memory in list leaks */ 
  struct mem_leaks *CurMemoryLeaks=NULL;  
  for(CurMemoryLeaks=MemoryLeaks; CurMemoryLeaks; CurMemoryLeaks = CurMemoryLeaks->next){
    if(CurMemoryLeaks->location == rmem){
      p = realloc(rmem,newsize);
      BSP_ASSERT("mem_trim: no free memory for allocation data", p);
      if(!p){
        CPU_CRITICAL_EXIT();
        return NULL;
      }
      CurMemoryLeaks->location = p;
      if(CurMemoryLeaks->size > newsize)
        CurrUserMemory -= (CurMemoryLeaks->size - newsize);
      else{
        CPU_INT32U delta = (newsize - CurMemoryLeaks->size);
        CurrUserMemory += delta;
        memset(&p[CurMemoryLeaks->size],0,delta);
      }
      CurMemoryLeaks->size = newsize;
      break;
    }
  }
  CPU_CRITICAL_EXIT();  
  return (void*)p;   
}