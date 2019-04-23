/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM322xG Evaluation Board
*
* Filename      : bsp_lib.h
* Version       : V2.00
* Programmer(s) : Michael Vysotsky
*********************************************************************************************************
*/
#ifndef _BSP_LIB_H
#define _BSP_LIB_H
/*
*********************************************************************************************************
*                                            GLOBAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            GLOBAL STRUCTURES
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                            GLOBAL MACRO'S
*********************************************************************************************************
*/
#define BSP_GPIO_CONF_DEV(dev,pin,af) \
  if(##dev##_##pin##_PIN){ \
    BSP_GpioPortEn(##dev##_##pin##_PORT); \
    GPIO_InitStructure.GPIO_Pin = ##dev##_##pin##_PIN;  \
    GPIO_Init((GPIO_TypeDef*)##dev##_##pin##_PORT, &GPIO_InitStructure); \
    GPIO_PinAFConfig((GPIO_TypeDef*)##dev##_##pin##_PORT, ##dev##_##pin##_PINSRC, af); \
  }
#define BSP_GPIO_CONF(pin,i) \
  if(BSP_##pin##_PORT[i]){ \
    BSP_GpioPortEn(BSP_##pin##_PORT[i]); \
    GPIO_InitStructure.GPIO_Pin = BSP_##pin##_PIN[i];  \
    GPIO_Init((GPIO_TypeDef*)BSP_##pin##_PORT[i], &GPIO_InitStructure); \
    GPIO_PinAFConfig((GPIO_TypeDef*)BSP_##pin##_PORT[i], BSP_##pin##_PINSRC[i], BSP_AF[i]); \
  }
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void  * BSP_mem_malloc(OS_MEM_SIZE length);
void    BSP_mem_free(void *rmem);
void    BSP_GpioPortEn(const GPIO_TypeDef* GPIOx);
void *  BSP_mem_trim(void *rmem, OS_MEM_SIZE newsize);

#endif /* _BSP_LIB_H */

