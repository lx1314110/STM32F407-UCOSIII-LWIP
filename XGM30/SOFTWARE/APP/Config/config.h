//*****************************************************************************
//
// config.h - Configuration of the serial to Ethernet converter.
//
// Copyright (c) 2008 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 3740 of the RDK-S2E Firmware Package.
//
//*****************************************************************************

#ifndef __CONFIG_H__
#define __CONFIG_H__

//*****************************************************************************
//
//! \addtogroup config_api
//! @{
//
//*****************************************************************************
typedef unsigned char tBoolean;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif
//*****************************************************************************
//
//! The number of serial to Ethernet ports supported by this module.
//
//*****************************************************************************
#define MAX_S2E_PORTS           1

//*****************************************************************************
//
// The length of the ucModName array in the tConfigParameters structure.  NOTE:
// Be extremely careful if changing this since the structure packing relies
// upon this values!
//
//*****************************************************************************
#define DEFAULT_STR_LEN         10
#define MOD_NAME_LEN            30
#define URL_NAME_LEN            64
#define BREAU_PD_LEN            6
#define DEVICE_INFO_LEN         32
#define GNSS_PRIO_LEN           4



//*****************************************************************************
//
//! This structure contains the PTP module parameters that are saved to flash.
//! A copy exists in RAM for use during the execution of the application, which
//! is loaded from flash at startup.  The modified parameter block can also be
//! written back to flash for use on the next power cycle.
//
//*****************************************************************************
#pragma pack(2)
typedef struct
{
    //
    //! The enable to be used for the GNSS,specifiled the rb to track.
    //
    unsigned char enable;       //1

    //
    //! The mode for GNSS,indicate as 1 : mix 2:GPS 3:BD 4:GLO 5:GAL
    //
    unsigned char mode;         //2

    //
    //! The priority to be use for the GNSS, specified as an MIX MODE.
    //! value for GPS,BD,GLO,GAL. 
    unsigned char priority[8]; //6

    //
    //! The delaycom for gnss mode,-32767
    //
    long delaycom;    //8

}
tGNSSParameters;

//*****************************************************************************
//
//! This structure contains the ptp module net parameters that are saved to flash.
//! A copy exists in RAM for use during the execution of the application, which
//! is loaded from flash at startup.  The modified parameter block can also be
//! written back to flash for use on the next power cycle.
//
//*****************************************************************************

typedef struct
{
    //
    //! The mac for ptp module.
    //
    unsigned char ptp_mac[6];       //6

    //
    //! The ip address for ptp module.
    //
    unsigned long ptp_ipaddr;       //4

    //
    //! The subnet mask for the ptp module.
    // 
    unsigned long ptp_submask;      //14

    //
    //! The gate way for the ptp module.
    //
    unsigned long  ptp_gateway;     //18
    
    //
    //! The enable to be used for vlan.
    //
    unsigned char vlan_enable;
    
    //
    //! The priority code point for vlan,indicate as 0 ~ 7.
    //
    unsigned char vlan_cfi;
    
    //
    //! The priority code point for vlan,indicate as 0 ~ 7.
    //
    unsigned char vlan_pcp;
    
    //
    //! The identifier for vlan, 12bit 0 1 0xfff reserved, 4093 use.
    //
    unsigned short valn_vid;        //4
}
tPtpNetParameters;

//*****************************************************************************
//
//! This structure contains the ptp module mode parameters that are saved to flash.
//! A copy exists in RAM for use during the execution of the application, which
//! is loaded from flash at startup.  The modified parameter block can also be
//! written back to flash for use on the next power cycle.
//
//*****************************************************************************

typedef struct
{
    //
    //! The port for ptp module.
    //
    unsigned char port;           //1

    //
    //! The status for ptp module.
    //
    unsigned char status[12];    //13

    
    
    //
    //! The sync frequency for ptp modules.
    //
    unsigned char sync_frequency;//20
    
    //
    //! The anounce frequency for ptp modules.
    //
    unsigned char anounce_frequency; //21
    
    
    //
    //! The domain for ptp modules.
    //
    unsigned char domain;           //22
    
    
    //
    //! The ptp out protocol for ptp modules.
    //
    unsigned char protocol;         //23
    
    //
    //! The ptp out delay compensate for ptp modules.
    //
    signed long delay_com;         //27
    
    //
    //! The priority 1 for ptp modules.
    //
    unsigned char priority1;         //28
    
    //
    //! The priority 2 for ptp modules.
    //
    unsigned char priority2;         //28
    
    unsigned short  port_enable:1;                /* The esmc enable for the ptp module.*/
    unsigned short  esmc_enable:1;                /* The esmc enable for the ptp module.*/
    unsigned short delay_type:1;                  /*The delay type for the ptp module*/
    unsigned short unicast:1;                     /* The unicast for the ptp module*/
    unsigned short encode_package:1;              /* The encode package for the ptp module*/
    unsigned short step_type:1;                   /* The step for ptp modules*/
    
    unsigned short flag_secu:1;                  /* PTP security,default false*/
    unsigned short flag_ps2:1;                   /* PTP profile Specific 2,default false  */
    unsigned short flag_ps1:1;                   /* PTP profile Specific 1,default false  */ 
    unsigned short flag_alt_master:1;            /* alternateMasterFlag ,default false    */
    unsigned short flag_freq_trac:1;             /* frequencyTraceable      */
    unsigned short flag_time_trac:1;             /* timeTraceable           */
    unsigned short flag_time_scale:1;            /* ptpTimescale,default true*/
    unsigned short flag_cur_utc_offset_valid:1;  /* currentUtcOffsetValid,default true   */
    unsigned short flag_leap59:1;                /* leap59                  */
    unsigned short flag_leap61:1;                /* leap61                  */
    unsigned short steps_rm;                     /* stepsRemoved            */
    unsigned int   gm_clk_qual;                  /* grandmasterClockQuality */
    unsigned char  time_source;                  /* timeSource              */
    unsigned char  esmc_ssm;                     /* ssm                     */
    unsigned char  clock_class[3];               /*<active> <holdover> <free> class value*/
    unsigned short utc_offset;
    unsigned char  ntp_en:1;
    unsigned char reserved:7;
}
tPtpModeParameters;


//*****************************************************************************
//
//! This structure contains the ptp module mtc ip arp table that are saved to flash.
//! A copy exists in RAM for use during the execution of the application, which
//! is loaded from flash at startup.  The modified parameter block can also be
//! written back to flash for use on the next power cycle.
//
//*****************************************************************************

typedef struct
{
  
   //
    //! The ip arp table,reserved.
    //
    unsigned short arp_reseved:13;      //13
    
    //
    //! The ip arp table,arp time ount counter.
    //
    unsigned short arp_time_out:2;      //2
    //
    //! ptp ip arp table,arp flag.
    //
    unsigned short arp_flag:1;           //1
   
    
    //
    //! the ip arp table,mac address.
    //
    unsigned char arp_mac[6];
    
    //
    //! the ip arp table,ip address.
    //
    unsigned char arp_ip[4];    
}
tPtpSlaveArpTable;
//*****************************************************************************
//
//! This structure contains the ptp module mode parameters that are saved to flash.
//! A copy exists in RAM for use during the execution of the application, which
//! is loaded from flash at startup.  The modified parameter block can also be
//! written back to flash for use on the next power cycle.
//
//*****************************************************************************
typedef struct
{
    //
    //! The type of detect port, value 0 system 1pps, 1 gnss 1pps,2 DC irigb, 3 2Mhz 4 10Mhz.
    //
     unsigned char decttype;       //1 

    //
    //! The delay compensation of system 1pps. 
    //
    signed long sys_pps_dlycom;   //5

    //
    //! The delay compensation of 1pps tod out.
    // 
    signed long pps_tod_dlycom;  //10

    //
    //! The delay compensation for irigb.
    //
    signed long irigb_dlycom;    //14
    
    //
    //! The duty of 1pps.
    //
    unsigned char pps_duty;     //15
    
    //
    //! the out protocol of telecom electric.
    //
    unsigned char protocol;     //16
    
    //
    //! the switch of optical.
    //
    unsigned char optical_sw;   //17
    
    //
    //! the switch of detect port.
    //
    unsigned char detect_sw;   //18
     
}
tOutParameters;

#pragma pack()
//*****************************************************************************
//
//! This structure contains the S2E module parameters that are saved to flash.
//! A copy exists in RAM for use during the execution of the application, which
//! is loaded from flash at startup.  The modified parameter block can also be
//! written back to flash for use on the next power cycle.
//
//*****************************************************************************
#pragma pack(2)
typedef struct
{
    //
    //! The sequence number of this parameter block.  When in RAM, this value
    //! is not used.  When in flash, this value is used to determine the
    //! parameter block with the most recent information.
    //
    unsigned char ucSequenceNum;  

    //
    //! The CRC of the parameter block.  When in RAM, this value is not used.
    //! When in flash, this value is used to validate the contents of the
    //! parameter block (to avoid using a partially written parameter block).
    //
    unsigned char ucCRC;         

    //
    //! The version of this parameter block.  This can be used to distinguish
    //! saved parameters that correspond to an old version of the parameter
    //! block.
    //
    unsigned char ucVersion;    

    //
    //! Character field used to store various bit flags.
    //
    unsigned char ucFlags;     

    //
    //! The TCP port number to be used for access to the UPnP Location URL that
    //! is part of the discovery response message.
    //
    unsigned short usLocationURLPort; 

    //
    //! Padding to ensure consistent parameter block alignment.
    //
    unsigned char ucReserved1[2];     

    //
    //! The MAC address .
    //
    unsigned char ulMACAddr[6]; 
    
    //
    //! The static IP address to use if DHCP is not in use.
    //
    unsigned long ulStaticIP;             

    //
    //! The default gateway IP address to use if DHCP is not in use.
    //
    unsigned long ulGatewayIP;           

    //
    //! The subnet mask to use if DHCP is not in use.
    //
    unsigned long ulSubnetMask;         
    //
    //! The DNS ADDRESS.
    //
    unsigned long ulDNSAddr; 
    //
    //! Padding to ensure the whole structure is 256 bytes long.
    //
    unsigned char userpasswd[16]; 
  
    //
    //! SNMP .
    //
    unsigned char     CommunityStr[16];		
    unsigned char     TrapService[URL_NAME_LEN];
    unsigned long     TrapPort;		
    
    //
    //! Web Port .
    //	     
    unsigned short     webport;
    
    //
    //!Device Info
    //	
    unsigned char      Model[DEVICE_INFO_LEN];	
    unsigned char      Manufacturer[DEVICE_INFO_LEN];
    unsigned char      Serial[12];		
    unsigned char      InstallPos[DEVICE_INFO_LEN];
    unsigned char      InstallPer[DEVICE_INFO_LEN];
    unsigned char      InstallTime[DEVICE_INFO_LEN]; 
    unsigned long      RunSecond;
    
    //
    //!updata flag
    //
    unsigned char      updata_flag[8];
    
    //
    //!GNSS parameters.
    //
    tGNSSParameters    GnssParameters;
    
    //
    //!PTP net parameters.
    //
    tPtpNetParameters  PtpNetParameters;
    
    //
    //!PTP mode parameters.
    //
    tPtpModeParameters PtpModeParameters;
    
    //
    //!PTP mode parameters.
    //
    tOutParameters OutParameters;
    
}
tConfigParameters;
#pragma pack()

//*****************************************************************************
//
//! If this flag is set in the ucFlags field of tConfigParameters, the module
//! uses a static IP.  If not, DHCP and AutoIP are used to obtain an IP
//! address.
//
//*****************************************************************************
#define CONFIG_FLAG_STATICIP            0x01

//*****************************************************************************
//
//! If this flag is set in the ucFlags field of tConfigParameters, the module
//! Enable trap.  If not, snmp trap disable.
//! address.
//
//*****************************************************************************
#define CONFIG_EN_TRAP    		0x02

//
// the max length of the mac address.
//
#define MAX_MACSTRING_LEN    19

//*****************************************************************************
//
// IP Address Acquisition Modes
//
//*****************************************************************************
#define IPADDR_USE_STATIC       0
#define IPADDR_USE_DHCP         1
#define IPADDR_USE_AUTOIP       2

//*****************************************************************************
//
// A flag to the main loop indicating that it should update the IP address
// after a short delay (to allow us to send a suitable page back to the web
// browser telling it the address has changed).
//
//*****************************************************************************
extern tBoolean g_bChangeIPAddress;

//*****************************************************************************
//
// Prototypes for the globals exported from the configuration module, along
// with public API function prototypes.
//
//*****************************************************************************
extern tConfigParameters g_sParameters;
extern const tConfigParameters *g_psDefaultParameters;
extern const tConfigParameters *const g_psFactoryParameters;
extern void ConfigInit(void);
extern void ConfigLoadFactory(void);
extern void ConfigLoad(void);
extern void ConfigSave(void);
extern void ConfigWebInit(void);
extern void ConfigUpdateIPAddress(void);
extern void wdogFeed(void);
extern void ConfigPreUpdateIPAddress(void);
extern void SystemSave(void);

#endif 
