/*****************************************************************************/
/* 文件名:    roll_print.h                                                    */
/* 描  述:    循环打印头文件                                                 */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __ROLL_PRINT_H_
#define __ROLL_PRINT_H_




/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/




/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void roll_print_info(void);
extern u8_t roll_print_set_handler(int num,...);
extern void roll_print_task_info(void);
#endif
