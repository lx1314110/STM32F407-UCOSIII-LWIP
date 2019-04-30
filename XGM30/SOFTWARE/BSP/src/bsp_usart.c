/*****************************************************************************/
/* 文件名:    dev_usart.c                                                 */
/* 描  述:    串口相关处理                                                   */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "Debug/mtfs30_debug.h"
#include "Util/util_ringbuffer.h"
#include "bsp_usart.h"




/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
u8 g_gnss_usart_rx_buf[GNSS_USART_BUF_MAX_SIZE];   /* 卫星模块串口接收缓冲区 */
ringbuffer_t g_gnss_usart_rb;                      /* 卫星消息缓冲区         */
u8 g_debug_usart_rx_buf[DEBUG_USART_BUF_MAX_SIZE]; /* 调试串口接收缓冲区     */



/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/
extern OS_SEM     g_usart_debug_sem;         /* 串口调试用信号量   */
extern OS_SEM     g_gnss_idle_sem;           /* GNSS空闲中断信号量 */
extern u8_t       g_gnss_init_finished_flag; /* 卫星初始化完成标志 */
extern OS_Q       g_gnss_msgq;         /* 卫星信息消息队列   */

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void GNSS_USART_Config(void);
static void DEBUG_USART_Config(void);
static void DEBUG_USART_IRQHandler(void);

/*****************************************************************************
 * 函  数:    GNSS_USART_Config                                                          
 * 功  能:    配置卫星模块串口                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void GNSS_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    
    
    /*--------------------------------*/
    /* 时钟配置                       */
    /*--------------------------------*/
    
	/* 配置GPIO时钟 */
    RCC_AHB1PeriphClockCmd(GNSS_USART_RX_GPIO_CLK | GNSS_USART_TX_GPIO_CLK, ENABLE);
	/* 配置DMA时钟 */						   
	RCC_AHB1PeriphClockCmd(GNSS_USART_DMA_CLK, ENABLE);  						
	/* 配置USART时钟 */
	RCC_APB2PeriphClockCmd(GNSS_USART_CLK, ENABLE);	
    
    

    /*--------------------------------*/
    /* GPIO配置                       */
    /*--------------------------------*/    
 
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    
    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_Pin = GNSS_USART_TX_PIN  ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GNSS_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GNSS_USART_RX_PIN;
    GPIO_Init(GNSS_USART_RX_GPIO_PORT, &GPIO_InitStructure);
      
    /* Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(GNSS_USART_RX_GPIO_PORT, GNSS_USART_RX_SOURCE, GNSS_USART_RX_AF);

    /* Connect PXx to USARTx_Rx*/
    GPIO_PinAFConfig(GNSS_USART_TX_GPIO_PORT, GNSS_USART_TX_SOURCE, GNSS_USART_TX_AF); 
 
    
    /*--------------------------------*/
    /* NVIC配置                       */
    /*--------------------------------*/
	NVIC_InitStructure.NVIC_IRQChannel = GNSS_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);    
    
    
    /*--------------------------------*/
    /* USART模式配置                  */
    /*--------------------------------*/      
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(GNSS_USART, &USART_InitStructure);

    /* 使能USART */
    USART_Cmd(GNSS_USART, ENABLE);   
    
    /* 使能USART的DMA接收 */
    USART_DMACmd(GNSS_USART, USART_DMAReq_Rx, ENABLE);
    
    /* 设置USART中断服务函数 */
    BSP_IntVectSet(GNSS_USART_INT_ID, GNSS_USART_IRQHandler);
    BSP_IntEn(GNSS_USART_INT_ID);  
    
    /* 中断配置 */
    USART_ITConfig(GNSS_USART, USART_IT_RXNE, DISABLE);/*  不使能接收中断 */
    USART_ITConfig(GNSS_USART, USART_IT_IDLE, ENABLE); /*  使能IDLE中断   */  
    
    /*--------------------------------*/
    /* DMA模式配置                    */
    /*--------------------------------*/      
    /* 恢复默认配置 */
	DMA_DeInit(GNSS_DMA_Stream);
    /* 等待DMA可配置 */
	while (DMA_GetCmdStatus(GNSS_DMA_Stream) != DISABLE);
    
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;                          /* 通道选择               */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&GNSS_USART->DR;        /* DMA外设地址            */
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)g_gnss_usart_rx_buf;       /* DMA 存储器0地址        */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;                /* 外设到存储器模式       */
	DMA_InitStructure.DMA_BufferSize = GNSS_USART_BUF_MAX_SIZE;             /* 传输数据大小           */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        /* 不使能外设地址增量模式 */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 /* 内存地址为增量模式     */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; /* 外设数据长度:8位       */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         /* 存储器数据长度:8位     */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           /* 使用普通模式           */
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   /* 中等优先级             */
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  /* 禁用FIFO模式           */         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;           /* FIFO大小               */
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             /* 存储器突发单次传输     */
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     /* 外设突发单次传输       */
    /* 配置DMA */
	DMA_Init(GNSS_DMA_Stream, &DMA_InitStructure);
    
	DMA_Cmd(GNSS_DMA_Stream, ENABLE);
    
    
}



/*****************************************************************************
 * 函  数:    DEBUG_USART_Config                                                          
 * 功  能:    配置卫星模块串口                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void DEBUG_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    /*--------------------------------*/
    /* PeriphClock clock enable       */
    /*--------------------------------*/
    
    /* PeriphClock usart 1 pin, GPIO A9(TX) GPIO A10(RX)  clock enable*/
    RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_GPIO_CLK | DEBUG_USART_TX_GPIO_CLK, ENABLE);
    /* PeriphDMA2 (usart1) clock enable */						   
    RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_CLK, ENABLE);  						
    /* PeriphUSART1 clock enable */
    RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);	

    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    
    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
      
    /* Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT, DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);

    /* Connect PXx to USARTx_Rx*/
    GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_SOURCE, DEBUG_USART_TX_AF); 
 
    /* NVIC configuration                               */
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
    
    /* USART1 baud .databit .stopbit .parity. flowcontrol*/     
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(DEBUG_USART, &USART_InitStructure);

    /* enable USART1 */
    USART_Cmd(DEBUG_USART, ENABLE);   
    
    /* enable USART1 DMA2 chanel 5 as receive */
    USART_DMACmd(DEBUG_USART, USART_DMAReq_Rx, ENABLE);
    
    /* enable usart1 interrupte and irq fuction */
    BSP_IntVectSet(DEBUG_USART_INT_ID, DEBUG_USART_IRQHandler);
    BSP_IntEn(DEBUG_USART_INT_ID);  
    
    /* enable idle interrupte*/
    USART_ITConfig(DEBUG_USART, USART_IT_RXNE, DISABLE);/*  不使能接收中断 */
    USART_ITConfig(DEBUG_USART, USART_IT_IDLE, ENABLE); /*  使能IDLE中断   */  
    
    /* DMA default initalize                    */    
    DMA_DeInit(DEBUG_DMA_Stream);

    /* wait until DMA default initalize end  */
    while (DMA_GetCmdStatus(DEBUG_DMA_Stream) != DISABLE);

    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                          /* 通道选择               */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&DEBUG_USART->DR;       /* DMA外设地址            */
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)g_debug_usart_rx_buf;      /* DMA 存储器0地址        */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;                /* 外设到存储器模式       */
    DMA_InitStructure.DMA_BufferSize = DEBUG_USART_BUF_MAX_SIZE;            /* 传输数据大小           */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        /* 不使能外设地址增量模式 */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 /* 内存地址为增量模式     */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; /* 外设数据长度:8位       */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         /* 存储器数据长度:8位     */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           /* 使用普通模式           */
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   /* 中等优先级             */
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  /* 禁用FIFO模式           */         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;           /* FIFO大小               */
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             /* 存储器突发单次传输     */
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     /* 外设突发单次传输       */
    
    DMA_Init(DEBUG_DMA_Stream, &DMA_InitStructure);
    DMA_Cmd(DEBUG_DMA_Stream, ENABLE);
   
}


/*****************************************************************************
 * 函  数:    GNSS_USART_IRQHandler                                                          
 * 功  能:    串口1接收函数                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void GNSS_USART_IRQHandler(void)
{
       u16 num;
       OS_ERR err;

     /* check usart6 interrupt idle flag  */
      if (USART_GetITStatus(GNSS_USART, USART_IT_IDLE) != RESET)
      {

      
              /* 先读SR，然后读DR才能清除 */
              num = GNSS_USART->SR;		
              num = GNSS_USART->DR;	

              /* 关闭DMA,防止处理其间有数据 */
              DMA_Cmd(GNSS_DMA_Stream, DISABLE);								 

              /* 得到真正接收数据个数 */
              num = GNSS_USART_BUF_MAX_SIZE - DMA_GetCurrDataCounter(GNSS_DMA_Stream); 
              if (num >= GNSS_USART_BUF_MAX_SIZE)
              {
                  MTFS30_DEBUG("GNSS信息过长");
                  goto GNSS_DMA_SET;
              }
              
              g_gnss_usart_rx_buf[num++] = '\0';        
              /* 将收到的卫星信息放入到环形缓冲区 */
              if (num != 0)
              {
                  //MTFS30_DEBUG("num = %d, 卫星消息: %s\n", num, g_gnss_usart_rx_buf);
                  
                  OSQPost ((OS_Q*      ) &g_gnss_msgq,
                           (void*      ) g_gnss_usart_rx_buf,
                           (OS_MSG_SIZE) num,
                           (OS_OPT     ) OS_OPT_POST_FIFO,
                           (OS_ERR*    ) &err);
              }
        
        
GNSS_DMA_SET:
        
              /* 清除标志位 */
              DMA_ClearFlag(GNSS_DMA_Stream, GNSS_DMA_CLEAR_FLAG);
                      
              /* 正常传输模式需要每次都重新设置接收数据个数 */		
              DMA_SetCurrDataCounter(GNSS_DMA_Stream, GNSS_USART_BUF_MAX_SIZE);
              
              /* 开启DMA */
              DMA_Cmd(GNSS_DMA_Stream, ENABLE);
      }
}


/*****************************************************************************
 * 函  数:    DEBUG_USART_IRQHandler                                                          
 * 功  能:    串口1接收函数                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void DEBUG_USART_IRQHandler(void)
{
       u16 num;
       OS_ERR err;

      /* check usart1 interrupt idle flag  */
	if (USART_GetITStatus(DEBUG_USART, USART_IT_IDLE) != RESET)
	{
		/* 先读SR，然后读DR才能清除 */
		num = DEBUG_USART->SR;		
		num = DEBUG_USART->DR;	

		/* 关闭DMA,防止处理其间有数据 */
		DMA_Cmd(DEBUG_DMA_Stream, DISABLE);								 

		/* 得到真正接收数据个数 */
		num = DEBUG_USART_BUF_MAX_SIZE - DMA_GetCurrDataCounter(DEBUG_DMA_Stream); 
                if ((num >= DEBUG_USART_BUF_MAX_SIZE) || num <= 0)
                {
                    goto DEBUG_DMA_SET;
                }
                
                g_debug_usart_rx_buf[num] = '\0';
        
                /* post调试信号量 */
                OSSemPost ((OS_SEM* ) &g_usart_debug_sem,
                           (OS_OPT  ) OS_OPT_POST_1,
                           (OS_ERR *) &err);
        
 DEBUG_DMA_SET:
                
                /* 清除标志位 */
                DMA_ClearFlag(DEBUG_DMA_Stream, DEBUG_DMA_CLEAR_FLAG);
                        
                /* 正常传输模式需要每次都重新设置接收数据个数 */		
                DMA_SetCurrDataCounter(DEBUG_DMA_Stream, DEBUG_USART_BUF_MAX_SIZE);
                
                /* 开启DMA */
                DMA_Cmd(DEBUG_DMA_Stream, ENABLE);
         }
	

}

/*****************************************************************************
 * 函  数:    GNSS_USART_Send                                                          
 * 功  能:    卫星模块串口发送函数                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void GNSS_USART_Send(u8_t *pdata)
{
	uint16_t i;
	u8_t buf[GNSS_USART_SEND_LEN_MAX]; /* 用于存放格式化成串口数据格式的数据 */
	u8_t len = 0;                      /* 函数输入的字符串长度               */


    snprintf((char *)buf, GNSS_USART_SEND_LEN_MAX - 3, "%s", pdata);
    
    /* 格式化为串口消息格式 */
    len = strlen((const char *)buf);
    buf[len] = '\r';
    buf[len + 1] = '\n';
    buf[len + 2] = '\0';
  
    /* 发送数据 */
    len = len + 2;
	for (i = 0; i < len; i++)
	{	
        /* 等待数据发送发送完成 */
		while(USART_GetFlagStatus(GNSS_USART, USART_FLAG_TC) == RESET);
		/* 发送数据 */
		USART_SendData(GNSS_USART, (uint16_t)buf[i]);
		
	}
    
}


/*****************************************************************************
 * 函  数:    DEBUG_USART_Send                                                          
 * 功  能:    调试串口发送函数                                                                  
 * 输  入:    pdata: 指向要发送的数据
 *            len  : 发送的数据长度  
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void DEBUG_USART_Send(u8_t *pdata, u16_t len)
{
    u16_t i = 0;
    
    
    /* 发送数据 */
	for (i = 0; i < len; i++)
	{
		/* 等待数据发送发送完成 */
		while(USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TC) == RESET);        
		/* 发送数据 */
		USART_SendData(DEBUG_USART, (uint16_t)pdata[i]);

	}
    
}



/*****************************************************************************
 * 函  数:    BSP_InitUsart                                                          
 * 功  能:    串口初始化                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void BSP_InitUsart(void)
{

    /* 卫星模块串口初始化 */
    GNSS_USART_Config();
    
    /* 调试串口初始化 */
    DEBUG_USART_Config();
    
    
//    /* 创建GNSS串口消息缓冲 */
    util_ringbuffer_create(&g_gnss_usart_rb, GNSS_USART_BUF_MAX_SIZE * 2);    
}










