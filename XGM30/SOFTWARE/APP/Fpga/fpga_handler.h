/*****************************************************************************/
/* 文件名:    fpga_handler.h                                                 */
/* 描  述:    FPGA读写处理头文件                                             */
/* 创  建:    2018-11-05 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __FPGA_HANDLER_H_
#define __FPGA_HANDLER_H_
#include "arch/cc.h"



/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define    FPGA_RW_MAX_SIZE    256

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern u8_t fpga_write_cmd_handler(int num,...);
extern u8_t fpga_read_cmd_handler(int num,...);
#endif
