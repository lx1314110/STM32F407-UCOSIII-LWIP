/*****************************************************************************/
/* 文件名:    util_check.h                                                   */
/* 描  述:    校验处理用头文件                                               */
/* 创  建:    2018-07-19 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __UTIL_CHECK_H_
#define __UTIL_CHECK_H_
#include "arch/cc.h"



/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define  CHECKCODE_EQ      0     /* 校验码相同 */
#define  CHECKCODE_NOEQ    1     /* 校验码不同 */

/* 校验方式 */
#define CHECK_TYPE_XOR     1     /* 异或校验   */
#define CHECK_TYPE_CRC32   2     /* crc32校验  */


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern u8_t util_check_crc32(const u8_t *str, u32_t len, const u8_t* chekcode);
extern u8_t util_xor_check(const u8_t *str, u32_t len, const u8_t* chekcode);
#endif
