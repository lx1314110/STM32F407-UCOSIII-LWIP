/*****************************************************************************/
/* 文件名:    fpga_handler.c                                                 */
/* 描  述:    FPGA读写处理                                                   */
/* 创  建:    2018-11-05 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "cc.h"
#include "Debug/user_cmd.h"
#include "Fpga/fpga_handler.h"

#include "mtfs30.h"

#include "Debug/mtfs30_debug.h"
#include "Util/util.h"
#include "BSP/inc/bsp_spi.h"
/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static u8_t fpga_write_data_parser(u8_t *p_param, u8_t *p_data, u8_t *p_size);

/*****************************************************************************
 * 函  数:    fpga_write_cmd_handler                                                           
 * 功  能:    向FPGA中写入数据                                                              
 * 输  入:    p_param: 命令参数                        
 * 输  出:    无                                                 
 * 返回值:    无                                               
 * 创  建:    2018-11-05 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 

u8_t fpga_write_cmd_handler(int num,...)
{
   u8_t *p_param[MAX_PARAM_NUM] = {NULL};
   u32_t write_addr = 0;
   u8_t  write_size = 0;
   u8_t *p_end = NULL;
   u8_t  err_flag = OK;
   u8_t  inx = 0;
   va_list valist;
    
   va_start(valist, num);
   
   for(inx = 0; inx < num; inx++)
   {
     p_param[inx] = (u8_t *)va_arg(valist, char *);
     
     if(p_param[inx] == NULL)
       err_flag = NG;
   }
   
   va_end(valist);
   
   if(err_flag == OK)
   {
      write_addr = (u32_t)strtoul((char const *)p_param[0], (char **)&p_end, 16);
      write_size = (u8_t)atoi((char const *)p_param[1]);
      
      if (write_size == 1) /* 写入1个字节 */
      {
          u8_t write_byte = 0;
          
          /* 将16进制字符串转换为无符号整数 */
          write_byte = (u8_t)strtoul((char const *)p_param[2], (char **)&p_end, 16);
  
          
          /* 通过SPI进行单字写操作 */
          if (NG == SPI_FPGA_ByteWrite(write_byte, write_addr))
          {
              MTFS30_DEBUG("SPI_FPGA_ByteWrite failed! 写地址(%#x), 写内容(%c)", write_addr, *p_param[2]);
              err_flag = NG;
              goto return_flag;
          }
      
      } 
      else /* 写入多个字节 */
      {
          u8_t write_buf[FPGA_RW_MAX_SIZE]; /* 存放转换后的数据 */    
          u8 write_cnt = 0;                 /* 实际写入字节数   */
          
          /* 将16进制字符串转换为无符号整数 */
          if (NG == fpga_write_data_parser(p_param[2], write_buf, &write_cnt))
          {
              MTFS30_DEBUG("fpga_write_data_parser ERROR! 写的内容为(%s)", p_param[2]);
              err_flag = NG;
              goto return_flag;
          }
          
          
          /* 写内容字节数超过指定字节数 */
          if (write_cnt > write_size)
          {
              write_cnt = write_size;
              MTFS30_TIPS("实际写入字节数为(%d)", write_cnt);
          }
        
          
          /* 通过SPI进行连续写操作 */
          if (NG == SPI_FPGA_BufferWrite(write_buf, write_addr, write_cnt))
          {
              MTFS30_DEBUG("SPI_FPGA_BufferWrite failed! 写地址(%#x), 写长度(%d), 写内容(%s)", write_addr, write_size, p_param[2]);
              err_flag = NG;
          }
      } 
   }
   
return_flag:    
    return err_flag;
}
/*****************************************************************************
 * 函  数:    fpga_read_cmd_handler                                                           
 * 功  能:    从FPGA中读出数据                                                               
 * 输  入:    p_param: 命令参数                        
 * 输  出:    p_rtrv : 指向读出的数据                                                  
 * 返回值:    无                                               
 * 创  建:    2018-11-05 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
u8_t fpga_read_cmd_handler(int num,...)
{
   u8_t *p_param[MAX_PARAM_NUM] = {NULL};
   u32_t read_addr = 0;
   u8_t  read_size = 0;
   u8_t *p_end = NULL;
   u8_t  err_flag = OK;
   u8_t  inx = 0;
   va_list valist;
    
   va_start(valist, num);
   
   for(inx = 0; inx < num; inx++)
   {
     p_param[inx] = (u8_t *)va_arg(valist, char *);
     
     if(p_param[inx] == NULL)
       err_flag = NG;
   }
   
  
   
   va_end(valist);
  
    
   if(err_flag == OK)
   {
      /* 取得读地址 */
      read_addr = (u32_t)strtoul((char const *)p_param[0], (char **)&p_end, 16);
  
      
      /* 取得读长度 */
      read_size = (u8_t)atoi((char const *)p_param[1]);
          
      
      /* 读FPGA */
      if (read_size == 1) /* 读取1个字节 */
      {
          
          /* 通过SPI进行单字读操作 */
          if (NG == SPI_FPGA_ByteRead(p_param[2], read_addr))
          {
              MTFS30_DEBUG("SPI_FPGA_ByteRead failed! 读地址(%#x), 读内容(%c)", read_addr, *p_param[2]);
              err_flag = NG;
              goto return_flag;
          }
      
          /* 读结果输出 */
          MTFS30_TIPS("读出的数据: %#x", *p_param[2]);
          
      } 
      else /* 读取多个字节 */
      {
          /* 通过SPI进行连续读操作 */
          if (NG == SPI_FPGA_BufferRead(p_param[2], read_addr, read_size))
          {
              MTFS30_DEBUG("SPI_FPGA_BufferRead failed! 读地址(%#x), 读长度(%d), 读内容(%s)", read_addr, read_size, p_param[2]);
              err_flag = NG;
              goto return_flag;
          }
          *(p_param[2]+read_size) = '\0';
          
          /* 读结果输出 */
          u8_t i = 0;
          
          MTFS30_TIPS("读出的数据:");
          for (i = 0; i < read_size; i++)
          {
              
              MTFS30_TIPS("第%d个字节: %#x", i+1, p_param[2]);
          }
          
      } 
   }
    
return_flag:    
    return err_flag;
}

/*****************************************************************************
 * 函  数:    fpga_write_data_parser                                                           
 * 功  能:    解析转换用户要写入FPGA的数据                                                               
 * 输  入:    p_param: 指向要写入的数据                        
 * 输  出:    p_data : 指向解析转换后的数据 
 *            p_size : 解析的字节数
 * 返回值:    OK：解析转换成功; NG：解析转换失败                                               
 * 创  建:    2018-11-05 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
static u8_t fpga_write_data_parser(u8_t *p_param, u8_t *p_data, u8_t *p_size)
{
    u8_t i = 0;
    u8_t param_len = 0;     /* 保存每个参数的长度   */
    u8_t param_buf[4]= {0}; /* 保存每个参数         */
    u8_t cnt = 0;           /* 已解析的字节数       */
    u8_t *p_end = NULL;
  
    
    if (p_param == NULL || p_data == NULL || p_size == NULL)
    {
        MTFS30_DEBUG("fpga_write_data_parser()参数错误");
        return NG;
    }
    
    
    cnt = 0;

    /* 解析每个参数 */
    while(p_param[i])
    {
        
        /* 忽略空格 */
        if (p_param[i] == ' ')
        {
            continue;
        }
        
        /* 字节分隔符 */
        if (p_param[i] == ',' || p_param[i] == '.')
        {
            param_buf[param_len] = '\0';
            p_data[cnt] = (u8_t)strtoul((char const *)param_buf, (char **)&p_end, 16);

            
            cnt++;
            param_len = 0;    
        }
        else 
        {
            /* 一个字节最多表示3位数,超过3位报错 */
            if (param_len >= 3) 
            {
                MTFS30_DEBUG("写入数据超过一个字节表示的最大值" );
                return NG;
            }
            
            param_buf[param_len++] = p_param[i];
        }
       
        i++;
    }  
    
    /* 最后1个逗号后面的数据 */
    p_data[cnt] = (u8_t)strtoul((char const *)param_buf, (char **)&p_end, 16);

    cnt++;    
    *p_size = cnt;
    
    return OK;
}


