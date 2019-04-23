/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM322xG Evaluation Board
*
* Filename      : bsp.h
* Version       : V1.10
* Programmer(s) : BAN
*                 Michael Vysotsky
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_PRESENT
#define  BSP_PRESENT

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_MODULE
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif
/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/

#define    UUID_DEVICE           (const BYTE*)0x1FFF7A10
/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/
#include  <bsp_lib.h>
#include  <bsp_eth.h>



/*
*********************************************************************************************************
*                                               INT DEFINES
*********************************************************************************************************
*/                                                                          
#define  BSP_INT_ID_WWDG                                   0    /* Window WatchDog Interrupt                            */
#define  BSP_INT_ID_PVD                                    1    /* PVD through EXTI Line detection Interrupt            */
#define  BSP_INT_ID_TAMPER                                 2    /* Tamper Interrupt                                     */
#define  BSP_INT_ID_RTC                                    3    /* RTC global Interrupt                                 */
#define  BSP_INT_ID_FLASH                                  4    /* FLASH global Interrupt                               */
#define  BSP_INT_ID_RCC                                    5    /* RCC global Interrupt                                 */
#define  BSP_INT_ID_EXTI0                                  6    /* EXTI Line0 Interrupt                                 */
#define  BSP_INT_ID_EXTI1                                  7    /* EXTI Line1 Interrupt                                 */
#define  BSP_INT_ID_EXTI2                                  8    /* EXTI Line2 Interrupt                                 */
#define  BSP_INT_ID_EXTI3                                  9    /* EXTI Line3 Interrupt                                 */
#define  BSP_INT_ID_EXTI4                                 10    /* EXTI Line4 Interrupt                                 */
#define  BSP_INT_ID_DMA1_ST0                              11    /* DMA1 Stream 0 global Interrupt                      */
#define  BSP_INT_ID_DMA1_ST1                              12    /* DMA1 Stream 1 global Interrupt                      */
#define  BSP_INT_ID_DMA1_ST2                              13    /* DMA1 Stream 2 global Interrupt                      */
#define  BSP_INT_ID_DMA1_ST3                              14    /* DMA1 Stream 3 global Interrupt                      */
#define  BSP_INT_ID_DMA1_ST4                              15    /* DMA1 Stream 4 global Interrupt                      */
#define  BSP_INT_ID_DMA1_ST5                              16    /* DMA1 Stream 5 global Interrupt                      */
#define  BSP_INT_ID_DMA1_ST6                              17    /* DMA1 Stream 6 global Interrupt                      */
#define  BSP_INT_ID_ADC1_2_3                              18    /* ADC1, ADC2 and ADC3s global Interrupt                */
#define  BSP_INT_ID_CAN1_TX                               19    /* CAN1 TX Interrupts                                   */
#define  BSP_INT_ID_CAN1_RX0                              20    /* CAN1 RX0 Interrupts                                  */
#define  BSP_INT_ID_CAN1_RX1                              21    /* CAN1 RX1 Interrupt                                   */
#define  BSP_INT_ID_CAN1_SCE                              22    /* CAN1 SCE Interrupt                                   */
#define  BSP_INT_ID_EXTI9_5                               23    /* External Line[9:5] Interrupts                        */
#define  BSP_INT_ID_TIM1_BRK_TIM9                         24    /* TIM1 Break and TIM9 Interrupt                        */
#define  BSP_INT_ID_TIM1_UP_TIM10                         25    /* TIM1 Update and TIM10 Interrupt                      */
#define  BSP_INT_ID_TIM1_TRG_COM_TIM11                    26    /* TIM1 Trigger and Commutation and TIM11 Interrupt     */
#define  BSP_INT_ID_TIM1_CC                               27    /* TIM1 Capture Compare Interrupt                       */
#define  BSP_INT_ID_TIM2                                  28    /* TIM2 global Interrupt                                */
#define  BSP_INT_ID_TIM3                                  29    /* TIM3 global Interrupt                                */
#define  BSP_INT_ID_TIM4                                  30    /* TIM4 global Interrupt                                */
#define  BSP_INT_ID_I2C1_EV                               31    /* I2C1 Event Interrupt                                 */
#define  BSP_INT_ID_I2C1_ER                               32    /* I2C1 Error Interrupt                                 */
#define  BSP_INT_ID_I2C2_EV                               33    /* I2C2 Event Interrupt                                 */
#define  BSP_INT_ID_I2C2_ER                               34    /* I2C2 Error Interrupt                                 */
#define  BSP_INT_ID_SPI1                                  35    /* SPI1 global Interrupt                                */
#define  BSP_INT_ID_SPI2                                  36    /* SPI2 global Interrupt                                */
#define  BSP_INT_ID_USART1                                37    /* USART1 global Interrupt                              */
#define  BSP_INT_ID_USART2                                38    /* USART2 global Interrupt                              */
#define  BSP_INT_ID_USART3                                39    /* USART3 global Interrupt                              */
#define  BSP_INT_ID_EXTI15_10                             40    /* External Line[15:10] Interrupts                      */
#define  BSP_INT_ID_RTCAlarm                              41    /* RTC Alarm through EXTI Line Interrupt                */
#define  BSP_INT_ID_USBWakeUp                             42    /* USB WakeUp from suspend through EXTI Line Interrupt  */
#define  BSP_INT_ID_TIM8_BRK_TIM12                        43    /* TIM8 Break and TIM12 Interrupt                       */
#define  BSP_INT_ID_TIM8_UP_TIM13                         44    /* TIM8 Update and TIM13 Interrupt                      */
#define  BSP_INT_ID_TIM8_TRG_COM_TIM14                    45    /* TIM8 Trigger and Commutation and TIM14 Interrupt     */
#define  BSP_INT_ID_TIM8_CC                               46    /* TIM8 Capture Compare Interrupt                       */
#define  BSP_INT_ID_DMA1_ST7                              47    /* DMA1 Stream 7 global Interrupt                      */
#define  BSP_INT_ID_FSMC                                  48    /* FSMC global Interrupt                                */
#define  BSP_INT_ID_SDIO                                  49    /* SDIO global Interrupt                                */
#define  BSP_INT_ID_TIM5                                  50    /* TIM5 global Interrupt                                */
#define  BSP_INT_ID_SPI3                                  51    /* SPI3 global Interrupt                                */
#define  BSP_INT_ID_UART4                                 52    /* UART4 global Interrupt                               */
#define  BSP_INT_ID_UART5                                 53    /* UART5 global Interrupt                               */
#define  BSP_INT_ID_TIM6_DAC                              54    /* TIM6 and DAC1&2 underrun errors global Interrupt     */
#define  BSP_INT_ID_TIM7                                  55    /* TIM7 global Interrupt                                */
#define  BSP_INT_ID_DMA2_ST0                              56    /* DMA2 Stream 0 global Interrupt                      */
#define  BSP_INT_ID_DMA2_ST1                              57    /* DMA2 Stream 1 global Interrupt                      */
#define  BSP_INT_ID_DMA2_ST2                              58    /* DMA2 Stream 2 global Interrupt                      */
#define  BSP_INT_ID_DMA2_ST3                              59    /* DMA2 Stream 3 global Interrupt                      */
#define  BSP_INT_ID_DMA2_ST4                              60    /* DMA2 Stream 4 global Interrupt                      */
#define  BSP_INT_ID_ETH                                   61    /* Ethernet global interrupt                            */
#define  BSP_INT_ID_ETH_WKUP                              62    /* Ethernet Wakeup through EXTI line interrupt          */
#define  BSP_INT_ID_CAN2_TX                               63    /* CAN2 TX interrupts                                   */
#define  BSP_INT_ID_CAN2_RX0                              64    /* CAN2 RX0 interrupts                                  */
#define  BSP_INT_ID_CAN2_RX1                              65    /* CAN2 RX1 interrupt                                   */
#define  BSP_INT_ID_CAN2_SCE                              66    /* CAN2 SCE interrupt                                   */
#define  BSP_INT_ID_OTG_FS                                67    /* USB On The Go FS global interrupt                    */
#define  BSP_INT_ID_DMA2_ST5                              68    /* DMA2 Stream 5 global Interrupt                      */
#define  BSP_INT_ID_DMA2_ST6                              69    /* DMA2 Stream 6 global Interrupt                      */
#define  BSP_INT_ID_DMA2_ST7                              70    /* DMA2 Stream 7 global Interrupt                      */
#define  BSP_INT_ID_USART6                                71    /* USART6 global Interrupt                              */                                   
#define  BSP_INT_ID_I2C3_EV                               72    /* I2C3 Event Interrupt                                 */
#define  BSP_INT_ID_I2C3_ER                               73    /* I2C3 Error Interrupt                                 */                                          
#define  BSP_INT_ID_OTG_HS_EP1_OUT                        74    /* USB On The Go HS End Point 1 Out global interrupt    */                                           
#define  BSP_INT_ID_OTG_HS_EP1_IN                         75    /* USB On The Go HS End Point 1 In global interrupt     */                      
#define  BSP_INT_ID_OTG_HS_WKUP                           76    /* USB On The Go HS Wakeup through EXTI In global interrupt*/                       
#define  BSP_INT_ID_OTG_HS                                77    /* USB On The Go HS global interrupt                    */                       
#define  BSP_INT_ID_DCMI                                  78    /* DCMI global interrupt                                */                                      
#define  BSP_INT_ID_CRYP                                  79    /* CRYP crypto global interrupt                         */                                           
#define  BSP_INT_ID_HASH_RNG                              80    /* Hash and Rng global interrupt                        */                                      

/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/
/*  RCC_AHB1        offset 0x10     */
#define  BSP_PERIPH_ID_IOPA                               0
#define  BSP_PERIPH_ID_IOPB                               1
#define  BSP_PERIPH_ID_IOPC                               2
#define  BSP_PERIPH_ID_IOPD                               3
#define  BSP_PERIPH_ID_IOPE                               4
#define  BSP_PERIPH_ID_IOPF                               5
#define  BSP_PERIPH_ID_IOPG                               6
#define  BSP_PERIPH_ID_IOPH                               7
#define  BSP_PERIPH_ID_IOPI                               8
#define  BSP_PERIPH_ID_CRC                                12
#define  BSP_PERIPH_ID_BKPSRAM                            18
#define  BSP_PERIPH_ID_DMA1                               21
#define  BSP_PERIPH_ID_DMA2                               22
#define  BSP_PERIPH_ID_ETH                                25
#define  BSP_PERIPH_ID_ETH_TX                             26
#define  BSP_PERIPH_ID_ETH_RX                             27
#define  BSP_PERIPH_ID_ETH_PTP                            28
#define  BSP_PERIPH_ID_OTGHS                              29
#define  BSP_PERIPH_ID_OTGHS_ULPI                         30
/*  RCC_AHB2        offset 0x14     */
#define  RCC_AHB2_OFFSET                                  32
#define  BSP_PERIPH_ID_DCMI                               RCC_AHB2_OFFSET+0
#define  BSP_PERIPH_ID_CRUP                               RCC_AHB2_OFFSET+4
#define  BSP_PERIPH_ID_HASH                               RCC_AHB2_OFFSET+5
#define  BSP_PERIPH_ID_RNG                                RCC_AHB2_OFFSET+6
#define  BSP_PERIPH_ID_OTGFS                              RCC_AHB2_OFFSET+7
/*  RCC_ABH3        offset 0x18     */
#define  RCC_AHB3_OFFSET                                  64
#define  BSP_PERIPH_ID_FSMC                               RCC_AHB3_OFFSET
/*  RCC_APB1        offset 0x20     */
#define  RCC_APB1_OFFSET                                  128
#define  BSP_PERIPH_ID_TIM2                               RCC_APB1_OFFSET+0
#define  BSP_PERIPH_ID_TIM3                               RCC_APB1_OFFSET+1
#define  BSP_PERIPH_ID_TIM4                               RCC_APB1_OFFSET+2
#define  BSP_PERIPH_ID_TIM5                               RCC_APB1_OFFSET+3
#define  BSP_PERIPH_ID_TIM6                               RCC_APB1_OFFSET+4
#define  BSP_PERIPH_ID_TIM7                               RCC_APB1_OFFSET+5
#define  BSP_PERIPH_ID_TIM12                              RCC_APB1_OFFSET+6
#define  BSP_PERIPH_ID_TIM13                              RCC_APB1_OFFSET+7
#define  BSP_PERIPH_ID_TIM14                              RCC_APB1_OFFSET+8
#define  BSP_PERIPH_ID_WWDG                               RCC_APB1_OFFSET+11
#define  BSP_PERIPH_ID_SPI2                               RCC_APB1_OFFSET+14
#define  BSP_PERIPH_ID_SPI3                               RCC_APB1_OFFSET+15
#define  BSP_PERIPH_ID_USART2                             RCC_APB1_OFFSET+17
#define  BSP_PERIPH_ID_USART3                             RCC_APB1_OFFSET+18
#define  BSP_PERIPH_ID_UART4                              RCC_APB1_OFFSET+19
#define  BSP_PERIPH_ID_UART5                              RCC_APB1_OFFSET+20
#define  BSP_PERIPH_ID_I2C1                               RCC_APB1_OFFSET+21
#define  BSP_PERIPH_ID_I2C2                               RCC_APB1_OFFSET+22
#define  BSP_PERIPH_ID_I2C3                               RCC_APB1_OFFSET+23
#define  BSP_PERIPH_ID_CAN1                               RCC_APB1_OFFSET+25
#define  BSP_PERIPH_ID_CAN2                               RCC_APB1_OFFSET+26
#define  BSP_PERIPH_ID_PWR                                RCC_APB1_OFFSET+28
#define  BSP_PERIPH_ID_DAC                                RCC_APB1_OFFSET+29
/*  RCC_APB2        offset 0x24     */
#define  RCC_APB2_OFFSET                                  160
#define  BSP_PERIPH_ID_TIM1                               RCC_APB2_OFFSET+0
#define  BSP_PERIPH_ID_TIM8                               RCC_APB2_OFFSET+1
#define  BSP_PERIPH_ID_USART1                             RCC_APB2_OFFSET+4
#define  BSP_PERIPH_ID_USART6                             RCC_APB2_OFFSET+5
#define  BSP_PERIPH_ID_ADC1                               RCC_APB2_OFFSET+8   /*  Reset All ADC */
#define  BSP_PERIPH_ID_ADC2                               RCC_APB2_OFFSET+9
#define  BSP_PERIPH_ID_ADC3                               RCC_APB2_OFFSET+10
#define  BSP_PERIPH_ID_SDIO                               RCC_APB2_OFFSET+11
#define  BSP_PERIPH_ID_SPI1                               RCC_APB2_OFFSET+12
#define  BSP_PERIPH_ID_SYSCFG                             RCC_APB2_OFFSET+14
#define  BSP_PERIPH_ID_TIM9                               RCC_APB2_OFFSET+16
#define  BSP_PERIPH_ID_TIM10                              RCC_APB2_OFFSET+17
#define  BSP_PERIPH_ID_TIM11                              RCC_APB2_OFFSET+18
/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/
#ifndef BSP_NOASSERT
#define BSP_PLATFORM_ASSERT(x) \
  do{ \
     printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
    } while(0)
#define BSP_ASSERT(message, assertion) do { if(!(assertion)) \
  BSP_PLATFORM_ASSERT(message); } while(0)
#else  /* BSP_NOASSERT */
#define BSP_ASSERT(message, assertion) 
#endif /* BSP_NOASSERT */


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         BSP_Init                    (void);

void         BSP_IntDisAll               (void);

void         BSP_Restart                 (void);
bool         BSP_IsReset                 (void);
void         BSP_SetTimestamp            (unsigned long * timestamp);
unsigned long *      BSP_GetTimestamp    (void);
void         BSP_UpdateTemestamp         (void);
bool         BSP_GetTime                 (struct tm *inTime);
void         BSP_GetSerialNumber         (char* buf);
void         BSP_WatchdogInit            (void);
void         BSP_WatchdogDisable         (void);
CPU_INT32U   BSP_CPU_ClkFreq             (void);
void         BSP_Sleep                   (OS_TICK dly);
/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/

void         BSP_IntInit                 (void);

void         BSP_IntEn                   (CPU_DATA       int_id);

void         BSP_IntDis                  (CPU_DATA       int_id);

void         BSP_IntClr                  (CPU_DATA       int_id);

void         BSP_IntVectSet              (CPU_DATA       int_id,
                                          CPU_FNCT_VOID  isr);

void         BSP_IntPrioSet              (CPU_DATA       int_id,
                                          CPU_INT08U     prio);

void  BSP_IntHandlerWWDG          (void);
void  BSP_IntHandlerPVD           (void);
void  BSP_IntHandlerTAMPER        (void);
void  BSP_IntHandlerRTC           (void);
void  BSP_IntHandlerFLASH         (void);
void  BSP_IntHandlerRCC           (void);
void  BSP_IntHandlerEXTI0         (void);
void  BSP_IntHandlerEXTI1         (void);
void  BSP_IntHandlerEXTI2         (void);
void  BSP_IntHandlerEXTI3         (void);
void  BSP_IntHandlerEXTI4         (void);
void  BSP_IntHandlerDMA1_CH0      (void);
void  BSP_IntHandlerDMA1_CH1      (void);
void  BSP_IntHandlerDMA1_CH2      (void);
void  BSP_IntHandlerDMA1_CH3      (void);
void  BSP_IntHandlerDMA1_CH4      (void);
void  BSP_IntHandlerDMA1_CH5      (void);
void  BSP_IntHandlerDMA1_CH6      (void);
void  BSP_IntHandlerADC1_2_3      (void);
void  BSP_IntHandlerCAN1_TX       (void);
void  BSP_IntHandlerCAN1_RX0      (void);
void  BSP_IntHandlerCAN1_RX1      (void);
void  BSP_IntHandlerCAN1_SCE      (void);
void  BSP_IntHandlerEXTI9_5       (void);
void  BSP_IntHandlerTIM1_BRK_TIM9 (void);
void  BSP_IntHandlerTIM1_UP_TIM10 (void);
void  BSP_IntHandlerTIM1_TRG_COM_TIM11(void);
void  BSP_IntHandlerTIM1_CC       (void);
void  BSP_IntHandlerTIM2          (void);
void  BSP_IntHandlerTIM3          (void);
void  BSP_IntHandlerTIM4          (void);
void  BSP_IntHandlerI2C1_EV       (void);
void  BSP_IntHandlerI2C1_ER       (void);
void  BSP_IntHandlerI2C2_EV       (void);
void  BSP_IntHandlerI2C2_ER       (void);
void  BSP_IntHandlerSPI1          (void);
void  BSP_IntHandlerSPI2          (void);
void  BSP_IntHandlerUSART1        (void);
void  BSP_IntHandlerUSART2        (void);
void  BSP_IntHandlerUSART3        (void);
void  BSP_IntHandlerEXTI15_10     (void);
void  BSP_IntHandlerRTCAlarm      (void);
void  BSP_IntHandlerUSBWakeUp     (void);
void  BSP_IntHandlerTIM8_BRK_TIM12(void);
void  BSP_IntHandlerTIM8_UP_TIM13 (void);
void  BSP_IntHandlerTIM8_TRG_COM_TIM14(void);
void  BSP_IntHandlerTIM8_CC       (void);
void  BSP_IntHandlerDMA1_CH7      (void);
void  BSP_IntHandlerFSMC          (void);
void  BSP_IntHandlerSDIO          (void);
void  BSP_IntHandlerTIM5          (void);
void  BSP_IntHandlerSPI3          (void);
void  BSP_IntHandlerUART4         (void);
void  BSP_IntHandlerUART5         (void);
void  BSP_IntHandlerTIM6_DAC      (void);
void  BSP_IntHandlerTIM7          (void);
void  BSP_IntHandlerDMA2_CH0      (void);
void  BSP_IntHandlerDMA2_CH1      (void);
void  BSP_IntHandlerDMA2_CH2      (void);
void  BSP_IntHandlerDMA2_CH3      (void);
void  BSP_IntHandlerDMA2_CH4      (void);

void  BSP_IntHandlerETH           (void);
void  BSP_IntHandlerETH_WKUP      (void);
void  BSP_IntHandlerCAN2_TX       (void);
void  BSP_IntHandlerCAN2_RX0      (void);
void  BSP_IntHandlerCAN2_RX1      (void);
void  BSP_IntHandlerCAN2_SCE      (void);
void  BSP_IntHandlerOTG_FS        (void);
void  BSP_IntHandlerDMA2_CH5      (void);
void  BSP_IntHandlerDMA2_CH6      (void);
void  BSP_IntHandlerDMA2_CH7      (void);
void  BSP_IntHandlerUART6         (void);
void  BSP_IntHandlerI2C3_EV       (void);
void  BSP_IntHandlerI2C3_ER       (void);
void  BSP_IntHandlerOTG_HS_EP1_OUT(void);
void  BSP_IntHandlerOTG_HS_EP1_IN (void);
void  BSP_IntHandlerOTG_HS_WKUP   (void);
void  BSP_IntHandlerOTG_HS        (void);
void  BSP_IntHandlerDCMI          (void);
void  BSP_IntHandlerCRYP          (void);
void  BSP_IntHandlerHASH_RNG      (void);

/*
*********************************************************************************************************
*                                     PERIPHERAL POWER/CLOCK SERVICES
*********************************************************************************************************
*/

void         BSP_PeriphEn                (CPU_DATA       pwr_clk_id);
void         BSP_PeriphDis               (CPU_DATA       pwr_clk_id);
void         BSP_PeriphRst               (CPU_DATA       pwr_clk_id);
void         BSP_PeriphLpEn              (CPU_DATA       pwr_clk_id);
void         BSP_PeriphLpDis             (CPU_DATA       pwr_clk_id);




/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
