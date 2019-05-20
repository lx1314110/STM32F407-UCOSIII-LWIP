/*****************************************************************************/
/* 文件名:    user_cmd.c                                                     */
/* 描  述:    用户命令相关处理                                               */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "user_cmd.h"
#include "mtfs30.h"
#include "Gnss/gnss_handler.h"
#include "Fpga/fpga_handler.h"
#include "Roll/roll_print.h"
#include "Output/out_ptp.h"
#include "Debug/mtfs30_debug.h"





/*******************************************************************************
*command handle.
********************************************************************************/
static user_cmdhandle_t user_cmdhandle[] =
{
    {USER_CMD_SET, (u8_t *)"$DEBUGPRINT",     mtfs30_debug_set_handler},         /* 打开/关闭GNSS模块调试信息 */  
     {USER_CMD_SET, (u8_t *)"$DEBUGLVL",      debug_level_set_handler},         /* 设置DEBUG LEVEL */
    {USER_CMD_SET, (u8_t *)"$ROLLPRINT",      roll_print_set_handler},           /* 打开/关闭循环打印信息     */
    {USER_CMD_SET, (u8_t *)"$SATEPRINT",      gnss_echo_set_handler},            /* 打开/关闭语句回显         */
    {USER_CMD_SET, (u8_t *)"$G-SET-MODE",     gnss_acmode_set_handler},          /* 设置GNSS工作模式          */
    {USER_CMD_SET, (u8_t *)"$G-SET-BAUD",     gnss_serial_baud_set_handler},     /* 设置GNSS串口波特率        */
    {USER_CMD_SET, (u8_t *)"$G-SET-STMT",     gnss_stmt_set_handler},            /* 打开/关闭GNSS输出指定语句 */
 //   {USER_CMD_GET, (u8_t *)"$G-GET-INFO",    gnss_get_info_handler},          /* 查询GNSS当前信息          */ 
    {USER_CMD_SET, (u8_t *)"$FPGA-W",         fpga_write_cmd_handler},           /* 向FPGA写入数据            */
    {USER_CMD_GET, (u8_t *)"$FPGA-R",         fpga_read_cmd_handler},            /* 从FPGA读出数据            */
    {USER_CMD_SET, (u8_t *)"gnss",            gnss_command_execute},             /* gnss command execute      */
    {USER_CMD_GET, (u8_t *)"ipconfig",        ptp_ipconfig_handler},            /* ptp ipconfig execute      */
    {USER_CMD_GET, (u8_t *)"ifconfig",        ptp_ifconfig_handler},            /* ptp ifconfig vlan execute      */    
    {USER_CMD_GET, (u8_t *)"ptp2",            ptp_set_handler},                 /* ptp mode execute      */

};


/*****************************************************************************
 * 函  数:    user_cmd_parser                                                          
 * 功  能:    解析并处理用户命令                                                             
 * 输  入:     *p_cmd: 用户输入的命令信息                        
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void user_cmd_parser(u8_t *p_cmd)
{
    u8_t i = 0;
    u8_t cnt = 0;          /* 命令表中命令条数 */
    u8_t *p_head = NULL;   /* 命令头           */
    u8_t *p_param[MAX_PARAM_NUM] = {NULL}; /* command1         */
    u8_t  tmp_buffer[RTRV_BUF_MAX_SIZE] = {0}; 
 //   u8_t *p_param2 = NULL; /* command2         */
    u8_t p_param_num = 0;
    u8_t inx = 0;
    u8_t len = 0;

    /* 参数检查 */
    if (p_cmd == NULL)
    {
        MTFS30_ERROR("用户输入的命令信息为NULL");
        return;
    }
  
    /* ignore the ' '*/
    while(*p_cmd == ' ')
    {
        p_cmd++;
    }
    
    /* 检查长度 */
    if(strstr((char const *)p_cmd,"\r\n") || strstr((char const *)p_cmd,"\r") 
                                         || strstr((char const *)p_cmd,"\n"))
    p_cmd = (u8_t *)strtok((char *)p_cmd, "\r\n"); /* 去掉后面的\r\n */
    
    len = strlen((const char *)p_cmd);
    MTFS30_DEBUG("########cmd len = %d", len);
    if (len > USER_CMD_MAX_LEN)
    {
        MTFS30_ERROR("command(%s) more than 128 byte!", p_cmd);
        return ;
    }
    
    /* 取出命令头和命令参数 */
    for(inx = 0; inx < len; inx++)
    {
      if(*(p_cmd+inx) == '\0')
        break;
      
      if(*(p_cmd+inx) == ' ')
        p_param_num++;
    }
    
    if(p_param_num >= MAX_PARAM_NUM)
    {
        MTFS30_ERROR("command(%s) format error,Usage(XXXX XXXX)!", p_cmd);
        return;
    }  
    else
        MTFS30_DEBUG("########param num = %d", p_param_num);
    //
    //!command echo
    //
    MTFS30_TIPS("%s", p_cmd);
    p_head = (u8_t *)strtok((char *)p_cmd, " ");
    
    for(inx = 0; inx < p_param_num; inx++)
    {
       p_param[inx] = (u8_t *)strtok(NULL, " ");
       MTFS30_DEBUG("########param = %s", p_param[inx]);
    }
    
    /* 计算命令条数 */
    cnt = sizeof(user_cmdhandle) / sizeof(user_cmdhandle[0]);

    /* 查找指定命令 */
    for (i = 0; i < cnt; i++)
    {
        
		if(strcmp((char *)user_cmdhandle[i].p_head, (char*)p_head) == 0)
		{
			break;
		}
    } 
    
    
    /* 在命令表中没有找到该命令 */
    if (i == cnt)
    {
	    /* 取出输出指令 */
        //MTFS30_ERROR("command(%s) failure!", p_head);        
        
        return;
        
    }

    
//    /* 判断命令类型 */
//    if (user_cmdhandle[i].cmd_type == USER_CMD_SET) /* 设置命令 */
//    {
//        //u8_t set_buf[RTRV_BUF_MAX_SIZE] = {0};
//        p_param[p_param_num] = tmp_buffer;
//        if (OK == user_cmdhandle[i].cmd_fun(p_param_num +1,p_param[0],p_param[1],
//                                                 p_param[2],p_param[3],p_param[4],p_param[5])) /* 设置成功 */
//        {
//            MTFS30_TIPS("success!");
//        }
//        else 
//        {
//            MTFS30_TIPS("failure!");
//        }
//          
//    } 
//    else /* 查询命令 */ 
//    {

        
        p_param[p_param_num] = tmp_buffer;
        if (OK == user_cmdhandle[i].cmd_fun(p_param_num + 1, p_param[0], p_param[1], p_param[2],
                                            p_param[3], p_param[4], p_param[5]))  /* 查询成功 */
            MTFS30_TIPS("%s%s", tmp_buffer, "success!"); 
        
        else
            MTFS30_TIPS("failure!");
}


