/*****************************************************************************/
/* 文件名:    gnss_receiver.c                                                */
/* 描  述:    卫星接收机相关处理                                             */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "Gnss/gnss_receiver.h"
#include "BSP/inc/bsp_usart.h"
#include "Gnss/ut4b0_receiver.h"
#include "Debug/mtfs30_debug.h"

/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
gnss_recviver_info_t    g_rev_info;                /* GNSS接收机信息     */ 
gnss_leap_t             g_leap;                    /* 闰秒               */
gnss_info_t             g_gnss_info;               /* GNSS信息           */
u8_t                    g_gnss_echo_switch;        /* 卫星语句回显开关   */
u8_t                    g_gnss_init_finished_flag; /* 卫星初始化完成标志 */
u8_t                    g_gnss_cmd_send_flag;      /*  卫星命令发送标志  */


/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/



/*****************************************************************************
 * 函  数:    gnss_receiver_init                                                           
 * 功  能:    卫星接收机初始化                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void gnss_receiver_init(void)
{

 
    BSP_Sleep(10000);// 10000/* 等待卫星模块启动 */  
    
	memset(&g_rev_info, 0x00, sizeof(gnss_recviver_info_t));
   
	/* 取得接收机类型 */
#ifdef TODO	
	g_rev_info.rec_type  = fpga_read(REV_TYPE);
#endif
    
    g_rev_info.rec_type = REV_TYPE_UT4B0;

	switch(g_rev_info.rec_type)
	{
		case REV_TYPE_UT4B0: /* ut4b0类型接收机 */
			/* 接收机型号名称 */
			strncpy((char *)g_rev_info.rec_name, "UT4B0", 5);
            
			/* ut4b0接收机初始化 */
			ut4b0_receiver_init();
			break;

		default:
			MTFS30_ERROR("接收机类型有错, 接收机类型为[%d]", g_rev_info.rec_type);

			break;
	}
    
    
    BSP_Sleep(10); /* 等待卫星模块初始完毕 */

    /* 卫星初始化完成 */
    g_gnss_init_finished_flag = 1; 
    
      
    MTFS30_TIPS("GNSS模块初始化完毕!\n");  
}



/*****************************************************************************
 * 函  数:    gnss_receiver_msg_parser                                                           
 * 功  能:    解析卫星数据                                                                 
 * 输  入:     *pdata: 卫星数据                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
void gnss_receiver_msg_parser(u8_t *pdata)
{

	/* 判断接收机类型,各类型的接收机输出数据指令可能不一样 */	 
	switch(g_rev_info.rec_type)
	{
		case REV_TYPE_UT4B0:    /* UT4B0接收机 */
			/* 解析ut4b0接收机输出的卫星数据 */
			ut4b0_receiver_msg_parser(pdata);
			break;

		default:
			break;

	}

}


/*****************************************************************************
 * 函  数:    gnss_receiver_acmode_set                                                           
 * 功  能:    配置接收机工作模式                                                                
 * 输  入:    p_mode: 指向工作模式  
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/ 
u8_t gnss_receiver_acmode_set(u8_t *p_mode)
{
    u8_t i = 0; 
    u8_t acmode = 0;        
    u8_t const *p_mode_tbl[5] = {"MIX", "GPS", "BDS", "GLO", "GAL"}; /* MODE参数表 */
    
    /* 在MODE参数表中查找 */
    for (i = 0; i < 5; i++)
    {
        if (0 == strncmp((char const *)p_mode, (char const *)p_mode_tbl[i], strlen((char const*)p_mode_tbl[i]))) 
        {
            acmode = i;
            break;
        }
    }
    
    /* 没有找到 */
    if (i >= 5)
    {
        MTFS30_TIPS("参数(%s)有误", p_mode);
        return NG;     
    }
      
   g_sParameters.GnssParameters.mode = i + 1;
   MTFS30_TIPS("gnss work mode %s", p_mode_tbl[i]); 
	/* 判断接收机类型 */	 
	switch(g_rev_info.rec_type)
	{
		case REV_TYPE_UT4B0:    /* UT4B0接收机 */
			/* 解析ut4b0接收机输出的卫星数据 */
			if (NG == ut4b0_receiver_acmode_set(acmode))
            {
                MTFS30_DEBUG("配置UT4B0接收机工作模式失败");
                return NG;
            }
			break;

		default:
			break;

	}   
    
    
    /* 保存工作模式信息 */
    strcpy((char *)g_gnss_info.mode.info, (char const *)p_mode);
    MTFS30_DEBUG("当前工作模式:%s\n", g_gnss_info.mode.info);
    
    return OK;
}


/*****************************************************************************
 * 函  数:    gnss_receiver_serial_baud_set                                                          
 * 功  能:    配置接收机串口波特率                                                                 
 * 输  入:    *p_baud: 波特率
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8_t gnss_receiver_serial_baud_set(u8_t *p_baud)
{
    
    u8_t i = 0;
    /* 支持的的波特率表 */
    u8_t const *p_buad_tbl[7] = {"9600", "19200", "38400", "57600", "115200", "230400", "460800"}; 
	
    
    /* 在表中查找指定波特率 */
    for (i = 0; i < 7; i++)
    {
        if (0 == strncmp((char *)p_baud, (char *)p_buad_tbl[i], strlen((char const*)p_buad_tbl[i])))
        {
           break;
        }
          
    }
    
    /* 没有找到 */
    if (i == 7)
    {
       MTFS30_TIPS("参数(%s)有误!\n", p_baud); 
       return NG;
        
    }
    

    /* 区分接收机类型 */
	switch(g_rev_info.rec_type)
	{
		case REV_TYPE_UT4B0:    /* UT4B0接收机 */
			/* 配置UT4B0接收机串口波特率 */
            if (NG == ut4b0_receiver_serial_baud_set(p_baud))
            {
                MTFS30_DEBUG("ut4b0_receiver_serial_baud_set() ERROR! p_baud(%s)", p_baud);
                return NG;
            }
			break;

		default:
			break;
	}	
    
    
    return OK;
}

/*****************************************************************************
 * 函  数:    gnss_receiver_echo_onoff_set                                                          
 * 功  能:    打开/关闭指定语句的回显功能                                                                 
 * 输  入:    p_param:命令参数
 * 输  出:    无                                                    
 * 返回值:    OK:设置成功;NG：设置失败                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8_t gnss_receiver_echo_set(u8_t *p_param)
{

    if (0 == strcmp((char const *)p_param, "ON"))
    {
        /* 打开卫星语句回显 */
        g_gnss_echo_switch = 1;   

    }
    else if (0 == strcmp((char const *)p_param, "OFF"))
    {
        /* 关闭卫星语句回显 */ 
        g_gnss_echo_switch = 0;
    }
    else
    {
        MTFS30_TIPS("参数(%s)有误!\n", p_param);
        return NG;
    }
    
    
    return OK;
}


/*****************************************************************************
 * 函  数:    gnss_receiver_stmt_set                                                           
 * 功  能:    打开/关闭指定语句输出                                                                
 * 输  入:    p_param  : 命令参数
 * 返回值:    OK: 设置成功; NG：设置失败                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
u8_t gnss_receiver_stmt_set(u8_t *p_param)
{

    u8_t i = 0;
    const u8_t *p_stmt_tbl[7] = {"ZDA", "GGA", "GSV", "UTC", "PPS", "ANT", "VER"}; /* 语句表 */
    u8_t on_off_flag = 0; /* 打开/关闭标志 */
    u8_t *p_stmt = NULL;  /* 语句          */
    u8_t *p_data = NULL;  /* 设置值        */
    
    
    /* 取出语句和设置值 */
    p_stmt = (u8_t *)strtok((char *)p_param, ":");
    p_data = (u8_t *)strtok(NULL, ";");
     
   
    /* 在回显语句表中查找指定语句 */
    for (i = 0; i < 7; i++)
    {
        if (0 == strncmp((char *)p_stmt, (char *)p_stmt_tbl[i], 
                                         strlen((char const *)p_stmt_tbl[i])))
        {
            break;
        }
    }

    /* 没有找到 */
    if (i == 7)
    {
        MTFS30_TIPS("参数(%s)有误!\n", p_stmt);
        return NG;
    }
    
    
    if (0 == strncmp((char const *)p_data, "ON", strlen("ON"))) 
    { 
        on_off_flag = 1;
    }
    else if (0 == strncmp((char const *)p_data, "OFF", strlen("OFF")))
    {
        on_off_flag = 0;
    }
    else
    {
        MTFS30_TIPS("参数(%s)有误!\n", p_data);
        return NG;
    }
    
  
    /* 区分接收机类型 */
    switch(g_rev_info.rec_type)
    {
	case REV_TYPE_UT4B0:    /* UT4B0接收机 */
	    /* 打开/关闭ut4b0接收机指定语句的输出 */
	    if (NG == ut4b0_receiver_statement_set(i, on_off_flag))
        {
            MTFS30_DEBUG("ut4b0_receiver_statement_set() ERROR! i= %d, on_off_flag = %d", i, on_off_flag);
            return NG;
        }
	    break;

	default:
	    break;
    }	 

    
    return OK;   
}



/*****************************************************************************
 * 函  数:    gnss_receiver_get_info                                                         
 * 功  能:    查询GNSS模块指定信息                                                               
 * 输  入:    p_param:  指定参数                      
 * 输  出:    p_rtrv: 查询结果                                                 
 * 返回值:    OK: 查询成功; NG：查询失败                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
u8_t gnss_receiver_get_info(u8_t *p_param, u8_t *p_rtrv)
{

    u8_t i = 0;
    const u8_t *p_get_tbl[8] = {"MODE", "TIME", "LOC", "SAT", "LEAP", "PPS", "ANT", "VER"}; /* 查询命令参数表 */

    
    /* 在参数表中查找指定参数 */
    for (i = 0; i < 8; i++)
    {
         if (0 == strcmp((char *)p_param, (char *)p_get_tbl[i]))
         {
             break;
         }
    }
    
    /* 没有找到 */
    if (i >= 8)
    {        
        MTFS30_TIPS("参数(%s)有误!\n", p_param);
        return NG;        
    }
    
      
    /* 区分查询信息 */
    switch(i)
    {
    case 0: /* 工作模式信息 */
        strcpy((char *)p_rtrv, (char const *)g_gnss_info.mode.info);
        break;
        
    case 1: /* 时间信息 */
        strcpy((char *)p_rtrv, (char const *)g_gnss_info.time.info);
        break;        
        
    case 2: /* 定位信息 */
        strcpy((char *)p_rtrv, (char const *)g_gnss_info.loc.info);
        break;
        
    case 3: /* 卫星信息 */
        strcpy((char *)p_rtrv, (char const *)g_gnss_info.sat.info);
        break;
        
    case 4: /* 闰秒信息 */
        strcpy((char *)p_rtrv, (char const *)g_gnss_info.leap.info);
        break;
        
    case 5: /* 1PPS状态信息 */    
        strcpy((char *)p_rtrv, (char const *)g_gnss_info.pps.info);
        break;        
    
    case 6: /* 天线状态信息 */    
        strcpy((char *)p_rtrv, (char const *)g_gnss_info.ant.info);
        break;
        
    case 7: /* GNSS版本信息 */
        strcpy((char *)p_rtrv, (char const *)g_gnss_info.ver.info);
        break;
        
    default:
        break;
    
    }
    

    return OK;
}


/*****************************************************************************
 * 函  数:    gnss_receiver_get_sat_info                                                         
 * 功  能:    查询卫星信息                                                               
 * 输  入:    无                  
 * 输  出:    p_rtrv: 查询结果                                                 
 * 返回值:    无                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
//void gnss_receiver_get_sat_info(u8_t *p_rtrv)
//{
//    
//    u8_t i = 0;
//    u8_t j = 0;
//    u16_t len = 0;
//    u16_t slen = 0;
//    u8_t *p_sat_tbl[4] = {"GPS", "BDS", "GLO", "GAL"};
//    
//    
//    
//  
//    /* GPS,BDS,GLO,GAL 4种卫星类型 */
//    for (i = 0; i < 4; i++)
//    {
//        /* 保存卫星类型和该类型的可视卫星参数到查询结果 */
//        slen = sprintf((char *)&p_rtrv[len], "%s,%02d,", p_sat_tbl[i], g_gnss_info.sat.amount[i]);
//        len += slen;
//               
// 
//        /* 保存某类型的卫星信息到查询结果 */
//        for (j = 0; j < g_gnss_info.sat.amount[i]; j++)
//        {
//            slen = sprintf((char *)&p_rtrv[len], "%d,%02d,%02d,%02d,", g_gnss_info.sat.info[i][j].enable, g_gnss_info.sat.info[i][j].prn, 
//                    g_gnss_info.sat.info[i][j].elev, g_gnss_info.sat.info[i][j].snr);
//            len += slen;
//        }
//
//
//        p_rtrv[len] = '\0';
//        
//    }
//    
//        /* 以";"结束 */
//        p_rtrv[len-1] = ';';
//    
//    
//    MTFS30_DEBUG("获取的卫星信息长度: %d", len);
//
//
//}

/*****************************************************************************
 * 函  数:    gnss_receiver_info_clear                                                         
 * 功  能:    清除保存的GNSS信息                                                               
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void gnss_receiver_info_clear(void)
{


    /* 时间信息 */
    strcpy((char *)g_gnss_info.time.info, "000000.00,00,00,0000;");

    /* 定位信息 */
    strcpy((char *)g_gnss_info.loc.info, "00,000000.00,0000.00000000,N,00000.00000000,E,000.0000;");

    /* 卫星信息 */
    //memset(&g_gnss_info.sat, 0x00, sizeof(g_gnss_info.sat));
    strcpy((char *)g_gnss_info.sat.info, "GPS,00;BDS,00;GLO,00;GAL,00;");
    g_gnss_info.sat.len = 0;

    /* 闰秒信息 */
    strcpy((char *)g_gnss_info.leap.info, "235960,31,12,2016,18,18,00;");

    /* 1PPS状态信息 */
    strcpy((char *)g_gnss_info.pps.info, "0;"); 

    /* 天线状态信息 */
    strcpy((char *)g_gnss_info.ant.info, "ON;");

    /* 版本信息 */
    strcpy((char *)g_gnss_info.ver.info, "UT4B0,000000000000000;");

}


