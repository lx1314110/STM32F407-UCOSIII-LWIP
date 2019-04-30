/*****************************************************************************/
/* 文件名:    bsp_usart.h                                                 */
/* 描  述:    串口相关处理头文件                                             */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __DEV_USART_H_
#define __DEV_USART_H_
#include "arch/cc.h"
//#include "Input/gnss.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/

/* 串口发送数据的最大长度 */



/* 卫星模块串口 */
#define GNSS_USART_BUF_MAX_SIZE                2048   /* 卫星模块串口缓冲区大小       */
#define GNSS_USART_SEND_LEN_MAX                128    /* 卫星模块串口发送数据最大长度 */

#define GNSS_USART                             USART6
#define GNSS_USART_CLK                         RCC_APB2Periph_USART6
#define GNSS_USART_DMA_CLK                     RCC_AHB1Periph_DMA2


#define GNSS_USART_RX_GPIO_PORT                GPIOC
#define GNSS_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define GNSS_USART_RX_PIN                      GPIO_Pin_7
#define GNSS_USART_RX_AF                       GPIO_AF_USART6
#define GNSS_USART_RX_SOURCE                   GPIO_PinSource7

#define GNSS_USART_TX_GPIO_PORT                GPIOC
#define GNSS_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define GNSS_USART_TX_PIN                      GPIO_Pin_6
#define GNSS_USART_TX_AF                       GPIO_AF_USART6
#define GNSS_USART_TX_SOURCE                   GPIO_PinSource6

#define GNSS_USART_IRQn                        USART6_IRQn
#define GNSS_USART_INT_ID                      BSP_INT_ID_USART6
#define GNSS_DMA_Stream                        DMA2_Stream1
#define GNSS_DMA_CLEAR_FLAG                    (DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1)


/* 调试串口 */
#define DEBUG_USART_BUF_MAX_SIZE                128   /* 调试串口缓冲区大小       */
#define DEBUG_USART_SEND_LEN_MAX                256   /* 调试串口发送数据最大长度 */

#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK                         RCC_APB2Periph_USART1
#define DEBUG_USART_DMA_CLK                     RCC_AHB1Periph_DMA2

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_PIN                      GPIO_Pin_10
#define DEBUG_USART_RX_AF                       GPIO_AF_USART1
#define DEBUG_USART_RX_SOURCE                   GPIO_PinSource10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_PIN                      GPIO_Pin_9
#define DEBUG_USART_TX_AF                       GPIO_AF_USART1
#define DEBUG_USART_TX_SOURCE                   GPIO_PinSource9

#define DEBUG_USART_IRQn                        USART1_IRQn
#define DEBUG_USART_INT_ID                      BSP_INT_ID_USART1
#define DEBUG_DMA_Stream                        DMA2_Stream5
#define DEBUG_DMA_CLEAR_FLAG                    (DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5)

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void GNSS_USART_IRQHandler(void);
extern void GNSS_USART_Send(u8_t *pdata);
extern void BSP_InitUsart(void);
extern void DEBUG_USART_Send(u8_t *pdata, u16_t len);

#endif
