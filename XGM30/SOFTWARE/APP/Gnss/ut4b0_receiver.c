/*****************************************************************************/
/* 文件名:    ut4b0_receiver.c                                               */
/* 描  述:    UT4B0接收机相关处理                                            */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "Gnss/ut4b0_receiver.h"
#include "Gnss/gnss_receiver.h"
#include "Util/util_check.h"
#include "BSP/inc/bsp_usart.h"
#include "Debug/mtfs30_debug.h"
#include "string.h"
#include "arch/cc.h"
#include "System/system.h"

#include "os_type.h"
#include "cpu.h"

/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
//u8_t   gut4b0_config_cmd[UT4B0_CONFIG_CMD_MAX_LEN] = {0};      /* 用于保存发送给UT4B0的配置命令 */
static ut4b0_combcmd_t   g_save_cmd;   /* 用于保存发送给UT4B0的配置命令 */

/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/

extern u8_t                    g_gnss_echo_switch;  /* 语句回显开关       */
extern OS_SEM                  g_ut4b0_reply_sem;   /* 命令回复信号量     */
extern gnss_info_t             g_gnss_info;         /* GNSS信息           */
extern gnss_recviver_info_t    g_rev_info;          /* GNSS接收机信息     */ 
extern gnss_leap_t             g_leap;              /* 闰秒               */
extern u8_t                    g_gnss_echo_switch;  /* 卫星语句回显开关   */
extern OS_SEM                  g_gnss_idle_sem;     /* GNSS空闲中断信号量 */
extern u8_t                    g_gnss_cmd_send_flag;/*  卫星命令发送标志  */


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void ut4b0_check_cmdhandler(u8_t *pdata);
static void ut4b0_zda_msg_parser(u8_t *pdata);
static void ut4b0_gga_msg_parser(u8_t *pdata);
static void ut4b0_gsv_msg_parser(u8_t *pdata, u8_t type);
static void ut4b0_gpgsv_msg_parser(u8_t *pdata);
static void ut4b0_bdgsv_msg_parser(u8_t *pdata);
static void ut4b0_glgsv_msg_parser(u8_t *pdata);
static void ut4b0_gagsv_msg_parser(u8_t *pdata);
static void ut4b0_gpsbds_utc_msg_parser(u8_t *pdata);
static void ut4b0_gal_utc_msg_parser(u8_t *pdata);
//static void ut4b0_leap_forecast(u8_t sat_type, u32_t wn, u8_t dn, u8_t now_leap,u8_t next_leap, u8_t sign);
static void ut4b0_leap_process(u8_t sat_type, u32_t wn, u8_t dn, u8_t now_leap, u8_t next_leap);
static void ut4b0_get_leap_time(u8_t sat_type, u32_t wn, u8_t dn, u8_t sign, gnss_time_t *p_time);
static u8_t ut4b0_checkcode_compare(u8_t *pmsg, u8_t *pcode, u8_t check_type);
static void ut4b0_1pps_msg_parser(u8_t *pdata);
static void ut4b0_cmd_reply_msg_parser(u8_t *pdata);
static u8_t ut4b0_receiver_configuration(u8_t *pcmd);
static void ut4b0_antenna_msg_parser(u8_t *pdata);
static void ut4b0_cmd_version_msg_parser(u8_t *pdata);
static u8_t ut4b0_receiver_utc_statement_set(void);
static void ut4b0_receiver_cmd_save(u8_t *p_cmd);


/* ut4b0接收机数据输出指令处理表 */
static gnss_stmt_handle_t ut4b0_stmt_handle[] =
{
	/* UTC信息 */
	{(u8_t *)"#GPSUTC",     CHECK_TYPE_CRC32,    ut4b0_gpsbds_utc_msg_parser},
	{(u8_t *)"#BDSUTC",     CHECK_TYPE_CRC32,    ut4b0_gpsbds_utc_msg_parser},
	{(u8_t *)"#GALUTC",     CHECK_TYPE_CRC32,    ut4b0_gal_utc_msg_parser},
    
    /* 版本信息 */
       {(u8_t *)"#VERSIONA",    CHECK_TYPE_CRC32,    ut4b0_cmd_version_msg_parser},

    /* 天线状态 */
       {(u8_t *)"#ANTENNAA",    CHECK_TYPE_CRC32,    ut4b0_antenna_msg_parser},
    
	/* GGA信息 */
	{(u8_t *)"$GNGGA",      CHECK_TYPE_XOR,      ut4b0_gga_msg_parser},
	{(u8_t *)"$GPSGGA",     CHECK_TYPE_XOR,      ut4b0_gga_msg_parser},
        {(u8_t *)"$GPGGA",      CHECK_TYPE_XOR,      ut4b0_gga_msg_parser},
        {(u8_t *)"$BDGGA",      CHECK_TYPE_XOR,      ut4b0_gga_msg_parser},    
	{(u8_t *)"$BDSGGA",     CHECK_TYPE_XOR,      ut4b0_gga_msg_parser},
	{(u8_t *)"$GLOGGA",     CHECK_TYPE_XOR,      ut4b0_gga_msg_parser},
	{(u8_t *)"$GALGGA",     CHECK_TYPE_XOR,      ut4b0_gga_msg_parser}, 
    
    /* 时间信息 */
        {(u8_t *)"$GNZDA",      CHECK_TYPE_XOR,      ut4b0_zda_msg_parser},    
        {(u8_t *)"$GPZDA",      CHECK_TYPE_XOR,      ut4b0_zda_msg_parser},
        {(u8_t *)"$BDZDA",      CHECK_TYPE_XOR,      ut4b0_zda_msg_parser},    

	/* GSV信息 */
	{(u8_t *)"$GPGSV",      CHECK_TYPE_XOR,      ut4b0_gpgsv_msg_parser},
	{(u8_t *)"$BDGSV",      CHECK_TYPE_XOR,      ut4b0_bdgsv_msg_parser},
	{(u8_t *)"$GLGSV",      CHECK_TYPE_XOR,      ut4b0_glgsv_msg_parser},
	{(u8_t *)"$GAGSV",      CHECK_TYPE_XOR,      ut4b0_gagsv_msg_parser},	

   
    /* 1PPS */
        {(u8_t *)"#BBPPSMSGAA", CHECK_TYPE_CRC32,    ut4b0_1pps_msg_parser},
            
    /* 命令回复 */
        {(u8_t *)"$command",    CHECK_TYPE_XOR,      ut4b0_cmd_reply_msg_parser},
    
};

/*****************************************************************************
 * 函  数:    ut4b0_receiver_init                                                           
 * 功  能:    ut4b0接收机初始化                                                                
 * 输  入:     *pdata: 卫星数据                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
void ut4b0_receiver_init(void)
{
    
     /* 初始化ut4b0接收机串口波特率 */
     GNSS_USART_Send("CONFIG COM1 115200"); /* 使用卫星模块串口发送命令 */
     BSP_Sleep(10);
     
     /* PPS脉冲配置 */
     GNSS_USART_Send("CONFIG PPS ENABLE GPS POSITIVE 500000 1000 0 0");
 
     /* 初始化ut4b0接收机工作模式 */
     GNSS_USART_Send("MASK -");
     

    
    /* 初始化ut4b0接收机输出的卫星语句 */
    GNSS_USART_Send("UNLOG"); /* 禁止所有语句输出 */
    GNSS_USART_Send("GPSUTCA 1\r\nBDSUTCA 1\r\nGALUTCA 1\r\nVERSIONA 1\r\nANTENNAA 1\r\nGNGGA 1\r\nGPZDA 1\r\nGPGSV 1\r\nBBPPSMSGA ONCHANGED\r\n");
       
}


/*****************************************************************************
 * 函  数:    ut4b0_receiver_msg_parser                                                           
 * 功  能:    解析ut4b0接收机输出的卫星数据                                                                 
 * 输  入:     *pdata: 卫星数据                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void ut4b0_receiver_msg_parser(u8_t *pdata)
{
	u8_t *p = pdata;
	u8_t *p_msg  = NULL;
	
	while(*p != '\0')
	{	
		//解析标准语句
		if(*p == '$' || *p == '#')
		{	
			p_msg  = (u8_t*)strtok((char*)p, "\n");
			p += strlen((char*)p_msg); 		  			//跳过‘\0’
			
			if (NULL != strchr((const char *)p_msg, '*') &&
				NULL != strchr((const char *)p_msg, '\r')) 
			{
				
                /* 查找并执行该条数据指令的解析函数 */
       		    ut4b0_check_cmdhandler(p_msg);  
			}
		}			
		p++;
	}		

}


/*****************************************************************************
 * 函  数:    ut4b0_checkcode_compare                                                           
 * 功  能:    计算数据指令数据部分的校验码并与该数据指令中的校验码进行比较                                                              
 * 输  入:    *pmsg      : 数据指令数据部分   
 *            *pcode     : 数据指令中的校验码
 *            check_type : 校验方式
 *                         CHECK_TYPE_XOR  : 异或校验
 *                         CHECK_TYPE_CRC32: crc32校验
 * 输  出:    无                                                    
 * 返回值:    CHECKCODE_EQ: 校验码相同; CHECKCODE_NOEQ: 校验码不同                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t ut4b0_checkcode_compare(u8_t *pmsg, u8_t *pcode, u8_t check_type)
{
    
    u8_t ret = CHECKCODE_NOEQ;
   
    /* 判断校验方式 */
    switch(check_type)
    {
    case CHECK_TYPE_XOR:    /* 异或校验 */
        ret = util_xor_check(pmsg, strlen((char *)pmsg), pcode);
        break;
        
    case CHECK_TYPE_CRC32: /* crc32校验 */
        ret = util_check_crc32(pmsg, strlen((char *)pmsg), pcode);
        break;
    
    default:
	MTFS30_ERROR("校验方式(%d)有误\n", check_type);   
        break;
    }


    return ret;
}

/*****************************************************************************
 * 函  数:    ut4b0_check_cmdhandler                                                           
 * 功  能:    查找UT4B0接收机数据输出语句的解析函数并调用                                                              
 * 输  入:     *pdata: 卫星数据                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
static void ut4b0_check_cmdhandler(u8_t *pdata)
{
    u8_t i = 0;
    u8_t cnt = 0;        /* 命令表中命令条数   */
    u8_t *pmsg = NULL;       /* "*"前的数据        */
    u8_t *pcode = NULL;      /* 指向校验码         */
    u8_t *pcheck_msg = NULL; /* 指向参加校验的数据 */


	/* 计算条数 */
	cnt = sizeof(ut4b0_stmt_handle) / sizeof(ut4b0_stmt_handle[0]);

	/* 查找指定语句 */
	for (i = 0; i < cnt; i++)
	{
        /* 找到该语句 */
	   if(strncmp((char *)ut4b0_stmt_handle[i].p_head, (char*)pdata, strlen((char *)ut4b0_stmt_handle[i].p_head)) == 0)
	   {
            /* 卫星语句以GPSUTC语句为开头，所以GPSUTC语句代表新的一秒的开始, 在此清除上一秒保存的GNSS数据 */ 
            if (0 == i) 
            {
                gnss_receiver_info_clear();   
            }
            
             /* 截取出"*"以前的数据 */           			
            pmsg = (u8_t*)strtok((char *)pdata, "*");
        
            /* 截取出校验码 */
            pcode =(u8_t*)strtok(NULL, "\r");
            
            if (pmsg == NULL || pcode == NULL)
            {
                return;
            }
            
       
            /* "command"以外的语句"$"或者"#"都不参加校验 */
            pcheck_msg = pmsg;
            if (0 != strncmp((char const *)pmsg, "$command", 8))
            {
             
                pcheck_msg = pmsg + 1;
            }
            
            /* 进行校验 */
            if (CHECKCODE_EQ == ut4b0_checkcode_compare(pcheck_msg, pcode, ut4b0_stmt_handle[i].check_type)) /* 校验码一致 */
            {
                pmsg++;
			    /* 调用解析函数 */
			    ut4b0_stmt_handle[i].parse_func(pmsg);
			    
            }
            else /* 校验码不一致 */
            {
               MTFS30_ERROR("(%s)校验码不一致", pdata);
            }
            
            return;
		}
	} 


    /* 没有找到该语句 */
	u8_t *pcmd = pdata;
	pcmd = (u8_t *)strtok((char *)pdata, ",");
	MTFS30_ERROR("输出语句(%s)有误", pcmd);
	

}

/*****************************************************************************
 * 函  数:    ut4b0_zda_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出ZDA语句                                                                
 * 输  入:    *pdata: ZDA语句信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_zda_msg_parser(u8_t *pdata)
{
    
    u8_t i = 0;
//    u8_t *p = NULL;
    u8_t comma_cnt = 0;   /* 逗号个数             */
    u8_t index[10] ={0};  /* 参数索引             */   
    u8_t len = 0;         /* 保存的消息长度       */    

    
    /* 获取到每个参数的索引 */    
    /* index: 0         1  2  3         
     * $GPZDA,060014.00,28,01,2019,,*64 
     */
    for(i = 0, comma_cnt = 0; comma_cnt < 5; i++)
    {
        if (',' == pdata[i])
        {
            index[comma_cnt] = i + 1;
            comma_cnt++;
        }
    }
    
    /* 检查ZDA信息是否有效 */
    /* 无效数据的两个逗号之间没有数据,如: $GPZDA,,,,,,*48 */
    if (',' == pdata[index[0]])
    {
        strcpy((char *)g_gnss_info.time.info, "000000.00,00,00,0000;");
        return;
    }
    
    /* 保存时间信息 */
    for(i = 0; i < 4; i++)
    {
        memcpy(&g_gnss_info.time.info[len], &pdata[index[i]], index[i+1]-index[i]);
        len += index[i+1]-index[i];
    
    }
    
    /* 将最后的逗号换成分号 */
    g_gnss_info.time.info[len - 1] = ';';
    g_gnss_info.time.info[len] = '\0';
    
    
    MTFS30_DEBUG("时间信息: %s", g_gnss_info.time.info);
    
       
}

/*****************************************************************************
 * 函  数:    ut4b0_gga_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出GGA信息                                                                
 * 输  入:    *pdata: XXXGGA信息                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
static void ut4b0_gga_msg_parser(u8_t *pdata)
{

    
    u8_t i = 0; 
    u8_t comma_cnt = 0;   /* 逗号个数             */
    u8_t index[10] ={0};  /* 参数索引             */   
    u8_t len = 0;         /* 保存的消息长度       */

    /* 获取到每个GGA参数的索引 */    
    /* index: 0         1             2 3              4 5 6  7   8        9 10       11
     * $GNGGA,044426.00,3032.49556656,N,10403.16548096,E,1,15,0.9,529.1063,M,-42.4491,M,,*5D 
     */
    for(i = 0, comma_cnt = 0; comma_cnt < 10; i++)
    {
        if (',' == pdata[i])
        {
            index[comma_cnt] = i + 1;
            comma_cnt++;
        }
    }
    
    /*--------------------------------*/
    /* 保存定位信息                   */
    /*--------------------------------*/     
    /* 使用中的卫星数 */
    memcpy(&g_gnss_info.loc.info[len], &pdata[index[6]], index[7]-index[6]);
    len += index[7]-index[6];
    
    g_rev_info.post_sats = (pdata[index[6]] - '0') * 10 + (pdata[index[6]+1] - '0');
    MTFS30_DEBUG("定位星数:%d", g_rev_info.post_sats);
    if (0 == g_rev_info.post_sats)
    {
        /* 定位星数为0，清除保存的卫星信息 */
        strcpy((char *)g_gnss_info.sat.info, "GPS,00;BDS,00;GLO,00;GAL,00;");
        g_gnss_info.sat.len = 0;

        return;        
    }

          
    /* UTC时间 */
    memcpy(&g_gnss_info.loc.info[len], &pdata[index[0]], index[1]-index[0]);
    len += index[1]-index[0];
    
    /* 纬度 */
    memcpy(&g_gnss_info.loc.info[len], &pdata[index[1]], index[2]-index[1]);
    len += index[2]-index[1];
    
    /* 纬度方向 */
    memcpy(&g_gnss_info.loc.info[len], &pdata[index[2]], index[3]-index[2]);
    len += index[3]-index[2];
    
    /* 经度 */
    memcpy(&g_gnss_info.loc.info[len], &pdata[index[3]], index[4]-index[3]);
    len += index[4]-index[3];

    /* 经度方向 */
    memcpy(&g_gnss_info.loc.info[len], &pdata[index[4]], index[5]-index[4]);
    len += index[5]-index[4];    
    
    /* 海拔 */
    memcpy(&g_gnss_info.loc.info[len], &pdata[index[8]], index[9]-index[8]); 
    len += index[9]-index[8];
    
    
    /* 将最后的逗号变为分号 */
    g_gnss_info.loc.info[len - 1] = ';';
    g_gnss_info.loc.info[len] = '\0';   
    

    
    MTFS30_DEBUG("定位信息:%s", g_gnss_info.loc.info);

}

/*****************************************************************************
 * 函  数:    ut4b0_gpgsv_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出GPGSV信息                                                                 
 * 输  入:    *pdata: GPGSV信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
static void ut4b0_gpgsv_msg_parser(u8_t *pmsg)
{
    ut4b0_gsv_msg_parser(pmsg, GNSS_SAT_TYPE_GPS);
}

/*****************************************************************************
 * 函  数:    ut4b0_bdgsv_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出BDGSV信息                                                                 
 * 输  入:    *pdata: BDGSV信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
static void ut4b0_bdgsv_msg_parser(u8_t *pmsg)
{
    ut4b0_gsv_msg_parser(pmsg, GNSS_SAT_TYPE_BDS);
}

/*****************************************************************************
 * 函  数:    ut4b0_glgsv_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出GLGSV信息                                                                 
 * 输  入:    *pdata: GLGSV信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
static void ut4b0_glgsv_msg_parser(u8_t *pmsg)
{
    ut4b0_gsv_msg_parser(pmsg, GNSS_SAT_TYPE_GLO);
}

/*****************************************************************************
 * 函  数:    ut4b0_gagsv_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出GAGSV信息                                                                 
 * 输  入:    *pdata: GAGSV信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
static void ut4b0_gagsv_msg_parser(u8_t *pmsg)
{
    ut4b0_gsv_msg_parser(pmsg, GNSS_SAT_TYPE_GAL);
}
/*****************************************************************************
 * 函  数:    ut4b0_gsv_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出GSV信息                                                                 
 * 输  入:    *pdata: GSV信息;   
 *             type: 卫星类型
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
static void ut4b0_gsv_msg_parser(u8_t *pdata, u8_t type)
{
	u8_t  i = 0;
	u8_t  index[20] ={0};    /* 参数索引            */
    u8_t  comma_cnt = 0;     /* 逗号个数            */
    u8_t  msg_total = 0;     /* 信息总数            */
	u8_t  msg_cur = 0;       /* 当前信息数          */
	u8_t  amount = 0;        /* 可见的卫星总数      */
    u8_t data[GNSS_GSV_MSG_MAX_LEN] = {0}; /* 存放GSV数据 */
    u16_t len = 0;

    
    


    /* 在最后添加一个','是为了取出','前面的数据 */
    len = strlen((const char *)pdata);
	if (len > GNSS_GSV_MSG_MAX_LEN - 2)
	{
	    len = GNSS_GSV_MSG_MAX_LEN - 2;
	}
    strncpy((char *)data, (const char *)pdata, len);
    data[len++] = ',';

    
    /* 取得每个参数的索引 */
    /* index: 0 1 2  3  4  5   6  7  8  9   10 11 12 13  14 15 16 17  18 19 
     * $GPGSV,2,1,07,25,44,074,36,31,55,256,37,12,19,045,36,14,53,335,36,\0
     */    
    while(data[i])
    {
        
        if((',' == data[i]))
        {
            index[comma_cnt] = i + 1;
            comma_cnt++;
        }
        
        i++;
    }
    
    
 
    /* 可见的卫星总数 */
    amount = (data[index[2]] - '0') * 10 + (data[index[2]+1] - '0');
    if (amount == 0)
    {
        strcpy((char *)g_gnss_info.sat.info, "GPS,00;BDS,00;GLO,00;GAL,00;");
        return;
    }
    

	/* 信息总数 */
    msg_total = data[index[0]];
	

	/* 当前信息数 */
	msg_cur = data[index[1]]; 
    if (msg_cur == '1')
    {
    
        switch(type)
        {
        case GNSS_SAT_TYPE_GPS:
            sprintf((char *)&g_gnss_info.sat.info[g_gnss_info.sat.len], "GPS,%02d,", amount);
            
            break;
        case GNSS_SAT_TYPE_BDS:
            sprintf((char *)&g_gnss_info.sat.info[g_gnss_info.sat.len], "BDS,%02d,", amount);
            break;
        case GNSS_SAT_TYPE_GLO:
            sprintf((char *)&g_gnss_info.sat.info[g_gnss_info.sat.len], "GLO,%02d,", amount);
            break;
        case GNSS_SAT_TYPE_GAL:
            sprintf((char *)&g_gnss_info.sat.info[g_gnss_info.sat.len], "GAL,%02d,", amount);
            break;
            
        default:
            break;   
        }
        
        g_gnss_info.sat.len += 7;
    }
    

	
    /* 取出每一颗卫星的信息 */
    for(i = 3; i < comma_cnt - 1; i++)
    {
        /* 真北方位角（不处理） */
        if (i == 5 || i == 9 || i == 13 || i == 17)
        {
            continue;
        }
    
        /* 检查卫星是否可用 */
        if (i == 6 || i == 10 || i == 14 || i == 18) 
        {
            /* 信噪比为0，卫星不可用 */
            if (data[index[i]] == '0' && data[index[i]+1] == '0')  
            {
                //g_gnss_info.sat.info[g_gnss_info.sat.len++] = '0';   
                strcpy((char *)&g_gnss_info.sat.info[g_gnss_info.sat.len], "0,");
            }
            else
            {
                strcpy((char *)&g_gnss_info.sat.info[g_gnss_info.sat.len], "1,"); 
            }
            
            g_gnss_info.sat.len += 2;
        }
        
        /* 卫星编号、仰角 */

        memcpy(&g_gnss_info.sat.info[g_gnss_info.sat.len], &data[index[i]], index[i+1]-index[i]);
        g_gnss_info.sat.len += index[i+1]-index[i];
            
    }
    
    /* 当前信息数等于信息总数，表明一种类型的卫星信息获取完毕 */
    if (msg_cur == msg_total)
    {
        strcpy((char *)&g_gnss_info.sat.info[g_gnss_info.sat.len-1], ";");
//      MTFS30_DEBUG("#####卫星信息[%s]     长度[%d]", g_gnss_info.sat.info, g_gnss_info.sat.len);
    }
    
   
     
}   


/*****************************************************************************
 * 函  数:    ut4b0_gpsbds_utc_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出的UTC信息并生成闰秒预告(GPS/BDS卫星)                                                                 
 * 输  入:    *pdata: UTC信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_gpsbds_utc_msg_parser(u8_t *pdata)
{


    u8_t i = 0; 
    u8_t comma_cnt = 0;     /* 逗号个数                     */
    u8_t param_len = 0;     /* 保存每个参数的长度           */
    u8_t param_buf[20]= {0};/* 保存每个参数                 */
    u32_t wn = 0;           /* 新的闰秒生效的周计数         */
    u8_t  dn = 0;           /* 新的闰秒生效的周内日计数     */
    u8_t now_leap = 0;      /* 当前闰秒                     */
    u8_t next_leap = 0;     /* 新的闰秒                     */
    u8_t sate_type = 0;     /* 卫星类型                     */


  
    /* 略过Log头 */
	while(pdata[i++] != ';');
      
    /* 解析每个参数 */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            /* 逗号个数加1 */
            comma_cnt++; 
            
            param_buf[param_len]='\0';
            switch(comma_cnt)
            {
            case 5:    /* 新的闰秒生效的周计数     */
                wn = (u32_t)atoi((char const *)param_buf);
                break;
            case 6:    /* 新的闰秒生效的周内日计数 */
                dn = (u8_t)atoi((char const *)param_buf);
                break;
            case 7:    /* 当前闰秒                */
                now_leap = (u8_t)atoi((char const *)param_buf);
                break;
            case 8:    /* 新的闰秒                */
                next_leap = (u8_t)atoi((char const *)param_buf);
                goto LEAP;
                break;
            }    
            
            param_len = 0;
        }
        else 
        {
            param_buf[param_len++] = pdata[i];
        }
        
        i++;
    } 
    

    
LEAP:

//            MTFS30_DEBUG("新的闰秒生效的周计数： %d\n新的闰秒生效的周内日计数： %d\n当前闰秒： %d\n新的闰秒： %d\n", 
//                         wn, dn, now_leap,next_leap);

	if (strncmp((char *)pdata, "GPSUTC", 6) == 0)   /* GPS卫星 */
	{
        
        sate_type = GNSS_SAT_TYPE_GPS;
	}
	else if (strncmp((char *)pdata, "BDSUTC", 6) == 0)    /* BDS卫星 */
	{
        
        sate_type = GNSS_SAT_TYPE_BDS;
        
        /* BDS特殊处理 */
        now_leap = now_leap + 14;  /* 当前闰秒，以GPS时间为准  */
        next_leap = next_leap + 14;/*  新的闰秒，以GPS时间为准 */        
	
	}
    
    
    /* 闰秒处理 */
    ut4b0_leap_process(sate_type, wn, dn, now_leap, next_leap);
    
               
    MTFS30_DEBUG("GPS|BDS闰秒预告:%s", g_gnss_info.leap.info); 
//    MTFS30_DEBUG("GPS|BDS闰秒预告:%x %x", g_gnss_info.leap.info[0],g_gnss_info.leap.info[1]); 
}

/*****************************************************************************
 * 函  数:    ut4b0_gal_utc_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出的UTC信息并生成闰秒预告(GAL卫星)                                                                 
 * 输  入:    *pdata: UTC信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_gal_utc_msg_parser(u8_t *pdata)
{


    u8_t i = 0; 
    u8_t comma_cnt = 0;     /* 逗号个数                     */
    u8_t param_len = 0;     /* 保存每个参数的长度           */
    u8_t param_buf[20]= {0};/* 保存每个参数                 */
	u32_t wn = 0;           /* 新的闰秒生效的周计数         */
	u8_t  dn = 0;           /* 新的闰秒生效的周内日计数     */
	u8_t now_leap = 0;      /* 当前闰秒                     */
	u8_t next_leap = 0;     /* 新的闰秒                     */

    
    /* 略过Log头 */
	while(pdata[i++] != ';');
      
    /* 解析每个参数 */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            param_buf[param_len]='\0';
            switch(comma_cnt)
            {
            case 3:    /* 当前闰秒                */
                now_leap = (u8_t)atoi((char const *)param_buf);
                break;
            case 6:   /* 新的闰秒生效的周计数     */
                wn = (u32_t)atoi((char const *)param_buf);
                break;
            case 7:   /* 新的闰秒生效的周内日计数 */
                dn = (u8_t)atoi((char const *)param_buf);
                break;
            case 8:   /* 新的闰秒                 */
                next_leap = (u8_t)atoi((char const *)param_buf);
                goto LEAP;
                break;
            }    
            
            param_len = 0;
        }
        else 
        {
            param_buf[param_len++] = pdata[i];
        }
        
        i++;
    } 
    
    
LEAP:
    
//            MTFS30_DEBUG("GAL 新的闰秒生效的周计数： %d\n新的闰秒生效的周内日计数： %d\n当前闰秒： %d\n新的闰秒： %d\n", 
//                         wn, dn, now_leap,next_leap);

    /* 闰秒处理 */
    ut4b0_leap_process(GNSS_SAT_TYPE_GAL, wn, dn, now_leap, next_leap);
    

    MTFS30_DEBUG("GAL闰秒预告:%s", g_gnss_info.leap.info);    
}



/*****************************************************************************
 * 函  数:    ut4b0_leap_process                                                          
 * 功  能:    闰秒处理                                                                 
 * 输  入:    sat_type  : 卫星类型
 *            wn        ：新的闰秒生效的周计数 
 *            dn        ：新的闰秒生效的周内日计数
 *            now_leap  : UTC语句中包含的当前闰秒       
 *            next_leap : UTC语句中包含的新的闰秒 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_leap_process(u8_t sat_type, u32_t wn, u8_t dn, u8_t now_leap, u8_t next_leap)
{
       
    if ((g_leap.sign == 0) && (now_leap != next_leap) && (now_leap == g_leap.now_leap)) /* 产生闰秒预告 */
    {
        g_leap.next_leap = next_leap;
        
        if ((g_leap.next_leap - g_leap.now_leap) > 0)
        {
            g_leap.sign = 1; /* 正闰秒 */    
        }
        else
        {
            g_leap.sign = 2; /* 负闰秒 */
            
        }
        
        /* 获取新的闰秒发生时间 */
        ut4b0_get_leap_time(sat_type, wn, dn, g_leap.sign, &g_leap.leap_time);
        
        /* 生成闰秒预告信息 */
        sprintf((char *)g_gnss_info.leap.info, "%6s,%2d,%2d,%4d,%d,%d,%d;", 
                g_leap.leap_time.hms, g_leap.leap_time.day, g_leap.leap_time.mon, g_leap.leap_time.year, 
                g_leap.next_leap, g_leap.now_leap, g_leap.sign);
        MTFS30_DEBUG("生成闰秒预告: %s", g_gnss_info.leap.info);

    }
    else if ((g_leap.sign != 0) && (now_leap == next_leap) && (next_leap == g_leap.next_leap)) /* 消除闰秒预告 */
    {
        g_leap.now_leap =  now_leap;
        g_leap.sign = 0;

        /* 生存闰秒预告消除信息 */
        sprintf((char *)g_gnss_info.leap.info, "%6s,%2d,%2d,%4d,%d,%d,%d;", 
                g_leap.leap_time.hms, g_leap.leap_time.day, g_leap.leap_time.mon, g_leap.leap_time.year, 
                g_leap.next_leap, g_leap.now_leap, g_leap.sign);  
        MTFS30_DEBUG("消除闰秒预告: %s", g_gnss_info.leap.info);
    } 


}


/*****************************************************************************
 * 函  数:    ut4b0_get_leap_time                                                           
 * 功  能:    获取闰秒发生时间                                                                 
 * 输  入:    sat_type  : 卫星类型
 *            wn        ：新的闰秒生效的周计数 
 *            dn        ：新的闰秒生效的周内日计数
 *            sign      ：正/负闰秒
 * 输  出:    p_time    : 指向闰秒发生时间                                                   
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_get_leap_time(u8_t sat_type, u32_t wn, u8_t dn, u8_t sign, gnss_time_t *p_time)
{
	u16_t year = 0;   /* 新的闰秒发生年 */ 
	u8_t  mon = 1;    /* 新的闰秒发生月 */
	u8_t  day = 0;    /* 新的闰秒发生日 */
	u32_t days = 0;   /* 新的闰秒发生日距离起始历元的总天数 */
	u8_t  mon_days[12] ={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* 每月天数 */


    

    /* 参数检查 */
    if (NULL == p_time)
    {
        MTFS30_ERROR("ut4b0_get_leap_time() p_time is NULL");
        return ;
    }
    
    
	switch(sat_type)
	{
		case GNSS_SAT_TYPE_GPS:    /* GPS卫星 */
			year = 1980; /* GPS时间起始年 */
			mon = 1;     /* GPS时间起始月 */
			day = 6;     /* GPS时间起始日 */
            dn = dn - 1;
			break;

		case GNSS_SAT_TYPE_BDS:    /* BDS卫星 */
			year = 2006; /* BDS时间起始年 */      
			mon = 1;     /* BDS时间起始月 */    
			day = 1;     /* BDS时间起始日 */
			break;
            
        case GNSS_SAT_TYPE_GAL:     /* GAL卫星 */
			year = 1999; /* GAL时间起始年 */      
			mon = 8;     /* GAL时间起始月 */    
			day = 22;    /* GAL时间起始日 */ 
            dn = dn - 1;
            break;
            
        default:            
            break;
	}


	/*-------------------------------*/
	/* 计算新的闰秒发生的年月日      */
	/*-------------------------------*/	 

	/* 新的闰秒发生日距离起始历元的总天数 */
	days = wn * 7 + dn + day;

	while(days > mon_days[mon-1])
	{

		/* 判定闰年还是平年 */
		if ((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0))
		{
			mon_days[1] = 29; /* 闰年的2月为29天 */
		}
		else 
		{
			mon_days[1] = 28; /* 平年的2月为28天 */
		}     
        
		/* 天数超过当月的总天数，则进入下一个月 */
		days -= mon_days[mon-1];
		mon++;

		/* 超过12月进入下一年 */
		if (mon > 12)
		{
			year++;
			mon = 1; /* 新的一年，月份重新从1月开始 */
		}


	}

	/* 新的闰秒发生日 */
	day = days; 
    
    

    if (1 == sign) /* 正闰秒时间为：23:59:60 */
    {
        strncpy((char *)p_time->hms, "235960", GNSS_LEAP_TIME_HMS_LEN);
    }
    else /* 负闰秒时间为：23:59:58 */
    {
        strncpy((char *)p_time->hms, "235958", GNSS_LEAP_TIME_HMS_LEN);
    }
    
    p_time->day = day;
    p_time->mon = mon;
    p_time->year = year;
   
}



/*****************************************************************************
 * 函  数:    ut4b0_1pps_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出的1PPS状态信息                                                                
 * 输  入:    *pdata: 1PPS状态信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_1pps_msg_parser(u8_t *pdata)
{
    u8_t pps_id = 0;
    u8_t pps_status = 0;    
    u8_t *p = NULL; 
  
    
    
    /* LOG头 */
    p = (u8_t *)strtok((char *)pdata, ";");
      
    /* 1PPS ID */
    p = (u8_t *)strtok(NULL, ",");
    pps_id = (u8_t)atoi((char const *)p);

    
    /* 1PPS STATUS */
    p = (u8_t *)strtok(NULL, ",");
    pps_status = (u8_t)atoi((char const *)p);
    
    
    /* 定位星数小于4时为无效定位，1PPS不可用 */
    if (g_rev_info.post_sats < 4) 
    {
        strcpy((char *)g_gnss_info.pps.info, "0;");   
        MTFS30_DEBUG("1PPS状态信息: %s", g_gnss_info.pps.info);   
    }
    else
    {
        
        if (0 == pps_id)
        {
            snprintf((char *)g_gnss_info.pps.info, GNSS_1PPS_INFO_LEN, "%d;", pps_status);
            MTFS30_DEBUG("1PPS状态信息: %s", g_gnss_info.pps.info);        
        }
    }
    
}


/*****************************************************************************
 * 函  数:    ut4b0_antenna_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出天线状态信息                                                                
 * 输  入:    *pdata: 天线状态信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_antenna_msg_parser(u8_t *pdata)
{
    u8_t *p = NULL;
 
    
    /* LOG头 */
    p = (u8_t *)strtok((char *)pdata, ";");
    
    /* P指向天线状态信息开头 */
    p = (u8_t *)strtok(NULL, ",");
    

    snprintf((char *)g_gnss_info.ant.info, GNSS_ANTENNA_INFO_LEN, "%s;", p);
    MTFS30_DEBUG("天线状态信息: %s", g_gnss_info.ant.info);
    
}

/*****************************************************************************
 * 函  数:    ut4b0_cmd_version_msg_parser                                                           
 * 功  能:    解析UT4B0接收机输出的版本信息                                                                
 * 输  入:    *pdata: 版本信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_cmd_version_msg_parser(u8_t *pdata)
{
    u8_t i = 0;
    u8_t *p = NULL;
	u8_t *q = NULL;
    u8_t buf[GNSS_VERSION_INFO_LEN] = {0};
    u8_t comma_cnt = 0;     /* 逗号个数             */
    u8_t len = 0;
    

    
    /* LOG头 */
    p = (u8_t *)strtok((char *)pdata, ";");
    
    /* P指向版本信息 */
    p = (u8_t *)strtok(NULL, "");
    
    while(p[i])
    {
    
        if (p[i] == ',')
        {
            comma_cnt++;
            /* 版本信息已全部取完 */
            if (comma_cnt == 2)
            {
                strncpy((char *)buf, (char *)p, len); 
                buf[len] = ';';
                buf[len+1] = '\0';

                break;
            }
        }
        
        len++;
        i++;
    }
       
    /* 去掉接收机版本号信息前后的双引号，如"R1.00Build19876" -> R1.00Build19876 */	
    q = g_gnss_info.ver.info;
	i = 0;
	while(buf[i])
	{
		if (buf[i] != '\"')
		{
	        *q++ = buf[i];
		}
		i++;
	}
	*q = '\0';	
    MTFS30_DEBUG("版本信息: %s", g_gnss_info.ver.info);
}


/*****************************************************************************
 * 函  数:    ut4b0_cmd_reply_msg_parser                                                           
 * 功  能:    解析UT4B0命令回复信息                                                                
 * 输  入:    *pdata: 命令回复信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_cmd_reply_msg_parser(u8_t *pdata)
{

    OS_ERR err;
    u8_t *p_reply_cmd;      /* 回复信息中包含的命令 */
    u8_t *p_reply_result;   /* 回复信息中的回复结果 */
    u8_t *p = NULL;
    static u8_t cmd_cnt = 0;/* 统计收到的回复命令个数 */
   

    /* $command头 */
    p = (u8_t *)strtok((char *)pdata, ",");
    
    /* 回复信息中包含的用户输入的命令 */
    p_reply_cmd = (u8_t *)strtok(NULL, ",");
    
    /* 取出回复结果 */
    p = (u8_t *)strtok(NULL, "*");
    p_reply_result = p + 10; 
 
    /* 回复信息中包含的命令与发送命令一致，且回复结果为OK */
    if ((0 == strncmp((char *)&g_save_cmd.buf[g_save_cmd.index[cmd_cnt]], (char *)p_reply_cmd, strlen((const char *)p_reply_cmd))) &&
        (0 == strcmp("OK", (char *)p_reply_result)))
    {

        //MTFS30_DEBUG("保存的命令: %s, 回复信息: %s,回复结果:%s", gut4b0_config_cmd, p_reply_cmd, p_reply_result);        
       
        cmd_cnt++;
        if (cmd_cnt == g_save_cmd.num) /* 所有发送的命令都收到了回复 */
        {
        
            /* post命令回复信号量 */
            OSSemPost ((OS_SEM* ) &g_ut4b0_reply_sem,
                       (OS_OPT  ) OS_OPT_POST_1,
                       (OS_ERR *) &err); 
            
            cmd_cnt = 0;
        }        
    }
    else
    {
        cmd_cnt = 0;
    }

    
}


/*****************************************************************************
 * 函  数:    ut4b0_receiver_acmode_set                                                           
 * 功  能:    设置ut4b0接收机工作模式                                                              
 * 输  入:    acmode: 接收机工作模式                      
 * 输  出:    无                                                    
 * 返回值:    OK：正确配置工作模式; NG：未能正确配置工作模式                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
u8_t ut4b0_receiver_acmode_set(u8_t acmode)
{
    
    switch(acmode)
    {
    
    case REV_ACMODE_MIX: /* 联合定位 */
        /* 启用GPS、BDS、GLO、GAL */
        if (NG == ut4b0_receiver_configuration("UNMASK GPS\r\nUNMASK BDS\r\nUNMASK GLO\r\nUNMASK GAL\r\n"))
        {
           return NG;
        }        
        break;
    case REV_ACMODE_GPS: /* 单GPS定位 */
        /* 禁用BDS、GLO、GAL，启用GPS */
        if (NG == ut4b0_receiver_configuration("MASK BDS\r\nMASK GLO\r\nMASK GAL\r\nUNMASK GPS\r\n"))
        {
            return NG;
        }                    
        break;
    case REV_ACMODE_BDS: /* 单BDS定位 */
        /* 禁用GPS、GLO、GAL，启用BDS */
        if (NG == ut4b0_receiver_configuration("MASK GPS\r\nMASK GLO\r\nMASK GAL\r\nUNMASK BDS\r\n"))
        {
            return NG;
        }              
        break;
    case REV_ACMODE_GLO: /* 单GLO定位 */
        /* 禁用GPS、BDS、GAL，启用GLO */
        if (NG == ut4b0_receiver_configuration("MASK GPS\r\nMASK BDS\r\nMASK GAL\r\nUNMASK GLO\r\n"))
        {
            return NG;
        }
                 
        break;
    case REV_ACMODE_GAL: /* 单GAL定位 */
        /* 禁用GPS、BDS、GLO，启用GAL */
        if (NG == ut4b0_receiver_configuration("MASK GPS\r\nMASK BDS\r\nMASK GLO\r\nUNMASK GAL\r\n"))
        {
            return NG;
        }
             
        break;
    default:
        break;
    }


    return OK;
}



/*****************************************************************************
 * 函  数:    ut4b0_receiver_serial_baud_set                                                          
 * 功  能:    配置UT4B0接收机串口波特率                                                                 
 * 输  入:    p_baud: 指定波特率
 * 输  出:    无                                                    
 * 返回值:    OK：正确配置波特率; NG：未能正确配置波特率                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8_t ut4b0_receiver_serial_baud_set(u8_t *p_baud)
{
 
    u8_t buf[20] = {0};

       
    sprintf((char *)buf, "CONFIG COM1 %s", p_baud);   
    /* 发送波特率配置指令给UT4B接收机并检查配置结果 */
    return ut4b0_receiver_configuration(buf);

  
}

/*****************************************************************************
 * 函  数:    ut4b0_receiver_statement_set                                                           
 * 功  能:    打开/关闭ut4b0接收机指定语句的输出                                                                 
 * 输  入:    stmt_index: 指定语句索引
 * 输  入:    on_off_flag: 打开/关闭语句输出的标志 
 * 输  出:    无                                                    
 * 返回值:    OK: 配置成功; NG: 配置失败                                                  
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
u8_t ut4b0_receiver_statement_set(u8_t stmt_index, u8_t on_off_flag)
{
    

   
    if (on_off_flag == GNSS_ON) /*打开 */
    {
        /* 区分语句类型 */
        switch(stmt_index)
        {
        case GNSS_STMT_ZDA: /* ZDA语句      */
            if (NG == ut4b0_receiver_configuration("GPZDA 1"))
            {
                return NG;
            }
           break;            
            
        case GNSS_STMT_GGA: /* GGA语句      */
            if (NG == ut4b0_receiver_configuration("GNGGA 1"))
            {
                return NG;
            }
           break;
                
        case GNSS_STMT_GSV: /* GSV语句      */
            if (NG == ut4b0_receiver_configuration("GPGSV 1"))
            {
                return NG;
            }
           break;                

        case GNSS_STMT_UTC: /* 闰秒信息语句 */
            if (NG == ut4b0_receiver_utc_statement_set())
            {
                return NG;
            }
            break;
            
        case GNSS_STMT_PPS: /* 1PPS信息语句 */
            if (NG == ut4b0_receiver_configuration("BBPPSMSGA ONCHANGED "))
            {
                return NG;
            }                
            break;
            
        case GNSS_STMT_ANT: /* 天线状态语句 */
            if (NG == ut4b0_receiver_configuration("ANTENNAA 1"))
            {
                return NG;
            }                
            break; 
            
        case GNSS_STMT_VER: /* 版本信息语句 */
            if (NG == ut4b0_receiver_configuration("VERSIONA 1"))
            {
                return NG;
            }                
            break;             
                
        default:
            break;
        }
              
    }
    else /* 关闭 */
    {
        /* 区分语句类型 */
        switch(stmt_index)
        {
        case GNSS_STMT_ZDA: /* ZDA语句      */
            if (NG == ut4b0_receiver_configuration("UNLOG GPZDA"))
            {
                return NG;
            }  
            
           break;            
            
        case GNSS_STMT_GGA: /* GGA语句      */
            if (NG == ut4b0_receiver_configuration("UNLOG GNGGA"))
            {
                return NG;
            }  
            
           break;
           
        case GNSS_STMT_GSV: /* GSV语句      */
            if (NG == ut4b0_receiver_configuration("UNLOG GPGSV"))
            {
                return NG;
            }
            
            break;
            
        case GNSS_STMT_UTC: /* 闰秒信息语句 */
            MTFS30_TIPS("UT4B0暂不支持关闭UTC语句!\n");
            return NG;
            break;
            
        case GNSS_STMT_PPS: /* 1PPS信息语句 */
            MTFS30_TIPS("UT4B0暂不支持关闭1PPS输出语句!\n");
            return NG;
            break;
            
        case GNSS_STMT_ANT: /* 天线状态语句 */
            if (NG == ut4b0_receiver_configuration("UNLOG ANTENNAA"))
            {
                return NG;
            }
            
            break;   
            
        case GNSS_STMT_VER: /* 版本信息语句 */
            if (NG == ut4b0_receiver_configuration("UNLOG VERSIONA"))
            {
                return NG;
            } 
            
            break;
            
        default:
            break;
        }    
  
    }
           
    return OK;       
}

/*****************************************************************************
 * 函  数:    ut4b0_receiver_statement_set                                                           
 * 功  能:    打开/关闭ut4b0接收机输出UTC语句                                                                 
 * 输  入:    stmt_index: 指定语句索引
 * 输  入:    on_off_flag: 打开/关闭语句输出的标志 
 * 输  出:    无                                                    
 * 返回值:    OK：配置成功                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
static u8_t ut4b0_receiver_utc_statement_set(void)
{

        if (NG == ut4b0_receiver_configuration("GPSUTC 1\r\nBDSUTC 1\r\nGALUTC 1\r\n"))
        {
            return NG;
        }
        

//        if (NG == ut4b0_receiver_configuration("BDSUTC 1"))
//        {
//            return NG;
//        }                
//
//        
//        if (NG == ut4b0_receiver_configuration("GALUTC 1"))
//        {
//            return NG;
//        }                 

    
    return OK;
}

/*****************************************************************************
 * 函  数:    ut4b0_receiver_configuration                                                          
 * 功  能:    发送配置指令给UT4B接收机并检查配置结果                                                               
 * 输  入:    p_cmd配置指令
 * 输  出:    无                                                    
 * 返回值:    OK: 正确执行配置指令
 *            NG: 未能正确执行配置指令
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t ut4b0_receiver_configuration(u8_t *p_cmd)
{
    OS_ERR err;
    
    

    /* 检查配置命令长度 */
    if (strlen((const char *)p_cmd)> UT4B0_CONFIG_CMD_MAX_LEN)
    {
        MTFS30_DEBUG("UT4B0配置命令(%s)过长, 最长为%d字节", p_cmd, UT4B0_CONFIG_CMD_MAX_LEN);
        return NG;
    }
        
    /* 保存配置命令 */
    ut4b0_receiver_cmd_save(p_cmd);
    //strncpy((char *)gut4b0_config_cmd, (char *)p_cmd, UT4B0_CONFIG_CMD_MAX_LEN);

    //MTFS30_DEBUG("保存的命令: %s\n", gut4b0_config_cmd);
    /* 使用卫星模块串口发送配置命令 */
    GNSS_USART_Send(p_cmd);
     
    /* 设置用户发送命令给卫星模块标志 */
    g_gnss_cmd_send_flag = 1;      
    
    /* 等待命令配置结果 */
    OS_SEM_CTR sem = OSSemPend ((OS_SEM *) &g_ut4b0_reply_sem,
                                (OS_TICK ) 2000,
                                (OS_OPT  ) OS_OPT_PEND_BLOCKING,
                                (CPU_TS *) NULL,
                                (OS_ERR *) &err);  
   

    /* 清除发送命令给卫星模块标志 */
    g_gnss_cmd_send_flag = 0; 
    
    if (err == OS_ERR_NONE)
    {
        return OK;
    }
        
    
    /* 等待超时 */
    MTFS30_DEBUG("配置命令(%s)失败: ", g_save_cmd.buf);
    return NG;
}

/*****************************************************************************
 * 函  数:    ut4b0_receiver_cmd_save                                                          
 * 功  能:    保存配置的命令                                                               
 * 输  入:    p_cmd配置指令
 * 输  出:    无                                                    
 * 返回值:    OK: 
 *            NG: 未能正确执行配置指令
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_receiver_cmd_save(u8_t *p_cmd)
{
    u8_t *p = p_cmd;
    g_save_cmd.num = 0;
    strcpy((char *)g_save_cmd.buf, (const char *)p_cmd); /* 保存命令*/

    /* 保存每条命令的储存位置 */
    g_save_cmd.index[0] = 0; /* 第一个命令存储位置 */ 
    g_save_cmd.num = 1;
    while(*p != '\0')
    {
        if(NULL != (p = (u8_t *)strstr((const char *)p, "\r\n")))
        {

            p += 2;
            if (*p != '\0') /* 检测是否为最后一个\r\n */
            {
                g_save_cmd.index[g_save_cmd.num] = p - p_cmd; /* 第2个~最后一个命令 的存储位置 */
                g_save_cmd.num++;
            }
        }
    
    }


}
