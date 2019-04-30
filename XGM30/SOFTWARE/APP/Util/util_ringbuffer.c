/*****************************************************************************/
/* 文件名:    util_ringbuffer.c                                              */
/* 描  述:    环形缓冲区相关处理                                             */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "Util/util_ringbuffer.h"
#include "arch/cc.h"
#include "Debug/mtfs30_debug.h"





/*****************************************************************************
 * 函  数:    util_ringbuffer_create                                                          
 * 功  能:    创建环形缓冲区                                                                
 * 输  入:    rb_size :  环形缓冲区大小              
 * 输  出:    *rb : 指向创建的环形缓冲区                                                    
 * 返回值:                                                        
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void util_ringbuffer_create(ringbuffer_t *rb, u16_t rb_size)
{
 
    /* 为环形缓冲区分配空间 */
    rb->prb_buf = (u8_t *)malloc(rb_size);
    if (rb->prb_buf == NULL)
    {     
        MTFS30_DEBUG("为环形缓冲区分配空间失败！");
        return;
    }
   
    /* 初始化 */
    rb->rb_write = 0;
    rb->rb_read = 0;
    rb->rb_size = rb_size ;   

    MTFS30_DEBUG("创建环形队列成功！！！");     
      
}


/*****************************************************************************
 * 函  数:    util_ringbuffer_can_read                                                          
 * 功  能:    获取环形缓冲区可读的空间大小                                                               
 * 输  入:    *rb : 指向创建的环形缓冲区               
 * 输  出:                                                       
 * 返回值:    环形缓冲区可读的空间大小                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u16_t util_ringbuffer_can_read(ringbuffer_t *rb)
{

    if (rb->rb_write == rb->rb_read)
    {
        return 0;
    } 
    else if (rb->rb_write > rb->rb_read)
    {
        return (rb->rb_write - rb->rb_read);
    }
    else
    {
        return (rb->rb_size - (rb->rb_read - rb->rb_write));
    }
    
    
}

/*****************************************************************************
 * 函  数:    util_ringbuffer_can_write                                                          
 * 功  能:    获取环形缓冲区可写的空间大小                                                               
 * 输  入:    *rb : 指向创建的环形缓冲区               
 * 输  出:                                                       
 * 返回值:    环形缓冲区可写的空间大小                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u16_t util_ringbuffer_can_write(ringbuffer_t *rb)
{

    return  (rb->rb_size - util_ringbuffer_can_read(rb));
}


/*****************************************************************************
 * 函  数:    util_ringbuffer_read                                                          
 * 功  能:    从环形缓冲区中读取指定大小的数据                                                              
 * 输  入:    *rb        : 指向创建的环形缓冲区  
 *            *data      : 读目标地址
 *            read_size  : 指定读的大小
 * 输  出:                                                       
 * 返回值:    实际读出的大小                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u16_t util_ringbuffer_read(ringbuffer_t *rb, u8_t *data, u16_t read_size)
{
 

    u16_t can_read = 0;
    u16_t tail_size = 0;
    
    
    
    /* 取得可读大小 */
    can_read = util_ringbuffer_can_read(rb);
//    MTFS30_DEBUG("读index: %d, 写index: %d, 读大小: %d, 可读大小: %d, 可写大小:%d\n", 
//                 rb->rb_read, rb->rb_write, read_size, can_read, util_ringbuffer_can_write(rb));     
    
    /* 无可读的数据，直接返回 */
    if (can_read == 0)
    {
        return 0;
    }
    
    /* 实际读大小 */
    read_size = min(read_size, can_read);
    
    
    /* 如果实际读的大小小于【读index到缓冲区末尾】的空间大小，则直接读;
     *  否则，分两段读，先读【读index到缓冲区末尾】的空间大小的数据，然后
     *  再从缓冲区开头接着读剩余的数据
     */ 
    tail_size = rb->rb_size - rb->rb_read;
    if (read_size < tail_size)
    {
        memcpy(data, rb->prb_buf+rb->rb_read, read_size);
        rb->rb_read = rb->rb_read + read_size;
    }
    else
    {
        memcpy(data, rb->prb_buf+rb->rb_read, tail_size);
        memcpy(data+tail_size, rb->prb_buf, read_size-tail_size);
        rb->rb_read =  (rb->rb_read + read_size) % rb->rb_size;
    }
    
    
//    MTFS30_DEBUG("新读index: %d, 写index: %d, 读大小: %d, 可读大小: %d, 可写大小:%d\n", 
//                 rb->rb_read, rb->rb_write, read_size, util_ringbuffer_can_read(rb), util_ringbuffer_can_write(rb));  
    
    return read_size; /* 返回实际读的大小 */

}

/*****************************************************************************
 * 函  数:    util_ringbuffer_end_ch_read                                                          
 * 功  能:    从环形缓冲区中读取到指定字符结束                                                             
 * 输  入:    *rb        : 指向创建的环形缓冲区  
 *            *data      : 读目标地址
 *            read_size : 指定读的大小
 *            *byte      : 结束字符
 * 输  出:                                                       
 * 返回值:    读出的大小                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u16_t util_ringbuffer_end_ch_read(ringbuffer_t *rb, u8_t *data, u16_t read_size, u8_t byte)
{
    u16_t can_read = 0;
    u16_t read_cnt = 0;
    
    /* 取得可读大小 */
    can_read = util_ringbuffer_can_read(rb);

//    MTFS30_DEBUG("读index: %d, 写index: %d, 读大小: %d, 可读大小: %d, 可写大小:%d\n", 
//                 rb->rb_read, rb->rb_write, read_size, can_read, util_ringbuffer_can_write(rb));     
    /* 无可读的数据，直接返回 */
    if (can_read == 0)
    {
        data[0]='\0';
        return 0;
    }    
    
    
    read_size = min(read_size, can_read);
    
    while(read_cnt < read_size)
    {
            if (rb->prb_buf[rb->rb_read] == byte)
            {
                data[read_cnt] = rb->prb_buf[rb->rb_read];
                read_cnt++;
                rb->rb_read++;
                
                /* 达到缓冲区末尾，接着从缓冲区开头计数 */
                if (rb->rb_read == rb->rb_size) {
                    rb->rb_read = 0;
                }                  
//    MTFS30_DEBUG("新读index: %d, 写index: %d, 读大小: %d, 可读大小: %d, 可写大小:%d\n", 
//                 rb->rb_read, rb->rb_write, read_size, util_ringbuffer_can_read(rb), util_ringbuffer_can_write(rb));                
                //return read_cnt;
                break;
            }
            
            
            data[read_cnt] = rb->prb_buf[rb->rb_read];
            read_cnt++; 
            
            
            rb->rb_read++;
            /* 达到缓冲区末尾，接着从缓冲区开头计数 */
            if (rb->rb_read == rb->rb_size) {
                rb->rb_read = 0;
            }        
    }

//    MTFS30_DEBUG("新读index: %d, 写index: %d, 读大小: %d, 可读大小: %d, 可写大小:%d\n", 
//                 rb->rb_read, rb->rb_write, read_size, util_ringbuffer_can_read(rb), util_ringbuffer_can_write(rb));    
    /* 没有找到指定字符 */    
    return read_cnt;

}

/*****************************************************************************
 * 函  数:    util_ringbuffer_write                                                         
 * 功  能:    向环形缓冲区中写入指定大小的数据                                                               
 * 输  入:    *rb         : 指向创建的环形缓冲区  
 *            *data       : 写数据地址
 *            *write_size : 指定写的大小
 * 输  出:                                                       
 * 返回值:    实际写大小                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u16_t util_ringbuffer_write(ringbuffer_t *rb, u8_t *data, u16_t write_size)
{
   
    u16_t can_write = 0;
    u16_t tail_size = 0;

    /* 取得可写空间大小 */
    can_write = util_ringbuffer_can_write(rb);

//    MTFS30_DEBUG("写index: %d, 读index: %d, 写入大小: %d, 可写大小: %d, 可读大小:%d\n", 
//                 rb->rb_write, rb->rb_read, write_size, can_write, util_ringbuffer_can_read(rb));    

    
    /* 超过可写空间大小，则不写 */
    if (write_size > can_write)
    {
       return 0; 
    }
    
    /* 如果实际写的大小小于【写index到缓冲区末尾】的空间大小，则直接写;
     *  否则，分两段写，先写【写index到缓冲区末尾】的空间大小的数据，然后
     *  再从缓冲区开头接着写剩余的数据
     */   
    tail_size = rb->rb_size - rb->rb_write;
    if (write_size < tail_size)
    {
        memcpy(rb->prb_buf + rb->rb_write, data, write_size);
        rb->rb_write = rb->rb_write + write_size;
    }
    else
    {
        memcpy(rb->prb_buf + rb->rb_write, data, tail_size);
        memcpy(rb->prb_buf, data+tail_size, write_size-tail_size);
        rb->rb_write = (rb->rb_write + write_size) %  (rb->rb_size);
    }
    
//    MTFS30_DEBUG("新写index: %d, 读index: %d, 写入大小: %d, 新可写大小: %d, 新可读大小:%d\n", 
//                 rb->rb_write, rb->rb_read, write_size, util_ringbuffer_can_write(rb), util_ringbuffer_can_read(rb));     
    
    return write_size; /* 返回实际写的大小 */
}
