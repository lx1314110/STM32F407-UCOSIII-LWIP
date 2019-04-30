/*****************************************************************************/
/* 文件名:    bsp_spi.h                                                      */
/* 描  述:    SPI相关处理头文件                                              */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __BSP_SPI_H_
#define __BSP_SPI_H_
#include "arch/cc.h"
#include "stm32f4xx.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define FPGA_SPI                           SPI1
#define FPGA_SPI_CLK                       RCC_APB2Periph_SPI1
//#define FPGA_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define FPGA_SPI_SCK_PIN                   GPIO_Pin_5                 
#define FPGA_SPI_SCK_GPIO_PORT             GPIOA                       
#define FPGA_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define FPGA_SPI_SCK_PINSOURCE             GPIO_PinSource5
#define FPGA_SPI_SCK_AF                    GPIO_AF_SPI1

#define FPGA_SPI_MISO_PIN                  GPIO_Pin_6               
#define FPGA_SPI_MISO_GPIO_PORT            GPIOA                   
#define FPGA_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define FPGA_SPI_MISO_PINSOURCE            GPIO_PinSource6
#define FPGA_SPI_MISO_AF                   GPIO_AF_SPI1

#define FPGA_SPI_MOSI_PIN                  GPIO_Pin_5                
#define FPGA_SPI_MOSI_GPIO_PORT            GPIOB                     
#define FPGA_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define FPGA_SPI_MOSI_PINSOURCE            GPIO_PinSource5
#define FPGA_SPI_MOSI_AF                   GPIO_AF_SPI1

#define FPGA_SPI_CS_PIN                    GPIO_Pin_4               
#define FPGA_SPI_CS_GPIO_PORT              GPIOA                     
#define FPGA_SPI_CS_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define SPI_FPGA_CS_LOW()      {FPGA_SPI_CS_GPIO_PORT->BSRRH=FPGA_SPI_CS_PIN;} /* FPGA: CS低电平 */
#define SPI_FPGA_CS_HIGH()     {FPGA_SPI_CS_GPIO_PORT->BSRRL=FPGA_SPI_CS_PIN;} /* FPGA: CS高电平 */

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))


/* FPGA命令定义 */
#define FPGA_WRITE_BYTE		      0x07  /* 单字写 */
#define FPGA_READ_BYTE            0x03  /* 单字读 */
#define FPGA_WRITE_MULBYTE        0x02  /* 连续写 */
#define FPGA_READ_MULBYTE         0x0B  /* 连续读 */



//#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte                0xFF  /* 空字节 */


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void BSP_SPI_Init(void);
extern u8 SPI_FPGA_ByteWrite(u8 byte, u32 write_addr);
extern u8 SPI_FPGA_ByteRead(u8 *pbyte, u32 read_addr);
extern u8 SPI_FPGA_BufferWrite(u8 *pbuf, u32 write_addr, u8 write_size);
extern u8 SPI_FPGA_BufferRead(u8 *pbuf, u32 read_addr, u8 read_size);
#endif
