old version
1、修改#define PBUF_POOL_SIZE         20//10
2、中断中增加
  CPU_SR_ALLOC(); 
  CPU_CRITICAL_ENTER();                                   
  OSIntEnter(); 
  CPU_CRITICAL_EXIT(); 
  
3、OSIntExit();  
中减短SnmpDataRefresh，临界区的长度。

2019-04-23 LB 
1.tcp_keepalive ->tcp_output_alloc_header (in fuction)：
      tcphdr->wnd = htons(pcb->rcv_ann_wnd);
modified as below:

      tcphdr->wnd = htons(pcb->rcv_ann_wnd + 1);

2.opt.h 
#ifndef LWIP_TCP_KEEPALIVE
#define LWIP_TCP_KEEPALIVE              1
#endif

modified as below.

#ifndef LWIP_TCP_KEEPALIVE
#define LWIP_TCP_KEEPALIVE              0
#endif

3.httpd.c  httpd_init -->httpd_init_addr(in function)

removed:
pcb->so_options |= SOF_KEEPALIVE;

4.tpc.c  tcp_kill_prio(in function)
 if (pcb->prio <= prio &&
  pcb->prio <= mprio &&
(u32_t)(tcp_ticks - pcb->tmr) >= inactivity)

modified as below,resoved the http send RST ACK:

 if (pcb->prio < prio &&
  pcb->prio <= mprio &&
(u32_t)(tcp_ticks - pcb->tmr) >= inactivity)

2019-04-30 LB 
1.modify the usart_debug_task.c and gnss_handler.c

"Control and monitor the gnss module\n"\
"Usage:gnss <command>\r\n"                                                                  \
"Command:\r\n"                                                                              \
"  gnss (m)"\
"        (m) config the work mode of satellite receiver,[[MIX]|[GPS]|[BDS]|[GLO]|[GAL]]\r\n" \
"  gnss (m) inquiry about the working mode of satellite receiver\r\n"                       \
"         (p) [priority] in order is the priority of gps,bd,glonass,galileo\r\n"     \
"       (t) inquiry about the utc time of statellite receiver hhmmss.xx,day,month,year\r\n" \
"       (l) inquiry about the locate information of statellite receiver, hhmmss.xx,day,month,year\r\n"\
"         (s):Query the number of satellite positioning stars\r\n"                          \
"         (t):Query the uct time of satellite receiver,hhmmss.xx\r\n"                       \
"         (i):Query the latitude of satellite positioning,xxxx.xxxxxxxxx,N|S\r\n"           \
"         (o):Query the longitude of satellite positioning,xxxx.xxxxxxxxx,E|W\r\n"          \
"         (a):Query the altitude of satellite positioning\r\n"                              \
"       (p) 1pps status\r\n"                                                                \
"       (a) antenna status\r\n"                                                             \
"       (v) the firm version of satellite receiver\r\n"                                     \
"       (o) the leap value of gps\r\n"                                                      \
"         (p) leap second forecasting hhmmss,day,month,year,next leap,now leap,leap mark\r\n"
2.modify fuction (user_cmd_parser),split the command with " " and modify struct (user_cmdhandle).
modify command_execute as variable parameter function,show below.

u8_t mtfs30_debug_set_handler(int num,...)
{
    va_list valist;
    u8_t *p_param = NULL;
    u8_t err_flag = OK;
    
    
    /*initalize the varlist */
    va_start(valist, num);
    p_param = va_arg(valist, char *);
    if(p_param != NULL)
    {
      if (0 == strncmp(p_param, "OFF", strlen("OFF")))
      {
          /* 关闭调试 */
          g_debug_switch = 0;
      }
      else if (0 == strncmp((char const *)p_param, "ON",strlen("ON")))
      {
          /* 打开调试 */
          g_debug_switch = 1;
      }
      else
      {
          MTFS30_TIPS("参数(%s)有误!\n", p_param);  
          err_flag = NG;
      }
    }
    /*clear valist memory*/
    va_end(valist);
 
    return err_flag; 
}
