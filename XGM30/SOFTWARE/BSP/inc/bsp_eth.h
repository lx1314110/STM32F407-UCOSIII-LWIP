/**
  ******************************************************************************
  * @file    bsp_eth.h
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    06-June-2011
  * @brief   Header for bsp_eth.c file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BSP_ETH_H
#define BSP_ETH_H
#define ETH_LINK_PIN                   GPIO_Pin_11
#define ETH_LINK_EXTI_LINE             EXTI_Line11
#define ETH_LINK_EXTI_PORT_SOURCE      EXTI_PortSourceGPIOD
#define ETH_LINK_EXTI_PIN_SOURCE       EXTI_PinSource11
#define ETH_LINK_EXTI_IRQn             EXTI15_10_IRQn 
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4x7_eth.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern void  (*BSP_ETH_IRQHandler)(void);   
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

BYTE *    BSP_ETH_GetMacAddress(void);
void      BSP_InitEth(void);
void reConnect_config();
BYTE *    BSP_ETH_GetCurrentTxBuffer(void);
bool      BSP_ETH_GetDescriptorStatus(FrameTypeDef * pframe);
void      BSP_ETH_ReleaseDescriptor(FrameTypeDef * pframe);
uint32_t  BSP_ETH_IsRxPktValid(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_ETH_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
