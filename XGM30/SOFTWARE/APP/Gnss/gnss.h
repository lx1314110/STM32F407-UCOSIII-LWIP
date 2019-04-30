/*****************************************************************************/
/* 文件名:    gnss.h                                                         */
/* 描  述:    gnss公用头文件                                                 */
/* 创  建:    2018-07-19 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __GNSS_H_
#define __GNSS_H_
#include "arch/cc.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/

/* 在定义长度时，要多加一个位结束符 '\0' */
/* 信息长度定义 */
#define    GNSS_LOCATION_INFO_LEN      56    /* 定位信息长度       */
#define    GNSS_STATELLITE_INFO_LEN    400   /* 卫星状态信息长度   */
#define    GNSS_LEAP_INFO_LEN          28    /* 闰秒预告信息长度   */
#define    GNSS_MODE_INFO_LEN          17    /* 工作模式信息长度   */
#define    GNSS_1PPS_INFO_LEN          3     /* 1PPS状态信息长度   */
#define    GNSS_ANTENNA_INFO_LEN       6     /* 天线状态信息长度   */
#define    GNSS_VERSION_INFO_LEN       30    /* GNSS接收机版本信息 */
#define    GNSS_TIME_INFO_LEN          22    /* GNSS时间信息       */
#define    GNSS_RECVIVER_NAME_LEN      10    /* GNSS接收机型号名称 */

#define    GNSS_LOCATION_SATS          3     /* 定位星数数据长度   */
#define    GNSS_LOCATION_TIME          10    /* UTC时间数据长度    */
#define    GNSS_LOCATION_LAT           14    /* 纬度数据长度       */
#define    GNSS_LOCATION_LON           15    /* 经度数据长度       */
#define    GNSS_LOCATION_ALT           9     /* 海拔数据长度       */

#define    GNSS_SAT_NUM_MAX            36    /* 接收的最大卫星数   */
#define    GNSS_SAT_INFO_MAX_LEN       400   /* 卫星信息最大长度   */


#define    GNSS_LEAP_TIME_HMS_LEN      7     /* 闰秒发生时分秒长度 */

#define    GNSS_GSV_MSG_MAX_LEN        70  /* GSV信息最大长度 */

#define    GNSS_MSG_MAX_LEN          2048 /* 卫星信息最大长度 */

     

/*-------------------------------*/
/* 结构定义                      */
/*-------------------------------*/

/* GNSS时间数据 */
typedef struct _TIME
{
    u8_t  hms[GNSS_LEAP_TIME_HMS_LEN]; /* 时分秒 */
    u8_t  day; /* 日   */
    u8_t  mon; /* 月   */
    u16_t year;/* 年   */
    u8_t  zone;/* 时区 */ 
}gnss_time_t;


/* 定位数据 */
typedef struct _GNSS_LOCATION_T_
{
    u8_t           sats;                          /* 定位星数        */
    u8_t           time[GNSS_LOCATION_TIME];      /* UTC时间         */
    u8_t           lat[GNSS_LOCATION_LAT];        /* 纬度            */ 
    u8_t           latdir;                        /* 纬度方向        */
    u8_t           lon[GNSS_LOCATION_LON];        /* 经度            */
    u8_t           londir;                        /* 经度方向        */
    u8_t           alt[GNSS_LOCATION_ALT];        /* 海拔            */
} gnss_location_t;




/* 定位信息 */
typedef struct _GNSS_LOCATION_INFO_T_
{

  u8_t info[GNSS_LOCATION_INFO_LEN];

} gnss_location_info_t;

/* 单颗卫星信息 */
typedef struct _GNSS_SAT_T_
{
    u8_t enable;/* 是否可用    */
    u8_t prn;   /* 卫星PRN编号 */
	u8_t snr;   /* 信噪比      */
	u8_t elev;  /* 仰角        */
    
} gnss_sat_t;

/* 卫星信息 */
typedef struct _GNSS_SAT_INFO_T_ 
{
    u16_t    len;
    u8_t     info[GNSS_STATELLITE_INFO_LEN];
    
} gnss_satellite_info_t;

/* 闰秒预告信息 */
typedef struct _GNSS_LEAP_INFO_T_
{
    u8_t info[GNSS_LEAP_INFO_LEN];
} gnss_leap_info_t;


/* 工作模式信息 */
typedef struct _GNSS_MODE_INFO_T_
{
    u8_t info[GNSS_MODE_INFO_LEN];
} gnss_mode_info_t;


/* 天线状态信息 */
typedef struct _ANTENNA_INFO_T_
{
    u8_t info[GNSS_ANTENNA_INFO_LEN];
} gnss_antenna_info_t;


/* 1PPS状态信息 */
typedef struct _GNSS_PPS_INFO_T_
{
    u8_t info[GNSS_1PPS_INFO_LEN];
} gnss_pps_info_t;


/* GNSS接收机版本信息 */
typedef struct _GNSS_VERSION_INFO_T_
{
    u8_t info[GNSS_VERSION_INFO_LEN];

} gnss_version_info_t;

/* GNSS时间信息 */
typedef struct _GNSS_TIME_INFO_T_
{
    u8_t info[GNSS_TIME_INFO_LEN];

} gnss_time_info_t;


/* GNSS信息 */
typedef struct _GNSS_INFO_T_
{
  gnss_mode_info_t        mode; /* 工作模式信息       */
  gnss_time_info_t        time; /* GNSS时间信息       */
  gnss_location_info_t    loc;  /* 定位信息           */
  gnss_satellite_info_t   sat;  /* 卫星信息           */
  gnss_leap_info_t        leap; /* 闰秒预告信息       */
  gnss_pps_info_t         pps;  /* 1PPS状态信息       */
  gnss_antenna_info_t     ant;  /* 天线状态信息       */
  gnss_version_info_t     ver;  /* GNSS接收机版本信息 */
  
} gnss_info_t;


/* GNSS语句解析器定义 */
typedef struct  _GNSS_STMT_HANDLE_T_
{
	u8_t* p_head;     /* 命令头                      */
    u8_t  check_type; /* 校验类型                    */
    void (*parse_func)(u8_t *p_param);   /* 解析函数 */
} gnss_stmt_handle_t;



/* 接收机信息 */
typedef struct _GNSS_RECEIVER_INFO_T_
{
    u8_t post_sats;                         /* 定位星数        */
    u8_t ac_mode;	                        /* 接收机工作模式  */
    u8_t ant_sta;					        /* 天线状态        */
    u8_t rec_type;		                    /* 接收的类型      */
    u8_t rec_name[GNSS_RECVIVER_NAME_LEN];  /*接收的型号名称   */
    u8_t pps_status;                        /* 1PPS状态        */
} gnss_recviver_info_t;


/* 闰秒信息 */
typedef struct _GNSS_LEAP_T_
{
    u8_t        now_leap;      /* 当前闰秒      */
    u8_t        next_leap;     /* 新的闰秒      */
    u8_t        sign;          /* 无/正/负 闰秒 */    
    gnss_time_t leap_time;     /* 闰秒发生时间  */

} gnss_leap_t;

#endif
