#include <includes.h>
#include "Sensor\serial.h"
#define  MAX_RX_BUFFER    3

uint8_t lamp_status = 1;//
static uint8_t RX_BUFFER[MAX_RX_BUFFER];
static uint8_t rx_buffer_index = 0;
/*******************************************************************************
*   Fuc Name:	lamp_status
* Fuc descript: judge the lamp is or not normal.
* input para:   NULL
* output para:  NULL
* return para:  0 : alarm 1: normal
*******************************************************************************/
static uint8_t Lamp_status (void)
{
  if((RX_BUFFER[0] == 0xda)&&(RX_BUFFER[2] == 0x0d))
  {
    if(!RX_BUFFER[1])
      return 0;
  }
  return 1;
}
/*******************************************************************************
*   Fuc Name:	USART2_IRQhandle
* Fuc descript: USART2 receive interrupt.
* input para:   NULL
* output para:  NULL
* return para:  NULL
*******************************************************************************/
static void USART2_IRQhandle(void)
{
    
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)            //if USART2 interrupt reche.
    {
        //read one data from the usart2
            RX_BUFFER[rx_buffer_index] = USART_ReceiveData(USART2);
            if(RX_BUFFER[0] == 0xda)
            rx_buffer_index++;
            if(rx_buffer_index >= MAX_RX_BUFFER)
            {
                lamp_status = Lamp_status();
                rx_buffer_index = 0;
            }
        
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}
/*******************************************************************************
*   Fuc Name:	USART_GPIO_Configuration
* Fuc descript: initialize all the uart2 pd5-pd6 gpio pin.
* input para:   NULL
* output para:  NULL
* return para:  NULL
*******************************************************************************/
static void SERIAL_GPIO_CONFIGURATION(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //GPIO CLOCK CONFIGURATION.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    //USARTx_Tx PD5.
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    //USARTx_Rx PD6.
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
    //GPIO CONFIGURATION.
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}
/*******************************************************************************
*   Fuc Name:	USART_Configuration
* Fuc descript: initialize UART parameters.
* input para:   NULL
* output para:  NULL
* return para:  NULL
*******************************************************************************/
void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;
    //open the respone uart2 clock.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    /* ------------------------------------------------------------
          USART2:
          - 115200 baud
          - 8 Bits databit
          - 1 stopbit
          - no parity
          - no hard flow control
          - receive enable.
    --------------------------------------------------------------------------*/
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);
    // enable rxne interrupt.
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ClearITPendingBit(USART2, USART_IT_TC);//??3y?D??TC??
    USART_Cmd(USART2,ENABLE);//¡Á?o¨®¨º1?¨¹¡ä?¨²
    BSP_IntVectSet(BSP_INT_ID_USART2, USART2_IRQhandle);
    BSP_IntPrioSet(BSP_INT_ID_USART2,1);
    BSP_IntEn(BSP_INT_ID_USART2);
}
void serial_init(void)
{
  SERIAL_GPIO_CONFIGURATION();
  USART_Configuration();
}