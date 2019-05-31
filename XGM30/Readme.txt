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


2019-5-6
1.add fuction  LVL_DEBUG(fmt,args...) as level to printf information.
enum {
	DBG_DISABLE             = 0x00,
        DBG_EMERGENCY		= 0x01,	//system is unusable
	DBG_ALERT		= 0x02, //immediate action needed
	DBG_CRITICAL		= 0x03,	//critical conditions
	DBG_ERROR		= 0x04, //error conditions
	DBG_WARNING		= 0x05, //warning conditions
	DBG_NOTICE		= 0x06, //normal but significant condition
	DBG_INFORMATIONAL	= 0x07, //informational messages
	DBG_DEBUG 		= 0x08 	//debug level messages
};

in the same time,add command as below,
{USER_CMD_SET, (u8_t *)"$DEBUGLVL",      debug_level_set_handler},         /* 设置DEBUG LEVEL */


2.modify  gnss_command_execute fuction,add command as below:
 const char *gnss_help = {
"Control and monitor the gnss module\n"\
"Usage:gnss <command>\r\n"                                                                  \
"Command:\r\n"                                                                              \
"  gnss (m)"\
"        (m) config the work mode of satellite receiver,[[MIX]|[GPS]|[BDS]|[GLO]|[GAL]]\r\n" \
"  gnss (m) inquiry about the working mode of satellite receiver\r\n"                       \
"         (p) [priority] in order is the priority of gps,bd,glonass,galileo\r\n"     \
"         (e) [enable|disable]\r\n" \
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
"         (p) leap second forecasting hhmmss,day,month,year,next leap,now leap,leap mark\r\n"\
"       (g) inquiry about the satellite information\r\n"\
"         (g) Query about the gps satellite information\r\n"\
"         (b) Query about the bds satellite information\r\n"\
"         (l) Query about the glonass satellite information\r\n"\
"         (a) Query about the galileo satellite information\r\n"\
"       (d) inquiry the compensation of delay information\r\n"\
"         (c) [compensation] the compensation of delay information,range -1000000 ~ 1000000ns\r\n"  
};
3.add check_gnss_available function, corrected till satellite pps,serial and 1pps status ok.

2019-5-20
1.add ptp2 help in out_ptp.c.

ipconfig help
Control and monitor the ptp module
                                  Usage: <command>
 ipconfig:inquiry about the ptp network information
         (-s) <mac> the hex of 6 byte,as 001c23fffe17 
         (-a) <ip address> ip address 
         (-m) <netmask> netmask address 
         (-g) <gateway> gateway address 
         
         
ifconfig help
Configure ethernet interface
                            Usage: ifconfig <OPTIONS>
Options
       vlan
         (-e)enable <on|off>: Send and receive tagged frames only.
         (-p)PCP <value>: Send priority-tagged frames, receive untagged and priority tagged frames
         (-v)VID <value>: Send priority-tagged frames, receive untagged and priority tagged frames
         (-c)CFI <value>: Canonical format indicator

Control and monitor the ptp module
                                  Usage: <command>
 ptp2 (p)
         (p)protocol <eth|udp>:change network protocol.
         (m)mechanism <e2e|p2p>: change delay mechanism. 
         (si)sync <value>: Change log sync interval.
         (di)delay <value>: Change min log delay request interval.
         (pi)pdelay <value>: Change min log pdelay request interval.
         (ai)announce <value>: change log announce interval. 
         (u)unicast <enable|disable>: Enable/disable unicast operation.
         (f)frequency <true|false>: set frequency tracable.
         (t)time <true|false>: set time tracable.
         (s)source <value>: change time source value.
         (e)enable <off|on>: change ptp port status.
         (n)enable <off|on>: change ptp port ntp status.
      (c)
         (t)twostep <on|off>: change two-step mode. 
         (q)quality <class>,<accuracy>,<variance>: Change clock quality.
         (p)priority 1|2 <value>: change priority 1 or 2. 
         (d)domain <number>: Change domain number.
      (u)
         (f)filter (s)slave [add|del]: <address>:change acceptable unicast slave table. 
      (g)
         (c)class <active> <holdover> <free>:Change clock state to clock class mapping. 
      (t)
         (s) <ptp|arp>: Print or set time in +/-SSSSSSSSS.NNNNNNNNN format or in TAI YYYY-MM-DD HH:MM:SS.NNNNNNNNN format.
         (l)leap 59|61 <date>: Print UTC leap flags or schedule new leap second.
         (o)offset <value>:Change UTC offset. 
      (e)
         (e)esmc <on|off>: Enable ESMC message transmission.
         (s)ssm Change clock class to SSM code mapping..
2.add fuctions in out_ptp.c

static u8_t  ptp_mac_parser(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_ip_parser(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_mask_parser(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_gateway_parser(u8_t *p_param, tConfigParameters *p_sParameters);

static u8_t  ptp_ipconfig_parser(u8_t *p_param);
static u8_t  ptp_macaddr_rtrv(u8_t *p_param);
static u8_t  ptp_ipaddr_rtrv(u8_t *p_param);
static u8_t  ptp_maskaddr_rtrv(u8_t *p_param);
static u8_t  ptp_gateway_rtrv(u8_t *p_param);

static u8_t  ptp_vlanvid_rtrv(u8_t *p_param);
static u8_t  ptp_vlancfi_rtrv(u8_t *p_param);
static u8_t  ptp_vlanpcp_rtrv(u8_t *p_param);
static u8_t  ptp_vlanen_rtrv(u8_t *p_param);

static u8_t  ptp_vlanvid_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_vlancfi_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_vlanpcp_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_vlanen_set(u8_t *p_param, tConfigParameters *p_sParameters);

static u8_t  ptp_protocol_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_mechanism_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_syncinter_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_delayrinter_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_pdelayrinter_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_announceinter_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_unicast_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_domain_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_timetraceable_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_fretraceable_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_timesource_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_porten_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_ntpen_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_steptype_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_clockquality_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_priority_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_priority1_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_priority2_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_ufs_add(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_ufs_del(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_clockclass_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_timeformat_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_leap59flag_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_leap61flag_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_utcoffset_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_esmcen_set(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t  ptp_esmcssm_set(u8_t *p_param, tConfigParameters *p_sParameters);


static u8_t  ptp_protocol_rtrv(u8_t *p_param);
static u8_t  ptp_mechanism_rtrv(u8_t *p_param);
static u8_t  ptp_syncinter_rtrv(u8_t *p_param);
static u8_t  ptp_delayrinter_rtrv(u8_t *p_param);
static u8_t  ptp_pdelayrinter_rtrv(u8_t *p_param);
static u8_t  ptp_announceinter_rtrv(u8_t *p_param);
static u8_t  ptp_unicast_rtrv(u8_t *p_param);
static u8_t  ptp_domain_rtrv(u8_t *p_param);
static u8_t  ptp_timetraceable_rtrv(u8_t *p_param);
static u8_t  ptp_fretraceable_rtrv(u8_t *p_param);
static u8_t  ptp_timesource_rtrv(u8_t *p_param);
static u8_t  ptp_porten_rtrv(u8_t *p_param);
static u8_t  ptp_ntpen_rtrv(u8_t *p_param);
static u8_t  ptp_steptype_rtrv(u8_t *p_param);
static u8_t  ptp_clockquality_rtrv(u8_t *p_param);
static u8_t  ptp_priority_rtrv(u8_t *p_param);
static u8_t  ptp_ufs_rtrv(u8_t *p_param);
static u8_t  ptp_clockclass_rtrv(u8_t *p_param);
static u8_t  ptp_timeformat_rtrv(u8_t *p_param);
static u8_t  ptp_leap59flag_rtrv(u8_t *p_param);
static u8_t  ptp_leap61flag_rtrv(u8_t *p_param);
static u8_t  ptp_utcoffset_rtrv(u8_t *p_param);
static u8_t  ptp_esmcen_rtrv(u8_t *p_param);
static u8_t  ptp_esmcssm_rtrv(u8_t *p_param);

3. modify the ptp_set_handler and add ipconfig\ ifconfig\ptp2 command. 


2019-5-31

1.add out help in out_pps_tod.c
const char *pps_help = {
"Control and monitor the 1PPS+TOD or 1PPS or IRIGB module\n"\
"Usage: <command>\r\n"\
" out (b)\r\n"\
"         (c)compensation <value>:change IRIGB compensation.\r\n" \
"         (l)leap <value>:change IRIGB leap precaution, 00 ~ 11.\r\n" \
"         (d)daylingt <value>:change IRIGB daylight precaution, 00 ~ 11.\r\n" \
"         (z)zone <value>:change IRIGB time zone,utc+0 ~ utc+12 and utc-11 ~ utc-1.\r\n" \
"         (q)quality <value>:change IRIGB quality zone.\r\n" \
" out (p)\r\n"\
"         (c)compensation <value>:change 1pps+tod compensation. \r\n"\
"         (t)type <value>: Change tod info contain status message.\r\n"\
"         (s)status <value>: Change pps status of tod message.\r\n"\
"         (j)Jitter <value>: Change jitter magnitude of tod message. \r\n"\
"         (k)clock <value>: Change clock type of tod message.\r\n"\
"         (l)lock <value>: Change lock mode of tod message.\r\n"\
"         (a)alarm <value>: Change clock source alarm.\r\n"\
" out (s)\r\n"\
"         (t)type <value>:change out signal type, gnss-pps|sys-pps|dc-irigb|2mhz|10mhz. \r\n"
};

2.add irigb and pps tod set fuctions

static u8_t out_set_irigb_dlycom(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_irigb_leapPreflag(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_irigb_daylight(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_irigb_timeskew(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_irigb_clockquality(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_ppstod_dlycom(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_tod_type(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_pps_status(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_tod_jitter(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_clock_type(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_clock_status(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_monitor_alarm(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_singal_type(u8_t *p_param, tConfigParameters *p_sParameters);

static u8_t out_irigb_dlycom_rtrv(u8_t *p_param);
static u8_t out_irigb_leapPreflag_rtrv(u8_t *p_param);
static u8_t out_irigb_daylight_rtrv(u8_t *p_param);
static u8_t out_irigb_timeskew_rtrv(u8_t *p_param);
static u8_t out_irigb_clockquality_rtrv(u8_t *p_param);
static u8_t out_ppstod_dlycom_rtrv(u8_t *p_param);
static u8_t out_pps_status_rtrv(u8_t *p_param);
static u8_t out_tod_type_rtrv(u8_t *p_param);
static u8_t out_tod_jitter_rtrv(u8_t *p_param);
static u8_t out_clock_type_rtrv(u8_t *p_param);
static u8_t out_clock_status_rtrv(u8_t *p_param);
static u8_t out_monitor_alarm_rtrv(u8_t *p_param);
static u8_t out_singal_type_rtrv(u8_t *p_param);

3.add u8_t out_pps_handler(int num,...),description as the pps tod and irigb tod. 



