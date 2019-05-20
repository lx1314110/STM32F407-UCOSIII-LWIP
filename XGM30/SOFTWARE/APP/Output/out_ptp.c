/*****************************************************************************
* out_ptp config.                          
****************************************************************************/
#include <includes.h>
#include "Config/config.h"
#include "Output/out_ptp.h"
#include "BSP/inc/bsp_spi.h"
#include "mtfs30.h"
#include "Debug/mtfs30_debug.h"
#include "Debug/user_cmd.h"
#include "Util/util_string.h"


/* user command function struct define */
typedef struct  _PTP_PARSER_T_
{
    u8_t  (*cmd_fun)(u8_t *p_param);
} ptp2_query_parse;

typedef struct  _PTP_PARSER_SET_
{
    u8_t  (*cmd_fun)(u8_t *p_param, tConfigParameters *p_sParameters);
} ptp2_set_parse;

/*private function define*/
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

/*ptp2 p subcommand comtent */
const char * sub_net_command[]  = {"-s", "-a", "-m", "-g"};
const char * sub_vlan_command[]  = {"-e", "-p", "-v", "-c"};
const char *p_ptp_port_subcmd[] = {"p", "m", "di", "pi", "ai", "u",
                                                        "f", "t", "s","e","n","si"};

const char *p_ptp_clock_subcmd[] ={"t","q","p","d"};
const char *p_ptp_time_subcmd[] ={"s","l","o"};
const char *p_ptp_esmc_subcmd[] = {"e","s"};
//const char *p_ptp_port_subcmd[] = {"p", "m", "si", "di", "pi", "ai", "u",
//                                                       "d", "f", "t", "s"};

u8_t  sub_netcom_num = sizeof(sub_net_command)/sizeof(const char *);
u8_t  ptp_port_subcmd_num = sizeof(p_ptp_port_subcmd)/sizeof(const char *);
u8_t  ptp_clock_subcmd_num = sizeof(p_ptp_clock_subcmd)/sizeof(const char *);
u8_t  ptp_time_subcmd_num = sizeof(p_ptp_time_subcmd)/sizeof(const char *);
u8_t  ptp_esmc_subcmd_num = sizeof(p_ptp_esmc_subcmd)/sizeof(const char *);
u8_t  ptp_vlan_subcmd_num = sizeof(sub_vlan_command)/sizeof(const char *);

ptp2_set_parse ipconfig_set_psubcmd_parser[] ={ ptp_mac_parser,
                                                ptp_ip_parser,
                                                ptp_mask_parser,
                                                ptp_gateway_parser};

ptp2_set_parse ifconfig_set_psubcmd_parser[] ={ ptp_vlanen_set,
                                                ptp_vlanpcp_set,
                                                ptp_vlanvid_set,
                                                ptp_vlancfi_set};

ptp2_query_parse ipconfig_query_psubcmd_parser[] ={ ptp_macaddr_rtrv,
                                                ptp_ipaddr_rtrv,
                                                ptp_maskaddr_rtrv,
                                                ptp_gateway_rtrv};

ptp2_query_parse ifconfig_query_psubcmd_parser[] ={ ptp_vlanen_rtrv,
                                                ptp_vlanpcp_rtrv,
                                                ptp_vlanvid_rtrv,
                                                ptp_vlancfi_rtrv};

ptp2_set_parse ptp2_set_psubcmd_parser[] =    { ptp_protocol_set,
                                                ptp_mechanism_set,
                                                ptp_delayrinter_set,
                                                ptp_pdelayrinter_set,
                                                ptp_announceinter_set,
                                                ptp_unicast_set,
//                                                ptp_domain_set,
                                                ptp_fretraceable_set,
                                                ptp_timetraceable_set,
                                                ptp_timesource_set,
                                                ptp_porten_set,
                                                ptp_ntpen_set,
                                                ptp_syncinter_set};

ptp2_set_parse ptp2_set_tsubcmd_parser[] =    { ptp_timeformat_set,
                                                NULL,
                                                ptp_utcoffset_set,
                                               };

ptp2_set_parse ptp2_set_csubcmd_parser[] =    { ptp_steptype_set,
                                                ptp_clockquality_set,
                                                ptp_priority_set,
                                                ptp_domain_set
                                                };

ptp2_set_parse ptp2_set_esubcmd_parser[] =    { ptp_esmcen_set,
                                                ptp_esmcssm_set
                                                };

ptp2_query_parse ptp2_query_psubcmd_parser[] ={ ptp_protocol_rtrv,
                                                ptp_mechanism_rtrv,
                                                ptp_delayrinter_rtrv,
                                                ptp_pdelayrinter_rtrv,
                                                ptp_announceinter_rtrv,
                                                ptp_unicast_rtrv,
//                                                ptp_domain_rtrv,
                                                ptp_fretraceable_rtrv,
                                                ptp_timetraceable_rtrv,
                                                ptp_timesource_rtrv,
                                                ptp_porten_rtrv,
                                                ptp_ntpen_rtrv,
                                                ptp_syncinter_rtrv};

ptp2_query_parse ptp2_query_csubcmd_parser[] = {ptp_steptype_rtrv,
                                                ptp_clockquality_rtrv,
                                                ptp_priority_rtrv,
                                                ptp_domain_rtrv
                                                };

ptp2_query_parse ptp2_query_tsubcmd_parser[] =  { ptp_timeformat_rtrv,
                                                NULL,
                                                ptp_utcoffset_rtrv,
                                               };

ptp2_query_parse ptp2_query_esubcmd_parser[] = { ptp_esmcen_rtrv,
                                                ptp_esmcssm_rtrv
                                                };


tPtpSlaveArpTable ptp_slave_arp_table[PTP_SLAVE_IP_TB_LEN];

const char *ipconfig_help = {
"Control and monitor the ptp module\n"\
"Usage: <command>\r\n"                                                                  \
" ipconfig:inquiry about the ptp network information\r\n"\
"         (-s) <mac> the hex of 6 byte,as 001c23fffe17 \r\n"\
"         (-a) <ip address> ip address \r\n"\
"         (-m) <netmask> netmask address \r\n"\
"         (-g) <gateway> gateway address \r\n"\
};

const char *ifconfig_help = {
"Configure ethernet interface\n"\
"Usage: ifconfig <OPTIONS>\r\n"\
"Options\r\n"\
"       vlan\r\n"\
"         (-e)enable <on|off>: Send and receive tagged frames only.\r\n"\
"         (-p)PCP <value>: Send priority-tagged frames, receive untagged and priority tagged frames\r\n"\
"         (-v)VID <value>: Send priority-tagged frames, receive untagged and priority tagged frames\r\n"\
"         (-c)CFI <value>: Canonical format indicator\r\n"
};

const char *ptp2_help = {
"Control and monitor the ptp module\n"\
"Usage: <command>\r\n"\
" ptp2 (p)\r\n"\
"         (p)protocol <eth|udp>:change network protocol.\r\n" \
"         (m)mechanism <e2e|p2p>: change delay mechanism. \r\n"\
"         (si)sync <value>: Change log sync interval.\r\n"\
"         (di)delay <value>: Change min log delay request interval.\r\n"\
"         (pi)pdelay <value>: Change min log pdelay request interval.\r\n"\
"         (ai)announce <value>: change log announce interval. \r\n"\
"         (u)unicast <enable|disable>: Enable/disable unicast operation.\r\n"\
"         (f)frequency <true|false>: set frequency tracable.\r\n"\
"         (t)time <true|false>: set time tracable.\r\n"\
"         (s)source <value>: change time source value.\r\n"\
"         (e)enable <off|on>: change ptp port status.\r\n"\
"         (n)enable <off|on>: change ptp port ntp status.\r\n"\
"      (c)\r\n"\
"         (t)twostep <on|off>: change two-step mode. \r\n"\
"         (q)quality <class>,<accuracy>,<variance>: Change clock quality.\r\n" \
"         (p)priority 1|2 <value>: change priority 1 or 2. \r\n"\
"         (d)domain <number>: Change domain number.\r\n"\
"      (u)\r\n"\
"         (f)filter (s)slave [add|del]: <address>:change acceptable unicast slave table. \r\n"\
"      (g)\r\n"\
"         (c)class <active> <holdover> <free>:Change clock state to clock class mapping. \r\n"\
"      (t)\r\n"\
"         (s) <ptp|arp>: Print or set time in +/-SSSSSSSSS.NNNNNNNNN format or in TAI YYYY-MM-DD HH:MM:SS.NNNNNNNNN format.\r\n" \
"         (l)leap 59|61 <date>: Print UTC leap flags or schedule new leap second.\r\n" \
"         (o)offset <value>:Change UTC offset. \r\n"\
"      (e)\r\n"\
"         (e)esmc <on|off>: Enable ESMC message transmission.\r\n" \
"         (s)ssm Change clock class to SSM code mapping..\r\n"  
};

/*******************************************************************************
*ipconfig:inquiry about the ptp network information
         -s <mac> the hex of 6 byte,as 001c23fffe17 
         -a <ip address> ip address 
         -m <netmask> netmask address 
         -g <gateway> gateway address 
 *******************************************************************************/
u8_t ptp_ipconfig_handler(int num,...)
{
    u8_t *p_param[MAX_PARAM_NUM] = NULL;
    u8_t  err_flag = OK;
    u8_t  inx = 0;
   
    va_list valist;
    va_start(valist, num);
    for(inx = 0; inx < num; inx++)
    {
        p_param[inx] = (u8_t *)va_arg(valist, char *);
        
        if(p_param[inx] == NULL)
          err_flag = NG;
    }
    va_end(valist);
    
    if(err_flag == OK)
    {
      /*ptp ipconfig help command respond*/
       if(0== strncmp((char const*)p_param[0], "help", strlen("help")))
       {
           DEBUG_USART_Send((u8_t *)ipconfig_help, strlen(ipconfig_help));
           goto return_mark; 
       }
       
       if(num == PNET_COMMAND_RTRV-1)
       {
           err_flag = ptp_ipconfig_parser(p_param[0]);
           goto return_mark; 
       }
       
       /*find out sub command "-s" "-a" "-m" "-g"*/
       for(inx = 0; inx < sub_netcom_num; inx++)
       {
           if(0 == strncmp((char const *)p_param[0], sub_net_command[inx],strlen(sub_net_command[inx])))
                  break;
       }
       
       /*find no return */
       if(inx == sub_netcom_num)
       {
           err_flag = NG;
           goto return_mark;
       }
       
       if(num == PNET_COMMAND_RTRV)
            err_flag = ipconfig_query_psubcmd_parser[inx].cmd_fun(p_param[1]);
   
       else if(num == PNET_COMMAND_SET)
            err_flag = ipconfig_set_psubcmd_parser[inx].cmd_fun(p_param[1], &g_sParameters);
       else
            err_flag = NG;
}
    
return_mark:   
    return err_flag; 
}

/*******************************************************************************
*ipconfig:inquiry about the ptp network information
         -s <mac> the hex of 6 byte,as 001c23fffe17 
         -a <ip address> ip address 
         -m <netmask> netmask address 
         -g <gateway> gateway address 
 *******************************************************************************/
u8_t ptp_ifconfig_handler(int num,...)
{
    u8_t *p_param[MAX_PARAM_NUM] = NULL;
    u8_t  err_flag = OK;
    u8_t  inx = 0;
   
    va_list valist;
    va_start(valist, num);
    for(inx = 0; inx < num; inx++)
    {
        p_param[inx] = (u8_t *)va_arg(valist, char *);
        
        if(p_param[inx] == NULL)
          err_flag = NG;
    }
    va_end(valist);
    
    if(err_flag == OK)
    {
      /*ptp ipconfig help command respond*/
       if(0== strncmp((char const*)p_param[0], "help", strlen("help")))
       {
           DEBUG_USART_Send((u8_t *)ifconfig_help, strlen(ifconfig_help));
           goto return_mark; 
       }
       
       if(num != PTP2_COMMAND_RTRV && num != PTP2_COMMAND_SET)
       {
           err_flag = NG;
           goto return_mark; 
       }
       
       if(0 != strncmp((char const *)p_param[0], "vlan", strlen("vlan")))
       {
           err_flag = NG;
           goto return_mark; 
       }
       /*find out sub command "-e" "-p" "-v" "-c"*/
       for(inx = 0; inx < ptp_vlan_subcmd_num; inx++)
       {
           if(0 == strncmp((char const *)p_param[1], sub_vlan_command[inx],strlen(sub_vlan_command[inx])))
                  break;
       }
       
       /*find no return */
       if(inx == ptp_vlan_subcmd_num)
       {
           err_flag = NG;
           goto return_mark;
       }
       
       if(num == PTP2_COMMAND_RTRV)
            err_flag = ifconfig_query_psubcmd_parser[inx].cmd_fun(p_param[num -1]);
   
       else if(num == PTP2_COMMAND_SET)
            err_flag = ifconfig_set_psubcmd_parser[inx].cmd_fun(p_param[num -2], &g_sParameters);
       else
            err_flag = NG;
}
    
return_mark:   
    return err_flag; 
}

 
/*******************************************************************************                                   
 *******************************************************************************/
u8_t ptp_set_handler(int num,...)
{
    u8_t *p_param[MAX_PARAM_NUM] = NULL;
    u8_t  err_flag = OK;
    u8_t  inx;
    
    
    /*uncertain parameter list*/
    va_list valist;
    
    va_start(valist, num);
    for(inx = 0; inx < num; inx++)
    {
        p_param[inx] = (u8_t *)va_arg(valist, char *);
        /*point parameter as null return NG*/
        if(p_param[inx] == NULL)
        err_flag = NG;
    }
    va_end(valist);
    
    /*point parameters is not null,judge the execute fuc*/
    if(err_flag == OK)
    {
      
       /*ptp help ,echo string*/
       if(0== strncmp((char const*)p_param[0], "help", strlen("help")))
       {
          DEBUG_USART_Send((u8_t *)ptp2_help, strlen(ptp2_help));
          goto return_mark; 
       }
       
       /*ptp2 command execute*/

      switch(*p_param[0])
      {
          case 'p':
          {
               for(inx = 0; inx < ptp_port_subcmd_num; inx++)
               {
                  if(0 == memcmp(p_ptp_port_subcmd[inx] ,(char const*)p_param[1], strlen((char const*)p_param[1])))
                      break;
               }
              
               //compare the sub command,if contain success,or not fail. 
               if(inx == ptp_port_subcmd_num)
               {
                  err_flag = NG;
                  MTFS30_ERROR("parameters(%s)error!", p_param[1]);
                  goto return_mark;
               }
               
               //execute the parser rtrv function and return inquery value.
               if(num == PTP2_COMMAND_RTRV)
                  err_flag = ptp2_query_psubcmd_parser[inx].cmd_fun(p_param[num - 1]);
               
               //execute the parser set function and return inquery value.
               else if(num ==PTP2_COMMAND_SET)
                  err_flag = ptp2_set_psubcmd_parser[inx].cmd_fun(p_param[num -2], &g_sParameters);
               
               else
                  err_flag = NG;
          }
         break;
         
         case 'c':
          {
               for(inx = 0; inx < ptp_clock_subcmd_num; inx++)
               {
                  if(0 == strncmp( p_ptp_clock_subcmd[inx] , (char const*)p_param[1], strlen((char const*)p_param[1])))
                      break;
               }
              
               //compare the sub command,if contain success,or not fail. 
               if(inx == ptp_clock_subcmd_num)
               {
                  err_flag = NG;
                  MTFS30_ERROR("parameters(%s)error!", p_param[1]);
                  goto return_mark;
               }
               
               //execute the parser rtrv function and return inquery value.
               if(num == PTP2_COMMAND_RTRV)
                  err_flag = ptp2_query_csubcmd_parser[inx].cmd_fun(p_param[num - 1]);
               
               //execute the parser set function and return inquery value.
               else if(num ==PTP2_COMMAND_SET)
                  err_flag = ptp2_set_csubcmd_parser[inx].cmd_fun(p_param[num -2], &g_sParameters);
               
               else if(num ==PTP2_COMMAND_SUB_SET)
               {
                  if(*p_param[1] == 'p')
                  {
                      if(*p_param[2] == '1' )
                        err_flag = ptp_priority1_set(p_param[3], &g_sParameters);
                        
                      else if(*p_param[2] == '2' )
                        err_flag = ptp_priority2_set(p_param[3], &g_sParameters);
                        
                      else
                        err_flag = NG;
                  }
                  else
                    err_flag = NG;
               }
               else
                  err_flag = NG;
          }
         break;
         
         case 'u':
          {
                  if(*p_param[1] == 'f')
                  {
                      if(*p_param[2] == 's' )
                      {
                          if(num == PTP2_COMMAND_SUB_SET +1)
                          {
                              if(0 == strncmp((const char *)p_param[3], "add", strlen("add")))
                                  err_flag = ptp_ufs_add(p_param[4], &g_sParameters);
                              
                              else if(0 == strncmp((const char *)p_param[3], "del", strlen("del")))
                                  err_flag = ptp_ufs_del(p_param[4], &g_sParameters);
                              
                              else
                                err_flag = NG;
                          }
                          else if(num == PTP2_COMMAND_SET)
                                err_flag = ptp_ufs_rtrv(p_param[num - 1]);
                          
                          else
                                err_flag = NG;
                      }
                      else
                         err_flag = NG;
                  }
                  else
                    err_flag = NG;
             
              
          }
         break;
         
        case 'g':
          {
               
              if(*p_param[1] == 'c')
              {
                  if(num == PTP2_COMMAND_RTRV)
                     err_flag = ptp_clockclass_rtrv(p_param[2]);
                  else if(num == PTP2_COMMAND_SET)
                      err_flag = ptp_clockclass_set(p_param[2], &g_sParameters);
                  else
                      err_flag = NG;
              }   
              else
                err_flag = NG;
          }
         break;
         
         case 't':
          {
               for(inx = 0; inx < ptp_time_subcmd_num; inx++)
               {
                  if(0 == strncmp( p_ptp_time_subcmd[inx] , (char const*)p_param[1], strlen((char const*)p_param[1])))
                      break;
               }
              
               //compare the sub command,if contain success,or not fail. 
               if(inx == ptp_time_subcmd_num)
               {
                  err_flag = NG;
                  MTFS30_ERROR("parameters(%s)error!", p_param[1]);
                  goto return_mark;
               }
               
               if(num == PTP2_COMMAND_RTRV)
               {
                  if(ptp2_query_tsubcmd_parser[inx].cmd_fun == NULL)
                  {
                    err_flag = NG;
                    goto return_mark;
                  }
                     
                  err_flag = ptp2_query_tsubcmd_parser[inx].cmd_fun(p_param[num - 1]);
               }
               
               //execute the parser set function and return inquery value.
               else if(num ==PTP2_COMMAND_SET)
               { 
                  if(ptp2_set_tsubcmd_parser[inx].cmd_fun == NULL)
                  {
                      if(0 == strncmp((const char*)p_param[num - 2], "59", strlen("59")) )  
                        err_flag = ptp_leap59flag_rtrv(p_param[num - 1]);
                      else if(0 == strncmp((const char*)p_param[num - 2], "61", strlen("61")) )  
                        err_flag = ptp_leap61flag_rtrv(p_param[num - 1]);
                      else
                        err_flag = NG;
                  }
                  else
                      err_flag = ptp2_set_tsubcmd_parser[inx].cmd_fun(p_param[num -2], &g_sParameters);
               }
               else if(num == PTP2_COMMAND_SUB_SET)
               { 
                  if(0 == strncmp((const char*)p_param[num - 3], "59", strlen("59")) )  
                        err_flag = ptp_leap59flag_set(p_param[num - 2], &g_sParameters);
                  
                  else if(0 == strncmp((const char*)p_param[num - 3], "61", strlen("61")) )  
                        err_flag = ptp_leap61flag_set(p_param[num - 2], &g_sParameters);
                  
                  else
                        err_flag = NG;
                  
               }
               else
                  err_flag = NG;
          }
         break;
         case 'e':
          {
               for(inx = 0; inx < ptp_esmc_subcmd_num; inx++)
               {
                  if(0 == strncmp( p_ptp_esmc_subcmd[inx] , (char const*)p_param[1], strlen((char const*)p_param[1])))
                      break;
               }
              
               //compare the sub command,if contain success,or not fail. 
               if(inx == ptp_esmc_subcmd_num)
               {
                  err_flag = NG;
                  MTFS30_ERROR("parameters(%s)error!", p_param[1]);
                  goto return_mark;
               }
               
               //execute the parser rtrv function and return inquery value.
               if(num == PTP2_COMMAND_RTRV)
                  err_flag = ptp2_query_esubcmd_parser[inx].cmd_fun(p_param[num - 1]);
               
               //execute the parser set function and return inquery value.
               else if(num ==PTP2_COMMAND_SET)
                  err_flag = ptp2_set_esubcmd_parser[inx].cmd_fun(p_param[num -2], &g_sParameters);
               
               else
                  err_flag = NG;
          }
         break;
      }
         
    }
    
return_mark:   
    return err_flag; 
}

/*******************************************************************************                                   
 *******************************************************************************/
static u8_t  ptp_mac_parser(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t *pstr = NULL;
      u8_t  inx   = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      /*check mac len must be MAC_STR_LIMIT_LEN(12)*/
     if(MAC_STR_LIMIT_LEN != strlen((char *)p_param))
        return NG;       
     
            
      /*check mac digit must range of '0' ~'9', 'a' ~'f', 'A'~'F'*/
      pstr = p_param;
      for(inx = 0; inx < MAC_STR_LIMIT_LEN; inx++)
      {
                if(!isxdigit(*pstr++)) // is not hex
                          break;
      }
     /*check all hex*/
      if(MAC_STR_LIMIT_LEN != inx)
                return NG;
      
    /*string to int value and save  p_sParameters*/
       pstr = p_param;
      for(inx = 0; inx < MAC_DIG_FIX_LEN; inx++)
      {
            p_sParameters->PtpNetParameters.ptp_mac[inx] = (u8_t)StringtoInt( ( char* )pstr);
            pstr += 2;
      }
                                                                             
     for(inx = 0; inx < MAC_DIG_FIX_LEN - 1; inx++)
     {
        /*config fpga reg,ptp mac address*/
        if (NG == SPI_FPGA_ByteWrite(p_sParameters->PtpNetParameters.ptp_mac[inx], PTP_MAC_REG1_ADDR + inx))
        {
              MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_MAC_REG1_ADDR + inx, p_sParameters->PtpNetParameters.ptp_mac[inx]);
              return NG;
        }
     }
      
      
      if (NG == SPI_FPGA_ByteWrite(p_sParameters->PtpNetParameters.ptp_mac[5], PTP_MAC_REG6_ADDR))
      {
            MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_MAC_REG6_ADDR,p_sParameters->PtpNetParameters.ptp_mac[5]);
            return NG;
      }
      return OK;
       
}

/*******************************************************************************
*fuction: u8_t  get_decimalip_value(u8_t *p_param, u32_t *p_value)
*descrip: get long network address from decimal ip string.
*paramet: p_param( decimal ip string point),p_value(long network )
*return:  NG|OK
********************************************************************************/
static u8_t get_decimalip_value(u8_t *p_param,u32_t *p_value)
{
      u8_t *pstr = NULL;
      u8_t  inx   = 0;
      u8_t *pParam[IP_DIG_FIX_NUM] = {NULL};
      u8_t  mark_count = 1;
      u8_t  tmp = 0;
      u32_t lvalue = 0;
    /*check ip len must be lt MAX_IPADDR_LEN*/
     if(MAX_IPADDR_LEN < strlen((char *)p_param))
        return NG;       
    
            
    /*check ip digit must range of '0' ~'9' and '.'*/
      pstr = p_param;
      inx = 0;
      for(; *pstr != '\0' && inx < strlen((char *)p_param); )
      {
                inx++;
                if(*pstr == '.')
                {
                    /*jump the next digit ch*/
                    pstr++;
                    mark_count++;
                }
                if(!isdigit(*pstr++)) // is not digit
                {
                    return NG;
                }
      }
      
      if(mark_count != IP_DIG_FIX_NUM)
         return NG;
     
     /*split the string with '.'*/
      pParam[0] = (u8_t *)strtok((char *)p_param, ".");
      lvalue = (u8_t)atoi((char const *)pParam[0]);
      for(inx = 1; inx < IP_DIG_FIX_NUM; inx++)
      {
          lvalue <<= 8; 
          pParam[inx] = (u8_t *)strtok((char *)NULL, ".");
          tmp = (u8_t)atoi((char const *)pParam[inx]);
          lvalue |= (tmp & 0xff);
      }
      
      *p_value = lvalue;
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_ip_parser(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: config the ptp about ip address.
*paramet: p_param( pass point),p_sParameters(save config point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_ip_parser(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u32_t ulipaddr = 0;
      u8_t  tmp[IP_DIG_FIX_NUM] = {0};
      u8_t  indx = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(NG == get_decimalip_value(p_param, &ulipaddr))
         return NG;
      /*save config parameters*/
      p_sParameters->PtpNetParameters.ptp_ipaddr = ulipaddr;
      
      tmp[0] = (ulipaddr >> 24) &0xff;
      tmp[1] = (ulipaddr >> 16) &0xff;
      tmp[2] = (ulipaddr >> 8) &0xff;
      tmp[3] = ulipaddr & 0xff;
          
      /*write the ptp ip reg */
      for(indx = 0; indx < IP_DIG_FIX_NUM; indx++ )
      {
          if (NG == SPI_FPGA_ByteWrite(tmp[indx], PTP_IP_REG1_ADDR+indx))
          {
              MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_IP_REG1_ADDR+indx, tmp[indx]);
              return NG;
          }
      }
      return OK;  
}

/*******************************************************************************
*fuction: u8_t  ptp_mask_parser(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: config the ptp about subnet mask address.
*paramet: p_param( pass point),p_sParameters(save config point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_mask_parser(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u32_t ulmask = 0;
      u8_t  tmp[IP_DIG_FIX_NUM] = {0};
      u8_t  indx = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      if(NG == get_decimalip_value(p_param, &ulmask))
         return NG;
      /*save config parameters*/
      p_sParameters->PtpNetParameters.ptp_submask = ulmask;
      
      tmp[0] = (ulmask >> 24) &0xff;
      tmp[1] = (ulmask >> 16) &0xff;
      tmp[2] = (ulmask >> 8) &0xff;
      tmp[3] = ulmask & 0xff;
          
      /*write the ptp ip reg */
      for(indx = 0; indx < IP_DIG_FIX_NUM; indx++ )
      {
          if (NG == SPI_FPGA_ByteWrite(tmp[indx], PTP_MASK_REG1_ADDR+indx))
          {
              MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_MASK_REG1_ADDR+indx, tmp[indx]);
              return NG;
          }
      }
      return OK;  
}

/*******************************************************************************
*fuction: u8_t  ptp_gateway_parser(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: config the ptp about gateway address.
*paramet: p_param( pass point),p_sParameters(save config point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_gateway_parser(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u32_t ulgateway = 0;
      u8_t  tmp[IP_DIG_FIX_NUM] = {0};
      u8_t  indx = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(NG == get_decimalip_value(p_param, &ulgateway))
         return NG;
      /*save config parameters*/
      p_sParameters->PtpNetParameters.ptp_gateway = ulgateway;
      
      tmp[0] = (ulgateway >> 24) &0xff;
      tmp[1] = (ulgateway >> 16) &0xff;
      tmp[2] = (ulgateway >> 8)  &0xff;
      tmp[3] = ulgateway&0xff;
      
         
      /*write the ptp ip reg */
      if (NG == SPI_FPGA_ByteWrite(tmp[indx], PTP_GATEWAY_REG1_ADDR+indx))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_MASK_REG1_ADDR, tmp[0]);
          return NG;
      }
      
      for(indx = 1; indx < IP_DIG_FIX_NUM; indx++ )
      {
          if (NG == SPI_FPGA_ByteWrite(tmp[indx], PTP_GATEWAY_REG2_ADDR+indx-1))
          {
              MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_MASK_REG2_ADDR+indx-1, tmp[indx]);
              return NG;
          }
      }
      return OK;  
}


/*******************************************************************************
*fuction: u8_t  ptp_vlanen_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: config the ptp vlan on|off.
*paramet: p_param( pass point),p_sParameters(save config point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_vlanen_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = {0};
  
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(0 == strncmp((const char*)p_param, "off", strlen("off")))
          tmp = 0;
      
      else if(0 == strncmp((const char*)p_param, "on", strlen("on")))
          tmp = 1;
      else
      {
          MTFS30_ERROR("vlan enable value:%d", tmp);
          return NG;
      } 
      /*write the ptp ip reg */
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_VLANEN_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_VLANEN_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;  
}

/*******************************************************************************
*fuction: u8_t  ptp_vlanpcp_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: config the ptp vlan pcp.
*paramet: p_param( pass point),p_sParameters(save config point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_vlanpcp_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = {0};
      u8_t  len = 0;
  
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      len = strlen((char const*)p_param);
      if(len != 1)
        return NG;
      
      if(*p_param < 0x30 || *p_param > 0x37)
        return NG;
      
      tmp = (*p_param - 0x30);
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_VLANPRI_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_VLANPRI_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;  
}

/*******************************************************************************
*fuction: u8_t  ptp_vlancfi_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: config the ptp vlancfi.
*paramet: p_param( pass point),p_sParameters(save config point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_vlancfi_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = {0};
      u8_t  len = 0;
  
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      len = strlen((char const*)p_param);
      if(len != 1)
        return NG;
      
     if(*p_param == '0')
       tmp = 0;
     
     else if(*p_param == '1')
       tmp = 1;
     else
       return NG;
      
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_VLANCFI_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_VLANCFI_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;  
}

/*******************************************************************************
*fuction: u8_t  ptp_vlanvid_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: config the ptp vlanvid.
*paramet: p_param( pass point),p_sParameters(save config point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_vlanvid_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = {0};
      u8_t  len = 0;
      u8_t  indx = 0;
      u16_t temp = 0;
  
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      len = strlen((char const*)p_param);
      if(len < 1 || len > 4)
        return NG;
      
    
      for(indx = 0; indx < len; indx++)
      {
        if(!isdigit(p_param[indx]))
            return NG;
      }
      
      temp = (u16_t)atoi((const char*)p_param);
      if(temp > 4095)
          return NG;
      
      tmp = (temp >> 8)&0x0f;
      
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_VLANVID_REG1_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_VLANVID_REG1_ADDR, tmp);
          return NG;
      }
      
      tmp = temp &0xff;
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_VLANVID_REG2_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_VLANVID_REG2_ADDR, tmp);
          return NG;
      }
      
      return OK;  
}
/*******************************************************************************
*fuction: u8_t  ptp_ipconfig_parser(u8_t *p_param)
*descrip: inquery the ptp about network address.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_ipconfig_parser(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp[IP_DIG_FIX_NUM] = {0};
      u8_t  temp[MAC_DIG_FIX_LEN] = {0};
      u8_t  indx = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*read mac address*/
      for(indx = 0; indx < MAC_DIG_FIX_LEN - 1; indx++)
      {
          if (NG == SPI_FPGA_ByteRead(&temp[indx], PTP_MAC_REG1_ADDR + indx))
          {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_MAC_REG1_ADDR + indx, temp[indx]);
                  return NG;
          }
      }
      if (NG == SPI_FPGA_ByteRead(&temp[5], PTP_MAC_REG6_ADDR))
      {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_MAC_REG6_ADDR, temp[5]);
                  return NG;
      }
      
      sprintf((char *)p_param, "%02x-%02x-%02x-%02x-%02x-%02x\r\n", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]); 
      
      /*read ip address*/
      for(indx = 0; indx < IP_DIG_FIX_NUM; indx++)
      {
          if (NG == SPI_FPGA_ByteRead(&tmp[indx], PTP_IP_REG1_ADDR + indx))
          {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_IP_REG1_ADDR + indx, tmp[indx]);
                  return NG;
          }
      }
      sprintf((char *)p_param, "%s%d.%d.%d.%d\r\n", p_param, tmp[0], tmp[1], tmp[2], tmp[3]);
      
      /*read netmask address*/
       for(indx = 0; indx < IP_DIG_FIX_NUM; indx++)
      {
          if (NG == SPI_FPGA_ByteRead(&tmp[indx], PTP_MASK_REG1_ADDR + indx))
          {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_MASK_REG1_ADDR + indx, tmp[indx]);
                  return NG;
          }
      }
      
      sprintf((char *)p_param, "%s%d.%d.%d.%d\r\n", p_param, tmp[0], tmp[1], tmp[2], tmp[3]);
      
      /*read gateway address*/
      if (NG == SPI_FPGA_ByteRead(&tmp[0], PTP_GATEWAY_REG1_ADDR))
      {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GATEWAY_REG1_ADDR, tmp[0]);
                  return NG;
      }
      for(indx = 1; indx < IP_DIG_FIX_NUM; indx++)
      {
          if (NG == SPI_FPGA_ByteRead(&tmp[indx], PTP_GATEWAY_REG2_ADDR + indx -1))
          {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GATEWAY_REG2_ADDR + indx -1, tmp[indx]);
                  return NG;
          }
      }
      
      sprintf((char *)p_param, "%s%d.%d.%d.%d\r\n", p_param, tmp[0], tmp[1], tmp[2], tmp[3]); 
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_macaddr_rtrv(u8_t *p_param)
*descrip: inquery the ptp about mac address.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_macaddr_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      //u8_t  tmp[IP_DIG_FIX_NUM] = {0};
      u8_t  temp[MAC_DIG_FIX_LEN] = {0};
      u8_t  indx = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      /*read mac address*/
      for(indx = 0; indx < MAC_DIG_FIX_LEN - 1; indx++)
      {
          if (NG == SPI_FPGA_ByteRead(&temp[indx], PTP_MAC_REG1_ADDR + indx))
          {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_MAC_REG1_ADDR + indx, temp[indx]);
                  return NG;
          }
      }
      if (NG == SPI_FPGA_ByteRead(&temp[5], PTP_MAC_REG6_ADDR))
      {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_MAC_REG6_ADDR, temp[5]);
                  return NG;
      }
      
      sprintf((char *)p_param, "%02x-%02x-%02x-%02x-%02x-%02x\r\n", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]); 
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_ipaddr_rtrv(u8_t *p_param)
*descrip: inquery the ptp about ip address.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_ipaddr_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp[IP_DIG_FIX_NUM] = {0};
      //u8_t  temp[MAC_DIG_FIX_LEN] = {0};
      u8_t  indx = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
     
      /*read ip address*/
      for(indx = 0; indx < IP_DIG_FIX_NUM; indx++)
      {
          if (NG == SPI_FPGA_ByteRead(&tmp[indx], PTP_IP_REG1_ADDR + indx))
          {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_IP_REG1_ADDR + indx, tmp[indx]);
                  return NG;
          }
      }
      sprintf((char *)p_param, "%d.%d.%d.%d\r\n", tmp[0], tmp[1], tmp[2], tmp[3]);
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_maskaddr_rtrv(u8_t *p_param)
*descrip: inquery the ptp about subnet mask address.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_maskaddr_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp[IP_DIG_FIX_NUM] = {0};
     // u8_t  temp[MAC_DIG_FIX_LEN] = {0};
      u8_t  indx = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
     
      
      /*read netmask address*/
       for(indx = 0; indx < IP_DIG_FIX_NUM; indx++)
      {
          if (NG == SPI_FPGA_ByteRead(&tmp[indx], PTP_MASK_REG1_ADDR + indx))
          {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_MASK_REG1_ADDR + indx, tmp[indx]);
                  return NG;
          }
      }
      
      sprintf((char *)p_param, "%d.%d.%d.%d\r\n", tmp[0], tmp[1], tmp[2], tmp[3]);
      return OK;
}


/*******************************************************************************
*fuction: u8_t  ptp_gateway_rtrv(u8_t *p_param)
*descrip: inquery the ptp about gateway address.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_gateway_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp[IP_DIG_FIX_NUM] = {0};
      //u8_t  temp[MAC_DIG_FIX_LEN] = {0};
      u8_t  indx = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*read gateway address*/
      if (NG == SPI_FPGA_ByteRead(&tmp[0], PTP_GATEWAY_REG1_ADDR))
      {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GATEWAY_REG1_ADDR, tmp[0]);
                  return NG;
      }
      for(indx = 1; indx < IP_DIG_FIX_NUM; indx++)
      {
          if (NG == SPI_FPGA_ByteRead(&tmp[indx], PTP_GATEWAY_REG2_ADDR + indx -1))
          {
                  MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GATEWAY_REG2_ADDR + indx -1, tmp[indx]);
                  return NG;
          }
      }
      
      sprintf((char *)p_param, "%d.%d.%d.%d\r\n", tmp[0], tmp[1], tmp[2], tmp[3]); 
      return OK;
}


/*******************************************************************************
*fuction: u8_t  ptp_vlanen_rtrv(u8_t *p_param)
*descrip: inquery the vlan enable(on|off).
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_vlanen_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
     
      
      /*read vlan enable address*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_VLANEN_REG_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_VLANEN_REG_ADDR, tmp);
              return NG;
      }
     
      if(tmp == 0)
          sprintf((char *)p_param, "%s\r\n","off");
      
      else if(tmp == 1)
          sprintf((char *)p_param, "%s\r\n","on");
      
      else
      {
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("vlan enable value:%d", tmp);
          return NG;
      }
         
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_vlanpcp_rtrv(u8_t *p_param)
*descrip: inquery the vlan pcp.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_vlanpcp_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
     
      
      /*read vlan enable address*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_VLANPRI_REG_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_VLANPRI_REG_ADDR, tmp);
              return NG;
      }
     
      sprintf((char *)p_param, "%d\r\n",tmp);
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_vlancfi_rtrv(u8_t *p_param)
*descrip: inquery the vlan cfi.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_vlancfi_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
     
      
      /*read vlan enable address*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_VLANCFI_REG_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_VLANCFI_REG_ADDR, tmp);
              return NG;
      }
     
      sprintf((char *)p_param, "%d\r\n",tmp);
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_vlanvid_rtrv(u8_t *p_param)
*descrip: inquery the vid.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_vlanvid_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
      u16_t temp = 0;
     
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
     
      
      /*read vlan enable address*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_VLANVID_REG1_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_VLANVID_REG1_ADDR, tmp);
              return NG;
      }
      
      temp = (tmp & 0x0f)<<8;
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_VLANVID_REG2_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_VLANVID_REG2_ADDR, tmp);
              return NG;
      }
      
      temp |= tmp;
      
     
      sprintf((char *)p_param, "%d\r\n",temp);
      return OK;
}
/* ptp2 (p)
         (s)state: print port state. 
         (p)protocol <eth|udp>:change network protocol.
         (m)mechanism <e2e|p2p>: change delay mechanism. 
         (si)sync <value>: Change log sync interval.
         (di)delay <value>: Change min log delay request interval.
         (pi)pdelay <value>: Change min log pdelay request interval.
         (ai)announce <value>: change log announce interval. 
         (u)unicast <master|disable>: Enable/disable unicast operation.
         (d)domain <value>: change ptp clock domain.
         (f)frequency <enable|disable>: enable frequency tracable.
         (t)time <enable|disable>: enable time tracable.
         (s)source <value>: change time source value.
*/
/*******************************************************************************
*fuction: u8_t  ptp_protocol_rtrv(u8_t *p_param)
*descrip: parser the protocol inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_protocol_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_LAYER_REG_ADDR))
      {
                  MTFS30_ERROR("spi error_address:%#x,value:%#x", PTP_LAYER_REG_ADDR, tmp);
                  return NG;
      }
      
      /*read ptp protocol */
      if(tmp == 0)
          sprintf((char *)p_param, "%s\r\n", "eth");
      
      else if(tmp == 1)
          sprintf((char *)p_param, "%s\r\n", "udp");
      
      else
      {
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("ptp protocol (eth|udp) value:%d", tmp);
          return NG;
      }
      
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_mechanism_rtrv(u8_t *p_param)
*descrip: parser the mechanism inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_mechanism_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_DELAY_REG_ADDR))
      {
                  MTFS30_ERROR("spi error_address:%#x,value:%#x", PTP_LAYER_REG_ADDR, tmp);
                  return NG;
      }
      
      /*read ptp protocol */
      if(tmp == 0)
          sprintf((char *)p_param, "%s\r\n", "e2e"); 
      
      else  if(tmp == 1)
          sprintf((char *)p_param, "%s\r\n", "p2p");
      
      else
      {
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("ptp mechanism (e2e|p2p) value:%d", tmp);
          return NG;
      }
      
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}

const char *p_frequcy[PTP_MESSAGE_FRE_NUM] = {"1/256", "1/128", "1/64", "1/32", "1/16", "1/8", "1/4",
                                              "1/2",   "1",     "2",    "4",    "8",    "16",  "32",
                                              "64"};

/*******************************************************************************
*fuction: u8_t  ptp_syncinter_rtrv(u8_t *p_param)
*descrip: parser the sync interval inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_syncinter_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      s8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      
      if (NG == SPI_FPGA_ByteRead((u8_t *)&tmp, PTP_SYNC_REG_ADDR))
      {
                  MTFS30_ERROR("spi error_address:%#x,value:%#x", PTP_SYNC_REG_ADDR, tmp);
                  return NG;
      }
      
      if(tmp < PTP_MESSAGE_MIN_FRE || tmp > PTP_MESSAGE_MAX_FRE )
      {
                  sprintf((char *)p_param, "%s\r\n", "failure");
                  MTFS30_ERROR("ptp sync fre(%d) error!", tmp);
                  return NG;
      }
      /*read ptp sync frequency */
      sprintf((char *)p_param, "%s\r\n", p_frequcy[PTP_MESSAGE_FRE_OFFSET + tmp]); 
      
      
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_delayrinter_rtrv(u8_t *p_param)
*descrip: parser the delay respond interval inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_delayrinter_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      //s8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      
      /*read ptp delay responds frequency */
      //sprintf((char *)p_param, "%s\r\n", "failure"); 
      
      
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return NG;
}

/*******************************************************************************
*fuction: u8_t  ptp_pdelayrinter_rtrv(u8_t *p_param)
*descrip: parser the pdelay respond interval inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_pdelayrinter_rtrv(u8_t *p_param)
{
    
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      
      /*read ptp pdelay responds frequency */
      //sprintf((char *)p_param, "%s\r\n", "failure"); 
      
      return NG;
}

/*******************************************************************************
*fuction: u8_t  ptp_announceinter_rtrv(u8_t *p_param)
*descrip: parser the announce interval inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_announceinter_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      s8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead((u8_t *)&tmp, PTP_ANNOUNCE_REG_ADDR))
      {
                  MTFS30_ERROR("spi error_address:%#x,value:%#x", PTP_ANNOUNCE_REG_ADDR, tmp);
                  return NG;
      }
      
      if(tmp < PTP_MESSAGE_MIN_FRE || tmp > PTP_MESSAGE_MAX_FRE )
      {
                  sprintf((char *)p_param, "%s\r\n", "failure");
                  MTFS30_ERROR("ptp announce fre(%d) error!", tmp);
                  return NG;
      }
      
      /*read ptp sync frequency */
      sprintf((char *)p_param, "%s\r\n", p_frequcy[PTP_MESSAGE_FRE_OFFSET + tmp]); 
      
      
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}





/*******************************************************************************
*fuction: u8_t  ptp_unicast_rtrv(u8_t *p_param)
*descrip: parser the multicast|unicast inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_unicast_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_UNIMUL_REG_ADDR))
      {
                  MTFS30_ERROR("spi error_address:%#x,value:%#x", PTP_UNIMUL_REG_ADDR, tmp);
                  return NG;
      }
      
      if(0 == tmp)    /*read ptp unicast enable/disable */
          sprintf((char *)p_param, "%s\r\n", "disable"); 
      
      else if(1 == tmp)/*read ptp unicast enable/disable */
          sprintf((char *)p_param, "%s\r\n", "enable");
      
      else
      {
                  sprintf((char *)p_param, "%s\r\n", "failure");
                  MTFS30_ERROR("ptp unicast value:%d", tmp);
                  return NG;
      }  
      
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}


/*******************************************************************************
*fuction: u8_t  ptp_domain_rtrv(u8_t *p_param)
*descrip: parser the clock domain inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_domain_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_DOMAIN_REG_ADDR))
      {
          MTFS30_ERROR("spi error_address:%#x,value:%#x", PTP_DOMAIN_REG_ADDR, tmp);
          return NG;
      }
      
      sprintf((char *)p_param, "%d\r\n", tmp);
     
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_fretraceable_rtrv(u8_t *p_param)
*descrip: parser the frequency traceable inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_fretraceable_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_FLAG_FREQ_TRAC_REG_ADDR))
      {
          MTFS30_ERROR("spi error_address:%#x,value:%#x", PTP_FLAG_FREQ_TRAC_REG_ADDR, tmp);
          return NG;
      }
      
      if(0 == tmp)
          sprintf((char *)p_param, "%s\r\n", "false");
      
      else if(1 == tmp)
          sprintf((char *)p_param, "%s\r\n", "true");
      
      else
      {           
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("ptp frequency traceable:%d", tmp);
          return NG;
      }  
     
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_timetraceable_rtrv(u8_t *p_param)
*descrip: parser the time traceable inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_timetraceable_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*read the time traceable inquery*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_FLAG_TIME_TRAC_REG_ADDR))
      {
          MTFS30_ERROR("spi error_address:%#x,value:%#x", PTP_FLAG_TIME_TRAC_REG_ADDR, tmp);
          return NG;
      }
      
      /*int to string (true|false|failure)*/
      if(0 == tmp)
          sprintf((char *)p_param, "%s\r\n", "false");
      
      else if(1 == tmp)
          sprintf((char *)p_param, "%s\r\n", "true");
      
      else
      {
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("ptp time traceable:%d", tmp);
          return NG;
      }  
     
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_timesource_rtrv(u8_t *p_param)
*descrip: parser the time source inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_timesource_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*read ptp time source*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_TIME_SOURCE_REG_ADDR))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_TIME_SOURCE_REG_ADDR, tmp);
          return NG;
      }
      
      sprintf((char *)p_param, "%d\r\n", tmp);
    
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_porten_rtrv(u8_t *p_param)
*descrip: parser the port enable inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_porten_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*read ptp time source*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_PORTEN_REG_ADDR))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_PORTEN_REG_ADDR, tmp);
          return NG;
      }
       
      /*int to string (true|false|failure)*/
      if(0 == tmp)
          sprintf((char *)p_param, "%s\r\n", "off");
      
      else if(1 == tmp)
          sprintf((char *)p_param, "%s\r\n", "on");
      
      else
      {
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("ptp status:%d", tmp);
          return NG;
      }  
      
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_ntpen_rtrv(u8_t *p_param)
*descrip: parser the ntp enable inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_ntpen_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*read ptp time source*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_NTPEN_REG_ADDR))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_NTPEN_REG_ADDR, tmp);
          return NG;
      }
      
      /*int to string (true|false|failure)*/
      if(0 == tmp)
          sprintf((char *)p_param, "%s\r\n", "off");
      
      else if(1 == tmp)
          sprintf((char *)p_param, "%s\r\n", "on");
      
      else
      {
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("ntp status:%d", tmp);
          return NG;
      }  
    
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}
/* ptp2 c subcommand inquery functions.                                       */
/*******************************************************************************
*fuction: u8_t  ptp_steptype_rtrv(u8_t *p_param)
*descrip: parser the step type inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_steptype_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*read ptp time source*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_STEP_REG_ADDR))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_STEP_REG_ADDR, tmp);
          return NG;
      }
      
      if(0 == tmp)
          sprintf((char *)p_param, "%s\r\n", "off");
      
      else if(1 == tmp)
          sprintf((char *)p_param, "%s\r\n", "on");
      
      else
      {           
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("ptp two step enable:%d", tmp);
          return NG;
      } 
    
      /*print string into serial*/
      //DEBUG_USART_Send(p_param,strlen((char *)p_param));
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_clockquality_rtrv(u8_t *p_param)
*descrip: parser the clock quality inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_clockquality_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t   tmp = 0;
      u8_t   clock_class = 0;
      u8_t   clock_accuracy = 0;
      u16_t  clock_variance = 0;
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*read ptp time source*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_GM_CLK_QUAL_REG_ADDR0))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GM_CLK_QUAL_REG_ADDR0, tmp);
          return NG;
      }
      
      clock_class = tmp;
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_GM_CLK_QUAL_REG_ADDR1))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GM_CLK_QUAL_REG_ADDR1, tmp);
          return NG;
      }
      
      clock_accuracy = tmp;
      
       if (NG == SPI_FPGA_ByteRead(&tmp, PTP_GM_CLK_QUAL_REG_ADDR2))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GM_CLK_QUAL_REG_ADDR2, tmp);
          return NG;
      }
      
      clock_variance = tmp;
      
       if (NG == SPI_FPGA_ByteRead(&tmp, PTP_GM_CLK_QUAL_REG_ADDR3))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GM_CLK_QUAL_REG_ADDR3, tmp);
          return NG;
      }
      clock_variance <<= 8;
      clock_variance |= tmp;
      
      sprintf((char *)p_param, "%02x,%02x,%04x\r\n", clock_class, clock_accuracy, clock_variance);
      
      /*print string into serial*/
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_priority_rtrv(u8_t *p_param)
*descrip: parser the clock prioriiy inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_priority_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t   tmp = 0;
      u8_t   priority1 = 0;
      u8_t   priority2 = 0;
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      
      /*read ptp time source*/
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_GM_PRIO1_REG_ADDR))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GM_PRIO1_REG_ADDR, tmp);
          return NG;
      }
      
      priority1 = tmp;
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_GM_PRIO2_REG_ADDR))
      {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_GM_PRIO2_REG_ADDR, tmp);
          return NG;
      }
      
      priority2 = tmp;
      
      sprintf((char *)p_param, "%d,%d\r\n", priority1, priority2);
      
      /*print string into serial*/
      return OK;
}
//
//ptp2 u f s slave [add|del]
//
/*******************************************************************************
*fuction: u8_t  ptp_ufs_rtrv(u8_t *p_param)
*descrip: parser the unicast filter slave ip table inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_ufs_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u16_t  indx = 0;
      
      u8_t   *p_buffer = NULL;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      for(indx = 0; indx < PTP_SLAVE_IP_TB_LEN; indx++)
      {
          p_buffer = (u8_t *)&ptp_slave_arp_table[indx];
          /*read ptp time source*/
          if (NG == SPI_FPGA_BufferRead(p_buffer, PTP_MTC_START_ADDR + indx, sizeof(tPtpSlaveArpTable)))
          {
              MTFS30_ERROR("spi read_address:%#x,value:%#x,%#x,%#x,%x-%x-%x-%x-%x-%x,%d.%d.%d.%d ", PTP_MTC_START_ADDR + indx, ptp_slave_arp_table[indx].arp_reseved,
                                                   ptp_slave_arp_table[indx].arp_time_out, ptp_slave_arp_table[indx].arp_flag, 
                                                   ptp_slave_arp_table[indx].arp_mac[5], ptp_slave_arp_table[indx].arp_mac[4],
                                                   ptp_slave_arp_table[indx].arp_mac[3], ptp_slave_arp_table[indx].arp_mac[2],
                                                   ptp_slave_arp_table[indx].arp_mac[1], ptp_slave_arp_table[indx].arp_mac[0],
                                                   ptp_slave_arp_table[indx].arp_ip[3], ptp_slave_arp_table[indx].arp_ip[2],
                                                   ptp_slave_arp_table[indx].arp_ip[1], ptp_slave_arp_table[indx].arp_ip[0]);
              return NG;
          }
          
          
          ptp_slave_arp_table[indx].arp_reseved = indx;
          
          if(ptp_slave_arp_table[indx].arp_ip[0] == 0)
            continue;
          
          MTFS30_TIPS("%#x%#x%#x %x-%x-%x-%x-%x-%x %d.%d.%d.%d\r\n",ptp_slave_arp_table[indx].arp_reseved,
                                                   ptp_slave_arp_table[indx].arp_time_out, ptp_slave_arp_table[indx].arp_flag, 
                                                   ptp_slave_arp_table[indx].arp_mac[0], ptp_slave_arp_table[indx].arp_mac[1],
                                                   ptp_slave_arp_table[indx].arp_mac[2], ptp_slave_arp_table[indx].arp_mac[3],
                                                   ptp_slave_arp_table[indx].arp_mac[4], ptp_slave_arp_table[indx].arp_mac[5],
                                                   ptp_slave_arp_table[indx].arp_ip[0], ptp_slave_arp_table[indx].arp_ip[1],
                                                   ptp_slave_arp_table[indx].arp_ip[2], ptp_slave_arp_table[indx].arp_ip[3]);
          
      }
      
      /*print string into serial*/
      return OK;
}


//
//ptp2 g c <active> <holdover> <free>
//
/*******************************************************************************
*fuction: u8_t  ptp_clockclass_rtrv(u8_t *p_param)
*descrip: parser the clock class inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_clockclass_rtrv(u8_t *p_param)
{
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      sprintf((char *)p_param, "%d,%d,%d\r\n", g_sParameters.PtpModeParameters.clock_class[0], 
                                               g_sParameters.PtpModeParameters.clock_class[1],
                                               g_sParameters.PtpModeParameters.clock_class[2]);
      
      /*print string into serial*/
      return OK;
}

//
//ptp2 t s <ptp|arb>
//
/*******************************************************************************
*fuction: u8_t  ptp_timeformat_rtrv(u8_t *p_param)
*descrip: parser the time format (ptp|arb) inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_timeformat_rtrv(u8_t *p_param)
{      
     /*tempority point variable*/
      u8_t tmp = 0;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
       if (NG == SPI_FPGA_ByteRead(&tmp, PTP_FLAG_TIME_SCALE_REG_ADDR))
       {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_FLAG_TIME_SCALE_REG_ADDR, tmp);
              return NG;
       }
      
      if(0 == tmp)
          sprintf((char *)p_param, "%s\r\n", "arb");
      
      else if(1 == tmp)
          sprintf((char *)p_param, "%s\r\n", "ptp");
      
      else
      {           
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("ptp time scale:%d", tmp);
          return NG;
      } 
      
      
      /*print string into serial*/
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_leap59flag_rtrv(u8_t *p_param)
*descrip: parser the leap59 flag inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_leap59flag_rtrv(u8_t *p_param)
{      
     /*tempority point variable*/
      u8_t tmp = 0;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
       if (NG == SPI_FPGA_ByteRead(&tmp, PTP_FLAG_LEAP59_REG_ADDR))
       {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_FLAG_LEAP59_REG_ADDR, tmp);
              return NG;
       }
      
       sprintf((char *)p_param, "%d\r\n", tmp);
      /*print string into serial*/
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_leap61flag_rtrv(u8_t *p_param)
*descrip: parser the leap61 flag inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_leap61flag_rtrv(u8_t *p_param)
{      
     /*tempority point variable*/
      u8_t tmp = 0;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
       if (NG == SPI_FPGA_ByteRead(&tmp, PTP_FLAG_LEAP61_REG_ADDR))
       {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_FLAG_LEAP61_REG_ADDR, tmp);
              return NG;
       }
      
       sprintf((char *)p_param, "%d\r\n", tmp);
      /*print string into serial*/
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_utcoffset_rtrv(u8_t *p_param)
*descrip: parser the utc offset inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_utcoffset_rtrv(u8_t *p_param)
{      
     /*tempority point variable*/
      u8_t tmp = 0;
      u16_t utc_offset = 0;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_UTC_OFFSET_REG_ADDR0))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_UTC_OFFSET_REG_ADDR0, tmp);
              return NG;
      }
      utc_offset = tmp;
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_UTC_OFFSET_REG_ADDR1))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_UTC_OFFSET_REG_ADDR1, tmp);
              return NG;
      }
      utc_offset <<= 8;
      utc_offset |= tmp;
       sprintf((char *)p_param, "%d\r\n", utc_offset);
      /*print string into serial*/
      return OK;
}
//
//esmc set
//
/*******************************************************************************
*fuction: u8_t  ptp_esmcen_rtrv(u8_t *p_param)
*descrip: parser the esmc enable inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_esmcen_rtrv(u8_t *p_param)
{      
     /*tempority point variable*/
      u8_t tmp = 0;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_ESMC_REG_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_ESMC_REG_ADDR, tmp);
              return NG;
      }
      
      if(0 == tmp)
          sprintf((char *)p_param, "%s\r\n", "off");
      
      else if(1 == tmp)
          sprintf((char *)p_param, "%s\r\n", "on");
      
      else
      {           
          sprintf((char *)p_param, "%s\r\n", "failure");
          MTFS30_ERROR("ptp esmc:%d", tmp);
          return NG;
      } 
      /*print string into serial*/
      return OK;
}



/*******************************************************************************
*fuction: u8_t  ptp_esmcssm_rtrv(u8_t *p_param)
*descrip: parser the esmc ssm inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_esmcssm_rtrv(u8_t *p_param)
{      
     /*tempority point variable*/
      u8_t tmp = 0;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead(&tmp, PTP_SSM_REG_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", PTP_SSM_REG_ADDR, tmp);
              return NG;
      }
      
      
     sprintf((char *)p_param, "%02x\r\n", tmp);
          
      /*print string into serial*/
      return OK;
}


//
//! ptp2 p set command fuctions
//
/*******************************************************************************
*fuction: u8_t  ptp_protocol_set(u8_t *p_param)
*descrip: parser the protocol inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/

static u8_t  ptp_protocol_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "eth", strlen("eth")) == 0 )
         p_sParameters->PtpModeParameters.encode_package = tmp = 0;
      
        
      else if(strncmp((char const*)p_param, "udp", strlen("udp")) == 0 )      
         p_sParameters->PtpModeParameters.encode_package = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set protocol (eth|udp) value:%s!", p_param);
          return NG; 
      }
      
      
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_LAYER_REG_ADDR))
      {
                  MTFS30_ERROR("spi write error_address:%#x,value:%#x", PTP_LAYER_REG_ADDR, tmp);
                  return NG;
      }
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_mechanism_set(u8_t *p_param)
*descrip: parser the mechanism inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_mechanism_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "e2e", strlen("e2e")) == 0 )
          p_sParameters->PtpModeParameters.delay_type = tmp = 0;
        
      else if(strncmp((char const*)p_param, "p2p", strlen("p2p")) == 0 )      
          p_sParameters->PtpModeParameters.delay_type = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp mechanism (e2e|p2p) value:%s!", p_param);
          return NG; 
      }
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_DELAY_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_LAYER_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;
}

/*const char *p_frequcy[PTP_MESSAGE_FRE_NUM] = {"1/256", "1/128", "1/64", "1/32", "1/16", "1/8", "1/4",
                                              "1/2",   "1",     "2",    "4",    "8",    "16",  "32",
                                              "64"};*/

/*******************************************************************************
*fuction: u8_t  ptp_syncinter_set(u8_t *p_param)
*descrip: parser the sync interval inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_syncinter_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      s8_t  tmp = 0;
      u8_t  indx =0;
      u8_t  fre_num = sizeof(p_frequcy)/sizeof(const char *);
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      for(indx = 0; indx < fre_num; indx++)
      {
          if( 0 == strncmp((char const*)p_param, p_frequcy[indx], strlen(p_frequcy[indx])))
            break;
      }
      
      /*find no result*/
      if(indx == fre_num)
      {
          MTFS30_ERROR("ptp sync fre%s!", p_param);
          return NG;
      }
      else
      {
          p_sParameters->PtpModeParameters.sync_frequency = indx;
          tmp = (s8_t)indx - PTP_MESSAGE_FRE_OFFSET;
      }
        
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_SYNC_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_SYNC_REG_ADDR, tmp);
          return NG;
      }
     
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_delayrinter_set(u8_t *p_param)
*descrip: parser the delay respond interval inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_delayrinter_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      //s8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
        
      return NG;
}

/*******************************************************************************
*fuction: u8_t  ptp_pdelayrinter_set(u8_t *p_param)
*descrip: parser the pdelay respond interval inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_pdelayrinter_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      //s8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      return NG;
}

/*******************************************************************************
*fuction: u8_t  ptp_announceinter_set(u8_t *p_param)
*descrip: parser the announce interval inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_announceinter_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      s8_t  tmp = 0;
      u8_t  indx =0;
      u8_t  fre_num = sizeof(p_frequcy)/sizeof(const char *);
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      for(indx = 0; indx < fre_num; indx++)
      {
          if( 0 == strncmp((char const*)p_param, p_frequcy[indx], strlen(p_frequcy[indx])))
            break;
      }
      
      /*find no result*/
      if(indx == fre_num)
      {
          MTFS30_ERROR("ptp announce fre%s!", p_param);
          return NG;
      }
      else
      {
          p_sParameters->PtpModeParameters.anounce_frequency = indx;
          tmp = (s8_t)indx - PTP_MESSAGE_FRE_OFFSET;
      }
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_ANNOUNCE_REG_ADDR))
      {
                  MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_ANNOUNCE_REG_ADDR, tmp);
                  return NG;
      }
      
  
      return OK;
}





/*******************************************************************************
*fuction: u8_t  ptp_unicast_set(u8_t *p_param)
*descrip: parser the multicast|unicast inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_unicast_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "disable", strlen("disable")) == 0 )
          p_sParameters->PtpModeParameters.unicast = tmp = 0;
        
      else if(strncmp((char const*)p_param, "enable", strlen("enable")) == 0 )      
          p_sParameters->PtpModeParameters.unicast = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set unicast value:%s!", p_param);
          return NG; 
      }
      
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_UNIMUL_REG_ADDR))
      {
                  MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_UNIMUL_REG_ADDR, tmp);
                  return NG;
      }
      
      return OK;
}


/*******************************************************************************
*fuction: u8_t  ptp_domain_set(u8_t *p_param)
*descrip: parser the clock domain inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_domain_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
      //u8_t  str_len = 0;
      u8_t  indx = 0;
      s32_t temp = 0; 
      
      
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*judge the string is digit*/
      for(indx = 0; indx < strlen((char const*)p_param); indx++)
      {
          if(!isdigit(*(p_param + indx)))
          {
              MTFS30_ERROR("ptp set clock domain value:%s", p_param);
              return NG;
          }
      }
      
      /*judge the range of 0 ~ 255 */
      temp = atoi((char const*)p_param);
             
      if(temp > PTP_BYTE_MAX_VALUE || temp < PTP_BYTE_MIN_VALUE)
      {
           MTFS30_ERROR("ptp set clock domain value:%s", p_param);
              return NG;
      }
      /*save the config parameters*/
      p_sParameters->PtpModeParameters.domain = tmp = (u8_t)temp & 0xff;
      
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_DOMAIN_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_DOMAIN_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_fretraceable_set(u8_t *p_param)
*descrip: parser the frequency traceable inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_fretraceable_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "false", strlen("false")) == 0 )
          p_sParameters->PtpModeParameters.flag_freq_trac = tmp = 0;
        
      else if(strncmp((char const*)p_param, "true", strlen("true")) == 0 )      
          p_sParameters->PtpModeParameters.flag_freq_trac = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set frequency traceable value:%s!", p_param);
          return NG; 
      }
      
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_FLAG_FREQ_TRAC_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_FLAG_FREQ_TRAC_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_timetraceable_set(u8_t *p_param)
*descrip: parser the time traceable inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_timetraceable_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
     
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "false", strlen("false")) == 0 )
          p_sParameters->PtpModeParameters.flag_time_trac = tmp = 0;
        
      else if(strncmp((char const*)p_param, "true", strlen("true")) == 0 )      
          p_sParameters->PtpModeParameters.flag_time_trac = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set time traceable value:%s!", p_param);
          return NG; 
      }
      
      /*read the time traceable inquery*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_FLAG_TIME_TRAC_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_FLAG_TIME_TRAC_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_timesource_set(u8_t *p_param)
*descrip: parser the time source inquery
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_timesource_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
      //u8_t  str_len = 0;
      u8_t  indx = 0;
      s32_t temp = 0; 
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*judge the string is digit*/
      for(indx = 0; indx < strlen((char const*)p_param); indx++)
      {
          if(!isdigit(*(p_param + indx)))
          {
              MTFS30_ERROR("ptp set time source value:%s", p_param);
              return NG;
          }
      }
      
      /*judge the range of 0 ~ 255 */
      temp = atoi((char const*)p_param);
             
      if(temp > PTP_BYTE_MAX_VALUE || temp < PTP_BYTE_MIN_VALUE)
      {
           MTFS30_ERROR("ptp set time source value:%s", p_param);
              return NG;
      }
      /*save the config parameters*/
      p_sParameters->PtpModeParameters.time_source = tmp = (u8_t)temp & 0xff;
      /*read ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_TIME_SOURCE_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_TIME_SOURCE_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;
}


/*******************************************************************************
*fuction: u8_t  ptp_porten_set(u8_t *p_param)
*descrip: parser the port enable set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_porten_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
      //u8_t  str_len = 0;
      //u8_t  indx = 0;
      //s32_t temp = 0; 
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "off", strlen("off")) == 0 )
          p_sParameters->PtpModeParameters.port_enable = tmp = 0;
        
      else if(strncmp((char const*)p_param, "on", strlen("on")) == 0 )      
          p_sParameters->PtpModeParameters.port_enable = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set port enable value:%s!", p_param);
          return NG; 
      }
      /*read ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_PORTEN_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_PORTEN_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_ntpen_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the ntp enable set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_ntpen_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
      //u8_t  str_len = 0;
      //u8_t  indx = 0;
      //s32_t temp = 0; 
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "off", strlen("off")) == 0 )
          p_sParameters->PtpModeParameters.protocol = tmp = 0;
        
      else if(strncmp((char const*)p_param, "on", strlen("on")) == 0 )      
          p_sParameters->PtpModeParameters.protocol = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set ntp enable value:%s!", p_param);
          return NG; 
      }
      /*read ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_NTPEN_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_NTPEN_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_steptype_set(u8_t *p_param , tConfigParameters *p_sParameters)
*descrip: parser the step type set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_steptype_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "off", strlen("off")) == 0 )
          p_sParameters->PtpModeParameters.step_type = tmp = 0;
        
      else if(strncmp((char const*)p_param, "on", strlen("on")) == 0 )      
          p_sParameters->PtpModeParameters.step_type = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set step type value:%s!", p_param);
          return NG; 
      }
      /*write ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_STEP_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_STEP_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;
}
static u8_t get_str_split_value(u8_t *p_param,u8_t *p_value,u8_t num,u8_t base)
{
      u8_t len = 0;
      u8_t split_num = 0;
      u8_t indx = 0;
      char  *p_str[6] = NULL;
      char  *p_end  = NULL;
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      len = strlen((char const*)p_param);
      for(indx = 0; indx < len; indx++)
      {
          if(*(p_param +indx) == ',')
          {
            split_num++;
            indx++;  //jump the next char
          }
          if(base == 16)
          {
            if(!isxdigit(*(p_param +indx)))
              return NG;
          }
          else
          {
            if(!isdigit(*(p_param +indx)))
              return NG;
          }
            
      }
      
      if(split_num + 1 != num)
          return NG;
      
       p_str[0] = strtok((char *)p_param, ",");
      if(p_str[0] == NULL)
          return NG;
      
      if(base == 16)
        p_value[0] = (u8_t)strtoul((char const *)p_str[0], (char **)&p_end, 16);
      else
        p_value[0] = (u8_t)atoi((char const *)p_str[0]);
      
      for(indx = 1; indx < num; indx++)
      {
          p_str[indx] = strtok((char *)NULL, ",");
          if(p_str[indx] == NULL)
            return NG;
          
          if(base == 16)
            p_value[indx] = (u8_t)strtoul((char const *)p_str[indx], (char **)&p_end, 16);
          else
            p_value[indx] = (u8_t)atoi((char const *)p_str[indx]);
      }
      
      return OK;
}
/*******************************************************************************
*fuction: u8_t  ptp_clockquality_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the clock quality set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_clockquality_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u32_t temp = 0;
      u8_t  tmp  = 0;
      u8_t  len  = 0;
      u8_t  split_num = 0;
      u8_t  indx = 0;
      
      char  *p_str1 = NULL;
      char  *p_str2 = NULL;
      char  *p_str3 = NULL;
      char  *p_end  = NULL;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      len = strlen((char const*)p_param);
      for(indx = 0; indx < len; indx++)
      {
          if(*(p_param +indx) == ',')
          {
            split_num++;
            indx++;  //jump the next char
          }
          if(!isxdigit(*(p_param +indx)))
            return NG;
      }
      
      if(split_num != PTP_QULITY_SPLIT_NUM)
        return NG;
      
      p_str1 = strtok((char *)p_param, ",");
      if(p_str1 == NULL)
        return NG;
      
       p_str2 = strtok(NULL, ",");
      if(p_str2 == NULL)
        return NG;
      
       p_str3 = strtok(NULL, ",");
      if(p_str3 == NULL)
        return NG;
      
      tmp = (u8_t)strtoul((char const *)p_str1, (char **)&p_end, 16);
       /*write ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_GM_CLK_QUAL_REG_ADDR0))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_GM_CLK_QUAL_REG_ADDR0, tmp);
          return NG;
      }
      
      tmp = (u8_t)strtoul((char const *)p_str2, (char **)&p_end, 16);
       /*write ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_GM_CLK_QUAL_REG_ADDR1))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_GM_CLK_QUAL_REG_ADDR1, tmp);
          return NG;
      }
      
      temp = strtoul((char const *)p_str3, (char **)&p_end, 16);
      tmp = (temp >> 8) & 0xff;
       /*write ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_GM_CLK_QUAL_REG_ADDR2))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_GM_CLK_QUAL_REG_ADDR2, tmp);
          return NG;
      }
      
      tmp = temp  & 0xff;
       /*write ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_GM_CLK_QUAL_REG_ADDR3))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_GM_CLK_QUAL_REG_ADDR3, tmp);
          return NG;
      }
       
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_priority_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the priority set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_priority_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp  = 0;
      u8_t  len  = 0;
      u8_t  split_num = 0;
      u8_t  indx = 0;
      
      char  *p_str1 = NULL;
      char  *p_str2 = NULL;
      
     // char  *p_end  = NULL;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      len = strlen((char const*)p_param);
      for(indx = 0; indx < len; indx++)
      {
          if(*(p_param +indx) == ',')
          {
            split_num++;
            indx++;  //jump the next char
          }
          if(!isdigit(*(p_param +indx)))
            return NG;
      }
      
      if(split_num != PTP_PRIORITY_SPLIT_NUM)
        return NG;
      
      p_str1 = strtok((char *)p_param, ",");
      if(p_str1 == NULL)
        return NG;
      
       p_str2 = strtok(NULL, ",");
      if(p_str2 == NULL)
        return NG;
    
      p_sParameters->PtpModeParameters.priority1 = tmp = (u8_t)atoi((char const *)p_str1);
       /*write ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_GM_PRIO1_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_GM_PRIO1_REG_ADDR, tmp);
          return NG;
      }
      
      p_sParameters->PtpModeParameters.priority2 =tmp = (u8_t)atoi((char const *)p_str2);
       /*write ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_GM_PRIO2_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_GM_PRIO2_REG_ADDR, tmp);
          return NG;
      }
       
      return OK;
}


/*******************************************************************************
*fuction: u8_t  ptp_priority1_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the priority1 set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_priority1_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp  = 0;
      u8_t  len  = 0;
      u8_t  indx = 0;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      len = strlen((char const*)p_param);
      for(indx = 0; indx < len; indx++)
      {
          if(!isdigit(*(p_param +indx)))
            return NG;
      }
      
      p_sParameters->PtpModeParameters.priority1 = tmp = (u8_t)atoi((char const *)p_param);
       /*write ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_GM_PRIO1_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_GM_PRIO1_REG_ADDR, tmp);
          return NG;
      }
      
       
      return OK;
}
/*******************************************************************************
*fuction: u8_t  ptp_priority2_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the priority2 set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_priority2_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t  tmp  = 0;
      u8_t  len  = 0;
      u8_t  indx = 0;
      
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      len = strlen((char const*)p_param);
      for(indx = 0; indx < len; indx++)
      {
          if(!isdigit(*(p_param +indx)))
            return NG;
      }
      
      p_sParameters->PtpModeParameters.priority2 = tmp = (u8_t)atoi((char const *)p_param);
       /*write ptp time source*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_GM_PRIO2_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_GM_PRIO2_REG_ADDR, tmp);
          return NG;
      }
      
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_ufs_add(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the ufs add
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_ufs_add(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u16_t  indx  = 0;
      u32_t ulipaddr = 0;
     
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      
      /*find ptp slave ip table is space*/
      for(indx = 0; indx < PTP_SLAVE_IP_TB_LEN; indx++)
      {
          if(ptp_slave_arp_table[indx].arp_ip[0] == 0)
                   break;
      }
      /*decimal ip to u32 */
      if(NG == get_decimalip_value(p_param, &ulipaddr))
         return NG;

      /**/ 
      ptp_slave_arp_table[indx].arp_ip[0] = (ulipaddr >>24) & 0xff;
      ptp_slave_arp_table[indx].arp_ip[1] = (ulipaddr >>16) & 0xff;
      ptp_slave_arp_table[indx].arp_ip[2] = (ulipaddr >>8) & 0xff;
      ptp_slave_arp_table[indx].arp_ip[3] = ulipaddr & 0xff;
      ptp_slave_arp_table[indx].arp_mac[0] = 0;
      ptp_slave_arp_table[indx].arp_mac[1] = 0;
      ptp_slave_arp_table[indx].arp_mac[2] = 0; 
      ptp_slave_arp_table[indx].arp_mac[3] = 0;
      ptp_slave_arp_table[indx].arp_mac[4] = 0;
      ptp_slave_arp_table[indx].arp_mac[5] = 0;
     
      ptp_slave_arp_table[indx].arp_reseved = 0;
      ptp_slave_arp_table[indx].arp_time_out = 0;
      ptp_slave_arp_table[indx].arp_flag = 0;
      if (NG == SPI_FPGA_BufferWrite((u8_t *)&ptp_slave_arp_table[indx], PTP_MTC_START_ADDR + indx,sizeof(tPtpSlaveArpTable)))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%d.%d.%d.%d", PTP_MTC_START_ADDR + indx, ptp_slave_arp_table[indx].arp_ip[0],
                                                                                             ptp_slave_arp_table[indx].arp_ip[1],
                                                                                             ptp_slave_arp_table[indx].arp_ip[2],
                                                                                             ptp_slave_arp_table[indx].arp_ip[3]);
          return NG;
      } 
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_ufs_del(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the ufs del
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_ufs_del(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
     // u8_t  tmp  = 0;
      u16_t  indx  = 0;
      u32_t ulipaddr = 0;
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      /*decimal ip to u32 */
      if(NG == get_decimalip_value(p_param, &ulipaddr))
         return NG;
      
      /*find ptp slave ip table is space*/
      for(indx = 0; indx < PTP_SLAVE_IP_TB_LEN; indx++)
      {
          if(ptp_slave_arp_table[indx].arp_ip[3] == (ulipaddr & 0xff) &&
             ptp_slave_arp_table[indx].arp_ip[2] == (ulipaddr>>8 & 0xff)&&
             ptp_slave_arp_table[indx].arp_ip[1] == (ulipaddr>>16 & 0xff)&&
             ptp_slave_arp_table[indx].arp_ip[0] == (ulipaddr>>24 & 0xff))
          {
             ptp_slave_arp_table[indx].arp_ip[0] = 0;
             ptp_slave_arp_table[indx].arp_ip[1] = 0;
             ptp_slave_arp_table[indx].arp_ip[2] = 0;
             ptp_slave_arp_table[indx].arp_ip[3] = 0;
             ptp_slave_arp_table[indx].arp_mac[0] = 0;
             ptp_slave_arp_table[indx].arp_mac[1] = 0;
             ptp_slave_arp_table[indx].arp_mac[2] = 0; 
             ptp_slave_arp_table[indx].arp_mac[3] = 0;
             ptp_slave_arp_table[indx].arp_mac[4] = 0;
             ptp_slave_arp_table[indx].arp_mac[5] = 0;
             
             ptp_slave_arp_table[indx].arp_reseved = 0;
             ptp_slave_arp_table[indx].arp_time_out = 0;
             ptp_slave_arp_table[indx].arp_flag = 0;
             break;
          }
                   
      }
      

      
      if (NG == SPI_FPGA_BufferWrite((u8_t *)&ptp_slave_arp_table[indx], PTP_MTC_START_ADDR + indx,sizeof(tPtpSlaveArpTable)))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%d.%d.%d.%d", PTP_MTC_START_ADDR + indx, ptp_slave_arp_table[indx].arp_ip[0],
                                                                                             ptp_slave_arp_table[indx].arp_ip[1],
                                                                                             ptp_slave_arp_table[indx].arp_ip[2],
                                                                                             ptp_slave_arp_table[indx].arp_ip[3]);
          return NG;
      } 
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_clockclass_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the clock class set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_clockclass_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t num = sizeof(p_sParameters->PtpModeParameters.clock_class);
      u8_t p_value[6] = {0};
      /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      if(NG == get_str_split_value(p_param, p_value, num, 10))
        return NG;
      else
      {
        p_sParameters->PtpModeParameters.clock_class[0] = p_value[0];
        p_sParameters->PtpModeParameters.clock_class[1] = p_value[1];
        p_sParameters->PtpModeParameters.clock_class[2] = p_value[2];
      }
       
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_timeformat_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the time format set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_timeformat_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t tmp = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "arb", strlen("arb")) == 0 )
           p_sParameters->PtpModeParameters.flag_time_scale = tmp = 0;
        
      else if(strncmp((char const*)p_param, "ptp", strlen("ptp")) == 0 )      
           p_sParameters->PtpModeParameters.flag_time_scale = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set time format value:%s!", p_param);
          return NG; 
      }
      /*write ptp time format*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_FLAG_TIME_SCALE_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_FLAG_TIME_SCALE_REG_ADDR, tmp);
          return NG;
      }
       
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_leap59flag_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the leap59flag format set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_leap59flag_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t tmp = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "0", strlen("0")) == 0 )
           p_sParameters->PtpModeParameters.flag_leap59 = tmp = 0;
        
      else if(strncmp((char const*)p_param, "1", strlen("1")) == 0 )      
           p_sParameters->PtpModeParameters.flag_leap59 = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set leap59 flag value:%s!", p_param);
          return NG; 
      }
      /*write ptp time format*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_FLAG_LEAP59_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_FLAG_LEAP59_REG_ADDR, tmp);
          return NG;
      }
       
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_leap61flag_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the leap61flag format set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_leap61flag_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u8_t tmp = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "0", strlen("0")) == 0 )
           p_sParameters->PtpModeParameters.flag_leap61 = tmp = 0;
        
      else if(strncmp((char const*)p_param, "1", strlen("1")) == 0 )      
           p_sParameters->PtpModeParameters.flag_leap61 = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set leap61 flag value:%s!", p_param);
          return NG; 
      }
      /*write ptp time format*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_FLAG_LEAP61_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_FLAG_LEAP61_REG_ADDR, tmp);
          return NG;
      }
       
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_utcoffset_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the utcoffset format set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_utcoffset_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u16_t temp = 0;
      u8_t  tmp  = 0;
      u8_t  len = 0;
      u8_t  indx = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      len = strlen((const char*)p_param);
      for(indx = 0; indx< len; indx++)
      {
          if(!isdigit(*(p_param+indx)))
            return NG;
      }
      
      
      p_sParameters->PtpModeParameters.utc_offset = temp = (u16_t)atoi((char const *)p_param);
      tmp = (temp>>8)&0xff;
      /*write ptp utc offset*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_UTC_OFFSET_REG_ADDR0))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_UTC_OFFSET_REG_ADDR0, tmp);
          return NG;
      }
      
      tmp = temp&0xff;
      /*write ptp utc offset*/
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_UTC_OFFSET_REG_ADDR1))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_UTC_OFFSET_REG_ADDR1, tmp);
          return NG;
      } 
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_esmcen_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the esmcen format set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_esmcen_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u16_t tmp = 0;
      
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
  
      if(strncmp((char const*)p_param, "off", strlen("off")) == 0 )
           p_sParameters->PtpModeParameters.esmc_enable = tmp = 0;
        
      else if(strncmp((char const*)p_param, "on", strlen("on")) == 0 )      
           p_sParameters->PtpModeParameters.esmc_enable = tmp = 1;
      
      else
      {
          MTFS30_ERROR("ptp set esmc enable value:%s!", p_param);
          return NG; 
      }
      
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_ESMC_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_ESMC_REG_ADDR, tmp);
          return NG;
      } 
      return OK;
}

/*******************************************************************************
*fuction: u8_t  ptp_esmcssm_set(u8_t *p_param, tConfigParameters *p_sParameters)
*descrip: parser the esmcen format set
*paramet: point(u8_t)
*return:  NG|OK
********************************************************************************/
static u8_t  ptp_esmcssm_set(u8_t *p_param, tConfigParameters *p_sParameters)
{
     /*tempority point variable*/
      u16_t tmp = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "00", strlen("00")) == 0 )
           p_sParameters->PtpModeParameters.esmc_ssm = tmp = 0x00;
        
      else if(strncmp((char const*)p_param, "02", strlen("02")) == 0 )      
           p_sParameters->PtpModeParameters.esmc_ssm = tmp = 0x02;
      
      else if(strncmp((char const*)p_param, "04", strlen("04")) == 0 )      
           p_sParameters->PtpModeParameters.esmc_ssm = tmp = 0x04;
      
      else if(strncmp((char const*)p_param, "0F", strlen("0F")) == 0 )      
           p_sParameters->PtpModeParameters.esmc_ssm = tmp = 0x0F;
      
      else
      {
          MTFS30_ERROR("ptp set esmc ssm value:%s!", p_param);
          return NG; 
      }
      
      if (NG == SPI_FPGA_ByteWrite(tmp, PTP_SSM_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", PTP_SSM_REG_ADDR, tmp);
          return NG;
      } 
      return OK;
}