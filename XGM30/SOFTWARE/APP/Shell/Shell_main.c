#include <includes.h>
#include "api.h"
#include "shell_main.h"
#include "Snmp\snmp_mib.h"
#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"

#include "fs.h"
#include "ustdlib.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>
#define SHELLSERVER_THREAD_PRIO    0
uint8_t NetbufOut[320];
static uint8_t bLogin = 0;

extern tConfigParameters g_sParameters;
extern tConfigParameters g_sWorkingDefaultParameters;
// updata the sysconfig  Fuc.
static void UpdateSysConfigBuf(uint8_t *temp, uint16_t len, uint8_t *pos)
{
    uint16_t i;
    // loop fill the charactor into the array.
    for (i=0; i<len; i++)
    {
		pos[i] = *temp++;  //copy the size of charactor into the system config buffer. 
    }
    g_sWorkingDefaultParameters = g_sParameters;
}
// set the ipaddr into the sys config Fuc.
static uint32_t GetCfgIPaddr(char *str )
{
    struct ip_addr CfgIP;
    char *ucurl; 
    uint32_t ipaddr = 0;
    ucurl= (char *)str;
    //
    //! get the ip addr from the url.
    //
    CfgIP.addr = inet_addr((char*)ucurl);
    //
    //! if the ipaddr get from the url nozero. 
    if( CfgIP.addr != INADDR_NONE )
    {
         //
         // ! ip addr transmite to the long.
         //
         ipaddr = htonl(CfgIP.addr);
         
    }
    return ipaddr;
}
// set the mac address into the sysconfig mac.
static uint8_t GetCfgMACAddr(char *str,char *CfgMAC)
{
    uint8_t i,i1;
    uint8_t strtemp[MAX_MACSTRING_LEN];
    char *string_tmep;
    
    string_tmep = str;
    for(i=0;i<6;i++)
    {
	  i1=0;
	  while(!((*string_tmep=='\x2E')||(*string_tmep==0)))
          {	
		if(isdigit(*string_tmep)||(*string_tmep=='\x2E'))
			strtemp[i1++]=*string_tmep++;
		else
		{
			return 0;
		}
	   }
			 strtemp[i1]=0;
			 string_tmep++;
			
			 CfgMAC[i]=atoi((char*)strtemp);
                         
   }
   return 1;
 }
//检查输入参数是否合法

uint8_t Checkin(uint8_t* com_in,uint8_t len,uint8_t type)
{
    uint8_t* cp;
    uint16_t i,i1,count;

    if (type == 1)
    {
        //MAC检查

      	if ((len < 11) || (len > 23)) return 0;      //min length of 11MAC，max length of 23MAC.

      	cp = com_in;
      	
	for (i = 0; i < 6; i++)
	{	    
      	    count = 0;
      		
	    for (i1 = 0; *cp && *cp != '.'; cp++)
	    {
	        i1 = i1 * 10 + (*cp - '0');
	            
            	count++;
            	if (count > 3) return 0;
	    }
	        	        
	    if (i1 > 255)  return 0;
            if (count == 0) return 0;        
	        
	    cp++ ;
	}
	    
	return 1;
    }

    if (type == 2)
    {
   	//check the ip and mask.
      	if ((len < 7) || (len > 15)) return 0;      //7 min len，15 max len.

      	cp = com_in;
      	
	for (i = 0; i < 4; i++)
	{	    
      	    count = 0;
      		
	    for (i1 = 0; *cp && *cp != '.'; cp++)
	    {
	        i1 = i1 * 10 + (*cp - '0');
	            
            	count++;
            	if (count > 3) return 0;
	    }
	        	        
	    if (i1 > 255)  return 0;
            if (count == 0) return 0;
	        	        
	    cp++ ;
        }
	    
	return 1;
    }

    return 0;
}

//substring find fuction.
uint8_t isSubString(uint8_t *a, uint8_t *b)
{
    uint8_t *a2, *b2;

    /* Search through string a to find string b */
   	
    for (a2 = a, b2 = b; *a2 && *b2 && !(*a2 ^ *b2); )
        a2++, b2++;
    if (*b2 == 0)
        return 1;
    return 0;
}

// excute the help command.

uint16_t help_exec(uint8_t* name)
{
    uint16_t j;

    j = sprintf((char*)NetbufOut,"\n\r");

    if(isSubString(name,(uint8_t*)"HELP VIEW")||isSubString(name,(uint8_t*)"help view"))
    {
   	j += sprintf((char*)NetbufOut + j, "VIEW---view the device configuration\n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: VIEW \n\r");
   	goto  command_exit;
    }

    if(isSubString(name,(uint8_t*)"HELP RCFG")||isSubString(name,(uint8_t*)"help rcfg"))
    {
   	j += sprintf((char*)NetbufOut + j, "RCFG---restore the device configuration \n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: rcfg\n\r");
        goto  command_exit;
    } 

    if(isSubString(name,(uint8_t*)"HELP HOST")||isSubString(name,(uint8_t*)"help host"))
    {
   	j += sprintf((char*)NetbufOut + j, "HOST--- set the host that receives trap messages \n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: HOST=192.168.0.1 \n\r");
        goto  command_exit;
    } 

    if(isSubString(name,(uint8_t*)"HELP ADDR")||isSubString(name,(uint8_t*)"help addr"))
    {
   	j += sprintf((char*)NetbufOut + j, "ADDR---set the device ip address\n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: addr=192.168.0.18\n\r");
        goto  command_exit;
    } 

    if(isSubString(name,(uint8_t*)"HELP MASK")||isSubString(name,(uint8_t*)"help mask"))
    {
   	j += sprintf((char*)NetbufOut + j, "MASK---set the device mask address\n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: mask=255.255.0.0\n\r");
        goto  command_exit;
    }      

    if(isSubString(name,(uint8_t*)"HELP GATE")||isSubString(name,(uint8_t*)"help gate"))
    {
   	j += sprintf((char*)NetbufOut + j, "GATE---set the network gateway \n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: gate=192.168.0.1\n\r");
	goto  command_exit;
    }

    if(isSubString(name,(uint8_t*)"HELP MADE")||isSubString(name,(uint8_t*)"help made"))
    {
   	j += sprintf((char*)NetbufOut + j, "MADE--- set the vendor id \n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: MADE=xxxx \n\r");
        goto  command_exit;
    }

    if(isSubString(name,(uint8_t*)"HELP MODE")||isSubString(name,(uint8_t*)"help mode"))
    {
   	j += sprintf((char*)NetbufOut + j, "MODE--- set the device id \n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: MODE=xxxx \n\r");
        goto  command_exit;
    }

    if(isSubString(name,(uint8_t*)"HELP NAME")||isSubString(name,(uint8_t*)"help name"))
    {
   	j += sprintf((char*)NetbufOut + j, "NAME--- set the device logical id \n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: NAME=xxxx \n\r");
      	goto  command_exit;
    }

    if(isSubString(name,(uint8_t*)"HELP CONX")||isSubString(name,(uint8_t*)"help conx"))
    {
   	j += sprintf((char*)NetbufOut + j, "CONX--- set the snmp context \n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: CONX=public \n\r");
        goto  command_exit;
    }

    if(isSubString(name,(uint8_t*)"HELP REST")||isSubString(name,(uint8_t*)"help rest"))
    {
   	j += sprintf((char*)NetbufOut + j, "REST--- reset the device \n\r");
   	j += sprintf((char*)NetbufOut + j, "Example: REST \n\r");
        goto  command_exit;
    }

    //j += sprintf((char*)NetbufOut + j, "*** HFC Device Manual ***\n\r");
    j += sprintf((char*)NetbufOut + j, "*   VIEW  RCFG  REST  ADDR   *\n\r");
    j += sprintf((char*)NetbufOut + j, "*   MASK  GATE  CONX  HOST   *\n\r");
    j += sprintf((char*)NetbufOut + j, "*   MADE  MODE  NAME   *\n\r");
    j += sprintf((char*)NetbufOut + j, "*** type \"help xxxx\" for detail ***\n\r");

command_exit:
	return j;
}

//the command excute fuction.
uint16_t command_exec(uint8_t *com_in,uint8_t len,uint8_t netflag)
{
    uint8_t* tempch;
    uint8_t  tempstr[32];
    uint8_t  sendstr[32];
    uint16_t j;
    u32_t U32_IP = 0;
    uint8_t  ucmac[6];
    uint8_t  error = 0;
    OS_ERR err;
    j = sprintf((char*)NetbufOut,"\n\r");

    //view the device information.
    if(isSubString(com_in,(uint8_t*)"VIEW")||isSubString(com_in,(uint8_t*)"view"))
    {
                j += sprintf((char*)NetbufOut + j, "ADDR = %d.%d.%d.%d\n\r",\
                             (g_sParameters.ulStaticIP>>24)&0xff,\
                             (g_sParameters.ulStaticIP>>16)&0xff,\
			     (g_sParameters.ulStaticIP>>8)&0xff,\
                             g_sParameters.ulStaticIP&0xff );

//                len=getNETaddr(GET0STRING_MASK,(uint8 *)sendstr);//
//		*(sendstr+len)='\0';
		j += sprintf((char*)NetbufOut + j,"MASK = %d.%d.%d.%d\n\r",\
                             (g_sParameters.ulSubnetMask>>24)&0xff,\
                             (g_sParameters.ulSubnetMask>>16)&0xff,\
			     (g_sParameters.ulSubnetMask>>8)&0xff,\
                             g_sParameters.ulSubnetMask&0xff );

//		len=getNETaddr(GET0STRING_MACADR,(uint8 *)sendstr);//
//		*(sendstr+len)='\0';
	   	j += sprintf((char*)NetbufOut + j,"MACA = %d.%d.%d.%d.%d.%d\n\r",\
                             g_sParameters.ulMACAddr[0],\
                             g_sParameters.ulMACAddr[1],\
                             g_sParameters.ulMACAddr[2],\
                             g_sParameters.ulMACAddr[3],\
                             g_sParameters.ulMACAddr[4],\
                             g_sParameters.ulMACAddr[5]);    
	
//		len=getNETaddr(GET0STRING_GATEWAY,(uint8 *)sendstr);//
//		*(sendstr+len)='\0';ulDNSAddr
	   	j += sprintf((char*)NetbufOut + j,"GATE = %d.%d.%d.%d\n\r",
                             (g_sParameters.ulGatewayIP>>24)&0xff,\
                             (g_sParameters.ulGatewayIP>>16)&0xff,\
			     (g_sParameters.ulGatewayIP>>8)&0xff,\
                             g_sParameters.ulGatewayIP&0xff );
                
                j += sprintf((char*)NetbufOut + j,"DNS = %d.%d.%d.%d\n\r",
                             (g_sParameters.ulDNSAddr>>24)&0xff,\
                             (g_sParameters.ulDNSAddr>>16)&0xff,\
			     (g_sParameters.ulDNSAddr>>8)&0xff,\
                             g_sParameters.ulDNSAddr&0xff );
	
//		len=getNETaddr(GET0STRING_SENU,(uint8 *)sendstr);//
//		*(sendstr+len)='\0';
	   	j += sprintf((char*)NetbufOut + j,"SERI = %s\n\r",g_sParameters.Serial);
	
//		len=getNETaddr(GET0STRING_FAEN,(uint8 *)sendstr);//Manufacturer
//		*(sendstr+len)='\0';
	   	j += sprintf((char*)NetbufOut + j,"MADE = %s\n\r",g_sParameters.Manufacturer);	           //生产商En
	
//		len=getNETaddr(GET0STRING_DENA,(uint8 *)sendstr);//
//		*(sendstr+len)='\0';
	   	j += sprintf((char*)NetbufOut + j,"MODE = %s\n\r",g_sParameters.Model);			   //设备型号
	
//		len=getNETaddr(GET0STRING_LONA,(uint8 *)sendstr);//
//		*(sendstr+len)='\0';
//	   	j += sprintf((char*)NetbufOut + j,"NAME = %s\n\r",g_sParameters.ucModName);	          //逻辑ID
	
//		len=getNETaddr(GET0STRING_COMU,(uint8 *)sendstr);//
//		*(sendstr+len)='\0';
	   	j += sprintf((char*)NetbufOut + j,"CONX = %s\n\r",g_sParameters.CommunityStr);
	
//		len=getNETaddr(GET0STRING_TRAPIP,(uint8 *)sendstr);//
//		*(sendstr+len)='\0';g_sParameters.RunSecond
                j += sprintf((char*)NetbufOut + j,"POSI = %s\n\r",g_sParameters.InstallPos);              
//INSTALL PERSON                            
                j += sprintf((char*)NetbufOut + j,"POSE = %s\n\r",g_sParameters.InstallPer);
//INSTALL TIME                
                j += sprintf((char*)NetbufOut + j,"INST = %s\n\r",g_sParameters.InstallTime);
//RUN SECONDS                
                j += sprintf((char*)NetbufOut + j,"RUNS = %d\n\r",g_sParameters.RunSecond);
//HOST TRAP SERVICE                
	   	j += sprintf((char*)NetbufOut + j,"HOST = %s\n\r",g_sParameters.TrapService);    
	
//		len=getNETaddr(GET0STRING_SOFT,(uint8 *)sendstr);//
//		*(sendstr+len)='\0';
	   	j += sprintf((char*)NetbufOut + j,"FIRM = %s\n\r","V1.0");
                
	return j;
    }
   	
    memset(tempstr,0,32);      	 // 清缓冲区

    if(isSubString(com_in,(uint8_t*)"REST")||isSubString(com_in,(uint8_t*)"rest"))
    {	
      	
	        CPU_SR_ALLOC();
                CPU_CRITICAL_ENTER();
		NVIC_SystemReset();
		CPU_CRITICAL_EXIT();
    }

    if(isSubString(com_in,(uint8_t*)"RCFG")||isSubString(com_in,(uint8_t*)"rcfg"))
    {
   		ConfigLoadFactory();
                memcpy(&g_sWorkingDefaultParameters.ulMACAddr[0], &g_sParameters.ulMACAddr[0], 6); //MAC Address
                memcpy(g_sWorkingDefaultParameters.Serial,g_sParameters.Serial,13);                
	        ConfigSave();	        
		//ConfigUpdateAllParameters();
		OSTimeDlyHMSM(0, 0,0, 500, OS_OPT_TIME_HMSM_STRICT, &err); 
		CPU_SR_ALLOC();
                CPU_CRITICAL_ENTER(); 
		NVIC_SystemReset();
                CPU_CRITICAL_EXIT();
    }

    //特权用户登陆
    if(isSubString(com_in,(uint8_t*)"LOGIN=")||isSubString(com_in,(uint8_t*)"login="))
    {
//	  if (strncmp((char*)com_in+6, (char *)g_sParameters.shellpasswd,strlen((char*)com_in+6)) == 0)
//      	{
//      	    bLogin = 1;  
//            goto command_exit;
//      	}
   	
    }

    //保存特权密码
    if(isSubString(com_in,(uint8_t*)"ADMIN=")||isSubString(com_in,(uint8_t*)"admin="))
    {
   	if (bLogin)
        {
      	    if (len-7 > 14) goto err;
      	    memcpy(tempstr,com_in+6,len-7);
//            UpdateSysConfigBuf(tempstr,14,g_sParameters.shellpasswd);
      	    ConfigSave();
            goto command_exit;
        }
    }

    //特权用户退出登陆
    if(isSubString(com_in,(uint8_t*)"EXIT")||isSubString(com_in,(uint8_t*)"exit"))
    {
   	if (bLogin)
      	{
            bLogin = 0;
            goto command_exit;
      	}
    } 

    //只有特权用户才能修改MAC地址
    if(isSubString(com_in,(uint8_t*)"MACA=")||isSubString(com_in,(uint8_t*)"maca="))
    {
   	//com_in+5为MAC数据开始，len-6为MAC数据实际长度（LEN中包含结束符\0 ）
   	if (bLogin && Checkin(com_in+5,len-6,1))
      	{
            memcpy(tempstr,com_in+5,len-6);
            error = GetCfgMACAddr((char*)tempstr,(char *)ucmac);
            if(error)
            {
               memmove(g_sParameters.ulMACAddr,ucmac,6);
               g_sWorkingDefaultParameters = g_sParameters;
               ConfigSave();
            }
            goto command_exit;
      	}
    }

    //只有特权用户才能修改序列号
    if(isSubString(com_in,(uint8_t*)"SERI=")||isSubString(com_in,(uint8_t*)"seri="))
    {
   	if (bLogin)
      	{  
      	    if (len-6 != 12) goto err;
      	    memcpy(tempstr,com_in+5,len-6);
      	    UpdateSysConfigBuf(tempstr,12,g_sParameters.Serial);
            ConfigSave();
            goto command_exit;
      	}
    }

    if(isSubString(com_in,(uint8_t*)"ADDR=")||isSubString(com_in,(uint8_t*)"addr="))
    {
        
   	if (!Checkin(com_in+5,len-6,2)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
        U32_IP =GetCfgIPaddr((char *)tempstr);
        if(U32_IP)
        {
          g_sParameters.ulStaticIP = U32_IP;
          g_sWorkingDefaultParameters = g_sParameters;
          ConfigSave();
        }
        
      	goto command_exit;
    }   

    if(isSubString(com_in,(uint8_t*)"MASK=")||isSubString(com_in,(uint8_t*)"mask="))
    {
   	if (!Checkin(com_in+5,len-6,2)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
        U32_IP =GetCfgIPaddr(tempstr);
        if(U32_IP)
        {
          g_sParameters.ulSubnetMask = U32_IP;
          g_sWorkingDefaultParameters = g_sParameters;
          ConfigSave();
        }
        
      	goto command_exit;
    }

    if(isSubString(com_in,(uint8_t*)"GATE=")||isSubString(com_in,(uint8_t*)"gate="))
    {
   	if (!Checkin(com_in+5,len-6,2)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
        U32_IP =GetCfgIPaddr(tempstr);
        if(U32_IP)
        {
          g_sParameters.ulGatewayIP = U32_IP;
          g_sWorkingDefaultParameters = g_sParameters;
          ConfigSave();
        }
      	goto command_exit;
    }
    
    if(isSubString(com_in,(uint8_t*)"DNSA=")||isSubString(com_in,(uint8_t*)"dnsa="))
    {
   	if (!Checkin(com_in+5,len-6,2)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
        U32_IP =GetCfgIPaddr(tempstr);
        if(U32_IP)
        {
          g_sParameters.ulDNSAddr = U32_IP;
          g_sWorkingDefaultParameters = g_sParameters;
          ConfigSave();
        }
      	goto command_exit;
    }

    if(isSubString(com_in,(uint8_t*)"MADE=")||isSubString(com_in,(uint8_t*)"made="))
    {
   	if (len-6 > (DEVICE_INFO_LEN-1)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
      	UpdateSysConfigBuf(tempstr,DEVICE_INFO_LEN,g_sParameters.Manufacturer);

        ConfigSave();
      	goto command_exit;
    }

    if(isSubString(com_in,(uint8_t*)"MODE=")||isSubString(com_in,(uint8_t*)"mode="))
    {
   	if (len-6 > (DEVICE_INFO_LEN-1)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
      	UpdateSysConfigBuf(tempstr,DEVICE_INFO_LEN,g_sParameters.Model);
        
        ConfigSave();
      	goto command_exit;
    }

    if(isSubString(com_in,(uint8_t*)"NAME=")||isSubString(com_in,(uint8_t*)"name="))
    {
   	if (len-6 > (DEVICE_INFO_LEN-1)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
//      	UpdateSysConfigBuf(tempstr,DEVICE_INFO_LEN,g_sParameters.ucModName);

        ConfigSave();
      	goto command_exit;
    }

    if(isSubString(com_in,(uint8_t*)"CONX=")||isSubString(com_in,(uint8_t*)"conx="))
    {
   	if (len-6 > 14) goto err;
      	memcpy(tempstr,com_in+5,len-6);
      	UpdateSysConfigBuf(tempstr,16,g_sParameters.CommunityStr);
        ConfigSave();
      	goto command_exit;
    }
    
    if(isSubString(com_in,(uint8_t*)"POSI=")||isSubString(com_in,(uint8_t*)"posi="))
    {
   	if (len-6 > (DEVICE_INFO_LEN-1)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
      	UpdateSysConfigBuf(tempstr,DEVICE_INFO_LEN,g_sParameters.InstallPos);
        ConfigSave();
      	goto command_exit;
    }
    
    if(isSubString(com_in,(uint8_t*)"POSE=")||isSubString(com_in,(uint8_t*)"pose="))
    {
   	if (len-6 > (DEVICE_INFO_LEN-1)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
      	UpdateSysConfigBuf(tempstr,DEVICE_INFO_LEN,g_sParameters.InstallPer);
        ConfigSave();
      	goto command_exit;
    }
    
     if(isSubString(com_in,(uint8_t*)"INST=")||isSubString(com_in,(uint8_t*)"INST="))
    {
   	if (len-6 > (DEVICE_INFO_LEN-1)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
      	UpdateSysConfigBuf(tempstr,DEVICE_INFO_LEN,g_sParameters.InstallTime);
        ConfigSave();
      	goto command_exit;
    }

    if(isSubString(com_in,(uint8_t*)"HOST=")||isSubString(com_in,(uint8_t*)"host="))
    {
   	if (!Checkin(com_in+5,len-6,2)) goto err;
      	memcpy(tempstr,com_in+5,len-6);
      	UpdateSysConfigBuf(tempstr,16,g_sParameters.TrapService);
        ConfigSave();
     	goto command_exit;
    }
	
   	
    if(isSubString(com_in,(uint8_t*)"HELP")||isSubString(com_in,(uint8_t*)"help"))
    {
   	 j = help_exec(com_in);
     //if (netflag) goto command_exit;
     return j;
    }

err:
    j += sprintf((char*)NetbufOut + j,"Error command\n\r");
            
    if (netflag) j = sprintf((char*)NetbufOut,"Error command");
	
    return j;

command_exit:
    j += sprintf((char*)NetbufOut + j,"OK\n\r");
            
    if (netflag) j = sprintf((char*)NetbufOut,"OK");
	
    return j;
}

void Task_SHELL(void  *pdata) 
{
	pdata = pdata;
	//shellInit();
	while(1)
	{
		shell_Maintask();	
		
	}
}
//extern unsigned char IPAddress[];
void shell_Maintask(void)
{
        struct netconn *conn;
	struct netbuf *Outbuf,*Inbuf;
	struct ip_addr *raddr;
	uint16_t rport;
	uint16_t len;
	uint8_t *NetbufIn;
	uint8_t *tp;
        err_t err,recv_err;
        conn = netconn_new(NETCONN_UDP);     	/* 创建UDP连接  */
        err = netconn_bind(conn,NULL,6002);     /* 绑定本地地址和监听的端口号 */
        if (err == ERR_OK)
        {
          while(1)
          {
                          recv_err = netconn_recv(conn,&Inbuf);
                          if((recv_err == ERR_OK)&&(Inbuf != NULL))
                          {    
                                  /* Process the incomming connection. */
                                  len = netbuf_len(Inbuf);
                                  if(len >= 300)//丢掉过长的数据 
                                  {
                                          netbuf_delete(Inbuf);
                                          continue;
                                  }
                                  //-------------------------------------------------
                                  netbuf_data(Inbuf, (void*)&NetbufIn, (uint16_t *)&len);
                                  rport = netbuf_fromport(Inbuf);
                                  raddr = netbuf_fromaddr(Inbuf);
                                  
 
                                  NetbufIn[len] = 0;
                                  len =  command_exec(NetbufIn,len+1,1);
                                  
                                  Outbuf = netbuf_new();
                                  tp = netbuf_alloc(Outbuf, len);
                                  memcpy(tp,NetbufOut,len);
                                          
                                  netconn_sendto(conn, Outbuf, raddr, rport);
                                  netbuf_delete(Inbuf);
                                  netbuf_delete(Outbuf);
                                  //-------------------------------------------------
                          }
              }
        }
	
}
void Shell_Server_Task(void)
{
  OS_ERR err;
  sys_thread_new("SHELL", Task_SHELL, NULL, DEFAULT_THREAD_STACKSIZE, SHELLSERVER_THREAD_PRIO);

}