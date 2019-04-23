/**
  ******************************************************************************
  * @file    bsp_eth.c
  * @author  MCD Application Team
  * @revision Michael Vysotsky
  * @version V1.0.2
  * @date    06-June-2011 
  * @brief   STM32F2x7 Ethernet hardware configuration.
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

/* Includes ------------------------------------------------------------------*/
#include <includes.h>
#include "bsp_eth_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define     ETH_MCO_PORT              GPIOA
#define     ETH_MCO_PIN               GPIO_Pin_8
#define     ETH_MCO_PINSRC            GPIO_PinSource8

#if defined (ETH_MII_CRS_PORTA)
#define     ETH_CRS_PORT              GPIOA
#define     ETH_CRS_PIN               GPIO_Pin_0
#define     ETH_CRS_PINSRC            GPIO_PinSource0
#elif defined (ETH_MII_CRS_PORTH)
#define     ETH_CRS_PORT              GPIOH
#define     ETH_CRS_PIN               GPIO_Pin_2
#define     ETH_CRS_PINSRC            GPIO_PinSource2
#else
#define     ETH_CRS_PIN               0
#define     ETH_CRS_PORT              0
#define     ETH_CRS_PINSRC            255
#warning "ETH port for CRS is not defined"
#endif

#if defined (ETH_MII_RX_CLK_PORTA) || defined (ETH_RMII_REF_CLK_PORTA)
#define     ETH_RX_CLK_PORT           GPIOA
#define     ETH_RX_CLK_PIN            GPIO_Pin_1
#define     ETH_RX_CLK_PINSRC         GPIO_PinSource1
#else
#define     ETH_RX_CLK_PIN            0
#define     ETH_RX_CLK_PORT           0
#define     ETH_RX_CLK_PINSRC         255
#warning "ETH port for RX_CLK is not defined"
#endif

#if defined (ETH_MDIO_PORTA)
#define     ETH_MDIO_PORT             GPIOA
#define     ETH_MDIO_PIN              GPIO_Pin_2
#define     ETH_MDIO_PINSRC           GPIO_PinSource2
#else
#define     ETH_MDIO_PIN              0
#define     ETH_MDIO_PORT             0
#define     ETH_MDIO_PINSRC           255
#warning "ETH port for MDIO is not defined"
#endif

#if defined (ETH_MII_COL_PORTA)
#define     ETH_COL_PORT              GPIOA
#define     ETH_COL_PIN               GPIO_Pin_3
#define     ETH_COL_PINSRC            GPIO_PinSource3
#elif defined (ETH_MII_COL_PORTH)
#define     ETH_COL_PORT              GPIOH
#define     ETH_COL_PIN               GPIO_Pin_3
#define     ETH_COL_PINSRC            GPIO_PinSource3
#else
#define     ETH_COL_PIN               0
#define     ETH_COL_PORT              0
#define     ETH_COL_PINSRC            255
#warning "ETH port for COL is not defined"
#endif

#if defined (ETH_MII_RX_DV_PORTA) || defined (ETH_RMII_CRS_DV_PORTA)
#define     ETH_RX_DV_PORT            GPIOA
#define     ETH_RX_DV_PIN             GPIO_Pin_7
#define     ETH_RX_DV_PINSRC          GPIO_PinSource7
#else
#define     ETH_RX_DV_PIN             0
#define     ETH_RX_DV_PORT            0
#define     ETH_RX_DV_PINSRC          255
#warning "ETH port for RX_DV is not defined"
#endif
                                      
#if defined (ETH_MII_RXD2_PORTB)
#define     ETH_RXD2_PORT             GPIOB
#define     ETH_RXD2_PIN              GPIO_Pin_0
#define     ETH_RXD2_PINSRC           GPIO_PinSource0
#elif defined (ETH_MII_RXD2_PORTH)
#define     ETH_RXD2_PORT             GPIOH
#define     ETH_RXD2_PIN              GPIO_Pin_6
#define     ETH_RXD2_PINSRC           GPIO_PinSource6
#else
#define     ETH_RXD2_PIN              0
#define     ETH_RXD2_PORT             0
#define     ETH_RXD2_PINSRC           255
#warning "ETH port for RXD2 is not defined"
#endif

#if defined (ETH_MII_RXD3_PORTB)
#define     ETH_RXD3_PORT             GPIOB
#define     ETH_RXD3_PIN              GPIO_Pin_1
#define     ETH_RXD3_PINSRC           GPIO_PinSource1
#elif defined (ETH_MII_RXD3_PORTH)
#define     ETH_RXD3_PORT             GPIOH
#define     ETH_RXD3_PIN              GPIO_Pin_7
#define     ETH_RXD3_PINSRC           GPIO_PinSource7
#else
#define     ETH_RXD3_PIN              0
#define     ETH_RXD3_PORT             0
#define     ETH_RXD3_PINSRC           255
#warning "ETH port for RXD3 is not defined"
#endif

#if defined (ETH_PPS_OUT_PORTB)
#define     ETH_PPS_OUT_PORT          GPIOB
#define     ETH_PPS_OUT_PIN           GPIO_Pin_5
#define     ETH_PPS_OUT_PINSRC        GPIO_PinSource5
#elif defined (ETH_PPS_OUT_PORTG)
#define     ETH_PPS_OUT_PORT          GPIOG
#define     ETH_PPS_OUT_PIN           GPIO_Pin_8
#define     ETH_PPS_OUT_PINSRC        GPIO_PinSource8
#else
#define     ETH_PPS_OUT_PIN           0
#define     ETH_PPS_OUT_PORT          0
#define     ETH_PPS_OUT_PINSRC        255
#warning "ETH port for PPS_OUT is not defined"
#endif

#if defined (ETH_MII_TXD3_PORTB)
#define     ETH_TXD3_PORT             GPIOB
#define     ETH_TXD3_PIN              GPIO_Pin_8
#define     ETH_TXD3_PINSRC           GPIO_PinSource8
#elif defined (ETH_MII_TXD3_PORTE)
#define     ETH_TXD3_PORT             GPIOE
#define     ETH_TXD3_PIN              GPIO_Pin_2
#define     ETH_TXD3_PINSRC           GPIO_PinSource2
#else
#define     ETH_TXD3_PIN              0
#define     ETH_TXD3_PORT             0
#define     ETH_TXD3_PINSRC           255
#warning "ETH port for TXD3 is not defined"
#endif

#if defined (ETH_MII_RX_ER_PORTB)
#define     ETH_RX_ER_PORT            GPIOB
#define     ETH_RX_ER_PIN             GPIO_Pin_10
#define     ETH_RX_ER_PINSRC          GPIO_PinSource10
#elif defined (ETH_MII_RX_ER_PORTI)
#define     ETH_RX_ER_PORT            GPIOI
#define     ETH_RX_ER_PIN             GPIO_Pin_10
#define     ETH_RX_ER_PINSRC          GPIO_PinSource10
#else
#define     ETH_RX_ER_PIN             0
#define     ETH_RX_ER_PORT            0
#define     ETH_RX_ER_PINSRC          255
#warning "ETH port for RX_ER is not defined"
#endif

#if defined (ETH_MII_TX_EN_PORTB) ||  defined (ETH_RMII_TX_EN_PORTB)
#define     ETH_TX_EN_PORT            GPIOB
#define     ETH_TX_EN_PIN             GPIO_Pin_11
#define     ETH_TX_EN_PINSRC          GPIO_PinSource11
#elif defined (ETH_MII_TX_EN_PORTG) || defined (ETH_RMII_TX_EN_PORTG) 
#define     ETH_TX_EN_PORT            GPIOG
#define     ETH_TX_EN_PIN             GPIO_Pin_11
#define     ETH_TX_EN_PINSRC          GPIO_PinSource11
#else
#define     ETH_TX_EN_PIN             0
#define     ETH_TX_EN_PORT            0
#define     ETH_TX_EN_PINSRC          255
#warning "ETH port for TX_EN is not defined"
#endif

#if defined (ETH_MII_TXD0_PORTB) || defined (ETH_RMII_TXD0_PORTB)
#define     ETH_TXD0_PORT             GPIOB
#define     ETH_TXD0_PIN              GPIO_Pin_12
#define     ETH_TXD0_PINSRC           GPIO_PinSource12
#elif defined (ETH_MII_TXD0_PORTG) || defined (ETH_RMII_TXD0_PORTG)
#define     ETH_TXD0_PORT             GPIOG
#define     ETH_TXD0_PIN              GPIO_Pin_13
#define     ETH_TXD0_PINSRC           GPIO_PinSource13
#else
#define     ETH_TXD0_PIN              0
#define     ETH_TXD0_PORT             0
#define     ETH_TXD0_PINSRC           255
#warning "ETH port for TXD0 is not defined"
#endif

#if defined (ETH_MII_TXD1_PORTB) || defined (ETH_RMII_TXD1_PORTB)
#define     ETH_TXD1_PORT             GPIOB
#define     ETH_TXD1_PIN              GPIO_Pin_13
#define     ETH_TXD1_PINSRC           GPIO_PinSource13
#elif defined (ETH_MII_TXD1_PORTG) || defined (ETH_RMII_TXD1_PORTG)
#define     ETH_TXD1_PORT             GPIOG
#define     ETH_TXD1_PIN              GPIO_Pin_14
#define     ETH_TXD1_PINSRC           GPIO_PinSource14
#else
#define     ETH_TXD1_PIN              0
#define     ETH_TXD1_PORT             0
#define     ETH_TXD1_PINSRC           255
#warning "ETH port for TXD1 is not defined"
#endif

#if defined (ETH_MDC_PORTC)
#define     ETH_MDC_PORT              GPIOC
#define     ETH_MDC_PIN               GPIO_Pin_1
#define     ETH_MDC_PINSRC            GPIO_PinSource1
#else
#define     ETH_MDC_PIN               0
#define     ETH_MDC_PORT              0
#define     ETH_MDC_PINSRC            255
#warning "ETH port for MDC is not defined"
#endif

#if defined (ETH_MII_TXD2_PORTC)
#define     ETH_TXD2_PORT             GPIOC
#define     ETH_TXD2_PIN              GPIO_Pin_2
#define     ETH_TXD2_PINSRC           GPIO_PinSource2
#else
#define     ETH_TXD2_PIN              0
#define     ETH_TXD2_PORT             0
#define     ETH_TXD2_PINSRC           255
#warning "ETH port for TXD2 is not defined"
#endif

#if defined (ETH_MII_TX_CLK_PORTC)
#define     ETH_TX_CLK_PORT           GPIOC
#define     ETH_TX_CLK_PIN            GPIO_Pin_3
#define     ETH_TX_CLK_PINSRC         GPIO_PinSource3
#else
#define     ETH_TX_CLK_PIN            0
#define     ETH_TX_CLK_PORT           0
#define     ETH_TX_CLK_PINSRC         255
#warning "ETH port for TX_CLK is not defined"
#endif

#if defined (ETH_MII_RXD0_PORTC) || defined (ETH_RMII_RXD0_PORTC)
#define     ETH_RXD0_PORT             GPIOC
#define     ETH_RXD0_PIN              GPIO_Pin_4
#define     ETH_RXD0_PINSRC           GPIO_PinSource4
#else
#define     ETH_RXD0_PIN              0
#define     ETH_RXD0_PORT             0
#define     ETH_RXD0_PINSRC           255
#warning "ETH port for RXD0 is not defined"
#endif

#if defined (ETH_MII_RXD1_PORTC) || defined (ETH_RMII_RXD1_PORTC)
#define     ETH_RXD1_PORT             GPIOC
#define     ETH_RXD1_PIN              GPIO_Pin_5
#define     ETH_RXD1_PINSRC           GPIO_PinSource5
#else
#define     ETH_RXD1_PIN              0
#define     ETH_RXD1_PORT             0
#define     ETH_RXD1_PINSRC           255
#warning "ETH port for RXD1 is not defined"
#endif

#define DP83848_PHY_ADDRESS       0x01 /* Relative to STM322xG-EVAL Board */





/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void ETH_GPIO_Config(void);
static void ETH_MACDMA_Config(void);

#define MII_MODE
/* Uncomment the define below to clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef 	MII_MODE
 #define PHY_CLOCK_MCO
#endif

#define ETH_TASK_STK_SIZE               TASK_STK_SIZE


/* Ethernet buffers */
extern ETH_DMADESCTypeDef  *DMATxDescToSet;
extern ETH_DMADESCTypeDef  *DMARxDescToGet;
extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB]; 
extern ETH_DMADESCTypeDef  DMATxDscrTab[ETH_TXBUFNB];
extern BYTE Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE], Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;
void  (*BSP_ETH_IRQHandler)(void) = NULL;
/* Private functions ---------------------------------------------------------*/

static void ETH_IRQHandler(void);



/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
static void ETH_MACDMA_Config(void)
{
  ETH_InitTypeDef ETH_InitStructure;

  /* Enable ETHERNET clock  */
  BSP_PeriphEn(BSP_PERIPH_ID_ETH);
  BSP_PeriphEn(BSP_PERIPH_ID_ETH_TX);
  BSP_PeriphEn(BSP_PERIPH_ID_ETH_RX);  
                                            
  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();

  /* Software reset */
  ETH_SoftwareReset();

  /* Wait for software reset */
  while (ETH_GetSoftwareResetStatus() == SET);

  /* ETHERNET Configuration --------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
  //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
  //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
  //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   

  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
  ETH_Init(&ETH_InitStructure, DP83848_PHY_ADDRESS);

  /* Enable the Ethernet Rx Interrupt */
  ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
//static void ETH_GPIO_Config(void)
//{
//  GPIO_InitTypeDef GPIO_InitStructure;
//    /* Enable GPIOs clocks */
//  BSP_PeriphEn(BSP_PERIPH_ID_IOPA);
//  /* Enable SYSCFG clock */
//  BSP_PeriphEn(BSP_PERIPH_ID_SYSCFG); 
//  /* Configure MCO (PA8) */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
//  GPIO_Init(GPIOA, &GPIO_InitStructure); 
//  
//  /* MII/RMII Media interface selection --------------------------------------*/
//#ifdef MII_MODE /* Mode MII with STM322xG-EVAL  */
// #ifdef PHY_CLOCK_MCO
//  /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
//  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
// #endif /* PHY_CLOCK_MCO */
//  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
//#elif defined RMII_MODE  /* Mode RMII with STM322xG-EVAL */
//  /* Output PLL clock divided by 2 (50MHz) on MCO pin (PA8) to clock the PHY */
//  RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_2);
//  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
//#endif
//#define GPIO_ETH_CONF(pin) \
//  if(ETH_##pin##_PIN){ \
//    BSP_GpioPortEn(ETH_##pin##_PORT); \
//    GPIO_InitStructure.GPIO_Pin = ETH_##pin##_PIN;  \
//    GPIO_Init(ETH_##pin##_PORT, &GPIO_InitStructure); \
//    GPIO_PinAFConfig(ETH_##pin##_PORT, ETH_##pin##_PINSRC, GPIO_AF_ETH); \
//  }   
//  GPIO_ETH_CONF(MDIO);
//  GPIO_ETH_CONF(MDC);
//  GPIO_ETH_CONF(PPS_OUT);
//  GPIO_ETH_CONF(CRS);
//  GPIO_ETH_CONF(COL);
//  GPIO_ETH_CONF(RX_ER);
//  GPIO_ETH_CONF(RXD0);
//  GPIO_ETH_CONF(RXD1);
//  GPIO_ETH_CONF(RXD2);
//  GPIO_ETH_CONF(RXD3); 
//  GPIO_ETH_CONF(TX_CLK);
//  GPIO_ETH_CONF(TXD0); 
//  GPIO_ETH_CONF(TXD1);
//  GPIO_ETH_CONF(TXD2);
//  GPIO_ETH_CONF(TXD3); 
//  GPIO_ETH_CONF(RX_CLK); 
//  GPIO_ETH_CONF(RX_DV);
//  GPIO_ETH_CONF(TX_EN);  
//}
static void ETH_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

  #define GPIO_ETH_CONF(pin) \
  if(ETH_##pin##_PIN){ \
    BSP_GpioPortEn(ETH_##pin##_PORT); \
    GPIO_InitStructure.GPIO_Pin = ETH_##pin##_PIN;  \
    GPIO_Init(ETH_##pin##_PORT, &GPIO_InitStructure); \
    GPIO_PinAFConfig(ETH_##pin##_PORT, ETH_##pin##_PINSRC, GPIO_AF); \
  }   
  #define GPIO_AF   GPIO_AF_MCO
  /* Configure MCO (PA8) */ 
  GPIO_ETH_CONF(MCO);  
  /* Enable SYSCFG clock */
  BSP_PeriphEn(BSP_PERIPH_ID_SYSCFG);  
  /* MII/RMII Media interface selection --------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM322xG-EVAL  */
 #ifdef PHY_CLOCK_MCO
  /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
 #endif /* PHY_CLOCK_MCO */
  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
#elif defined RMII_MODE  /* Mode RMII with STM322xG-EVAL */
  /* Output PLL clock divided by 2 (50MHz) on MCO pin (PA8) to clock the PHY */
  RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_2);
  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
#endif
  #undef  GPIO_AF
  #define GPIO_AF   GPIO_AF_ETH
  GPIO_ETH_CONF(MDIO);
  GPIO_ETH_CONF(MDC);
  GPIO_ETH_CONF(PPS_OUT);
  GPIO_ETH_CONF(CRS);
  GPIO_ETH_CONF(COL);
  GPIO_ETH_CONF(RX_ER);
  GPIO_ETH_CONF(RXD0);
  GPIO_ETH_CONF(RXD1);
  GPIO_ETH_CONF(RXD2);
  GPIO_ETH_CONF(RXD3); 
  GPIO_ETH_CONF(TX_CLK);
  GPIO_ETH_CONF(TXD0); 
  GPIO_ETH_CONF(TXD1);
  GPIO_ETH_CONF(TXD2);
  GPIO_ETH_CONF(TXD3); 
  GPIO_ETH_CONF(RX_CLK); 
  GPIO_ETH_CONF(RX_DV);
  GPIO_ETH_CONF(TX_EN);  
}
/**
  * @brief  ETH_BSP_Config
  * @param  None
  * @retval None
  */
static void ETH_BSP_Config(void)
{
  /* Configure the GPIO ports for ethernet pins */
  ETH_GPIO_Config();
  /* Configure the Ethernet MAC/DMA */
  ETH_MACDMA_Config();
}
/**
  * @brief  BSP Initializes the Ethernet
  * @param  void
  * @retval None
  */
void  BSP_InitEth(void)
{
  CPU_SR_ALLOC();  
  CPU_CRITICAL_ENTER();
  /* Configure the GPIO ports */
  ETH_BSP_Config();
  /* prefix vendor STMicroelectronics */
  ETH_MACAddressConfig(ETH_MAC_Address0,BSP_ETH_GetMacAddress());
 
  /* Initialize Tx Descriptors list: Chain Mode */
  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  /* Initialize Rx Descriptors list: Chain Mode  */
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
 
  /* Rx interrupt enable */
  BSP_IntVectSet(BSP_INT_ID_ETH, ETH_IRQHandler);
  BSP_IntEn(BSP_INT_ID_ETH);    
  int i;
  for(i=0; i<ETH_RXBUFNB; ++i){
    ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
  }
  /* Enable the checksum insertion for the Tx frames */
  for(i=0; i<ETH_TXBUFNB; i++){
    ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
  }
  CPU_CRITICAL_EXIT();
  /* Enable MAC and DMA transmission and reception */
  ETH_Start();  
}
/**
  * @brief  BSP Generation MAC Address Ethernet
  * @param  void
  * @retval pointer static array MAC address
  */
uint8_t * BSP_ETH_GetMacAddress(void)
{
      /* Enable CRC clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    /* prefix vendor STMicroelectronics */
  static uint8_t Addr[] = {0, 0x80, 0xE1,0,0,0 };
  CRC_ResetDR();
  int i;
  uint32_t *w = (DWORD *)0x1FFFF7E8,Crc32;
  for(i=0; i<3; ++i)
    Crc32 = CRC_CalcCRC(*w++);
  Crc32 = (0xFFFFFF&Crc32)+((0xFF000000&Crc32)>>24);
  Addr[3] = (Crc32 >> 16)&0xFF;
  Addr[4] = (Crc32 >> 8)&0xFF;
  Addr[5] = (Crc32)&0xFF;
  return Addr;
}




/*******************************************************************************
* Function Name  : ETH_GetCurrentTxBuffer
* Description    : Return the address of the buffer pointed by the current descritor.
* Input          : None
* Output         : None
* Return         : Buffer address
*******************************************************************************/
uint8_t * BSP_ETH_GetCurrentTxBuffer(void)
{
  uint8_t * tx_addr = (BYTE*)DMATxDescToSet->Buffer1Addr;
  /* Return Buffer address */
  return tx_addr;   
}


uint32_t BSP_ETH_IsRxPktValid(void)
{
  return ETH_GetRxPktSize(DMARxDescToGet); 
}


/**
  * @brief  OS ETH_IRQHandler()
  *
  * IT function to receive and send Ethernet packet
  * @param  void
  * @retval None
  */
  /* Handles all the received frames */
static void ETH_IRQHandler(void)
{
  /* Frame received */
  if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET){
    /* Give the semaphore to wakeup LwIP task */
    if(BSP_ETH_IRQHandler)
      BSP_ETH_IRQHandler();
  }
	
  /* Clear the interrupt flags. */
  /* Clear the Eth DMA Rx IT pending bits */
  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}

void BSP_ETH_ReleaseDescriptor(FrameTypeDef * pframe)
{
  int i;
   __IO ETH_DMADESCTypeDef *DMARxNextDesc;
  /* Release descriptors to DMA */
  /* Check if received frame with multiple DMA buffer segments */
  if (DMA_RX_FRAME_infos->Seg_Count > 1){
    DMARxNextDesc = DMA_RX_FRAME_infos->FS_Rx_Desc;
  }else{
    DMARxNextDesc = pframe->descriptor;
  }
  
  /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
  for (i=0; i<DMA_RX_FRAME_infos->Seg_Count; i++){  
    DMARxNextDesc->Status = ETH_DMARxDesc_OWN;
    DMARxNextDesc = (ETH_DMADESCTypeDef *)(DMARxNextDesc->Buffer2NextDescAddr);
  }
  
  /* Clear Segment_Count */
  DMA_RX_FRAME_infos->Seg_Count =0;
  
  
  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET){
    /* Clear RBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_RBUS;
      
    /* Resume DMA reception */
    ETH->DMARPDR = 0;
  }
}
bool BSP_ETH_GetDescriptorStatus(FrameTypeDef * pframe)
{
  return ((pframe->descriptor->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET)?
    true:false;
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
