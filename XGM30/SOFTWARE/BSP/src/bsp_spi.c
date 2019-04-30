/*****************************************************************************/
/* 文件名:    bsp_spi.c                                                      */
/* 描  述:    SPI相关处理                                                    */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "BSP/inc/bsp_spi.h"
#include "Debug/mtfs30_debug.h"
#include "mtfs30.h"



/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT; /* 超时时间 */  

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void SPI_FPGA_Init(void);
static u8 SPI_FPGA_SendByte(u8 send_byte, u8 *recv_byte);
static u8 SPI_FPGA_ReadByte(u8 *recv_byte);


/*****************************************************************************
 * 函  数:    SPI_Init                                                          
 * 功  能:    SPI初始化处理                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-11-02 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void BSP_SPI_Init(void)
{
    /* FPGA SPI初始化 */
    SPI_FPGA_Init();
}

/*****************************************************************************
 * 函  数:    FPGA_SPI_Init                                                          
 * 功  能:    FPGA SPI初始化处理                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-11-02 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void SPI_FPGA_Init(void)
{

    SPI_InitTypeDef  FPGA_SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    
    /* 使能GPIO时钟 */
    RCC_AHB1PeriphClockCmd (FPGA_SPI_SCK_GPIO_CLK  | 
                            FPGA_SPI_MISO_GPIO_CLK |
                            FPGA_SPI_MOSI_GPIO_CLK | 
                            FPGA_SPI_CS_GPIO_CLK, ENABLE);
    
    /* 使能SPI时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    
    /* 引脚复用 */
    GPIO_PinAFConfig(FPGA_SPI_SCK_GPIO_PORT, FPGA_SPI_SCK_PINSOURCE, FPGA_SPI_SCK_AF); 
	GPIO_PinAFConfig(FPGA_SPI_MISO_GPIO_PORT, FPGA_SPI_MISO_PINSOURCE, FPGA_SPI_MISO_AF); 
	GPIO_PinAFConfig(FPGA_SPI_MOSI_GPIO_PORT, FPGA_SPI_MOSI_PINSOURCE, FPGA_SPI_MOSI_AF); 
    
    
    /* 配置SCK引脚 */
    GPIO_InitStructure.GPIO_Pin = FPGA_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  
    GPIO_Init(FPGA_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/* 配置MISO引脚 */
    GPIO_InitStructure.GPIO_Pin = FPGA_SPI_MISO_PIN;
    GPIO_Init(FPGA_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/* 配置MOSI引脚 */
    GPIO_InitStructure.GPIO_Pin = FPGA_SPI_MOSI_PIN;
    GPIO_Init(FPGA_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/* 配置CS引脚 */
    GPIO_InitStructure.GPIO_Pin = FPGA_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(FPGA_SPI_CS_GPIO_PORT, &GPIO_InitStructure);
    SPI_FPGA_CS_HIGH(); /* 停止信号: CS引脚高电平*/    
    
    /* FPGA SPI模式配置:  模式3*/
    FPGA_SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    FPGA_SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    FPGA_SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    FPGA_SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    FPGA_SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    FPGA_SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    FPGA_SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    FPGA_SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    FPGA_SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(FPGA_SPI, &FPGA_SPI_InitStructure);

   /* 使能 FPGA SPI  */
   SPI_Cmd(FPGA_SPI, ENABLE);
    
   
}

/*****************************************************************************
 * 函  数:    SPI_FPGA_SendByte                                                          
 * 功  能:    使用SPI发送一个字节的数据                                                                  
 * 输  入:    send_byte: 要发送得数据                          
 * 输  出:    无                                                    
 * 返回值:    OK: 发送成功; NG: 发送失败                                                    
 * 创  建:    2018-11-02 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8 SPI_FPGA_SendByte(u8 send_byte, u8 *recv_byte)
{
    SPITimeout = SPIT_FLAG_TIMEOUT;

    /* 等待发送缓冲区为空，TXE事件 */
    while (SPI_I2S_GetFlagStatus(FPGA_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
        /* 等待超时 */
        if((SPITimeout--) == 0) 
        {
              MTFS30_ERROR("SPI 发送等待超时!");
              return NG;
        }
    }

    /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
    //MTFS30_DEBUG("发送字节: %#x", send_byte);
    SPI_I2S_SendData(FPGA_SPI, send_byte);

    
    SPITimeout = SPIT_FLAG_TIMEOUT;
    /* 等待接收缓冲区非空，RXNE事件 */
    while (SPI_I2S_GetFlagStatus(FPGA_SPI, SPI_I2S_FLAG_RXNE) == RESET)
    {
        /* 等待超时 */
        if((SPITimeout--) == 0) 
        {
              MTFS30_ERROR("SPI 接收等待超时!");
              return NG;
        }
    }


    /* 读取数据寄存器，获取接收缓冲区数据 */
    *recv_byte = SPI_I2S_ReceiveData(FPGA_SPI);
    //MTFS30_DEBUG("SPI 接收数据: %#x", *recv_byte);
    
    return OK;
}

/*****************************************************************************
 * 函  数:    SPI_FPGA_ReadByte                                                          
 * 功  能:    使用SPI读取一个字节的数据                                                                  
 * 输  入:    无                          
 * 输  出:    recv_byte： 读取到的1字节数据                                                    
 * 返回值:    OK: 读取成功; NG:读取失败                                                   
 * 创  建:    2018-11-02 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8 SPI_FPGA_ReadByte(u8 *recv_byte)
{ 
  
  return SPI_FPGA_SendByte(Dummy_Byte, recv_byte);   
}


/*****************************************************************************
 * 函  数:    SPI_FPGA_ByteWrite                                                          
 * 功  能:    通过SPI进行单字写操作                                                                  
 * 输  入:    write_byte: 要写入的数据    
 *            write_addr: 要写入的地址
 * 输  出:    无                                                    
 * 返回值:    OK: 写入成功; NG: 写入失败                                                    
 * 创  建:    2018-11-02 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8 SPI_FPGA_ByteWrite(u8 write_byte, u32 write_addr)
{
    u8 ret = OK;
    
    
    /* 选择FPGA: CS低电平 */
    SPI_FPGA_CS_LOW();
  
    /* 发送写命令*/
    if (NG == SPI_FPGA_SendByte(FPGA_WRITE_BYTE, NULL)) 
    {
        MTFS30_DEBUG("SPI_FPGA_SendByte ERROR! (%#x)", FPGA_WRITE_BYTE);
        SPI_FPGA_CS_HIGH();
        return NG;
    }

    /*发送写入的地址*/
    ret |= SPI_FPGA_SendByte(((write_addr & 0xFF000000) >> 24), NULL); 
    ret |= SPI_FPGA_SendByte(((write_addr & 0xFF0000) >> 16), NULL);
    ret |= SPI_FPGA_SendByte(((write_addr & 0xFF00) >> 8), NULL);
    ret |= SPI_FPGA_SendByte((write_addr & 0xFF), NULL);
    
    if (ret != OK)
    {
        MTFS30_DEBUG("SPI_FPGA_SendByte ERROR! (%#x)", write_addr);
        SPI_FPGA_CS_HIGH();
        return NG;
    }



    /* 发送要写入的字节数据 */
    if (NG == SPI_FPGA_SendByte(write_byte, NULL))
    {
        MTFS30_DEBUG("SPI_FPGA_SendByte ERROR! (%c)", write_byte);
        SPI_FPGA_CS_HIGH();
        return NG;
    }


    /* 停止信号 FPGA: CS 高电平 */
    SPI_FPGA_CS_HIGH();

    return OK;
}

/*****************************************************************************
 * 函  数:    SPI_FPGA_ByteRead                                                          
 * 功  能:    通过SPI进行单字读操作                                                                  
 * 输  入:    p_byte: 指向读出的数据    
 *            read_addr: 要写入的地址
 * 输  出:    无                                                    
 * 返回值:    OK: 读取成功; NG:读取失败                                                    
 * 创  建:    2018-11-02 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8 SPI_FPGA_ByteRead(u8 *p_byte, u32 read_addr)
{
    
    u8 ret = OK;
    
    
    /* 选择FPGA: CS低电平 */
    SPI_FPGA_CS_LOW();

    /* 发送读指令 */
    if (NG == SPI_FPGA_SendByte(FPGA_READ_BYTE, NULL))
    {
        MTFS30_DEBUG("SPI_FPGA_SendByte ERROR! (%#x)", FPGA_READ_BYTE);
        SPI_FPGA_CS_HIGH();
        return NG;
    }

    /* 发送读地址 */
    ret |= SPI_FPGA_SendByte(((read_addr & 0xFF000000) >> 24), NULL); 
    ret |= SPI_FPGA_SendByte(((read_addr & 0xFF0000) >> 16), NULL);
    ret |= SPI_FPGA_SendByte(((read_addr& 0xFF00) >> 8), NULL);
    ret |= SPI_FPGA_SendByte((read_addr & 0xFF), NULL);
    
    if (ret != OK) 
    {
        MTFS30_DEBUG("SPI_FPGA_SendByte ERROR! (%#x)", read_addr);
        SPI_FPGA_CS_HIGH();
        return NG;        
    }

    /* 读取一个字节*/
    //MTFS30_DEBUG("SPI_FPGA_ReadByte Start!");
    if (NG == SPI_FPGA_ReadByte(p_byte))
    {
        MTFS30_DEBUG("SPI_FPGA_ReadByte ERROR!");
        SPI_FPGA_CS_HIGH();
        return NG;
    }

    /* 停止信号 FPGA: CS 高电平 */
    SPI_FPGA_CS_HIGH();

    return OK;
}

/*****************************************************************************
 * 函  数:    SPI_FPGA_BufferWrite                                                          
 * 功  能:    通过SPI进行连续写操作                                                                  
 * 输  入:    pbuf: 要写入的数据    
 *            write_addr: 要写入的地址
 *            write_size: 要写入的数据长度
 * 输  出:    无                                                    
 * 返回值:    OK: 写入成功; NG: 写入失败                                                    
 * 创  建:    2018-11-02 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8 SPI_FPGA_BufferWrite(u8 *pbuf, u32 write_addr, u8 write_size)
{
    
    u8 ret = OK;
    
    
    /* 选择FPGA: CS低电平 */
    SPI_FPGA_CS_LOW();
  
    /* 发送写命令*/
    if (NG == SPI_FPGA_SendByte(FPGA_WRITE_MULBYTE, NULL))
    { 
        MTFS30_DEBUG("SPI_FPGA_ReadByte ERROR! (%#x)", FPGA_WRITE_MULBYTE);
        SPI_FPGA_CS_HIGH();
        return NG;
    }

    /*发送写入的地址*/
    ret |= SPI_FPGA_SendByte(((write_addr & 0xFF000000) >> 24), NULL);
    ret |= SPI_FPGA_SendByte(((write_addr & 0xFF0000) >> 16), NULL);
    ret |= SPI_FPGA_SendByte(((write_addr & 0xFF00) >> 8), NULL);
    ret |= SPI_FPGA_SendByte((write_addr & 0xFF), NULL);
 
    if (ret != OK) 
    {
        MTFS30_DEBUG("SPI_FPGA_SendByte ERROR! (%#x)", write_addr);
        SPI_FPGA_CS_HIGH();
        return NG;        
    }    
    
    /* 发送写入的数据长度 */
    if (NG == SPI_FPGA_SendByte(write_size, NULL))
    {
        MTFS30_DEBUG("SPI_FPGA_SendByte ERROR! (%d)", write_size);
        SPI_FPGA_CS_HIGH();
        return NG;          
    }


    ret = OK;
    /* 发送要写入的字节数据 */
    while(write_size--)
    {
        ret |= SPI_FPGA_SendByte(*pbuf++, NULL);
    }
    
    if (ret != OK)
    {
        MTFS30_DEBUG("写入FPGA的数据有误");
        SPI_FPGA_CS_HIGH();
        return NG;
    }
    

    
    /* 停止信号 FPGA: CS 高电平 */
    SPI_FPGA_CS_HIGH();
    
    return OK;
}

/*****************************************************************************
 * 函  数:    SPI_FPGA_BufferRead                                                          
 * 功  能:    通过SPI进行连续读操作                                                                  
 * 输  入:    pbuf: 指向读出的数据    
 *            read_addr: 要读的地址
 *            read_size: 要读的数据长度
 * 输  出:    无                                                    
 * 返回值:    OK:连续读成功; NG：连续读失败                                                    
 * 创  建:    2018-11-02 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8 SPI_FPGA_BufferRead(u8 *pbuf, u32 read_addr, u8 read_size)
{
    
    u8 ret = OK;
    
    /* 选择FPGA: CS低电平 */
    SPI_FPGA_CS_LOW();
  
    /* 发送写命令*/
    if (NG == SPI_FPGA_SendByte(FPGA_READ_MULBYTE, NULL))
    {
        MTFS30_DEBUG("SPI_FPGA_ReadByte ERROR! (%#x)", FPGA_READ_MULBYTE);
        SPI_FPGA_CS_HIGH();
        return NG;        
    }

    /*发送写入的地址*/
    ret |= SPI_FPGA_SendByte(((read_addr & 0xFF000000) >> 24), NULL);
    ret |= SPI_FPGA_SendByte(((read_addr & 0xFF0000) >> 16), NULL);
    ret |= SPI_FPGA_SendByte(((read_addr & 0xFF00) >> 8), NULL);
    ret |= SPI_FPGA_SendByte((read_addr & 0xFF), NULL);

    if (ret != OK) 
    {
        MTFS30_DEBUG("SPI_FPGA_SendByte ERROR! (%#x)", read_addr);
        SPI_FPGA_CS_HIGH();
        return NG;        
    } 
    
    /* 发送写入的数据长度 */
    if (NG == SPI_FPGA_SendByte(read_size, NULL))
    {
        MTFS30_DEBUG("SPI_FPGA_SendByte ERROR! (%d)", read_size);
        SPI_FPGA_CS_HIGH();
        return NG;     
    }


    ret = OK;
    /* 接收数据 */
    while(read_size--)
    {
        
        ret |= SPI_FPGA_ReadByte(pbuf++);   
    }

    if (ret != OK)
    {
        MTFS30_DEBUG("从FPGA读取的数据有误");
        SPI_FPGA_CS_HIGH();
        return NG;                
    }   
    
    /* 停止信号 FPGA: CS 高电平 */
    SPI_FPGA_CS_HIGH();

    return OK;
}
