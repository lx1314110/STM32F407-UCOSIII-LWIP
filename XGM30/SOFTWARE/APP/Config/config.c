
#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "fs.h"
#include "ustdlib.h"
#include "config.h"
#include "lwip.h"
#include "Gnss/gnss.h"

#include "Sntp/sntp.h"
//#include "Control/control.h"
#include "Eeprom/eeprom.h"
#include "Debug/mtfs30_debug.h"
#include "Output/out_ptp.h"
#include "Debug/user_cmd.h"
#include "Gnss/gnss_receiver.h"



#include <string.h>
#include <stdlib.h>
#define FLASH_SYSCFG_SIZE sizeof(tConfigParameters)
#define BYTE_HALFWORD  2
//#define FLASH_SYCCAL_SIZE sizeof(tSysCALParameters)
//#define CACHE_BLOCK_SIZE  4096

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
unsigned char HostAddr[4];
unsigned char MACAddress[6];
extern const char *p_frequcy[PTP_MESSAGE_FRE_NUM];
//*****************************************************************************
//
//! A local flag indicating that a firmware update has been requested via the
//! web-based configuration pages.
//
//*****************************************************************************
static tBoolean g_bUpdateRequested = false;

//*****************************************************************************
//
//! A flag to the main loop indicating that it should enter the bootloader and
//! perform a firmware update.
//
//*****************************************************************************
//tBoolean g_bStartBootloader = false;

//*****************************************************************************
//
//! A flag to the main loop indicating that it should update the IP address
//! after a short delay (to allow us to send a suitable page back to the web
//! browser telling it the address has changed).
//
//*****************************************************************************
tBoolean g_bChangeIPAddress = false;
tBoolean bReset = false;
tBoolean ugSntp_Changed = false;

//*****************************************************************************
//
//! The maximum length of any HTML form variable name used in this application.
//
//*****************************************************************************
#define MAX_VARIABLE_NAME_LEN   16

//*****************************************************************************
//
// SSI tag indices for each entry in the g_pcSSITags array.
//
//*****************************************************************************
enum
{
SSI_INDEX_GNSSVARS = 0,
SSI_INDEX_PTPNETVARS,
SSI_INDEX_PTPMODEVARS,
SSI_INDEX_OUTVARS, 
SSI_INDEX_GPSPRI,         
SSI_INDEX_GNSSDLYCOM,     
SSI_INDEX_PTPMAC,        
SSI_INDEX_PTPIP,         
SSI_INDEX_PTPMASK,        

SSI_INDEX_PTPGW,          
SSI_INDEX_PTPVID,        
SSI_INDEX_PTPPORT,       
SSI_INDEX_PTPPORTSTAT,   
SSI_INDEX_PTPDOMAIN,     
SSI_INDEX_PTPDLYCOM,     
SSI_INDEX_PTPPRIO1,      
SSI_INDEX_PTPPRIO2,      
SSI_INDEX_1PPSDLYCOM,    
SSI_INDEX_1PPSTODDLYCOM, 
SSI_INDEX_IRIGBDLYCOM,   
SSI_INDEX_1PPSDUTY,      

SSI_INDEX_ANTSTATUS,     
SSI_INDEX_POSTSTATUS,    
SSI_INDEX_POSTNUM,       
SSI_INDEX_PPSSTATUS,     

SSI_INDEX_PRELEAP,       
SSI_INDEX_LEAP,          
SSI_INDEX_UTC,           
SSI_INDEX_LATITUDE,      
SSI_INDEX_LONGITUDE,     
SSI_INDEX_ALTITUDE,      
SSI_INDEX_GNSSMODE,      
//#define SSI_INDEX_SATNUM        32
SSI_INDEX_GPS,           

SSI_INDEX_BDS,           
SSI_INDEX_GLO,           
SSI_INDEX_GAL,
SSI_INDEX_REFTYPE,
SSI_INDEX_CLKTYPE,
SSI_INDEX_REFSTATUS,
SSI_INDEX_TRACK,
SSI_INDEX_PHASE,
SSI_INDEX_SSM,
    
SSI_INDEX_PTPSTATUS,
SSI_INDEX_RECVOPT,
SSI_INDEX_RECVOPTH,
SSI_INDEX_RECVOPL,
SSI_INDEX_SENDOPT,
SSI_INDEX_SENDOPTH,
SSI_INDEX_SENDOPTL,
SSI_INDEX_BIASCUR,
SSI_INDEX_BIASCURH,
SSI_INDEX_BIASCURL,
          
};
//! XML Temp Buffer Size
#define XML_TMPBUF_SIZE              96

//
//threshold define.
//
#define DLYCOM_MAX_THRESHOLD        50000000 
#define DLYCOM_MIN_THRESHOLD       -50000000 
enum
{
	GNSS_DISABLE = 0,
	GNSS_ENABLED,
};

enum
{
	GNSS_MIX_MODE = 1,
	GNSS_GPS_MODE,
        GNSS_BD_MODE,
        GNSS_GLO_MODE,
        GNSS_GAL_MODE,
};


static unsigned long old_SysRunDay = 0;
//*****************************************************************************
//
// The address of the most recent parameter block in flash.
//
//*****************************************************************************
static unsigned char* g_pucFlashPBCurrent;

//! analog value.
//extern u16_t analogValue[3];  //0: AC VOL 1:LEAKAGE CURRENT. 2: LOAD CURRENT.
//extern s16_t discreteValue[8];
//extern int temp_value;
//extern int humi_value;


//*****************************************************************************
//
//! This array holds all the strings that are to be recognized as SSI tag
//! names by the HTTPD server.  The server will call ConfigSSIHandler to
//! request a replacement string whenever the pattern <!--#tagname--> (where
//! tagname appears in the following array) is found in ``.ssi'' or ``.shtm''
//! files that it serves.
//
//*****************************************************************************
static const char* g_pcConfigSSITags[] =
{
    "gnvars",    // SSI_INDEX_GNSSVARS
    "netvars",   // SSI_INDEX_PTPNETVARS
    "modvars",   // SSI_INDEX_PTPMODEVARS
    "outvars",   // SSI_INDEX_OUTVARS
    "gpri",      // SSI_INDEX_GPSPRI
    "gdlycom",   // SSI_INDEX_GNSSDLYCOM
    "mac",       // SSI_INDEX_PTPMAC
    "ip",        // SSI_INDEX_PTPIP
    "mask",      // SSI_INDEX_PTPMASK
    "gw",        // SSI_INDEX_PTPGW
    "vid",       // SSI_INDEX_PTPVID
    "port",      // SSI_INDEX_PTPPORT
    "stat",      // SSI_INDEX_PTPPORTSTAT
    "domain",    // SSI_INDEX_PTPDOMAIN
    "dlycom",    // SSI_INDEX_PTPDLYCOM
    "prio1",     // SSI_INDEX_PTPPRIO1
    "prio2",     // SSI_INDEX_PTPPRIO2
    "sdlycom",   // SSI_INDEX_1PPSDLYCOM
    "stdlycom",  // SSI_INDEX_1PPSTODDLYCOM
    "idlycom",   // SSI_INDEX_IRIGBDLYCOM
    "duty",      // SSI_INDEX_1PPSDUTY
    
    "antst",     // SSI_INDEX_ANTSTATUS
    "postst",    // SSI_INDEX_POSTSTATUS
    "postnum",   // SSI_INDEX_POSTNUM
    "ppsst",     // SSI_INDEX_PPSSTATUS
    "preleap",   // SSI_INDEX_PRELEAP
    "leap",      // SSI_INDEX_LEAP
    "utc",       // SSI_INDEX_UTC
    "lat",       // SSI_INDEX_LATITUDE
    "longti",    // SSI_INDEX_LONGITUDE
    "altitu",    // SSI_INDEX_ALTITUDE
    "gnssmd",    // SSI_INDEX_GNSSMODE
 //   "satnum",    // SSI_INDEX_SATNUM
    "gps",       // SSI_INDEX_GPS
    "bds",       // SSI_INDEX_BDS
    "glo",       // SSI_INDEX_GLO
    "gal",       // SSI_INDEX_GAL
    
    "reftype",   // SSI_INDEX_REFTYPE
    "clktype",   // SSI_INDEX_CLKTYPE
    "refstat",   // SSI_INDEX_REFSTATUS
    "track",     // SSI_INDEX_TRACK
    "phase",     // SSI_INDEX_PHASE
    "ssm",       // SSI_INDEX_SSM
    
    "ptpstat",   // SSI_INDEX_PTPSTATUS
    "recvop",    // SSI_INDEX_RECVOPT
    "recvoph",   // SSI_INDEX_RECVOPTH
    "recvopl",   // SSI_INDEX_RECVOPL
    "sndop",     // SSI_INDEX_SENDOPT
    "sndoph",    // SSI_INDEX_SENDOPTH
    "sndopl",    // SSI_INDEX_SENDOPTL
    "biacur",    // SSI_INDEX_BIASCUR
    "biacuh",    // SSI_INDEX_BIASCURH
    "biacul",    // SSI_INDEX_BIASCURL
    
    "instal",     // SSI_INDEX_INSTALPERSON
    "instime",    // SSI_INDEX_INSTALTIME
    "runtime",    // SSI_INDEX_RUNTIME
    "sofver",     // SSI_INDEX_SOFTVER
    "harver",     // SSI_INDEX_HARDVER
    "currt",      // SSI_INDEX_CURTIME
    "sysvol",     // SSI_INDEX_SYSVOL
    "syscur",     // SSI_INDEX_SYSCUR
    "lamp",       // SSI_INDEX_LAMP
    "tempvar",    // SSI_INDEX_TEMPVAR
    "humivar"     // SSI_INDEX_HUMIVAR
};

//*****************************************************************************
//
//! The number of individual SSI tags that the HTTPD server can expect to
//! find in our configuration pages.
//
//*****************************************************************************
#define NUM_CONFIG_SSI_TAGS     (sizeof(g_pcConfigSSITags) / sizeof (char *))


//*****************************************************************************
//
//! Prototype for the function which handles requests for gnss.cgi.
//
//*****************************************************************************
static const char*
ConfigGnssHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] );

//*****************************************************************************
//
//! Prototype for the function which handles requests for pnet.cgi.
//
//*****************************************************************************
static const char*
ConfigPtpNetHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] );

//*****************************************************************************
//
//! Prototype for the function which handles requests for pnet.cgi.
//
//*****************************************************************************
static const char*
ConfigPtpModHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] );
//*****************************************************************************
//
//! Prototype for the function which handles requests for config.cgi.
//
//*****************************************************************************
static const char* SNTPCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                                   char* pcValue[] );
//*****************************************************************************
//
//! Prototype for the function which handles requests for config1.cgi.
//
//*****************************************************************************
static const char* Config2CGIHandler1( int iIndex, int iNumParams, char* pcParam[],
                                       char* pcValue[] );

//*****************************************************************************
//
//! Prototype for the function which handles requests for misc.cgi.
//
//*****************************************************************************
static const char* ConfigTrapCGIHandler( int iIndex, int iNumParams, char* pcParam[],
        char* pcValue[] );

//*****************************************************************************
//
//! Prototype for the function which handles requests for ip.cgi.
//
//*****************************************************************************
static const char* ConfigIPCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                                       char* pcValue[] );

//*****************************************************************************
//
//! Prototype for the function which handles requests for update.cgi.
//
//*****************************************************************************
static const char* ConfigSwitchCGIHandler( int iIndex, int iNumParams,
        char* pcParam[], char* pcValue[] );

//*****************************************************************************
//
//! Prototype for the function which handles requests for defaults.cgi.
//
//*****************************************************************************
static const char* ConfigRegateCGIHandler( int iIndex, int iNumParams,
        char* pcParam[], char* pcValue[] );

static const char* LoginCGIHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] );
static const char* ConfigsetpwdCGIHandler( int iIndex, int iNumParams, char* pcParam[],
        char* pcValue[] );
static const char*
ConfigCTRStyeCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                         char* pcValue[] );
static const char*
ConfigInfoCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                      char* pcValue[] );
static const char*
ConfigresetCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                       char* pcValue[] );

static const char*
Orther_CGIHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] );
static const char*
Opther_CGIHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] );
//*****************************************************************************
//
//! Prototype for the main handler used to process server-side-includes for the
//! application's web-based configuration screens.
//
//*****************************************************************************
static uint16_t ConfigSSIHandler( int iIndex, char* pcInsert, int iInsertLen );



//*****************************************************************************
//
// CGI URI indices for each entry in the g_psConfigCGIURIs array.
//
//*****************************************************************************
#define CGI_INDEX_CONFIG        0
#define CGI_INDEX_MISC          1
#define CGI_INDEX_UPDATE        2
#define CGI_INDEX_DEFAULTS      3
#define CGI_INDEX_IP            4

//*****************************************************************************
//
//! This array is passed to the HTTPD server to inform it of special URIs
//! that are treated as common gateway interface (CGI) scripts.  Each URI name
//! is defined along with a pointer to the function which is to be called to
//! process it.
//
//*****************************************************************************
static const tCGI g_psConfigCGIURIs[] =
{
    { "/login.cgi", LoginCGIHandler },              // CGI_INDEX_Login
    { "/gnss.cgi", ConfigGnssHandler },           // CGI_INDEX_CONFIG
    { "/pnet.cgi", ConfigPtpNetHandler },            // CGI_INDEX_CONFIG
    { "/pmod.cgi", ConfigPtpModHandler },              // CGI_INDEX_IP
    { "/sntp.cgi", SNTPCGIHandler },            // CGI_INDEX_CONFIG
    { "/trap.cgi", ConfigTrapCGIHandler },          // CGI_INDEX_MISC
    { "/sw.cgi", ConfigSwitchCGIHandler },      // CGI_INDEX_UPDATE
    { "/regate.cgi", ConfigRegateCGIHandler },  // CGI_INDEX_DEFAULTS
    { "/reset.cgi", ConfigresetCGIHandler },
    { "/setpwd.cgi", ConfigsetpwdCGIHandler },
    { "/ctrstye.cgi", ConfigCTRStyeCGIHandler },
    { "/info.cgi", ConfigInfoCGIHandler },
    { "/orther.cgi",  Orther_CGIHandler },
    { "/opther.cgi",  Opther_CGIHandler }
};

//*****************************************************************************
//
//! The number of individual CGI URIs that are used by our configuration
//! web pages.
//
//*****************************************************************************
#define NUM_CONFIG_CGI_URIS     (sizeof(g_psConfigCGIURIs) / sizeof(tCGI))

//*****************************************************************************
//
//! The file sent back to the browser by default following completion of any
//! of our CGI handlers.
//
//*****************************************************************************
#define LOGIN_CGI_RESPONE       "/index.html"
#define LOGIN_CGI_RESPONE1      "/index.html"
#define PASSWD_CGI_RESPONE      "/passwd.shtml"
#define CONFIG2_CGI_RESPONE     "/config2.shtml"
#define DEFAULT_CGI_RESPONSE    "/home.shtml"
#define DEFAULT_CGI_RESPONSE1   "/home1.shtml"
#define LOG_CGI_RESPONE         "/alarm.shtml"
#define SNTP_CGI_RESPONE        "/sntp.shtml"
#define TRAP_CGI_RESPONE        "/snmp.shtml"
#define TFIX_CGI_RESPONE        "/timesw.shtml"
#define RECNT_CGI_RESPONSE      "/regate.shtml"
#define CTRSTYPE_CGI_RESPONSE   "/config.shtml"
#define INFO_CGI_RESONSE        "/info.shtml"
#define UPD_CGI_RESONSE         "/udp.shtml"
#define SET_CGI_RESPONSE        "/set.shtml"
#define RESPONSE_PAGE_SET_CGI_RSP_URL     "/response.ssi"
//*****************************************************************************
//
//! The file sent back to the browser in cases where a parameter error is
//! detected by one of the CGI handlers.  This should only happen if someone
//! tries to access the CGI directly via the browser command line and doesn't
//! enter all the required parameters alongside the URI.
//
//*****************************************************************************
#define PARAM_ERROR_RESPONSE    "/perror.shtml"

//*****************************************************************************
//
//! The file sent back to the browser to signal that the bootloader is being
//! entered to perform a software update.
//
//*****************************************************************************
#define FIRMWARE_UPDATE_RESPONSE "/blstart.shtml"

//*****************************************************************************
//
//! The file sent back to the browser to signal that the IP address of the
//! device is about to change and that the web server is no longer operating.
//
//*****************************************************************************
#define IP_UPDATE_RESPONSE  "/ipchg.shtml"

//*****************************************************************************
//
//! The URI of the ``Miscellaneous Settings'' page offered by the web server.
//
//*****************************************************************************
#define MISC_PAGE_URI       "/misc.shtml"

//*****************************************************************************
//
// Strings used for format JavaScript parameters for use by the configuration
// web pages.
//
//*****************************************************************************
#define JAVASCRIPT_HEADER                                                      \
    "<script type='text/javascript' language='JavaScript'><!--\n"

#define GNSS_JAVASCRIPT_VARS                                                   \
    "var gen = %d;\n"                                                          \
    "var gmode = %d;\n"    

#define NET_JAVASCRIPT_VARS                                                    \
    "var ven = %d;\n"                                                          \
    "var pcp = %d;\n"                                                          \
    "var cfi = %d;\n"                                                         

#define MOD_JAVASCRIPT_VARS                                                   \
    "var en = %d;\n"                                                          \
    "var esmcen = %d;\n"                                                          \
    "var dlytype = %d;\n"                                                          \
    "var unicast = %d;\n"                                                          \
    "var enp = %d;\n"                                                         \
    "var step = %d;\n"                                                         \
    "var sync = %d;\n"                                                         \
    "var anounce = %d;\n"                                                          \
    "var outype = %d;\n" 

#define OUT_JAVASCRIPT_VARS                                                   \
    "var decttype = %d;\n"                                                          \
    "var todtype = %d;\n"                                                          \
    "var optsw = %d;\n"                                                          \
    "var detsw = %d;\n"                                                          
    
#define MOD1_JAVASCRIPT_VARS                                                   \
    "var todtype = %d;\n"                                                          \
    "var optsw = %d;\n"                                                          \
    "var detsw = %d;\n"                                                          
#define IP_JAVASCRIPT_VARS                                                    \
    "var staticip = %d;\n"                                                    \
    "var sip1 = %d;\n"                                                        \
    "var sip2 = %d;\n"                                                        \
    "var sip3 = %d;\n"                                                        \
    "var sip4 = %d;\n"
#define SUBNET_JAVASCRIPT_VARS                                                \
    "var syslog = %d;\n"                                                        \
    "var mip1 = %d;\n"                                                        \
    "var mip2 = %d;\n"                                                        \
    "var mip3 = %d;\n"                                                        \
    "var mip4 = %d;\n"
#define GW_JAVASCRIPT_VARS                                                    \
    "var gip1 = %d;\n"                                                        \
    "var gip2 = %d;\n"                                                        \
    "var gip3 = %d;\n"                                                        \
    "var gip4 = %d;\n"
#define NTP_JAVASCRIPT_VARS                                                \
    "var nip1 = %d;\n"                                                        \
    "var nip2 = %d;\n"                                                        \
    "var nip3 = %d;\n"                                                        \
    "var nip4 = %d;\n"                                                        \
        "var kpt = %d;\n"
#define DNS_JAVASCRIPT_VARS                                                \
    "var dns1 = %d;\n"                                                        \
    "var dns2 = %d;\n"                                                        \
    "var dns3 = %d;\n"                                                        \
    "var dns4 = %d;\n"                                                        \
        "var wpt = %d;\n"
#define SNTP_JAVASCRIPT_VARS                                                \
    "var sntpen = %d;\n"

#define SNMP_JAVASCRIPT_VARS                                                \
    "var trapen = %d;\n"
/*#define SWITCH_JAVASCRIPT_VARS                                               \
    "var swen1 = %d;\n"                                                          \
        "var swen2 = %d;\n"                                                          \
        "var swen3 = %d;\n"                                                          \
        "var swen4 = %d;\n"*/
#define BOOTH_JAVASCRIPT_VARS                                               \
    "var ch1on1 = %d;\n"                                                         \
        "var ch2on1 = %d;\n"                                                         \
        "var ch3on1 = %d;\n"                                                         \
        "var ch4on1 = %d;\n"
#define BOOTM_JAVASCRIPT_VARS                                               \
    "var ch1on2 = %d;\n"                                                         \
        "var ch2on2 = %d;\n"                                                         \
        "var ch3on2 = %d;\n"                                                         \
        "var ch4on2 = %d;\n"
#define SHUTH_JAVASCRIPT_VARS                                               \
    "var ch1on3 = %d;\n"                                                         \
        "var ch2on3 = %d;\n"                                                         \
        "var ch3on3 = %d;\n"                                                         \
        "var ch4on3 = %d;\n"
#define SHUTM_JAVASCRIPT_VARS                                               \
    "var ch1on4 = %d;\n"                                                         \
        "var ch2on4 = %d;\n"                                                         \
        "var ch3on4 = %d;\n"                                                         \
        "var ch4on4 = %d;\n"
#define REGATE_JAVASCRIPT_VARS                                                \
    "var rgcnt = %d;\n"
#define JAVASCRIPT_FOOTER                                                     \
    "//--></script>\n"

//*****************************************************************************
//
//! Structure used in mapping numeric IDs to human-readable strings.
//
//*****************************************************************************
typedef struct
{
    //
    //! A human readable string related to the identifier found in the ucId
    //! field.
    //
    const char* pcString;

    //
    //! An identifier value associated with the string held in the pcString
    //! field.
    //
    unsigned char ucId;
}
tStringMap;

//*****************************************************************************
//
//! This structure instance contains the factory-default set of configuration
//! parameters for S2E module.
//
//*****************************************************************************
static const tConfigParameters g_sParametersFactory =
{
    //
    // The sequence number (ucSequenceNum); this value is not important for
    // the copy in SRAM.
    //
    0xA5,

    //
    // The CRC (ucCRC); this value is not important for the copy in SRAM.
    //
    0xAA,

    //
    // The parameter block version number (ucVersion).
    //
    0,

    //
    // Flags (ucFlags)
    //
    0x07,   //(//static ip 0x01  ,time sw en 0x02   TRAP EN 0x04)

    //
    // The TCP port number for UPnP discovery/response (usLocationURLPort).
    //
    6432,

    //
    // Reserved (ucReserved1).
    //
    {
        0, 0
    },

    //
    // MAC ADDRESS
    //
    {
        0x00, 0x04, 0x76, 0x72, 0xb6, 0x62
    },
    
    //
    // Static IP address (used only if indicated in ucFlags).
    //
    0xac120550, /* 172.18.5.100 */
    
    //
    // Default gateway IP address (used only if static IP is in use).
    //
    0xac1205fe, /* 172.18.5.254 */ 
    
    //
    // Subnet mask (used only if static IP is in use).
    //
    0xFFFFFF00,

    //
    //DNS ADDRESS
    //
    0xcf6110c3,
    
    //
    // USER PASSWORD
    //
    {
        'r', 'o', 'o', 't',
    },
   
    //
    // community string
    //
    {
        'P', 'U', 'B', 'L', 'I', 'C', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    //
    // trap service ip address
    //
    {
        "172.18.5.50"
    },
    //
    // trap port
    //
    162,
    //
    // web port
    //
    80,
    //
    // Device Infomation
    //
    /*MODEL NAME*/
    {
        'M', 'T', 'F', 'S', '3', '0', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },
    /*Manufacturer NAME*/
    {
        "大唐电信"
    },
    /*serial*/
    {
        '2', '0', '1', '9', '0', '1', '0', '0', '0', '0', '0', '1'
    },
    /*Install Position*/
    {
        "大唐电信"
    },
    /*Install Persion*/
    {
        "---"
    },
    /*Install time*/
    {
        "2019-01-02 12:12:12"
    },
    /*run seconds*/
    0,
    
    {'u', 'p', 'o', 'k'},
    
    /*GNSS modules parameters, mean gnss_enable, mix mode, gps prior, 0 delay compensate*/
    {1, 1, "1234", 0},
    
    /*ptp net mac address,ip address, subnet mask, gateway, vlan enable, vlan priority code point, 
      canonical format indicate,vlan identifier*/
    {
      {0x00, 0x11, 0x22, 0x33, 0x44, 0x55},
      0xac1205fe,
      0xffffff00,
      0xac120501,
      0,
      7,
      0,
      2
    },
    /*ptp mode parameters,port,port status, enalbe, esmc enable, delay type, 
    unicast, encode package, step, sync frequecy, anounce frequecy, domain, 
    protocol, delay compensation, priority1, priority2*/
    {
      1,        //    port,
    "link down",//    port status,
      8,        //    sync frequecy,
      8,       //    anounce frequecy,
      24,       //    domain, 
      0,        //    protocol,
      0,        //    delay compensation,
      128,      //    priority1, 
      128,       //    priority2
      1,        //    enalbe,
      1,        //    esmc enable,
      0,        //    delay type, 
      0,        //    unicast,
      1,        //    encode package,
      0,        //    step,
      0,        /* PTP security,default false*/
      0,        /* PTP profile Specific 2,default false  */
      0,        /* PTP profile Specific 1,default false  */
      0,        /* alternateMasterFlag ,default false    */
      0,        /* frequencyTraceable      */
      0,        /* timeTraceable           */
      1,        /* ptpTimescale,default true*/
      1,        /* currentUtcOffsetValid,default true   */
      0,        /* leap59                  */
      0,        /* leap61                  */
      0,        /* stepsRemoved            */
      0,        /* grandmasterClockQuality */
      0,        /* timeSource              */
      0,        /* ssm                     */
      {0,0,0},  /*clock quality: class accuracy variance */
      37,        /*ntp enable*/ 
      0,
      0
    },
    
    /*out parameters*/
    {
      0,        //detect out signal type,default as system 1pps.
      0,        //the delay compensation of system 1pps.
      0,        //the delay compensation of 1pps tod.
      0,        //the delay compensation of irigb.
      50,       //the duty of 1pps,default 50. 
      1,        //mobile protocol default.
      1,        //optical port is enable as default.
      1         //detect port is enable as default.
    }
    
};



//*****************************************************************************
//
//! This structure instance contains the run-time set of configuration
//! parameters for S2E module.  This is the active parameter set and may
//! contain changes that are not to be committed to flash.
//
//*****************************************************************************
tConfigParameters g_sParameters;

//*****************************************************************************
//
//! This structure instance points to the most recently saved parameter block
//! in flash.  It can be considered the default set of parameters.
//
//*****************************************************************************
const tConfigParameters* g_psDefaultParameters;

//*****************************************************************************
//
//! This structure contains the latest set of parameter committed to flash
//! and is used by the configuration pages to store changes that are to be
//! written back to flash.  Note that g_sParameters may contain other changes
//! which are not to be written so we can't merely save the contents of the
//! active parameter block if the user requests some change to the defaults.
//
//*****************************************************************************
tConfigParameters g_sWorkingDefaultParameters;

//*****************************************************************************
//
//! This structure instance points to the factory default set of parameters in
//! flash memory.
//
//*****************************************************************************
const tConfigParameters* const g_psFactoryParameters = &g_sParametersFactory;

//*****************************************************************************
//
//! The version of the firmware.  Changing this value will make it much more
//! difficult for Luminary Micro support personnel to determine the firmware in
//! use when trying to provide assistance; it should only be changed after
//! careful consideration.
//
//*****************************************************************************
const unsigned short g_usFirmwareVersion = 3742;
tBoolean syslog = false;
//*****************************************************************************
//
//! Loads the S2E parameter block from factory-default table.
//!
//! This function is called to load the factory default parameter block.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigLoadDeafault( void )
{
    //
    // Copy the factory default parameter set to the active and working
    //
    g_sWorkingDefaultParameters = g_sParametersFactory;
    g_sWorkingDefaultParameters.ulStaticIP = g_sParameters.ulStaticIP;
    g_sWorkingDefaultParameters.ulGatewayIP = g_sParameters.ulGatewayIP;
    g_sWorkingDefaultParameters.ulSubnetMask = g_sParameters.ulSubnetMask;
    memcpy( &g_sWorkingDefaultParameters.ulMACAddr[0], &g_sParameters.ulMACAddr[0], 6 ); //MAC Address
    memcpy( g_sWorkingDefaultParameters.Serial, g_sParametersFactory.Serial, 13 );
}

//*****************************************************************************
//
//! Loads the S2E parameter block from factory-default table.
//!
//! This function is called to load the factory default parameter block.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigLoadFactory( void )
{
    //
    // Copy the factory default parameter set to the active and working
    // parameter blocks.
    //
    g_sParameters = g_sParametersFactory;
    g_sWorkingDefaultParameters = g_sParametersFactory;
}
//*****************************************************************************
//
//! Gets the address of the most recent parameter block.
//!
//! This function returns the address of the most recent parameter block that
//! is stored in flash.
//!
//! \return Returns the address of the most recent parameter block, or NULL if
//! there are no valid parameter blocks in flash.
//
//*****************************************************************************
unsigned char*
FlashPBGet( void )
{
    //
    // See if there is a valid parameter block.
    //
    if( g_pucFlashPBCurrent )
    {
        //
        // Return the address of the most recent parameter block.
        //
        return( g_pucFlashPBCurrent );
    }
    //
    // There are no valid parameter blocks in flash, so return NULL.
    //
    return( 0 );
}
//*****************************************************************************
//
//! Loads the S2E parameter block from flash.
//!
//! This function is called to load the most recently saved parameter block
//! from flash.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigLoad( void )
{
    unsigned char* pucBuffer = NULL;
    //
    // Get a pointer to the latest parameter block in flash.
    //
    pucBuffer = FlashPBGet();
    //
    // See if a parameter block was found in flash.
    //
    if( pucBuffer )
    {
        //
        // A parameter block was found so copy the contents to both our
        // active parameter set and the working default set.
        //
        g_sParameters = *( tConfigParameters* )pucBuffer;
        g_sWorkingDefaultParameters = g_sParameters;
    }
}
void
FlashPBSave( unsigned char* pucBuffer )
{
    //
    //define the index of array.
    //
    unsigned short index;
    unsigned char *pWorkParameters = NULL;
    //
    // Check the arguments.
    //
    assert_param( pucBuffer != ( void* )0 );
    pucBuffer[0] = 0xA5;
    pucBuffer[1] = 0xAA;
    
    //
    // save the parameters into flash.
    //
    pWorkParameters = pucBuffer;
    for(index = 0; index < FLASH_SYSCFG_SIZE/2; index++)
    {
      EE_WriteVariableByte(START_VIR_ADD + index, pWorkParameters);
      pWorkParameters += BYTE_HALFWORD;
    }
}
void FlashSYSSave( unsigned char* pucBuffer )
{
    assert_param( pucBuffer != ( void* )0 );
    pucBuffer[0] = 0xAA;
    pucBuffer[1] = 0xA5;
    //
    // erase the block of 4096 size
    //
    //SPI_FLASH_SectorErase( SSICONFIG_ADDR + CACHE_BLOCK_SIZE );
    //SPI_FLASH_BufferWrite( pucBuffer, SSICONFIG_ADDR + CACHE_BLOCK_SIZE, sizeof( tSysCALParameters ) );
}
//*****************************************************************************
//
//! Saves the S2E parameter block to flash.
//!
//! This function is called to save the current S2E configuration parameter
//! block to flash memory.
//!
//! \return None.
//
//*****************************************************************************
void SystemSave( void )
{
    //FlashSYSSave( ( unsigned char* )&g_sSyscalparaters );
}
void
ConfigSave( void )
{
    unsigned char* pucBuffer;
    //
    // Save the working defaults parameter block to flash.
    //
    FlashPBSave( ( unsigned char* )&g_sWorkingDefaultParameters );
    
    //
    // Get the pointer to the most recenly saved buffer.
    // (should be the one we just saved).
    //
    pucBuffer = FlashPBGet();
    
    //
    // Update the default parameter pointer.
    //
    if( pucBuffer )
    {
        g_psDefaultParameters = ( tConfigParameters* )pucBuffer;
    }
    else
    {
        g_psDefaultParameters = ( tConfigParameters* )g_psFactoryParameters;
    }
}






/* Private functions ---------------------------------------------------------*/
static unsigned char CurrentConfig[FLASH_SYSCFG_SIZE];

unsigned char * ConfigReadFlash(void)
{
    //
    //define the index of array variables and point.
    //
    unsigned short index;
    unsigned char *pCurrentConfig = CurrentConfig;
   
    //
    // First, load the factory default values.g_pucFlashSYSCurrent
    //
    for(index = 0; index < FLASH_SYSCFG_SIZE/2; index++)
    {
       EE_ReadVariableByte(START_VIR_ADD + index, pCurrentConfig, BYTE_HALFWORD);
			
       pCurrentConfig += BYTE_HALFWORD;
    }
    
    //
    //Check crc is or not 0xAA and 0xA5.
    //
    if(CurrentConfig[0] == 0xA5 && CurrentConfig[1] == 0xAA)
    {
      //get the current config.
      return &CurrentConfig[0];
    }
    else
    {
      //if it do not get current config,save the default config.
      ConfigSave();
      return NULL;
    }
}
//*****************************************************************************
//
//! Initializes the configuration parameter block.
//!
//! This function initializes the configuration parameter block.  If the
//! version number of the parameter block stored in flash is older than
//! the current revision, new parameters will be set to default values as
//! needed.
//!
//! \return None.
//
//*****************************************************************************
void ConfigInit( void )
{
    unsigned char* pucBuffer = NULL;
    struct ip_addr pucremoteIP;
    char* pucurl = NULL;
    u32_t pucIPaddr;
   
    
    //
    // First, load the factory default values.g_pucFlashSYSCurrent
    //
    ConfigLoadFactory();
    
    //
    // Read the flash, return byte array first point. 
    //
    g_pucFlashPBCurrent = ConfigReadFlash();
    
    //
    // Then, if available, load the latest non-volatile set of values.
    //
    ConfigLoad();
    
    //
    // Get the pointer to the most recently saved buffer.
    //
    pucBuffer = FlashPBGet();
    
    //
    // Update the default parameter pointer.
    //
    if( pucBuffer )
    {
        g_psDefaultParameters = ( tConfigParameters* )pucBuffer;
    }
    else
    {
        g_psDefaultParameters = ( tConfigParameters* )g_psFactoryParameters;
    }
  
    //
    //! get host IPAdress saved point.
    //
    pucurl = ( char* )g_sParameters.TrapService;
    
    //
    //! get the ip addr from the url.
    //
    pucremoteIP.addr = inet_addr( ( char* )pucurl );
    
    //
    //! if the ipaddr get from the url nozero.
    if( pucremoteIP.addr != INADDR_NONE )
    {
        //
        // ! ip addr transmite to the long.
        //
        pucIPaddr = htonl( pucremoteIP.addr );
        
        //
        // get the host addr from the pucIPaddr.
        //
        HostAddr[0] = ( pucIPaddr >> 24 ) & 0xff;
        HostAddr[1] = ( pucIPaddr >> 16 ) & 0xff;
        HostAddr[2] = ( pucIPaddr >> 8 ) & 0xff;
        HostAddr[3] =  pucIPaddr & 0xff ;
    }
    memcpy( MACAddress, &g_sParameters.ulMACAddr[0], 6); //MAC Address
}

//*****************************************************************************
//
//! Configures HTTPD server SSI and CGI capabilities for our configuration
//! forms.
//!
//! This function informs the HTTPD server of the server-side-include tags
//! that we will be processing and the special URLs that are used for
//! CGI processing for the web-based configuration forms.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigWebInit( void )
{
    //
    // Pass our tag information to the HTTP server.
    //
    http_set_ssi_handler( ConfigSSIHandler, g_pcConfigSSITags,
                          NUM_CONFIG_SSI_TAGS );
    //
    // Pass our CGI handlers to the HTTP server.
    //
    http_set_cgi_handlers( g_psConfigCGIURIs, NUM_CONFIG_CGI_URIS );
}

//*****************************************************************************
//
//! \internal
//!
//! Searches a mapping array to find a human-readable description for a
//! given identifier.
//!
//! \param psMap points to an array of \e tStringMap structures which contain
//! the mappings to be searched for the provided identifier.
//! \param ulEntries contains the number of map entries in the \e psMap array.
//! \param ucId is the identifier whose description is to be returned.
//!
//! This function scans the given array of ID/string maps and returns a pointer
//! to the string associated with the /e ucId parameter passed.  If the
//! identifier is not found in the map array, a pointer to ``**UNKNOWN**'' is
//! returned.
//!
//! \return Returns a pointer to an ASCII string describing the identifier
//! passed, if found, or ``**UNKNOWN**'' if not found.
//
//*****************************************************************************
static const char*
ConfigMapIdToString( const tStringMap* psMap, unsigned long ulEntries,
                     unsigned char ucId )
{
    unsigned long ulLoop;
    //
    // Check each entry in the map array looking for the ID number we were
    // passed.
    //
    for( ulLoop = 0; ulLoop < ulEntries; ulLoop++ )
    {
        //
        // Does this map entry match?
        //
        if( psMap[ulLoop].ucId == ucId )
        {
            //
            // Yes - return the IDs description string.
            //
            return( psMap[ulLoop].pcString );
        }
    }
    //
    // If we drop out, the ID passed was not found in the map array so return
    // a default "**UNKNOWN**" string.
    //
    return( "**UNKNOWN**" );
}

//*****************************************************************************
//
//! \internal
//!
//! Updates all parameters associated with a single port.
//!
//! \param ulPort specifies which of the two supported ports to update.  Valid
//! values are 0 and 1.
//!
//! This function changes the serial and telnet configuration to match the
//! values stored in g_sParameters.sPort for the supplied port.  On exit, the
//! new parameters will be in effect and g_sParameters.sPort will have been
//! updated to show the actual parameters in effect (in case any of the
//! supplied parameters are not valid or the actual hardware values differ
//! slightly from the requested value).
//!
//! \return None.
//
//*****************************************************************************
void
ConfigUpdatePortParameters( unsigned long ulPort, tBoolean bSerial,
                            tBoolean bTelnet )
{
    //
    // Do we have to update the telnet settings?  Note that we need to do this
    // first since the act of initiating a telnet connection resets the serial
    // port settings to defaults.
    //
    //     if(bTelnet)
    //     {
    //         //
    //         // Close any existing connection and shut down the server if required.
    //         //
    //         TelnetClose(ulPort);
    //         //
    //         // Are we to operate as a telnet server?
    //         //
    //         if((g_sParameters.sPort[ulPort].ucFlags & PORT_FLAG_TELNET_MODE) ==
    //            PORT_TELNET_SERVER)
    //         {
    //             //
    //             // Yes - start listening on the required port.
    //             //
    //             TelnetListen(g_sParameters.sPort[ulPort].usTelnetLocalPort,
    //                          ulPort);
    //         }
    //         else
    //         {
    //             //
    //             // No - we are a client so initiate a connection to the desired
    //             // IP address using the configured ports.
    //             //
    //             TelnetOpen(g_sParameters.sPort[ulPort].ulTelnetIPAddr,
    //                        g_sParameters.sPort[ulPort].usTelnetRemotePort,
    //                        g_sParameters.sPort[ulPort].usTelnetLocalPort,
    //                        ulPort);
    //         }
    //     }
    //     //
    //     // Do we need to update the serial port settings?  We do this if we are
    //     // told that the serial settings changed or if we just reconfigured the
    //     // telnet settings (which resets the serial port parameters to defaults as
    //     // a side effect).
    //     //
    //     if(bSerial || bTelnet)
    //     {
    //         SerialSetCurrent(ulPort);
    //     }
}

//*****************************************************************************
//
//! \internal
//!
//! Performs any actions necessary in preparation for a change if IP address.
//!
//! This function is called before ConfigUpdateIPAddress to remove the device
//! from the UPnP network in preparation for a change of IP address or
//! switch between DHCP and StaticIP.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigPreUpdateIPAddress( void )
{
    //
    // Stop UPnP and remove ourselves from the network.
    //
    //UPnPStop();
    //sntp_stop();
}

//*****************************************************************************
//
//! \internal
//!
//! Sets the IP address selection mode and associated parameters.
//!
//! This function ensures that the IP address selection mode (static IP or
//! DHCP/AutoIP) is set according to the parameters stored in g_sParameters.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigUpdateIPAddress( void )
{
    //sntp_stop();
    //
    // Change to static/dynamic based on the current settings in the
    // global parameter block.
    //
    if( ( g_sParameters.ucFlags & CONFIG_FLAG_STATICIP ) == CONFIG_FLAG_STATICIP )
    {
        lwIPNetworkConfigChange( g_sParameters.ulStaticIP,
                                 g_sParameters.ulSubnetMask,
                                 g_sParameters.ulGatewayIP,
                                 IPADDR_USE_STATIC );
    }
    else
    {
        lwIPNetworkConfigChange( 0, 0, 0, IPADDR_USE_DHCP );
    }
    //     //
    //     // Restart UPnP discovery.
    //     //
    //     UPnPStart();
    //
    // Restart sntp handle.
    //
    //sntp_init();
}

//*****************************************************************************
//
//! \internal
//!
//! Performs changes as required to apply all active parameters to the system.
//!
//! This function ensures that the system configuration matches the values in
//! the current, active parameter block.  It is called after the parameter
//! block has been reset to factory defaults.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigUpdateAllParameters( void )
{
    //
    // Update the IP address selection parameters.
    //
    ConfigPreUpdateIPAddress();
    ConfigUpdateIPAddress();
    //     //
    //     // Update the parameters for each of the individual ports.
    //     //
    //     ConfigUpdatePortParameters(0, true, true);
    //     ConfigUpdatePortParameters(1, true, true);
}

//*****************************************************************************
//
//! \internal
//!
//! Searches the list of parameters passed to a CGI handler and returns the
//! index of a given parameter within that list.
//!
//! \param pcToFind is a pointer to a string containing the name of the
//! parameter that is to be found.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting the CGI.
//! \param iNumParams is the number of elements in the pcParam array.
//!
//! This function searches an array of parameters to find the string passed in
//! \e pcToFind.  If the string is found, the index of that string within the
//! \e pcParam array is returned, otherwise -1 is returned.
//!
//! \return Returns the index of string \e pcToFind within array \e pcParam
//! or -1 if the string does not exist in the array.
//
//*****************************************************************************
static int
ConfigFindCGIParameter( const char* pcToFind, char* pcParam[], int iNumParams )
{
    int iLoop;
    //
    // Scan through all the parameters in the array.
    //
    for( iLoop = 0; iLoop < iNumParams; iLoop++ )
    {
        //
        // Does the parameter name match the provided string?
        //
        if( strcmp( pcToFind, pcParam[iLoop] ) == 0 )
        {
            //
            // We found a match - return the index.
            //
            return( iLoop );
        }
    }
    //
    // If we drop out, the parameter was not found.
    //
    return( -1 );
}

static tBoolean
ConfigIsValidHexDigit( const char cDigit )
{
    if( ( ( cDigit >= '0' ) && ( cDigit <= '9' ) ) ||
            ( ( cDigit >= 'a' ) && ( cDigit <= 'f' ) ) ||
            ( ( cDigit >= 'A' ) && ( cDigit <= 'F' ) ) )
    {
        return( true );
    }
    else
    {
        return( false );
    }
}

static unsigned char
ConfigHexDigit( const char cDigit )
{
    if( ( cDigit >= '0' ) && ( cDigit <= '9' ) )
    {
        return( cDigit - '0' );
    }
    else
    {
        if( ( cDigit >= 'a' ) && ( cDigit <= 'f' ) )
        {
            return( ( cDigit - 'a' ) + 10 );
        }
        else
        {
            if( ( cDigit >= 'A' ) && ( cDigit <= 'F' ) )
            {
                return( ( cDigit - 'A' ) + 10 );
            }
        }
    }
    //
    // If we get here, we were passed an invalid hex digit so return 0xFF.
    //
    return( 0xFF );
}

//*****************************************************************************
//
//! \internal
//!
//! Decodes a single %xx escape sequence as an ASCII character.
//!
//! \param pcEncoded points to the ``%'' character at the start of a three
//! character escape sequence which represents a single ASCII character.
//! \param pcDecoded points to a byte which will be written with the decoded
//! character assuming the escape sequence is valid.
//!
//! This function decodes a single escape sequence of the form ``%xy'' where
//! x and y represent hexadecimal digits.  If each digit is a valid hex digit,
//! the function writes the decoded character to the pcDecoded buffer and
//! returns true, else it returns false.
//!
//! \return Returns \b true on success or \b false if pcEncoded does not point
//! to a valid escape sequence.
//
//*****************************************************************************
static tBoolean
ConfigDecodeHexEscape( const char* pcEncoded, char* pcDecoded )
{
    if( ( pcEncoded[0] != '%' ) || !ConfigIsValidHexDigit( pcEncoded[1] ) ||
            !ConfigIsValidHexDigit( pcEncoded[2] ) )
    {
        return( false );
    }
    else
    {
        *pcDecoded = ( ( ConfigHexDigit( pcEncoded[1] ) * 16 ) +
                       ConfigHexDigit( pcEncoded[2] ) );
        return( true );
    }
}

//*****************************************************************************
//
//! \internal
//!
//! Encodes a string for use within an HTML tag, escaping non alphanumeric
//! characters.
//!
//! \param pcDecoded is a pointer to a null terminated ASCII string.
//! \param pcEncoded is a pointer to a storage for the encoded string.
//! \param ulLen is the number of bytes of storage pointed to by pcEncoded.
//!
//! This function encodes a string, adding escapes in place of any special,
//! non-alphanumeric characters.  If the encoded string is too long for the
//! provided output buffer, the output will be truncated.
//!
//! \return Returns the number of characters written to the output buffer
//! not including the terminating NULL.
//
//*****************************************************************************
static unsigned long
ConfigEncodeFormString( const char* pcDecoded, char* pcEncoded,
                        unsigned long ulLen )
{
    unsigned long ulLoop;
    unsigned long ulCount;
    //
    // Make sure we were not passed a tiny buffer.
    //
    if( ulLen <= 1 )
    {
        return( 0 );
    }
    //
    // Initialize our output character counter.
    //
    ulCount = 0;
    //
    // Step through each character of the input until we run out of data or
    // space to put our output in.
    //
    for( ulLoop = 0; pcDecoded[ulLoop] && ( ulCount < ( ulLen - 1 ) ); ulLoop++ )
    {
        switch( pcDecoded[ulLoop] )
        {
            //
            // Pass most characters without modification.
            //
            default:
            {
                pcEncoded[ulCount++] = pcDecoded[ulLoop];
                break;
            }
            case '\'':
            {
                ulCount += usnprintf( &pcEncoded[ulCount], ( ulLen - ulCount ),
                                      "&#39;" );
                break;
            }
        }
    }
    return( ulCount );
}

//*****************************************************************************
//
//! \internal
//!
//! Decodes a string encoded as part of an HTTP URI.
//!
//! \param pcEncoded is a pointer to a null terminated string encoded as per
//! RFC1738, section 2.2.
//! \param pcDecoded is a pointer to storage for the decoded, null terminated
//! string.
//! \param ulLen is the number of bytes of storage pointed to by pcDecoded.
//!
//! This function decodes a string which has been encoded using the method
//! described in RFC1738, section 2.2 for URLs.  If the decoded string is too
//! long for the provided output buffer, the output will be truncated.
//!
//! \return Returns the number of character written to the output buffer, not
//! including the terminating NULL.
//
//*****************************************************************************
static unsigned long
ConfigDecodeFormString( const  char* pcEncoded, char* pcDecoded,
                        unsigned long ulLen )
{
    unsigned long ulLoop;
    unsigned long ulCount;
    tBoolean bValid;
    ulCount = 0;
    ulLoop = 0;
    //
    // Keep going until we run out of input or fill the output buffer.
    //
    while( pcEncoded[ulLoop] && ( ulCount < ( ulLen - 1 ) ) )
    {
        switch( pcEncoded[ulLoop] )
        {
            //
            // '+' in the encoded data is decoded as a space.
            //
            case '+':
            {
                pcDecoded[ulCount++] = ' ';
                ulLoop++;
                break;
            }
            //
            // '%' in the encoded data indicates that the following 2
            // characters indicate the hex ASCII code of the decoded character.
            //
            case '%':
            {
                if( pcEncoded[ulLoop + 1] && pcEncoded[ulLoop + 2] )
                {
                    //
                    // Decode the escape sequence.
                    //
                    bValid = ConfigDecodeHexEscape( &pcEncoded[ulLoop],
                                                    &pcDecoded[ulCount] );
                    //
                    // If the escape sequence was valid, move to the next
                    // output character.
                    //
                    if( bValid )
                    {
                        ulCount++;
                    }
                    //
                    // Skip past the escape sequence in the encoded string.
                    //
                    ulLoop += 3;
                }
                else
                {
                    //
                    // We reached the end of the string partway through an
                    // escape sequence so just ignore it and return the number
                    // of decoded characters found so far.
                    //
                    pcDecoded[ulCount] = '\0';
                    return( ulCount );
                }
                break;
            }
            //
            // For all other characters, just copy the input to the output.
            //
            default:
            {
                pcDecoded[ulCount++] = pcEncoded[ulLoop++];
                break;
            }
        }
    }
    //
    // Terminate the string and return the number of characters we decoded.
    //
    pcDecoded[ulCount] = '\0';
    return( ulCount );
}

//*****************************************************************************
//
//! \internal
//!
//! Ensures that a string passed represents a valid decimal number and,
//! if so, converts that number to a long.
//!
//! \param pcValue points to a null terminated string which should contain an
//! ASCII representation of a decimal number.
//! \param plValue points to storage which will receive the number represented
//! by pcValue assuming the string is a valid decimal number.
//!
//! This function determines whether or not a given string represents a valid
//! decimal number and, if it does, converts the string into a decimal number
//! which is returned to the caller.
//!
//! \return Returns \b true if the string is a valid representation of a
//! decimal number or \b false if not.

//*****************************************************************************
static tBoolean
ConfigCheckDecimalParam( const char* pcValue, long* plValue )
{
    unsigned long ulLoop;
    tBoolean bStarted;
    tBoolean bFinished;
    tBoolean bNeg;
    long lAccum;
    //
    // Check that the string is a valid decimal number.
    //
    bStarted = false;
    bFinished = false;
    bNeg = false;
    ulLoop = 0;
    lAccum = 0;
    while( pcValue[ulLoop] )
    {
        //
        // Ignore whitespace before the string.
        //
        if( !bStarted )
        {
            if( ( pcValue[ulLoop] == ' ' ) || ( pcValue[ulLoop] == '\t' ) )
            {
                ulLoop++;
                continue;
            }
            //
            // Ignore a + or - character as long as we have not started.
            //
            if( ( pcValue[ulLoop] == '+' ) || ( pcValue[ulLoop] == '-' ) )
            {
                //
                // If the string starts with a '-', remember to negate the
                // result.
                //
                bNeg = ( pcValue[ulLoop] == '-' ) ? true : false;
                bStarted = true;
                ulLoop++;
            }
            else
            {
                //
                // We found something other than whitespace or a sign character
                // so we start looking for numerals now.
                //
                bStarted = true;
            }
        }
        if( bStarted )
        {
            if( !bFinished )
            {
                //
                // We expect to see nothing other than valid digit characters
                // here.
                //
                if( pcValue[ulLoop] == '.' )
                {
                    ulLoop++;
                }
                if( ( pcValue[ulLoop] >= '0' ) && ( pcValue[ulLoop] <= '9' ) )
                {
                    lAccum = ( lAccum * 10 ) + ( pcValue[ulLoop] - '0' );
                }
                else
                {
                    //
                    // Have we hit whitespace?  If so, check for no more
                    // characters until the end of the string.
                    //
                    if( ( pcValue[ulLoop] == ' ' ) || ( pcValue[ulLoop] == '\t' ) )
                    {
                        bFinished = true;
                    }
                    else
                    {
                        //
                        // We got something other than a digit or whitespace so
                        // this makes the string invalid as a decimal number.
                        //
                        return( false );
                    }
                }
            }
            else
            {
                //
                // We are scanning for whitespace until the end of the string.
                //
                if( ( pcValue[ulLoop] != ' ' ) && ( pcValue[ulLoop] != '\t' ) )
                {
                    //
                    // We found something other than whitespace so the string
                    // is not valid.
                    //
                    return( false );
                }
            }
            //
            // Move on to the next character in the string.
            //
            ulLoop++;
        }
    }
    //
    // If we drop out of the loop, the string must be valid.  All we need to do
    // now is negate the accumulated value if the string started with a '-'.
    //
    *plValue = bNeg ? -lAccum : lAccum;
    return( true );
}
//*****************************************************************************
//
//! \internal  240.00  ->24000   240.0 ->24000  240 ->24000
//!
//! Ensures that a string passed represents a valid decimal number and,
//! if so, converts that number to a long.
//!
//! \param pcValue points to a null terminated string which should contain an
//! ASCII representation of a decimal number.
//! \param plValue points to storage which will receive the number represented
//! by pcValue assuming the string is a valid decimal number.
//!
//! This function determines whether or not a given string represents a valid
//! decimal number and, if it does, converts the string into a decimal number
//! which is returned to the caller.
//!
//! \return Returns \b true if the string is a valid representation of a
//! decimal number or \b false if not.

//*****************************************************************************
static tBoolean
ConfigCheckDecimalParam1( const char* pcValue, long* plValue )
{
    unsigned long ulLoop;
    tBoolean bStarted;
    tBoolean bFinished;
    tBoolean bNeg;
    tBoolean bDecimal;
    unsigned char cAftDecimalPos = 0;
    long lAccum;
    //
    // Check that the string is a valid decimal number.
    //
    bStarted = false;
    bFinished = false;
    bNeg = false;
    bDecimal = false;
    ulLoop = 0;
    lAccum = 0;
    while( pcValue[ulLoop] )
    {
        //
        // Ignore whitespace before the string.
        //
        if( !bStarted )
        {
            if( ( pcValue[ulLoop] == ' ' ) || ( pcValue[ulLoop] == '\t' ) )
            {
                ulLoop++;
                continue;
            }
            //
            // Ignore a + or - character as long as we have not started.
            //
            if( ( pcValue[ulLoop] == '+' ) || ( pcValue[ulLoop] == '-' ) )
            {
                //
                // If the string starts with a '-', remember to negate the
                // result.
                //
                bNeg = ( pcValue[ulLoop] == '-' ) ? true : false;
                bStarted = true;
                ulLoop++;
            }
            else
            {
                //
                // We found something other than whitespace or a sign character
                // so we start looking for numerals now.
                //
                bStarted = true;
            }
        }
        if( bStarted )
        {
            if( !bFinished )
            {
                //
                // We expect to see nothing other than valid digit characters
                // here.
                //
                if( pcValue[ulLoop] == '.' )
                {
                    ulLoop++;
                    bDecimal = true;
                }
                if( ( pcValue[ulLoop] >= '0' ) && ( pcValue[ulLoop] <= '9' ) )
                {
                    lAccum = ( lAccum * 10 ) + ( pcValue[ulLoop] - '0' );
                    if( bDecimal )
                    {
                        cAftDecimalPos += 1;
                    }
                    if( cAftDecimalPos > 2 )
                    {
                        return( false );
                    }
                }
                else
                {
                    //
                    // Have we hit whitespace?  If so, check for no more
                    // characters until the end of the string.
                    //
                    if( ( pcValue[ulLoop] == ' ' ) || ( pcValue[ulLoop] == '\t' ) )
                    {
                        bFinished = true;
                    }
                    else
                    {
                        //
                        // We got something other than a digit or whitespace so
                        // this makes the string invalid as a decimal number.
                        //
                        return( false );
                    }
                }
            }
            else
            {
                //
                // We are scanning for whitespace until the end of the string.
                //
                if( ( pcValue[ulLoop] != ' ' ) && ( pcValue[ulLoop] != '\t' ) )
                {
                    //
                    // We found something other than whitespace so the string
                    // is not valid.
                    //
                    return( false );
                }
            }
            //
            // Move on to the next character in the string.
            //
            ulLoop++;
        }
    }
    //
    // If we drop out of the loop, the string must be valid.  All we need to do
    // now is negate the accumulated value if the string started with a '-'.
    //
    if( cAftDecimalPos == 0 )
    {
        lAccum *= 100;
    }
    else if( cAftDecimalPos == 1 )
    {
        lAccum *= 10;
    }
    *plValue = bNeg ? -lAccum : lAccum;
    return( true );
}
//*****************************************************************************
//
//! \internal
//!
//! Searches the list of parameters passed to a CGI handler for a parameter
//! with the given name and, if found, reads the parameter value as a decimal
//! number.
//!
//! \param pcName is a pointer to a string containing the name of the
//! parameter that is to be found.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting the CGI.
//! \param iNumParams is the number of elements in the pcParam array.
//! \param pcValues is an array of values associated with each parameter from
//! the pcParam array.
//! \param pbError is a pointer that will be written to \b true if there is any
//! error during the parameter parsing process (parameter not found, value is
//! not a valid decimal number).
//!
//! This function searches an array of parameters to find the string passed in
//! \e pcName.  If the string is found, the corresponding parameter value is
//! read from array pcValues and checked to make sure that it is a valid
//! decimal number.  If so, the number is returned.  If any error is detected,
//! parameter \e pbError is written to \b true.  Note that \e pbError is NOT
//! written if the parameter is successfully found and validated.  This is to
//! allow multiple parameters to be parsed without the caller needing to check
//! return codes after each individual call.
//!
//! \return Returns the value of the parameter or 0 if an error is detected (in
//! which case \e *pbError will be \b true).
//
//*****************************************************************************
static long
ConfigGetCGIParam( const char* pcName, char* pcParams[], char* pcValue[],
                   int iNumParams, tBoolean* pbError )
{
    int iParam;
    long lValue;
    tBoolean bRetcode;
    //
    // Is the parameter we are looking for in the list?
    //
    lValue = 0;
    iParam = ConfigFindCGIParameter( pcName, pcParams, iNumParams );
    if( iParam != -1 )
    {
        //
        // We found the parameter so now get its value.
        //
        bRetcode = ConfigCheckDecimalParam( pcValue[iParam], &lValue );
        if( bRetcode )
        {
            //
            // All is well - return the parameter value.
            //
            return( lValue );
        }
    }
    //
    // If we reach here, there was a problem so return 0 and set the error
    // flag.
    //
    *pbError = true;
    return( 0 );
}
//240 ->24000 240.0 ->24000 240.00 ->24000
//*****************************************************************************
static long
ConfigGetCGIParam1( const char* pcName, char* pcParams[], char* pcValue[],
                    int iNumParams, tBoolean* pbError )
{
    int iParam;
    long lValue;
    tBoolean bRetcode;
    //
    // Is the parameter we are looking for in the list?
    //
    lValue = 0;
    iParam = ConfigFindCGIParameter( pcName, pcParams, iNumParams );
    if( iParam != -1 )
    {
        //
        // We found the parameter so now get its value.
        //
        bRetcode = ConfigCheckDecimalParam1( pcValue[iParam], &lValue );
        if( bRetcode )
        {
            //
            // All is well - return the parameter value.
            //
            return( lValue );
        }
    }
    //
    // If we reach here, there was a problem so return 0 and set the error
    // flag.
    //
    *pbError = true;
    return( 0 );
}


//*****************************************************************************
//
//! \internal
//!
//! Searches the list of parameters passed to a CGI handler for 4 parameters
//! representing an IP address and extracts the IP address defined by them.
//!
//! \param pcName is a pointer to a string containing the base name of the IP
//! address parameters.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting the CGI.
//! \param iNumParams is the number of elements in the pcParam array.
//! \param pcValues is an array of values associated with each parameter from
//! the pcParam array.
//! \param pbError is a pointer that will be written to \b true if there is any
//! error during the parameter parsing process (parameter not found, value is
//! not a valid decimal number).
//!
//! This function searches an array of parameters to find four parameters
//! whose names are \e pcName appended with digits 1 - 4.  Each of these
//! parameters is expected to have a value which is a decimal number between
//! 0 and 255.  The parameter values are read and concatenated into an unsigned
//! long representing an IP address with parameter 1 in the leftmost postion.
//!
//! For example, if \e pcName points to string ``ip'', the function will look
//! for 4 CGI parameters named ``ip1'', ``ip2'', ``ip3'' and ``ip4'' and read
//! their values to generate an IP address of the form 0xAABBCCDD where ``AA''
//! is the value of parameter ``ip1'', ``BB'' is the value of ``p2'', ``CC''
//! is the value of ``ip3'' and ``DD'' is the value of ``ip4''.
//!
//! \return Returns the IP address read or 0 if an error is detected (in
//! which case \e *pbError will be \b true).
//
//*****************************************************************************
unsigned long
ConfigGetCGIIPAddr( const char* pcName, char* pcParam[], char* pcValue[],
                    int iNumParams, tBoolean* pbError )
{
    unsigned long ulIPAddr;
    unsigned long ulLoop;
    long lValue;
    char pcVariable[MAX_VARIABLE_NAME_LEN];
    tBoolean bError;
    //
    // Set up for the loop which reads each address element.
    //
    ulIPAddr = 0;
    bError = false;
    //
    // Look for each of the four variables in turn.
    //
    for( ulLoop = 1; ulLoop <= 4; ulLoop++ )
    {
        //
        // Generate the name of the variable we are looking for next.
        //
        usnprintf( pcVariable, MAX_VARIABLE_NAME_LEN, "%s%d", pcName, ulLoop );
        //
        // Shift our existing IP address to the left prior to reading the next
        // byte.
        //
        ulIPAddr <<= 8;
        //
        // Get the next variable and mask it into the IP address.
        //
        lValue = ConfigGetCGIParam( pcVariable, pcParam, pcValue, iNumParams,
                                    &bError );
        ulIPAddr |= ( ( unsigned long )lValue & 0xFF );
    }
    //
    // Did we encounter any error while reading the parameters?
    //
    if( bError )
    {
        //
        // Yes - mark the clients error flag and return 0.
        //
        *pbError = true;
        return( 0 );
    }
    else
    {
        //
        // No - all is well so return the IP address.
        //
        return( ulIPAddr );
    }
}

//*****************************************************************************
//
//! \internal
//!
//! Searches the list of parameters passed to a CGI handler for 4 parameters
//! representing an IP address and extracts the IP address defined by them.
//!
//! \param pcName is a pointer to a string containing the base name of the IP
//! address parameters.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting the CGI.
//! \param iNumParams is the number of elements in the pcParam array.
//! \param pcValues is an array of values associated with each parameter from
//! the pcParam array.
//! \param pbError is a pointer that will be written to \b true if there is any
//! error during the parameter parsing process (parameter not found, value is
//! not a valid decimal number).
//!
//! This function searches an array of parameters to find four parameters
//! whose names are \e pcName appended with digits 1 - 4.  Each of these
//! parameters is expected to have a value which is a decimal number between
//! 0 and 255.  The parameter values are read and concatenated into an unsigned
//! long representing an IP address with parameter 1 in the leftmost postion.
//!
//! For example, if \e pcName points to string ``ip'', the function will look
//! for 4 CGI parameters named ``ip1'', ``ip2'', ``ip3'' and ``ip4'' and read
//! their values to generate an IP address of the form 0xAABBCCDD where ``AA''
//! is the value of parameter ``ip1'', ``BB'' is the value of ``p2'', ``CC''
//! is the value of ``ip3'' and ``DD'' is the value of ``ip4''.
//!
//! \return Returns the IP address read or 0 if an error is detected (in
//! which case \e *pbError will be \b true).
//
//*****************************************************************************
unsigned long
ConfigGetCGIDeciIPAddr( const char* pcName, char* pcParam[], char* pcValue[],
                    int iNumParams, tBoolean* pbError )
{
    struct ip_addr IPAddr;
    unsigned long ulIPAddr;
    int iParam;
    char pcVariable[MAX_VARIABLE_NAME_LEN];
    tBoolean bError;
    //
    // Set up for the loop which reads each address element.
    //
    ulIPAddr = 0;
    bError = false;
    
    
    iParam = ConfigFindCGIParameter( pcName, pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                pcVariable,
                                MAX_VARIABLE_NAME_LEN );
    }
    else
       bError = true;
    
    IPAddr.addr = inet_addr( ( char* )pcVariable );
    if( IPAddr.addr == INADDR_NONE || bError == true )
    {
      *pbError = true;
        return( 0 );
    }
    else
    {
        ulIPAddr = htonl( IPAddr.addr );
        return ulIPAddr;
    }
}
//
//! String into an integer
//
unsigned char StringtoInt( char* s )
{
    unsigned lvaule = 0;
    if( ( s[0] >= '0' ) && ( s[0] <= '9' ) )
    {
        lvaule = s[0] - '0';
    }
    else
    {
        if( ( s[0] >= 'A' ) && ( s[0] <= 'F' ) )
        {
            lvaule = s[0] - 'A' + 10;
        }
        else if( ( s[0] >= 'a' ) && ( s[0] <= 'f' ) )
        {
            lvaule = s[0] - 'a' + 10;
        }
    }
    if( ( s[1] >= '0' ) && ( s[1] <= '9' ) )
    {
        lvaule  = ( lvaule << 4 ) | ( s[1] - '0' );
    }
    else
    {
        if( ( s[1] >= 'A' ) && ( s[1] <= 'F' ) )
        {
            lvaule = ( lvaule << 4 ) | ( s[1] - 'A' + 10 );
        }
        else if( ( s[1] >= 'a' ) && ( s[1] <= 'f' ) )
        {
            lvaule  = ( lvaule << 4 ) | ( s[1] - 'a' + 10 );
        }
    }
    return lvaule;
}

//*****************************************************************************
//
//! \internal
//!
//! Searches the list of parameters passed to a CGI handler for 4 parameters
//! representing an IP address and extracts the IP address defined by them.
//!
//! \param pcName is a pointer to a string containing the base name of the IP
//! address parameters.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting the CGI.
//! \param iNumParams is the number of elements in the pcParam array.
//! \param pcValues is an array of values associated with each parameter from
//! the pcParam array.
//! \param pbError is a pointer that will be written to \b true if there is any
//! error during the parameter parsing process (parameter not found, value is
//! not a valid decimal number).
//!
//! This function searches an array of parameters to find four parameters
//! whose names are \e pcName appended with digits 1 - 4.  Each of these
//! parameters is expected to have a value which is a decimal number between
//! 0 and 255.  The parameter values are read and concatenated into an unsigned
//! long representing an IP address with parameter 1 in the leftmost postion.
//!
//! For example, if \e pcName points to string ``ip'', the function will look
//! for 4 CGI parameters named ``ip1'', ``ip2'', ``ip3'' and ``ip4'' and read
//! their values to generate an IP address of the form 0xAABBCCDD where ``AA''
//! is the value of parameter ``ip1'', ``BB'' is the value of ``p2'', ``CC''
//! is the value of ``ip3'' and ``DD'' is the value of ``ip4''.
//!
//! \return Returns the IP address read or 0 if an error is detected (in
//! which case \e *pbError will be \b true).
//
//*****************************************************************************
static tBoolean
ConfigGetCGIMacaddr( const char* pcToFind, int iNumParams, char* pcParam[],
                     char* pcValue[], char* pcDecoded,
                     unsigned long ulLen )
{
    int iParam;
    unsigned char i;
    unsigned char i1;
    unsigned char strtemp[MAX_MACSTRING_LEN];
    char* string_tmep;
    tBoolean berror;
    berror = false;
    i = 0;
    i1 = 0;
    //Query corresponding parameters and parameter values
    iParam = ConfigFindCGIParameter( pcToFind, pcParam, iNumParams );
    if( iParam != -1 )
    {
        //Take out the corresponding string
        //          ConfigDecodeFormString(pcValue[iParam],
        //                                 (char *)string_tmep,
        //                                 ulLen);
        string_tmep = pcValue[iParam];
        //String into an integer
        for( i = 0; i < 6; i++ )
        {
            i1 = 0;
            while( !( ( *string_tmep == '\x2d' ) || ( *string_tmep == 0 ) ) )
            {
                if( isxdigit( *string_tmep ) || ( *string_tmep == '\x2d' ) )
                {
                    strtemp[i1++] = *string_tmep++;
                }
                else
                {
                    berror = true;
                    return berror;
                }
            }
            strtemp[i1] = 0;
            string_tmep++;
            //sprintf(&pcDecoded[i],"%02X",(char*)strtemp);
            pcDecoded[i] = StringtoInt( ( char* )strtemp );
        }
        return berror;
    }
    berror = true;
    return berror;
}

//*****************************************************************************
//
//! \internal
//!
//! Performs processing for the URI ``/config.cgi''.
//!
//! \param iIndex is an index into the g_psConfigCGIURIs array indicating which
//! CGI URI has been requested.
//! \param uNumParams is the number of entries in the pcParam and pcValue
//! arrays.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting this CGI.
//! \param pcValue is an array of character pointers, each containing the value
//! of a parameter as encoded in the URI requesting this CGI.
//!
//! This function is called whenever the HTTPD server receives a request for
//! URI ``/config.cgi''.  Parameters from the request are parsed into the
//! \e pcParam and \e pcValue arrays such that the parameter name and value
//! are contained in elements with the same index.  The strings contained in
//! \e pcParam and \e pcValue contain all replacements and encodings performed
//! by the browser so the CGI function is responsible for reversing these if
//! required.
//!
//! After processing the parameters, the function returns a fully qualified
//! filename to the HTTPD server which will then open this file and send the
//! contents back to the client in response to the CGI.
//!
//! This specific CGI expects the following parameters:
//!
//! - ``port'' indicates which connection's settings to update.  Valid
//!   values are ``0'' or ``1''.
//! - ``br'' supplies the baud rate.
//! - ``bc'' supplies the number of bits per character.
//! - ``parity'' supplies the parity.  Valid values are ``0'', ``1'', ``2'',
//!   ``3'' or ``4'' with meanings as defined by \b SERIAL_PARITY_xxx in
//!   serial.h.
//! - ``stop'' supplies the number of stop bits.
//! - ``flow'' supplies the flow control setting.  Valid values are ``1'' or
//!   ``3'' with meanings as defined by the \b SERIAL_FLOW_CONTROL_xxx in
//!   serial.h.
//! - ``telnetlp'' supplies the local port number for use by the telnet server.
//! - ``telnetrp'' supplies the remote port number for use by the telnet
//!   client.
//! - ``telnett'' supplies the telnet timeout in seconds.
//! - ``telnetip1'' supplies the first digit of the telnet server IP address.
//! - ``telnetip2'' supplies the second digit of the telnet server IP address.
//! - ``telnetip3'' supplies the third digit of the telnet server IP address.
//! - ``telnetip4'' supplies the fourth digit of the telnet server IP address.
//! - ``tnmode'' supplies the telnet mode, ``0'' for server, ``1'' for client.
//! - ``tnprot'' supplies the telnet protocol, ``0'' for telnet, ``1'' for raw.
//! - ``default'' will be defined with value ``1'' if the settings supplied are
//!   to be saved to flash as the defaults for this port.
//!
//! \return Returns a pointer to a string containing the file which is to be
//! sent back to the HTTPD client in response to this request.
//
//*****************************************************************************
static const char*
LoginCGIHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] )
{
    int iParam;
    //long lpasswd;
    tBoolean bParamError;
    bParamError = false;
    char* rootpasswd = "root";
    //
    // Find the "modname" parameter.
    //
    iParam = ConfigFindCGIParameter( "PASSWD", pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                ( char* )g_sWorkingDefaultParameters.userpasswd,
                                16 );
    }
    if( strncmp( ( char* )g_sWorkingDefaultParameters.userpasswd, rootpasswd, strlen( rootpasswd ) ) == 0 )
    {
        syslog = true;
        return( LOGIN_CGI_RESPONE);
    }
    else
    {
        syslog = false;
        return( LOGIN_CGI_RESPONE);
    }
}
static const char*
ConfigsetpwdCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                        char* pcValue[] )
{
//    int iParam;
//    //long lValue;
//    tBoolean bChanged;
//    //tBoolean bError;
//    //
//    // We have not made any changes that need written to flash yet.
//    //
//    bChanged = false;
//    //
//    // Find the "modname" parameter.
//    //
//    iParam = ConfigFindCGIParameter( "NEWPWD2", pcParam, iNumParams );
//    if( iParam != -1 )
//    {
//        ConfigDecodeFormString( pcValue[iParam],
//                                ( char* )g_sWorkingDefaultParameters.userpasswd,
//                                16 );
//        strncpy( ( char* )g_sParameters.userpasswd,
//                 ( char* )g_sWorkingDefaultParameters.userpasswd, 16 );
//        bChanged = true;
//    }
    return( PASSWD_CGI_RESPONE );
}

static const char*
ConfigCTRStyeCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                         char* pcValue[] )
{
//    tBoolean bError;
//    unsigned char ulCtrStype;
//    //
//    //General the control stype from the cgi of http.
//    //
//    //
//    // Find the "port" parameter.
//    //
//    bError = false;
//    ulCtrStype = ConfigGetCGIParam( "ctrl", pcParam, pcValue, iNumParams, &bError );
//    switch( ulCtrStype )
//    {
//        //
//        // saved contrl stype .
//        //
////        case SAVED_CTR_STYPE:
////            //save the current parameters and reset.
////            g_sWorkingDefaultParameters = g_sParameters;
////            ConfigSave();
////            //set reset flag as tell the system to reset .
////            // bReset = true;
////            break;
////        //
////        // reset contrl stype .
////        //
////        case RESET_CTR_STYPE:
////            //set reset flag as tell the system to reset .
////            bReset = true;
////            break;
////        //
////        // restore contrl stype .
////        //
////        case RESTORE_CTR_STYPE:
////        {
////            //
////            // Update the working parameter set with the factory defaults.
////            //
////            ConfigLoadDeafault();
////            //
////            // Save the new defaults to flash.
////            //
////            ConfigSave();
////            //
////            // Apply the various changes required as a result of changing back to
////            // the default settings.
////            //
////            ConfigUpdateAllParameters();
////            //
////            //set reset flag as tell the system to reset .
////            //
////            bReset = true;
////        }
////        break;
////        //
////        // redefault contrl stype .
////        //
////        case REDEFAULT_CTR_STYPE:
////        {
////            //
////            // Update the working parameter set with the factory defaults.
////            //
////            ConfigLoadFactory();
////            memcpy( &g_sWorkingDefaultParameters.ulMACAddr[0], &g_sParameters.ulMACAddr[0], 6 ); //MAC Address
////            memcpy( g_sWorkingDefaultParameters.Serial, g_sParameters.Serial, 13 );
////            //g_sSyscalparaters = g_FactorySyscalparameters;
////            //
////            // Save the new defaults to flash.
////            //
////            ConfigSave();
////            //SystemSave();
////            // Apply the various changes required as a result of changing back to
////            // the default settings.
////            //
////            ConfigUpdateAllParameters();
////            //
////            //set reset flag as tell the system to reset .
////            //
////            bReset = true;
////        }
////        break;
//    }
    return( CTRSTYPE_CGI_RESPONSE );
}
static const char*
ConfigresetCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                       char* pcValue[] )
{
    //
    //General the control stype from the cgi of http.
    //
//    ConfigPreUpdateIPAddress();
//    //
//    // Find the "port" parameter.
//    //
//    g_sWorkingDefaultParameters.updata_flag[0] = 'f';
//    g_sWorkingDefaultParameters.updata_flag[1] = 'a';
//    g_sWorkingDefaultParameters.updata_flag[2] = 'i';
//    g_sWorkingDefaultParameters.updata_flag[3] = 'l';
//    ConfigSave();
//    bReset   = true;
    return( UPD_CGI_RESONSE );
}
//unsigned char b_zonechange = false;

//extern u32_t   ACVOTAGE;

//extern u32_t   ACLOADCUR;


static const char*
ConfigGnssHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] )
{
    //
    //!temp varible zone.
    //
    int iParam;
    tBoolean bParamError;          //cgi error .
    unsigned char gnss_en = 0 ;     //gnss enable temp val.
    unsigned char gnss_mode = 0;        //gnss work mode temp val.
    //unsigned char gnss_prio[4];   //gnss priority temp val.
    long gnss_dlycom = 0;             //gnss delay compensation temp val.
    unsigned char command[COMM_MAX_LEN];
    //
    // We have not encountered any parameter errors yet.
    //
    bParamError = false;
    iParam = -1;
    
    //
    // the remote cgi command of GNSS enable.
    //
    gnss_en = ( unsigned char )ConfigGetCGIParam( "gen", pcParam,
                   pcValue,
                   iNumParams,
                   &bParamError );
    //
    // the remote cgi command of the GNSS mode.
    //
    gnss_mode = ( unsigned char )ConfigGetCGIParam( "gmode", pcParam,
                   pcValue,
                   iNumParams,
                   &bParamError );
    //
    // the remote cgi command of the gnss mode priority .
    //
    iParam = ConfigFindCGIParameter( "gpri", pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                ( char* )g_sParameters.GnssParameters.priority,
                                GNSS_PRIO_LEN );
    }
 
    //
    // the remote cgi command of dlycom .
    //
    gnss_dlycom = ( long )ConfigGetCGIParam( "dlycom", pcParam,
                   pcValue,
                   iNumParams,
                   &bParamError );
    
    
    //
    // check the value legitimate.
    //
    if(gnss_dlycom > DLYCOM_MAX_THRESHOLD || gnss_dlycom < DLYCOM_MIN_THRESHOLD ||
       gnss_en > GNSS_ENABLED || gnss_en < GNSS_DISABLE || gnss_mode > GNSS_GAL_MODE ||
       gnss_mode < GNSS_MIX_MODE)
    {
       bParamError = true;
    }
    
    //
    // We have now read all the parameters and made sure that they are valid
    // decimal numbers.  Did we see any errors during this process?
    //
    if( bParamError )
    {
        //
        // Yes - tell the user there was an error.
        //
        return( PARAM_ERROR_RESPONSE );
    }
    else
    {
        //
        // if check corrected,save config into flash.
        //
        g_sParameters.GnssParameters.enable = gnss_en;
        g_sParameters.GnssParameters.mode = gnss_mode;
        g_sParameters.GnssParameters.delaycom = gnss_dlycom;
        g_sWorkingDefaultParameters = g_sParameters;
        sprintf(command, "gnss d c %d\n", gnss_dlycom);
        user_cmd_parser(command);
        ConfigSave();
        return( SET_CGI_RESPONSE);
    }
   
    
}

static const char*
ConfigPtpNetHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] )
{
    //
    //!temp varible zone.
    //
    tBoolean bParamError = false;
    unsigned long ulIPAddr = 0;
    unsigned long ulGatewayAddr = 0;
    unsigned long ulSubnetMask = 0;
    unsigned char  ulMACAddr[6] = {0};
    unsigned char bVlanEnable = 0;
    unsigned char bVlanPcp = 0;
    unsigned char bVlanCfi = 0;
    unsigned short bVlanVid = 0;
    unsigned char command[COMM_MAX_LEN];
    //
    // get the mac address of ptp module.
    //
//#if 0
    bParamError = ConfigGetCGIMacaddr( "mac", iNumParams, pcParam, pcValue, ( char* )ulMACAddr, MAX_MACSTRING_LEN );
    //
    // the remote cgi command of the ptp network .
    //
    if(!bParamError)
//#endif    
      ulIPAddr = ConfigGetCGIDeciIPAddr( "ip", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        ulSubnetMask = ConfigGetCGIDeciIPAddr( "mask", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        ulGatewayAddr = ConfigGetCGIDeciIPAddr( "gw", pcParam, pcValue, iNumParams, &bParamError );
    //
    // the vlan enable and priority, cfi,vid setting.
    //
    if(!bParamError)
        bVlanEnable =  ConfigGetCGIParam( "ven", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        bVlanPcp =  ConfigGetCGIParam( "pcp", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        bVlanCfi =  ConfigGetCGIParam( "cfi", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        bVlanVid =  ConfigGetCGIParam( "vid", pcParam, pcValue, iNumParams, &bParamError );

    //
    // We have now read all the parameters and made sure that they are valid
    // decimal numbers.  Did we see any errors during this process?
    //
    if( bParamError )
    {
        //
        // Yes - tell the user there was an error.
        //
        return( PARAM_ERROR_RESPONSE );
    }
    else
    {
        //
        // if check network changed.
        //
        if(g_sParameters.PtpNetParameters.ptp_ipaddr != ulIPAddr || g_sParameters.PtpNetParameters.ptp_submask != ulSubnetMask
           || g_sParameters.PtpNetParameters.ptp_gateway != ulGatewayAddr)
        {
            g_sParameters.PtpNetParameters.ptp_ipaddr = ulIPAddr;
            g_sParameters.PtpNetParameters.ptp_submask = ulSubnetMask;
            g_sParameters.PtpNetParameters.ptp_gateway = ulGatewayAddr;  
            
            //
            //execute the spi command
            sprintf(command, "ipconfig -a %d.%d.%d.%d\n", ((ulIPAddr >> 24) &0xff),
                                                          ((ulIPAddr >> 16) &0xff),
                                                          ((ulIPAddr >> 8) &0xff),
                                                          (ulIPAddr  &0xff));
            user_cmd_parser(command);
            
            //
            //
            sprintf(command, "ipconfig -m %d.%d.%d.%d\n", ((ulSubnetMask >> 24) &0xff),
                                                          ((ulSubnetMask >> 16) &0xff),
                                                          ((ulSubnetMask >> 8) &0xff),
                                                          (ulSubnetMask  &0xff));
            user_cmd_parser(command);
            
            //
            //
            sprintf(command, "ipconfig -g %d.%d.%d.%d\n", ((ulGatewayAddr >> 24) &0xff),
                                                          ((ulGatewayAddr >> 16) &0xff),
                                                          ((ulGatewayAddr >> 8) &0xff),
                                                          (ulGatewayAddr  &0xff));
            user_cmd_parser(command);
            
        }
        
        g_sParameters.PtpNetParameters.vlan_enable = bVlanEnable;
        
        
        g_sParameters.PtpNetParameters.vlan_pcp = bVlanPcp;
        g_sParameters.PtpNetParameters.vlan_cfi = bVlanCfi;
        g_sParameters.PtpNetParameters.vlan_vid = bVlanVid;
        //
        //
        sprintf(command, "ifconfig vlan -e %s\n", bVlanEnable ? "on" : "off");
        user_cmd_parser(command);
        //
        //
        sprintf(command, "ifconfig vlan -p %d\n", bVlanPcp);
        user_cmd_parser(command);
        //
        //
        sprintf(command, "ifconfig vlan -c %d\n", bVlanCfi);
        user_cmd_parser(command);
        //
        //
        sprintf(command, "ifconfig vlan -v %d\n", bVlanVid);
        user_cmd_parser(command);
        //
        g_sWorkingDefaultParameters = g_sParameters;
        
        ConfigSave();
        
        return( SET_CGI_RESPONSE);
    }
}


static const char*
ConfigPtpModHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] )
{
    //
    //!temp varible zone.
    //
    tBoolean bParamError = false;
    tBoolean ptp_en = false;
    tBoolean esmc_en = false;
    tBoolean dly_type = false;
    tBoolean unicast = false;
    tBoolean enpack_en = false;
    tBoolean step_type = false;
    tBoolean ntp_en = false;
    unsigned char sync_fre = 0;
    unsigned char anounce_fre = 0;
    unsigned char clock_domain = 0;
    unsigned char priority1 = 0;
    unsigned char priority2 = 0;
    unsigned char change_flag[COMM_MAX_LEN] = {0};
    
    unsigned char command[COMM_MAX_LEN];
    //
    // get the mac address of ptp module.
    //
     ptp_en = ConfigGetCGIParam( "en", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        esmc_en = ConfigGetCGIParam( "esmcen", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        dly_type = ConfigGetCGIParam( "dlytype", pcParam, pcValue, iNumParams, &bParamError );
    //
    // the vlan enable and priority, cfi,vid setting.
    //
    if(!bParamError)
        unicast =  ConfigGetCGIParam( "unicast", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        enpack_en =  ConfigGetCGIParam( "enp", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        step_type =  ConfigGetCGIParam( "step", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        sync_fre =  ConfigGetCGIParam( "sync", pcParam, pcValue, iNumParams, &bParamError );

    if(!bParamError)
        anounce_fre =  ConfigGetCGIParam( "anounce", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        clock_domain =(unsigned char) ConfigGetCGIParam( "domain", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        ntp_en = ConfigGetCGIParam( "outype", pcParam, pcValue, iNumParams, &bParamError );
     
    if(!bParamError)
        priority1 = (unsigned char)ConfigGetCGIParam( "prio1", pcParam, pcValue, iNumParams, &bParamError );
    
    if(!bParamError)
        priority2 = (unsigned char)ConfigGetCGIParam( "prio2", pcParam, pcValue, iNumParams, &bParamError );
    //
    // We have now read all the parameters and made sure that they are valid
    // decimal numbers.  Did we see any errors during this process?
    //
    if( bParamError )
    {
        //
        // Yes - tell the user there was an error.
        //
        return( PARAM_ERROR_RESPONSE );
    }
    else
    {
        //
        // if check network changed.
        //
        if(g_sParameters.PtpModeParameters.port_enable != ptp_en)
        {
            g_sParameters.PtpModeParameters.port_enable = ptp_en;
            sprintf(command, "ptp2 p e %s\n", ptp_en? "on" : "off");
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.esmc_enable != esmc_en)
        {
            g_sParameters.PtpModeParameters.esmc_enable = esmc_en;
            sprintf(command, "ptp2 e e %s\n", esmc_en? "on" : "off");
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.delay_type != dly_type)
        {
            g_sParameters.PtpModeParameters.delay_type = dly_type;
            sprintf(command, "ptp2 p m %s\n", dly_type? "p2p" : "e2e");
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.unicast != unicast)
        {
            g_sParameters.PtpModeParameters.unicast = unicast;
            sprintf(command, "ptp2 p u %s\n", unicast? "enable" : "disable");
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.encode_package != enpack_en)
        {
            g_sParameters.PtpModeParameters.encode_package = enpack_en;
            sprintf(command, "ptp2 p p %s\n", enpack_en? "udp" : "eth");
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.step_type != step_type)
        {
            g_sParameters.PtpModeParameters.step_type = step_type;
            sprintf(command, "ptp2 c t %s\n", step_type? "on" : "off");
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.sync_frequency != sync_fre)
        {
            g_sParameters.PtpModeParameters.sync_frequency = sync_fre;
            sprintf(command, "ptp2 p si %s\n", p_frequcy[sync_fre]);
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.anounce_frequency != anounce_fre)
        {
            g_sParameters.PtpModeParameters.anounce_frequency = anounce_fre;
            sprintf(command, "ptp2 p ai %s\n", p_frequcy[anounce_fre]);
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.domain != clock_domain)
        {
            g_sParameters.PtpModeParameters.domain = clock_domain;
            sprintf(command, "ptp2 c d %d\n", clock_domain);
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.ntp_en != ntp_en)
        {
            g_sParameters.PtpModeParameters.ntp_en = ntp_en;
            sprintf(command, "ptp2 p n %s\n", ntp_en? "on" : "off");
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.priority1 != priority1)
        {
            g_sParameters.PtpModeParameters.priority1 = priority1;
            sprintf(command, "ptp2 c p 1 %d\n", priority1);
            user_cmd_parser(command);
        }
        
        if(g_sParameters.PtpModeParameters.priority2 != priority2)
        {
            g_sParameters.PtpModeParameters.priority2 = priority2;
            sprintf(command, "ptp2 c p 2 %d\n", priority2);
            user_cmd_parser(command);
        }
        //
        g_sWorkingDefaultParameters = g_sParameters;
        
        ConfigSave();
        
        return( SET_CGI_RESPONSE);
    }
}
static const char*
SNTPCGIHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] )
{
//    int iParam;
//    //long lValue;
//    tBoolean bParamError;// /
//    unsigned char ulSntpEn ;
//    unsigned short ulSntpIntervatSecond;
//    //
//    // We have not encountered any parameter errors yet.
//    //
//    bParamError = false;
//    //
//    // SNTP Enable flag
//    //
//    ulSntpEn = ( unsigned char )ConfigGetCGIParam( "sntpen", pcParam,
//               pcValue,
//               iNumParams,
//               &bParamError );
//    //
//    // SNTP URL/IP Adress.
//    //
//    iParam = ConfigFindCGIParameter( "ntpurl", pcParam, iNumParams );
////    if( iParam != -1 )
////    {
////        ConfigDecodeFormString( pcValue[iParam],
////                                ( char* )g_sParameters.SNTPURL,
////                                URL_NAME_LEN );
////        ugSntp_Changed = true ;
////        //
////        // Shut down connections in preparation for the IP address change.
////        //
////        ConfigPreUpdateIPAddress();
////    }
//    //
//    //General the SNTP Intervate seconds from the http cgi.
//    //
//    bParamError = false;
//    ulSntpIntervatSecond = ( unsigned short )ConfigGetCGIParam( "spinter", pcParam, pcValue, iNumParams,
//                           &bParamError );
//    //
//    // We have now read all the parameters and made sure that they are valid
//    // decimal numbers.  Did we see any errors during this process?
//    //
////    if( bParamError || ( ulSntpIntervatSecond <= SNTP_LOWER_INTER ) || ( ulSntpIntervatSecond >= SNTP_UPPER_INTER ) || ( ulSntpEn > SNTP_EN_UPPER_OPTION ) )
////    {
////        //
////        // Yes - tell the user there was an error.
////        //
////        return( PARAM_ERROR_RESPONSE );
////    }
//    //    else
//    //    {
//    //        ugSntp_Changed = true;
//    //    }
//    //
//    // Did any of the serial parameters change?
//    //
////    if( ulSntpEn )
////    {
////        g_sParameters.ucFlags |= SNTP_EN_FLAG;
////    }
////    else
////    {
////        g_sParameters.ucFlags &= ~SNTP_EN_FLAG;
////        //
////        // Shut down connections in preparation for the IP address change.
////        //
////        ConfigPreUpdateIPAddress();
////        ugSntp_Changed = false;
////    }
//   // g_sParameters.SNTPInterval = ulSntpIntervatSecond;
//    if( ugSntp_Changed )
//    {
//        sntp_init();
//    }
    //
    // Send the user back to the main status page.
    //
    return( SNTP_CGI_RESPONE );
}
static const char*
Config2CGIHandler1( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] )
{
//    int iParam;
//    tBoolean bParamError;
//    unsigned short     ulOverVoltage;
//    unsigned short     ulUVLO;
//    unsigned short     ulOverCurrent;
//    unsigned short     ulLeakCurrent;
//    //
//    // We have not encountered any parameter errors yet.
//    //
//    bParamError = false;
//    //
//    // General overvoltage from the cgi parameters of http.
//    //
//    ulOverVoltage = ( unsigned short )ConfigGetCGIParam1( "ovvol", pcParam,
//                    pcValue,
//                    iNumParams,
//                    &bParamError );
//    //
//    // General UVLO from the cgi parameters of http.
//    //
//    ulUVLO = ( unsigned short )ConfigGetCGIParam1( "uvlow", pcParam,
//             pcValue,
//             iNumParams,
//             &bParamError );
//    //
//    // General over current from the cgi parameters of http.
//    //
//    ulOverCurrent = ( unsigned short )ConfigGetCGIParam1( "overcur", pcParam,
//                    pcValue,
//                    iNumParams,
//                    &bParamError );
//    //
//    // General the leakage current from the cgi parameters of http.
//    //
//    ulLeakCurrent = ( unsigned short )ConfigGetCGIParam1( "leakcur", pcParam,
//                    pcValue,
//                    iNumParams,
//                    &bParamError );
//    if( bParamError )
//    {
//        //
//        // Yes - tell the user there was an error.
//        //
//        return( PARAM_ERROR_RESPONSE );
//    }
    //
    // Did any of the module parameters change?
    //
//    if( ( ulOverVoltage < VOL_UPPER_LIMIT ) && ( ulUVLO > VOL_LOWER_LIMIT ) && ( ulOverCurrent <= CUR_UPPER_LIMIT ) && ( ulLeakCurrent < LEAK_UPPER_LIMIT ) )
//    {
//        g_sParameters.OverVoltage = ulOverVoltage;
//        g_sParameters.UVLO = ulUVLO;
//        g_sParameters.OverCurrent = ulOverCurrent;
//        g_sParameters.LeakCurrent = ulLeakCurrent;
//    }
    //
    // Send the user back to the main status page.
    //
    return( CONFIG2_CGI_RESPONE );
}

//*****************************************************************************
//
//! \internal
//!
//! Performs processing for the URI ``/ip.cgi''.
//!
//! \param iIndex is an index into the g_psConfigCGIURIs array indicating which
//! CGI URI has been requested.
//! \param uNumParams is the number of entries in the pcParam and pcValue
//! arrays.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting this CGI.
//! \param pcValue is an array of character pointers, each containing the value
//! of a parameter as encoded in the URI requesting this CGI.
//!
//! This function is called whenever the HTTPD server receives a request for
//! URI ``/ip.cgi''.  Parameters from the request are parsed into the
//! \e pcParam and \e pcValue arrays such that the parameter name and value
//! are contained in elements with the same index.  The strings contained in
//! \e pcParam and \e pcValue contain all replacements and encodings performed
//! by the browser so the CGI function is responsible for reversing these if
//! required.
//!
//! After processing the parameters, the function returns a fully qualified
//! filename to the HTTPD server which will then open this file and send the
//! contents back to the client in response to the CGI.
//!
//! This specific CGI expects the following parameters:
//!
//! - ``staticip'' contains ``1'' to use a static IP address or ``0'' to use
//!   DHCP/AutoIP.
//! - ``sip1'' contains the first digit of the static IP address.
//! - ``sip2'' contains the second digit of the static IP address.
//! - ``sip3'' contains the third digit of the static IP address.
//! - ``sip4'' contains the fourth digit of the static IP address.
//! - ``gip1'' contains the first digit of the gateway IP address.
//! - ``gip2'' contains the second digit of the gateway IP address.
//! - ``gip3'' contains the third digit of the gateway IP address.
//! - ``gip4'' contains the fourth digit of the gateway IP address.
//! - ``mip1'' contains the first digit of the subnet mask.
//! - ``mip2'' contains the second digit of the subnet mask.
//! - ``mip3'' contains the third digit of the subnet mask.
//! - ``mip4'' contains the fourth digit of the subnet mask.
//!
//! \return Returns a pointer to a string containing the file which is to be
//! sent back to the HTTPD client in response to this request.
//
//*****************************************************************************
static const char*
ConfigIPCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                    char* pcValue[] )
{
    int iParam;
    long lValue;
    tBoolean bChanged;
    tBoolean bError;
    tBoolean bParamError;
    long lMode;
    unsigned long ulIPAddr;
    unsigned long ulGatewayAddr;
    unsigned long ulSubnetMask;
    unsigned long ulDNSIPAddr;
    unsigned char  ulMACAddr[6];
    unsigned short ulWebport;
    //
    // Nothing has changed and we have seen no errors so far.
    //
    bChanged = false;
    bParamError = false;
    ulIPAddr = 0;
    ulGatewayAddr = 0;
    ulSubnetMask = 0;
    //
    //General the mac address from the cgi of http.
    //
    if( syslog )
    {
        bParamError = ConfigGetCGIMacaddr( "macadd", iNumParams, pcParam, pcValue, ( char* )ulMACAddr, MAX_MACSTRING_LEN );
    }
    //
    // Get the IP selection mode.
    //
    lMode = ConfigGetCGIParam( "staticip", pcParam, pcValue, iNumParams,
                               &bParamError );
    //
    // This parameter is required so tell the user there has been a problem if
    // it is not found or is invalid.
    //
    if( bParamError )
    {
        return( PARAM_ERROR_RESPONSE );
    }
    //
    // If we are being told to use a static IP, read the remaining information.
    //
    if( lMode )
    {
        //
        // Get the static IP address to use.
        //
        ulIPAddr = ConfigGetCGIIPAddr( "sip", pcParam, pcValue, iNumParams,
                                       &bParamError );
        ulSubnetMask = ConfigGetCGIIPAddr( "mip", pcParam, pcValue, iNumParams,
                                           &bParamError );
        //
        // Get the gateway IP address to use.
        //
        ulGatewayAddr = ConfigGetCGIIPAddr( "gip", pcParam, pcValue, iNumParams,
                                            &bParamError );
    }
    //
    // Get the dns IP address to use.
    //
    ulDNSIPAddr  = ConfigGetCGIIPAddr( "dns", pcParam, pcValue, iNumParams,
                                       &bParamError );
    if( syslog )
    {
        //
        // Find the "wpt" parameter.
        //
        bError = false;
        ulWebport = ( unsigned short )ConfigGetCGIParam( "wpt", pcParam, pcValue, iNumParams,
                    &bError );
        if( !bError )
            if( ( ulWebport != g_sParameters.webport ) && ( ulWebport != g_sParameters.usLocationURLPort ) && ( ulWebport >= 80 ) && ( ulWebport <= 8080 ) )
            {
                g_sParameters.webport = ulWebport;
                g_sWorkingDefaultParameters = g_sParameters;
                ConfigSave();
                bReset   = true;
            }
        //              //
        //              // Find the "port" parameter.
        //              //
        //              bError = false;
        //              lValue = ConfigGetCGIParam("port", pcParam, pcValue, iNumParams, &bError);
        //              if(!bError)
        //              {
        //                      //
        //                      // The parameter was a valid decimal number.  If it is different
        //                      // from the current value, store it and note that we made a change
        //                      // that needs saving.
        //                      //
        //                      if((unsigned short int)lValue !=
        //                           g_sWorkingDefaultParameters.usLocationURLPort)
        //                      {
        //                              //
        //                              // Shut down UPnP temporarily.
        //                              //
        //                              //UPnPStop();
        //
        //                              //
        //                              // Update our working parameters and the default set.
        //                              //
        //                              g_sParameters.usLocationURLPort = (unsigned short)lValue;
        //                              g_sWorkingDefaultParameters.usLocationURLPort =
        //                                      (unsigned short int)lValue;
        //
        //                              //
        //                              // Restart UPnP with the new location port number.
        //                              //
        //                              //UPnPStart();
        //
        //                              //
        //                              // Remember that something changed.
        //                              //
        //                              bChanged = true;
        //                      }
        //              }
    }
    //
    // Make sure we read all the required parameters correctly.
    //
    if( bParamError )
    {
        //
        // Oops - some parameter was invalid.
        //
        return( PARAM_ERROR_RESPONSE );
    }
    //
    // We have all the parameters so determine if anything changed.
    //
    //
    // Did the basic mode change?
    //
    if( ( lMode && !( g_sParameters.ucFlags & CONFIG_FLAG_STATICIP ) ) ||
            ( !lMode && ( g_sParameters.ucFlags & CONFIG_FLAG_STATICIP ) ) )
    {
        //
        // The mode changed so set the new mode in the parameter block.
        //
        if( !lMode )
        {
            g_sParameters.ucFlags &= ~CONFIG_FLAG_STATICIP;
        }
        else
        {
            g_sParameters.ucFlags |= CONFIG_FLAG_STATICIP;
        }
        //
        // Remember that something changed.
        //
        bChanged = true;
    }
    //
    // If we are now using static IP, check for modifications to the IP
    // addresses and mask.
    //
    if( lMode )
    {
        if( ( g_sParameters.ulStaticIP != ulIPAddr ) ||
                ( g_sParameters.ulGatewayIP != ulGatewayAddr ) ||
                ( g_sParameters.ulSubnetMask != ulSubnetMask ) ||
                ( g_sParameters.ulDNSAddr != ulDNSIPAddr ) )
        {
            //
            // Something changed so update the parameter block.
            //
            bChanged = true;
            g_sParameters.ulStaticIP = ulIPAddr;
            g_sParameters.ulGatewayIP = ulGatewayAddr;
            g_sParameters.ulSubnetMask = ulSubnetMask;
            g_sParameters.ulDNSAddr = ulDNSIPAddr;
            g_sWorkingDefaultParameters = g_sParameters;
            ConfigSave();
        }
    }
    if( syslog )
    {
        memcpy( g_sParameters.ulMACAddr, ulMACAddr, 6 );
    }
    //
    // If anything changed, we need to resave the parameter block.
    //
    if( bChanged )
    {
        //
        // Shut down connections in preparation for the IP address change.
        //
        ConfigPreUpdateIPAddress();
        //
        // Update the working default set and save the parameter block.
        //
        //g_sWorkingDefaultParameters = g_sParameters;
        //ConfigSave();
        //
        // Tell the main loop that a IP address update has been requested.
        //
        g_bChangeIPAddress = true;
        //
        // Direct the browser to a page warning about the impending IP
        // address change.
        //
        return( IP_UPDATE_RESPONSE );
    }
    //
    // Direct the user back to our miscellaneous settings page.
    //
    return( MISC_PAGE_URI );
}

//*****************************************************************************
//
//! \internal
//!
//! Performs processing for the URI ``/misc.cgi''.
//!
//! \param iIndex is an index into the g_psConfigCGIURIs array indicating which
//! CGI URI has been requested.
//! \param uNumParams is the number of entries in the pcParam and pcValue
//! arrays.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting this CGI.
//! \param pcValue is an array of character pointers, each containing the value
//! of a parameter as encoded in the URI requesting this CGI.
//!
//! This function is called whenever the HTTPD server receives a request for
//! URI ``/misc.cgi''.  Parameters from the request are parsed into the
//! \e pcParam and \e pcValue arrays such that the parameter name and value
//! are contained in elements with the same index.  The strings contained in
//! \e pcParam and \e pcValue contain all replacements and encodings performed
//! by the browser so the CGI function is responsible for reversing these if
//! required.
//!
//! After processing the parameters, the function returns a fully qualified
//! filename to the HTTPD server which will then open this file and send the
//! contents back to the client in response to the CGI.
//!
//! This specific CGI expects the following parameters:
//!
//! - ``modname'' provides a string to be used as the friendly name for the
//!   module.  This is encoded by the browser and must be decoded here.
//! - ``port'' supplies TCP port to be used by UPnP.
//!
//! \return Returns a pointer to a string containing the file which is to be
//! sent back to the HTTPD client in response to this request.
//
//*****************************************************************************
static const char*
ConfigTrapCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                      char* pcValue[] )
{
    int iParam;
    long lValue;
    tBoolean bParamError;
    tBoolean bError;
    unsigned char ulTrapEn ;
    unsigned short ulTrapPort;
    //
    // We have not encountered any parameter errors yet.
    //
    bParamError = false;
    //
    // SNTP Enable flag
    //
    ulTrapEn = ( unsigned char )ConfigGetCGIParam( "trapen", pcParam,
               pcValue,
               iNumParams,
               &bParamError );
    //
    // SNTP URL/IP Adress.
    //
    iParam = ConfigFindCGIParameter( "trapip", pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                ( char* )g_sParameters.TrapService,
                                URL_NAME_LEN );
    }
    //
    //General the SNTP Intervate seconds from the http cgi.
    //
    bError = false;
    ulTrapPort = ( unsigned short )ConfigGetCGIParam( "trapt", pcParam, pcValue, iNumParams,
                 &bError );
    //
    // We have now read all the parameters and made sure that they are valid
    // decimal numbers.  Did we see any errors during this process?
    //
//    if( bParamError || ( ulTrapEn > TRAP_EN_UPPER_OPTION ) )
//    {
//        //
//        // Yes - tell the user there was an error.
//        //
//        return( PARAM_ERROR_RESPONSE );
//    }
    //
    // Did any of the serial parameters change?
    //
    if( ulTrapEn )
    {
        g_sParameters.ucFlags |= CONFIG_EN_TRAP;
    }
    else
    {
        g_sParameters.ucFlags &= ~CONFIG_EN_TRAP;
    }
    g_sParameters.TrapPort = ulTrapPort;
    //
    // Send the user back to the main status page.
    //
    return( TRAP_CGI_RESPONE );
}

//*****************************************************************************
//
//! \internal
//!
//! Performs processing for the URI ``/defaults.cgi''.
//!
//! \param iIndex is an index into the g_psConfigCGIURIs array indicating which
//! CGI URI has been requested.
//! \param uNumParams is the number of entries in the pcParam and pcValue
//! arrays.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting this CGI.
//! \param pcValue is an array of character pointers, each containing the value
//! of a parameter as encoded in the URI requesting this CGI.
//!
//! This function is called whenever the HTTPD server receives a request for
//! URI ``/defaults.cgi''.  Parameters from the request are parsed into the
//! \e pcParam and \e pcValue arrays such that the parameter name and value
//! are contained in elements with the same index.  The strings contained in
//! \e pcParam and \e pcValue contain all replacements and encodings performed
//! by the browser so the CGI function is responsible for reversing these if
//! required.
//!
//! After processing the parameters, the function returns a fully qualified
//! filename to the HTTPD server which will then open this file and send the
//! contents back to the client in response to the CGI.
//!
//! This specific CGI expects no specific parameters and any passed are
//! ignored.
//!
//! \return Returns a pointer to a string containing the file which is to be
//! sent back to the HTTPD client in response to this request.
//
//*****************************************************************************
static const char*
ConfigRegateCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                        char* pcValue[] )
{
    tBoolean bError;
    unsigned char ulReCnt;
    unsigned short ulDelay1;
    unsigned short ulDelay2;
    unsigned short ulDelay3;
    //
    //General the  recount from the cgi of http.
    //
    bError = false;
    ulReCnt = ConfigGetCGIParam( "recnt", pcParam, pcValue, iNumParams, &bError );
    //
    //General the  first delay from the cgi of http.
    //
    bError = false;
    ulDelay1 = ConfigGetCGIParam( "rgdey1", pcParam, pcValue, iNumParams, &bError );
    //
    //General the  second delay from the cgi of http.
    //
    bError = false;
    ulDelay2 = ConfigGetCGIParam( "rgdey2", pcParam, pcValue, iNumParams, &bError );
    //
    //General the  third delay from the cgi of http.
    //
    bError = false;
    ulDelay3 = ConfigGetCGIParam( "rgdey3", pcParam, pcValue, iNumParams, &bError );
    //
    // Make sure we read all the required parameters correctly.
    //
    if( bError )
    {
        //
        // Oops - some parameter was invalid.
        //
        return( PARAM_ERROR_RESPONSE );
    }
    //
    //save the parameters into g_sParameters.
    //
//    if( ulReCnt <= RECNT_UPPER_LIMIT )
//    {
//        g_sParameters.rgcnt = ulReCnt;
//    }
//    if( ( ulDelay1 >= REDEY_LOWER_LIMIT ) && ( ulDelay1 <= REDEY_UPPER_LIMIT ) )
//    {
//        g_sParameters.delay1 = ulDelay1;
//    }
//    if( ( ulDelay2 >= REDEY_LOWER_LIMIT ) && ( ulDelay2 <= REDEY_UPPER_LIMIT ) )
//    {
//        g_sParameters.delay2 = ulDelay2;
//    }
//    if( ( ulDelay3 >= REDEY_LOWER_LIMIT ) && ( ulDelay3 <= REDEY_UPPER_LIMIT ) )
//    {
//        g_sParameters.delay3 = ulDelay3;
//    }
    return( RECNT_CGI_RESPONSE );
}

//*****************************************************************************
//
//! \internal
//!
//! Performs processing for the URI ``/update.cgi''.
//!
//! \param iIndex is an index into the g_psConfigCGIURIs array indicating which
//! CGI URI has been requested.
//! \param uNumParams is the number of entries in the pcParam and pcValue
//! arrays.
//! \param pcParam is an array of character pointers, each containing the name
//! of a single parameter as encoded in the URI requesting this CGI.
//! \param pcValue is an array of character pointers, each containing the value
//! of a parameter as encoded in the URI requesting this CGI.
//!
//! This function is called whenever the HTTPD server receives a request for
//! URI ``/update.cgi''.  Parameters from the request are parsed into the
//! \e pcParam and \e pcValue arrays such that the parameter name and value
//! are contained in elements with the same index.  The strings contained in
//! \e pcParam and \e pcValue contain all replacements and encodings performed
//! by the browser so the CGI function is responsible for reversing these if
//! required.
//!
//! After processing the parameters, the function returns a fully qualified
//! filename to the HTTPD server which will then open this file and send the
//! contents back to the client in response to the CGI.
//!
//! This specific CGI expects no parameters and ignores all passed.
//!
//! \return Returns a pointer to a string containing the file which is to be
//! sent back to the HTTPD client in response to this request.
//
//*****************************************************************************
static const char*
ConfigSwitchCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                        char* pcValue[] )
{
    long lValue;
    tBoolean bError;
    unsigned char iindex;
    unsigned char ulChannel1En;
    unsigned char ulChannel2En;
    unsigned char ulChannel3En;
    unsigned char ulChannel4En;
   // tTimeParameters  ulBootTime[4];
   // tTimeParameters  ulShutTime[4];
    //
    //General the swtich enable from the cgi of http.
    //
    bError = false;
    ulChannel1En = ConfigGetCGIParam( "swen1", pcParam, pcValue, iNumParams, &bError );
    //
    //General the hours of the boot time from the cgi of http.
    //
    bError = false;
    //ulBootTime[0].hour = ConfigGetCGIParam( "ch1on1", pcParam, pcValue, iNumParams, &bError );
    //
    //General the minutes of the boot time from the cgi of http.
    //
    bError = false;
    //ulBootTime[0].minute = ConfigGetCGIParam( "ch1on2", pcParam, pcValue, iNumParams, &bError );
    //
    //General the hours of the shut time from the cgi of http.
    //
    bError = false;
    //ulShutTime[0].hour = ConfigGetCGIParam( "ch1on3", pcParam, pcValue, iNumParams, &bError );
    //
    //General the minutes of the shut time from the cgi of http.
    //
    bError = false;
    //ulShutTime[0].minute = ConfigGetCGIParam( "ch1on4", pcParam, pcValue, iNumParams, &bError );
    /*********the parameters of the channel 2 time fixed ************/
    bError = false;
    ulChannel2En = ConfigGetCGIParam( "swen2", pcParam, pcValue, iNumParams, &bError );
    //
    //General the hours of the boot time from the cgi of http.
    //
    bError = false;
    //ulBootTime[1].hour = ConfigGetCGIParam( "ch2on1", pcParam, pcValue, iNumParams, &bError );
    //
    //General the minutes of the boot time from the cgi of http.
    //
    bError = false;
    //ulBootTime[1].minute = ConfigGetCGIParam( "ch2on2", pcParam, pcValue, iNumParams, &bError );
    //
    //General the hours of the shut time from the cgi of http.
    //
    bError = false;
    //ulShutTime[1].hour = ConfigGetCGIParam( "ch2on3", pcParam, pcValue, iNumParams, &bError );
    //
    //General the minutes of the shut time from the cgi of http.
    //
    bError = false;
    //ulShutTime[1].minute = ConfigGetCGIParam( "ch2on4", pcParam, pcValue, iNumParams, &bError );
    /*********the parameters of the channel 3 time fixed ************/
    bError = false;
    ulChannel3En = ConfigGetCGIParam( "swen3", pcParam, pcValue, iNumParams, &bError );
    //
    //General the hours of the boot time from the cgi of http.
    //
    bError = false;
    //ulBootTime[2].hour = ConfigGetCGIParam( "ch3on1", pcParam, pcValue, iNumParams, &bError );
    //
    //General the minutes of the boot time from the cgi of http.
    //
    bError = false;
    //ulBootTime[2].minute = ConfigGetCGIParam( "ch3on2", pcParam, pcValue, iNumParams, &bError );
    //
    //General the hours of the shut time from the cgi of http.
    //
    bError = false;
    //ulShutTime[2].hour = ConfigGetCGIParam( "ch3on3", pcParam, pcValue, iNumParams, &bError );
    //
    //General the minutes of the shut time from the cgi of http.
    //
    bError = false;
    //ulShutTime[2].minute = ConfigGetCGIParam( "ch3on4", pcParam, pcValue, iNumParams, &bError );
    /*********the parameters of the channel 4 time fixed ************/
    bError = false;
    ulChannel4En = ConfigGetCGIParam( "swen4", pcParam, pcValue, iNumParams, &bError );
    //
    //General the hours of the boot time from the cgi of http.
    //
    bError = false;
    //ulBootTime[3].hour = ConfigGetCGIParam( "ch4on1", pcParam, pcValue, iNumParams, &bError );
    //
    //General the minutes of the boot time from the cgi of http.
    //
    bError = false;
    //ulBootTime[3].minute = ConfigGetCGIParam( "ch4on2", pcParam, pcValue, iNumParams, &bError );
    //
    //General the hours of the shut time from the cgi of http.
    //
    bError = false;
    //ulShutTime[3].hour = ConfigGetCGIParam( "ch4on3", pcParam, pcValue, iNumParams, &bError );
    //
    //General the minutes of the shut time from the cgi of http.
    //
    bError = false;
    //ulShutTime[3].minute = ConfigGetCGIParam( "ch4on4", pcParam, pcValue, iNumParams, &bError );
    //
    //bError.
    //
    if( bError )
    {
        //
        // Yes - tell the user there was an error.
        //
        return( PARAM_ERROR_RESPONSE );
    }
    //
    //channel 1-4 time enable flag set.
    //
//    if( ulChannel1En )
//    {
//        g_sParameters.remote_cmd |= imark[4] ;
//    }
//    else
//    {
//        g_sParameters.remote_cmd &= ~imark[4] ;
//    }
//    if( ulChannel2En )
//    {
//        g_sParameters.remote_cmd |= imark[5] ;
//    }
//    else
//    {
//        g_sParameters.remote_cmd &= ~imark[5] ;
//    }
//    if( ulChannel3En )
//    {
//        g_sParameters.remote_cmd |= imark[6] ;
//    }
//    else
//    {
//        g_sParameters.remote_cmd &= ~imark[6] ;
//    }
//    if( ulChannel4En )
//    {
//        g_sParameters.remote_cmd |= imark[7] ;
//    }
//    else
//    {
//        g_sParameters.remote_cmd &= ~imark[7] ;
//    }
//    //
//    //save the parameters into g_sParameters.
//    //
//    for( iindex = 0; iindex < 4; iindex++ )
//    {
//        if( ( ulBootTime[iindex].hour < 24 ) && ( ulBootTime[iindex].minute < 60 ) )
//        {
//            g_sParameters.boottime[iindex] = ulBootTime[iindex];
//        }
//        if( ( ulShutTime[iindex].hour < 24 ) && ( ulShutTime[iindex].minute < 60 ) )
//        {
//            g_sParameters.shuttime[iindex] = ulShutTime[iindex];
//        }
//    }
    //
    //Respone the cgi html.
    //
    return( TFIX_CGI_RESPONE );
    //   bReset=true;
    //   return(DEFAULT_CGI_RESPONSE);
}
extern unsigned char keepalive_status;
extern unsigned char  Beathearfailcn;
extern unsigned link_status;
extern unsigned short Beathearstopcn;
extern unsigned short BeathearCNT;

static const char*
ConfigInfoCGIHandler( int iIndex, int iNumParams, char* pcParam[],
                      char* pcValue[] )
{
    int iParam;
    //long lValue;
    // tBoolean bError;
    //
    //General the mode name  from the cgi of http .
    //
    //
    // Find the "modname" parameter.
    //
    iParam = ConfigFindCGIParameter( "modname", pcParam, iNumParams );
    if( iParam != -1 )
    {
//        ConfigDecodeFormString( pcValue[iParam],
//                                ( char* )g_sParameters.ucModName,
//                                MOD_NAME_LEN );
    }
    //
    // Find the "model" parameter.
    //
    iParam = ConfigFindCGIParameter( "model", pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                ( char* )g_sParameters.Model,
                                DEVICE_INFO_LEN );
    }
    //
    // Find the "manu" parameter.
    //
    iParam = ConfigFindCGIParameter( "manu", pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                ( char* )g_sParameters.Manufacturer,
                                DEVICE_INFO_LEN );
    }
    //
    // Find the "serial" parameter.
    //
    iParam = ConfigFindCGIParameter( "ser", pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                ( char* )g_sParameters.Serial,
                                13 );
    }
    //
    // Find the "pos" parameter.
    //
    iParam = ConfigFindCGIParameter( "pos", pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                ( char* )g_sParameters.InstallPos,
                                DEVICE_INFO_LEN );
    }
    //
    // Find the "instal" parameter.
    //
    iParam = ConfigFindCGIParameter( "instal", pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                ( char* )g_sParameters.InstallPer,
                                DEVICE_INFO_LEN );
    }
    //
    // Find the "pos" parameter.
    //
    iParam = ConfigFindCGIParameter( "instime", pcParam, iNumParams );
    if( iParam != -1 )
    {
        ConfigDecodeFormString( pcValue[iParam],
                                ( char* )g_sParameters.InstallTime,
                                DEVICE_INFO_LEN );
    }
    return( INFO_CGI_RESONSE );
}
//
//clear the buffer,memset 0.
//
static  void  clear_response_bufer( unsigned char* buffer )
{
    memset( buffer, 0, strlen( ( const char* )buffer ) );
}
//
//！ clear sub buffer
//
static void clear_sub_buffer( unsigned char* buffer )
{
    memset( buffer, 0, strlen( ( const char* )buffer ) );
}
//
//!generate a string
//
static void Generate_String( u8_t quene, unsigned char* buffer )
{
    unsigned char i = 0;
    clear_sub_buffer( buffer );
    switch( quene )
    {
//        case XML_SYSVOL_STRING:
//        {
//            //          if(syslog)
//            //              usnprintf(buffer,XML_TMPBUF_SIZE,"<input type='text' value='%d.%02d' name='vref' maxlength='6' size='8'>",analogValue[0]/100,analogValue[0]%100);
//            //          else
//            usnprintf((char *)buffer, XML_TMPBUF_SIZE, "%d.%02d", analogValue[0] / 100, analogValue[0] % 100 );
//        }
//        break;
//        case XML_SYSCUR_STRING:
//        {
//            //        if(syslog)
//            //             usnprintf(buffer,XML_TMPBUF_SIZE,"<input type='text' value='%d.%02d'name='iref' maxlength='6' size='8'>",analogValue[2]/100,analogValue[2]%100);
//            //        else
//            usnprintf((char *)buffer, XML_TMPBUF_SIZE, "%d.%02d", analogValue[2] / 100, analogValue[2] % 100 );
//        }
//        break;
//        case XML_SYSTMP_STRING:
//        {
//            usnprintf((char *)buffer, XML_TMPBUF_SIZE, "%d.%d", temp_value / 10, temp_value % 10 );
//        }
//        break;
//        case XML_SYSHUI_STRING:
//        {
//            usnprintf((char *)buffer, XML_TMPBUF_SIZE, "%d.%d", humi_value / 10, humi_value % 10 );
//        }
//        break;
//        case XML_CH1STA_STRING:
//        case XML_CH2STA_STRING:
//        case XML_CH3STA_STRING:
//        case XML_CH4STA_STRING:
//        {
//            i = quene - XML_CH1STA_STRING;
////            if( ChannelStatus & imark[i] )
////            {
////                usnprintf( ( char* )buffer, XML_TMPBUF_SIZE, "A" );
////            }
////            else
////            {
////                usnprintf( ( char* )buffer, XML_TMPBUF_SIZE, "B" );
////            }
//        }
//        break;
//        case XML_CH1RSN_STRING:
//        case XML_CH2RSN_STRING:
//        case XML_CH3RSN_STRING:
//        case XML_CH4RSN_STRING:
//        {
////            i = quene - XML_CH1RSN_STRING;
////            if( SysAlarm_Flag & HIGH_RISK_BIT )
////            {
////                usnprintf( buffer, XML_TMPBUF_SIZE, "C" );
////            }
////            else if( g_sParameters.remote_cmd & imark[i] )
////            {
////                usnprintf( buffer, XML_TMPBUF_SIZE, "A" );
////            }
////            else
////            {
////                usnprintf( buffer, XML_TMPBUF_SIZE, "B" );
////            }
//        }
//        break;
        default:
            break;
    }
}
//
//! insert to the RESPONE
//
static void Insert2XML_Respone_String( void )
{
    unsigned char uLoopCnt = 0;
    unsigned char uTempBuf[XML_TMPBUF_SIZE];
//    for( uLoopCnt = 0; uLoopCnt <= XML_CH4RSN_STRING; uLoopCnt++ )
//    {
//        Generate_String( uLoopCnt, uTempBuf );
//        strcat( ( char* )( data_response_buf ), uTempBuf );
//        strcat( ( char* )( data_response_buf ), ";" );
//    }
}

//
//xml respone .
//
static const char* Orther_CGIHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] )
{
    clear_response_bufer( data_response_buf );
    Insert2XML_Respone_String();
    return RESPONSE_PAGE_SET_CGI_RSP_URL;
}
//
//xml passwrd respone.
//
static const char* Opther_CGIHandler( int iIndex, int iNumParams, char* pcParam[], char* pcValue[] )
{
    char buf[32];
    clear_response_bufer( data_response_buf );
    memset( buf, 0, sizeof( buf ) );
    sprintf( buf, "%s;root1234;", g_sParameters.userpasswd );
    strcat( ( char* )( data_response_buf ), buf );
    return RESPONSE_PAGE_SET_CGI_RSP_URL;
}

//
//! get gnss info .
//
void get_gnss_info(unsigned char *type, char startch, unsigned char *endstr, unsigned char *tmp)
{   
    char *p = NULL;
    char *p1 = NULL;
    char ok_flag = 0;
    //unsigned char len = 0;
    unsigned char buf[GNSS_STATELLITE_INFO_LEN];
  
    
    memset(buf, '\0', GNSS_STATELLITE_INFO_LEN);
    gnss_receiver_get_info(type, buf);
    //
    //! debug gnss info
    //
    MTFS30_DEBUG("gnss type,%s:%s!\r\n",type,buf);
   
    if(endstr != NULL)
        p = strstr((char const*)buf, (char const*)endstr);
    
    MTFS30_DEBUG("findstr gnss type,%s:%s!\r\n",type,p);
      
    if(startch != '\0')
    {
        p1 = p;
        while(p1--)
        {
          if(p1 == (char *)&buf[0])
              break;
          
          if(*p1 == startch )
          {
             ok_flag = 1;
              break;
          } 
        }
        
        if(ok_flag)  
        {
          strncpy((char*)tmp, p1+1, p - p1 -1);
          tmp[p-p1] = '\0';
        }
        else
          memset(tmp, '\0', sizeof(tmp)); 
    }
    else
    {
        if(p)
        {
           
           *p = '\0';
           strncpy((char*)tmp, buf, strlen((const char *)buf));
        }
        else
           memset(tmp, '\0', sizeof(tmp));
    }   
}

//
//! split gnss info1 .
//
void split_gnss_info(unsigned char *type, char startch, char endch,unsigned char split_num,
                            unsigned char end_num, unsigned char *tmp)
{   
    char *p = NULL;
    char *p1 = NULL;
    char ok_flag = 0;
    unsigned short len = 0;
    unsigned short inx =0;
    unsigned char count = 0;
    unsigned char count1 = 0;
    unsigned char buf[GNSS_STATELLITE_INFO_LEN];
  
    
    memset(buf, '\0', GNSS_STATELLITE_INFO_LEN);
    gnss_receiver_get_info(type, buf);
    
    if((startch != '\0') && (endch != '\0'))
    {
       len =  strlen(buf);
       for(inx = 0; inx < len; inx++)
       {
         if(buf[inx] == startch)
         {
           count++; 
           if(split_num == count)
              p = &buf[inx];
         }
                 
         
         if(buf[inx] == endch)
         {
           count1++; 
           if(end_num == count1)
              p1 = &buf[inx];
         }
       }
       
       if(p == NULL || p1 == NULL)
         memset(tmp, '\0', sizeof(tmp)); 
       else
       {
         strncpy(tmp, p+1, p1 - p -1);
         tmp[p-p1] = '\0';
       }
    } 
}

//
//! split gnss info1 .
//
void split1_gnss_info(unsigned char *type, char* startstr, char endch,
                             unsigned char *tmp)
{   
    char *p = NULL;
    char *p1 = NULL;
    char ok_flag = 0;
    unsigned short len = 0;
    unsigned char buf[GNSS_STATELLITE_INFO_LEN];
  
    
    memset(buf, '\0', GNSS_STATELLITE_INFO_LEN);
    gnss_receiver_get_info(type, buf);
    
    if((startstr != NULL) && (endch != '\0'))
    {
       len =  strlen((char *)buf);
       p =  strstr(buf, startstr);
       p1 = p;
       
       while(p1++)
       {
         if(p1 == &buf[len-1])
         {
           if(*p1 == endch)
           {
             ok_flag = 1;
             break;
           }
           else
              break;
         }
         
         if(*p1 == endch)
         {
           ok_flag = 1;
           break;
         }
       }
       
       if(ok_flag)
       {
         strncpy((char *)tmp, p, p1 - p);
         tmp[p-p1] = '\0';
       }
       else
         memset(tmp, '\0', sizeof(tmp));   
    } 
}
//*****************************************************************************
//! \internal
//!
//! Provides replacement text for each of our configured SSI tags.
//!
//! \param iIndex is an index into the g_pcConfigSSITags array and indicates
//! which tag we are being passed
//! \param pcInsert points to a buffer into which this function should write
//! the replacement text for the tag.  This should be plain text or valid HTML
//! markup.
//! \param iInsertLen is the number of bytes available in the pcInsert buffer.
//! This function must ensure that it does not write more than this or memory
//! corruption will occur.
//!
//! This function is called by the HTTPD server whenever it is serving a page
//! with a ``.ssi'', ``.shtml'' or ``.shtm'' file extension and encounters a
//! tag of the form <!--#tagname--> where ``tagname'' is found in the
//! g_pcConfigSSITags array.  The function writes suitable replacement text to
//! the \e pcInsert buffer.
//!
//! \return Returns the number of bytes written to the pcInsert buffer, not
//! including any terminating NULL.
//
//*****************************************************************************
unsigned char page_en = 0;
static uint16_t
ConfigSSIHandler( int iIndex, char* pcInsert, int iInsertLen )
{
    //unsigned long ulPort;
    int iCount;
    unsigned char tmp_buf[GNSS_STATELLITE_INFO_LEN];
    //unsigned char command[COMM_MAX_LEN];
    //
    // Which SSI tag are we being asked to provide content for?
    //
    switch( iIndex )
    {
        //
        //initialize of the GNSS parameters,when onload html.
        //
       case SSI_INDEX_GNSSVARS:
       {
 
        //
        //! ssi tag insert.
        iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
              if( iCount < iInsertLen )
              {
                  iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
                                       GNSS_JAVASCRIPT_VARS,
                                       (g_sParameters.GnssParameters.enable ),
                                       (g_sParameters.GnssParameters.mode)
                                     );
              }
              if( iCount < iInsertLen )
              {
                  iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
                                       "%s", JAVASCRIPT_FOOTER );
              }
              return( iCount );
       }
       
       //
       // initialize ptp net parameters,when onload html.
       // 
       case SSI_INDEX_PTPNETVARS:
       {
         
         //
         //! call the application before ssi.
        
        user_cmd_parser("ifconfig vlan -e\n");
        user_cmd_parser("ifconfig vlan -c\n");
        user_cmd_parser("ifconfig vlan -p\n");
        
        //
        //! insert tag .
        iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
              if( iCount < iInsertLen )
              {
                  iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
                                       NET_JAVASCRIPT_VARS,
                                       g_sParameters.PtpNetParameters.vlan_enable,
                                       g_sParameters.PtpNetParameters.vlan_pcp,
                                       g_sParameters.PtpNetParameters.vlan_cfi);
              }
              if( iCount < iInsertLen )
              {
                  iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
                                       "%s", JAVASCRIPT_FOOTER );
              }
              return( iCount );
        }
       
       //
       // initialize ptp mode parameters, when onload html.
       //      
       case SSI_INDEX_PTPMODEVARS:
       {
         
               //
               //! call the application before ssi.
              user_cmd_parser("ptp2 p e\n");
              user_cmd_parser("ptp2 e e\n");
              user_cmd_parser("ptp2 p m\n");
              user_cmd_parser("ptp2 p u\n");
              user_cmd_parser("ptp2 p p\n");
              user_cmd_parser("ptp2 c t\n");
              user_cmd_parser("ptp2 p si\n");
              user_cmd_parser("ptp2 p ai\n");
              user_cmd_parser("ptp2 p n\n");
              
              
              iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
              if( iCount < iInsertLen )
              {
                  iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
                                       MOD_JAVASCRIPT_VARS,
                                       g_sParameters.PtpModeParameters.port_enable,
                                       g_sParameters.PtpModeParameters.esmc_enable,
                                       g_sParameters.PtpModeParameters.delay_type,
                                       g_sParameters.PtpModeParameters.unicast,
                                       g_sParameters.PtpModeParameters.encode_package,
                                       g_sParameters.PtpModeParameters.step_type,
                                       g_sParameters.PtpModeParameters.sync_frequency,
                                       g_sParameters.PtpModeParameters.anounce_frequency,
                                       g_sParameters.PtpModeParameters.protocol
                                     );
              }
              if( iCount < iInsertLen )
              {
                  iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
                                       "%s", JAVASCRIPT_FOOTER );
              }
              return( iCount );
        }
        
       //
       // Initialize the out parameters,when onload html.
       //      
       case SSI_INDEX_OUTVARS:
       {
              iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
              if( iCount < iInsertLen )
              {
                  iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
                                       OUT_JAVASCRIPT_VARS,
                                       g_sParameters.OutParameters.decttype,
                                       g_sParameters.OutParameters.protocol,
                                       g_sParameters.OutParameters.optical_sw,
                                       g_sParameters.OutParameters.detect_sw
                                     );
              }
              if( iCount < iInsertLen )
              {
                  iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
                                       "%s", JAVASCRIPT_FOOTER );
              }
              return( iCount );
        }
        
       //
       // Insert tag mark,when onload html.
       //
        case SSI_INDEX_GPSPRI:
        {
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='gpri' style='width:152' value='");
            if( iCount < iInsertLen )
            {
                iCount +=
                    ConfigEncodeFormString( ( char* )g_sParameters.GnssParameters.priority,
                                            pcInsert + iCount,
                                            iInsertLen - iCount );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'>");
            }
            return( iCount );
        }
        
        //
        //Insert the tag mark,when onload html.
        //
        case SSI_INDEX_GNSSDLYCOM:
        {
           //
           //call pps delay compensation.
            user_cmd_parser("gnss d c\n");
            
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='dlycom' style='width:152' value='" );
            if( iCount < iInsertLen )
            {
                /*iCount +=
                    ConfigEncodeFormString( ( char* )g_sParameters.GnssParameters.delaycom,
                                            pcInsert + iCount,
                                            iInsertLen - iCount );*/
                iCount +=
                        usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                   g_sParameters.GnssParameters.delaycom);
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",
                               8);
            }
            return( iCount );
        }
        
        //
        //Insert the tag mark,when onload html.
        //
        case SSI_INDEX_PTPMAC:
        {
            
            //
            //call application before insert tag.
            user_cmd_parser("ipconfig -s\n");
            
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='mac' style='width:152' disabled='disable' value='" );
            if( iCount < iInsertLen )
            {

                iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%02X:%02X:%02X:%02X:%02X:%02X",
                                     g_sParameters.PtpNetParameters.ptp_mac[0],
                                     g_sParameters.PtpNetParameters.ptp_mac[1],
                                     g_sParameters.PtpNetParameters.ptp_mac[2],
                                     g_sParameters.PtpNetParameters.ptp_mac[3],
                                     g_sParameters.PtpNetParameters.ptp_mac[4],
                                     g_sParameters.PtpNetParameters.ptp_mac[5]);
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'>");
            }
            return( iCount );
           
        }
        //
        //Insert the ptp ip address tag mark,when onload html.
        //
        case SSI_INDEX_PTPIP:
        {
            
            //
            //call the ip address inquery.
            //
            user_cmd_parser("ipconfig -a\n");
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='ip' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d.%d.%d.%d",
                                     ((g_sParameters.PtpNetParameters.ptp_ipaddr >> 24) & 0xff),
                                     ((g_sParameters.PtpNetParameters.ptp_ipaddr >> 16) & 0xff),
                                     ((g_sParameters.PtpNetParameters.ptp_ipaddr >> 8) & 0xff),
                                     (g_sParameters.PtpNetParameters.ptp_ipaddr  & 0xff)
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",15);
            }
            return( iCount );
           
        }
        //
        //Insert the ptp subnet mask address tag mark,when onload html.
        //
        case SSI_INDEX_PTPMASK:
        {
            
            //
            //call the netmask inquery.
            //
            user_cmd_parser("ipconfig -m\n");
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='mask' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d.%d.%d.%d",
                                     ((g_sParameters.PtpNetParameters.ptp_submask >> 24) & 0xff),
                                     ((g_sParameters.PtpNetParameters.ptp_submask >> 16) & 0xff),
                                     ((g_sParameters.PtpNetParameters.ptp_submask >> 8) & 0xff),
                                     (g_sParameters.PtpNetParameters.ptp_submask  & 0xff)
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",15);
            }
            return( iCount );
           
        }
        
        //
        //Insert the ptp gateway address tag mark,when onload html.
        //
        case SSI_INDEX_PTPGW:
        {
            //
            //call the gateway inquery.
            //
            user_cmd_parser("ipconfig -g\n");
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='gw' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d.%d.%d.%d",
                                     ((g_sParameters.PtpNetParameters.ptp_gateway >> 24) & 0xff),
                                     ((g_sParameters.PtpNetParameters.ptp_gateway >> 16) & 0xff),
                                     ((g_sParameters.PtpNetParameters.ptp_gateway >> 8) & 0xff),
                                     (g_sParameters.PtpNetParameters.ptp_gateway  & 0xff)
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",15);
            }
            return( iCount );
           
        }
        
        //
        //Insert the ptp vlan vid tag mark,when onload html.
        //
        case SSI_INDEX_PTPVID:
        {
            //
            //call the inquery.
            //
            user_cmd_parser("ifconfig vlan -v\n");
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='vid' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.PtpNetParameters.vlan_vid
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",4);
            }
            return( iCount );
           
        }
        
        //
        //Insert the ptp port tag mark,when onload html.
        //
        case SSI_INDEX_PTPPORT:
        {
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='port' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.PtpModeParameters.port
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",1);
            }
            return( iCount );
           
        }
        
        //
        //Insert the ptp port status tag mark,when onload html.
        //
        case SSI_INDEX_PTPPORTSTAT:
        {
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='stat' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                iCount +=
                    ConfigEncodeFormString( ( char* )g_sParameters.PtpModeParameters.status,
                                            pcInsert + iCount,
                                            iInsertLen - iCount );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",12);
            }
            return( iCount );
           
        }
        
        //
        //Insert the ptp domain tag mark,when onload html.
        //
        case SSI_INDEX_PTPDOMAIN:
        {
            
            //
            //call the inquery.
            //
            user_cmd_parser("ptp2 c d\n");
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='domain' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                 iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.PtpModeParameters.domain
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",3);
            }
            return( iCount );
           
        }
        
        //
        //Insert the ptp delay compensation tag mark,when onload html.
        //
        case SSI_INDEX_PTPDLYCOM:
        {
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='dlycom' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                 iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.PtpModeParameters.delay_com
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",8);
            }
            return( iCount );
           
        }
        
        //
        //Insert the ptp priority 1 tag mark,when onload html.
        //
        case SSI_INDEX_PTPPRIO1:
        {
            //
            //call the inquery.
            //
            user_cmd_parser("ptp2 c p\n");
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='prio1' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                 iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.PtpModeParameters.priority1
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",3);
            }
            return( iCount );
           
        }
         //
        //Insert the ptp priority 2 tag mark,when onload html.
        //
        case SSI_INDEX_PTPPRIO2:
        {
            //
            //call the inquery.
            //
            user_cmd_parser("ifconfig vlan -v\n");
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='prio2' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                 iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.PtpModeParameters.priority2
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",3);
            }
            return( iCount );
           
        }
        
        //
        //Insert out 1pps delay compensation tag mark,when onload html.
        //
        case SSI_INDEX_1PPSDLYCOM:
        {
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='sdlycom' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                 iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.OutParameters.sys_pps_dlycom
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",8);
            }
            return( iCount );
           
        }
        
        //
        //Insert out 1pps tod delay compensation tag mark,when onload html.
        //
        case SSI_INDEX_1PPSTODDLYCOM:
        {
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='stdlycom' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                 iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.OutParameters.pps_tod_dlycom
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",8);
            }
            return( iCount );
           
        }
        //
        //Insert out irigb delay compensation tag mark,when onload html.
        //
        case SSI_INDEX_IRIGBDLYCOM:
        {
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='idlycom' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                 iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.OutParameters.irigb_dlycom
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",8);
            }
            return( iCount );
           
        }
        //
        //Insert out 1pps duty tag mark,when onload html.
        //
        case SSI_INDEX_1PPSDUTY:
        {
            
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='duty' style='width:152' value='" );
            if( iCount < iInsertLen )
            {

                 iCount +=
                  usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
                                              g_sParameters.OutParameters.pps_duty
                                     );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "' maxlength='%d'>",3);
            }
            return( iCount );
           
        }
        
        //
        //Insert out antstatus tag mark,when onload html.
        //
        case SSI_INDEX_ANTSTATUS:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            get_gnss_info("ANT", '\0', ";", tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='antst' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                 iCount +=
                    ConfigEncodeFormString( ( char* )tmp_buf,
                                            pcInsert + iCount,
                                            iInsertLen - iCount );
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
        //
        //Insert gnss post status tag mark,when onload html.
        //
        case SSI_INDEX_POSTSTATUS:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            get_gnss_info("PPS", '\0', ";", tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='postst' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                 if(tmp_buf[0] == '1')
                     iCount +=
                        ConfigEncodeFormString( ( char* )"post",
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 else
                     iCount +=
                        ConfigEncodeFormString( ( char* )"hold",
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
        //
        //Insert gnss post satelite num tag mark,when onload html.
        //
        case SSI_INDEX_POSTNUM:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            get_gnss_info("LOC", '\0', ",", tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='postnum' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        //
        //Insert gnss 1pps status tag mark,when onload html.
        //
        case SSI_INDEX_PPSSTATUS:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            get_gnss_info("PPS", '\0', ";", tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='ppsst' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
        //
        //Insert prede leap tag mark,when onload html.
        //
        case SSI_INDEX_PRELEAP:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            get_gnss_info("LEAP", ',', ";", tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='preleap' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
        //
        //Insert  leap tag mark,when onload html.
        //
        case SSI_INDEX_LEAP:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            split_gnss_info("LEAP", ',', ',', 5, 6, tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='leap' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
         //
        //Insert  UTC time tag mark,when onload html.
        //
        case SSI_INDEX_UTC:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            get_gnss_info("TIME", '\0', ";", tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='utc' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
        //
        //Insert  latitude tag mark,when onload html.
        //
        case SSI_INDEX_LATITUDE:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            split_gnss_info("LOC", ',', ',', 2, 4, tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='lat' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
         //
        //Insert  longitude tag mark,when onload html.
        //
        case SSI_INDEX_LONGITUDE:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            split_gnss_info("LOC", ',', ',', 4, 6, tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='longti' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        //
        //Insert  altitude tag mark,when onload html.
        //
        case SSI_INDEX_ALTITUDE:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            get_gnss_info("LOC", ',', ";", tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='altitu' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        //
        //Insert  gnss mode tag mark,when onload html.
        //
        case SSI_INDEX_GNSSMODE:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            get_gnss_info("VER", '\0', ";", tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='gnssmd' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
        //
        //Insert  satellite GPS tag mark,when onload html.
        //
        case SSI_INDEX_GPS:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            split1_gnss_info("SAT", "GPS", ';',tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='gps' style='width:900' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
         //
        //Insert  satellite BDS tag mark,when onload html.
        //
        case SSI_INDEX_BDS:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            split1_gnss_info("SAT", "BDS", ';', tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='bds' style='width:900' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        //
        //Insert  satellite GLO tag mark,when onload html.
        //
        case SSI_INDEX_GLO:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            split1_gnss_info("SAT", "GLO", ';', tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='glo' style='width:900' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
        //
        //Insert  satellite GAL tag mark,when onload html.
        //
        case SSI_INDEX_GAL:
        {   
            memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            split1_gnss_info("SAT", "GAL", ';', tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='gal' style='width:900' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )tmp_buf,
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        //
        //Insert  reference type tag mark,when onload html.
        //
        case SSI_INDEX_REFTYPE:
        {   
            //memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            //split1_gnss_info("SAT", "GAL", ';', tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='reftype' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )"GNSS",
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
        //
        //Insert  clock type tag mark,when onload html.
        //
        case SSI_INDEX_CLKTYPE:
        {   
            //memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
            //split1_gnss_info("SAT", "GAL", ';', tmp_buf);
            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='clktype' style='width:152' disabled='true' value='" );
            if( iCount < iInsertLen )
            {

                  iCount +=
                        ConfigEncodeFormString( ( char* )"OCXO",
                                                pcInsert + iCount,
                                                iInsertLen - iCount );
                 
            }
            if( iCount < iInsertLen )
            {
                iCount +=
                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
                               "'");
            }
            return( iCount );
           
        }
        
        //
        //Insert  reference status tag mark,when onload html.
        //
//        case SSI_INDEX_REFSTATUS:
//        {   
//            iCount = usnprintf( pcInsert, iInsertLen, "<input class='text_time' name='refstat' style='width:152' disabled='true' value='" );
//            if( iCount < iInsertLen )
//            {
//
//                  iCount +=
//                        ConfigEncodeFormString( ( char* )"OCXO",
//                                                pcInsert + iCount,
//                                                iInsertLen - iCount );
//                 
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "'");
//            }
//            return( iCount );
//           
//        }
        
        
        //SSI_INDEX_POSTSTATUSSSI_INDEX_POSTNUM SSI_INDEX_PPSSTATUS SSI_INDEX_PRELEAP 
        //
        //Insert the tag mark PTP ip address,when onload html.
        //
//        case SSI_INDEX_PTPIP://SSI_INDEX_PTPIP
//        {
//            if( syslog )
//            {
//                iCount = usnprintf( pcInsert, iInsertLen, "<input type='text' value='" );
//                if( iCount < iInsertLen )
//                {
//                    iCount +=
//                        usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d.%02d",
//                                   0, 0 );
//                }
//                if( iCount < iInsertLen )
//                {
//                    iCount +=
//                        usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                   "' name='iref' maxlength='%d' size='%d'>", 6, 8 );
//                }
//                return( iCount );
//            }
//            else
//                return( usnprintf( pcInsert, iInsertLen, "%d.%02d A.",
//                                   0,
//                                   0 ) );
//        }
//        case SSI_INDEX_TEMPVAR:
//        {
//            return( usnprintf( pcInsert, iInsertLen, "%d.%d",
//                               temp_value / 10,
//                               temp_value % 10 ) );
//        }
//        case SSI_INDEX_HUMIVAR:
//        {
//            return( usnprintf( pcInsert, iInsertLen, "%d.%d",
//                               humi_value / 10,
//                               humi_value % 10 ) );
//        }
//        case SSI_INDEX_CTRVARS:
//        {
//            unsigned char command[4];
////            command[0] = ( g_sParameters.remote_cmd & 0x01 ) ? 1 : 0;
////            command[1] = ( g_sParameters.remote_cmd & 0x02 ) ? 1 : 0;
////            command[2] = ( g_sParameters.remote_cmd & 0x04 ) ? 1 : 0;
////            command[3] = ( g_sParameters.remote_cmd & 0x08 ) ? 1 : 0;
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     CTR_JAVASCRIPT_VARS,
//                                     command[0],
//                                     command[1],
//                                     command[2],
//                                     command[3]
//                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_STATUS1:
//        case SSI_INDEX_STATUS2:
//        case SSI_INDEX_STATUS3:
//        case SSI_INDEX_STATUS4:
//        {
//            unsigned char aindex = ( iIndex - SSI_INDEX_STATUS1 );
//            iCount = usnprintf( pcInsert, iInsertLen, "<font color='" );
//            if( iCount < iInsertLen )
//            {
////                if( ChannelStatus & imark[aindex] )
////                {
////                    iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "green'>线路开</font>" );
////                }
////                else
////                {
////                    iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "red'>线路关</font>" );
////                }
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_LAMP1:
//        case SSI_INDEX_LAMP2:
//        case SSI_INDEX_LAMP3:
//        case SSI_INDEX_LAMP4:
//        {
//            unsigned char aindex = ( iIndex - SSI_INDEX_LAMP1 );
//            iCount = usnprintf( pcInsert, iInsertLen, "<font color='" );
//            if( iCount < iInsertLen )
//            {
//                if( 0 )//g_sParameters.remote_cmd & imark[aindex]
//                {
////                    if( ChannelStatus & imark[aindex] )
////                    {
////                        iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "green'>远程命令开</font>" );
////                    }
////                    else
////                    {
////                        iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "red'>报警</font>" );
////                    }
//                }
//                else
//                {
//                    iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "red'>远程命令关</font>" );
//                }
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_UNEARTH:
//        case SSI_INDEX_LNORDER:
//        case SSI_INDEX_LPROFAIL:
//        case SSI_INDEX_OVCURRA:
//        case SSI_INDEX_OVVOLA:
//        case SSI_INDEX_UVLOA:
//        case SSI_INDEX_LEAKCA:
//        case SSI_INDEX_INVCH1:
//        case SSI_INDEX_INVCH2:
//        case SSI_INDEX_INVCH3:
//        case SSI_INDEX_INVCH4:
//        {
//            unsigned char aindex = ( iIndex - SSI_INDEX_UNEARTH );
//            iCount = usnprintf( pcInsert, iInsertLen, "<font color='" );
//            if( iCount < iInsertLen )
//            {
////                if( SysAlarm_Flag & imark[aindex] )
////                {
////                    iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "red'>报警</font>" );
////                }
////                else
////                {
////                    iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "green'>无报警</font>" );
////                }
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_LAMP:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<font color='" );
//            if( iCount < iInsertLen )
//            {
//                if( 0 )
//                {
//                    iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "red'>报警</font>" );
//                }
//                else
//                {
//                    iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "green'>无报警</font>" );
//                }
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_OVERVOL:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input type='text' value='" );
//            if( iCount < iInsertLen )
//            {
////                iCount +=
////                    usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d.%02d",
////                               g_sParameters.OverVoltage / 100, g_sParameters.OverVoltage % 100 );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' name='ovvol' maxlength='%d' size='%d'>", 6, 10 );
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_UVLO:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input type='text' value='" );
//            if( iCount < iInsertLen )
//            {
////                iCount +=
////                    usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d.%02d",
////                               g_sParameters.UVLO / 100, g_sParameters.UVLO % 100 );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' name='uvlow' maxlength='%d' size='%d'>", 6, 10 );
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_OVERCUR:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input type='text' value='" );
//            if( iCount < iInsertLen )
//            {
////                iCount +=
////                    usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d.%02d",
////                               g_sParameters.OverCurrent / 100, g_sParameters.OverCurrent % 100 );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' name='overcur' maxlength='%d' size='%d'>", 5, 10 );
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_LEAKCUR:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input type='text' value='" );
//            if( iCount < iInsertLen )
//            {
////                iCount +=
////                    usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d.%02d",
////                               g_sParameters.LeakCurrent / 100, g_sParameters.LeakCurrent % 100 );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' name='leakcur' maxlength='%d' size='%d'>", 5, 10 );
//            }
//            return( iCount );
//        }
//        //
//        //! IP Config Vars
//        //
//        //
//        // The local MAC address tag "macaddr".
//        //
//        case SSI_INDEX_MACVARS:
//        {
//            if( syslog )
//            {
//                iCount = usnprintf( pcInsert, iInsertLen, "<input type='text' value='" );
//                if( iCount < iInsertLen )
//                {
//                    iCount +=
//                        usnprintf( pcInsert + iCount, iInsertLen - iCount, "%02X-%02X-%02X-%02X-%02X-%02X",
//                                   g_sParameters.ulMACAddr[0],
//                                   g_sParameters.ulMACAddr[1],
//                                   g_sParameters.ulMACAddr[2],
//                                   g_sParameters.ulMACAddr[3],
//                                   g_sParameters.ulMACAddr[4],
//                                   g_sParameters.ulMACAddr[5] );
//                }
//                if( iCount < iInsertLen )
//                {
//                    iCount +=
//                        usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                   "' name='macadd' maxlength='%d' size='%d'>", MAX_MACSTRING_LEN, MAX_MACSTRING_LEN );
//                }
//                return( iCount );
//            }
//            else
//                return( usnprintf( pcInsert, iInsertLen, "%02X-%02X-%02X-%02X-%02X-%02X",
//                                   g_sParameters.ulMACAddr[0],
//                                   g_sParameters.ulMACAddr[1],
//                                   g_sParameters.ulMACAddr[2],
//                                   g_sParameters.ulMACAddr[3],
//                                   g_sParameters.ulMACAddr[4],
//                                   g_sParameters.ulMACAddr[5] ) );
//        }
//        case SSI_INDEX_IPVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     IP_JAVASCRIPT_VARS,
//                                     ( g_sParameters.ucFlags &
//                                       CONFIG_FLAG_STATICIP ) ? 1 : 0,
//                                     ( g_sParameters.ulStaticIP >> 24 ) & 0xFF,
//                                     ( g_sParameters.ulStaticIP >> 16 ) & 0xFF,
//                                     ( g_sParameters.ulStaticIP >> 8 ) & 0xFF,
//                                     ( g_sParameters.ulStaticIP >> 0 ) & 0xFF );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_SNVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     SUBNET_JAVASCRIPT_VARS,
//                                     0,
//                                     ( g_sParameters.ulSubnetMask >> 24 ) & 0xFF,
//                                     ( g_sParameters.ulSubnetMask >> 16 ) & 0xFF,
//                                     ( g_sParameters.ulSubnetMask >> 8 ) & 0xFF,
//                                     ( g_sParameters.ulSubnetMask >> 0 ) & 0xFF );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        //
//        // Generate a block of JavaScript variables containing the current
//        // subnet mask.
//        //
//        case SSI_INDEX_GWVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     GW_JAVASCRIPT_VARS,
//                                     ( g_sParameters.ulGatewayIP >> 24 ) & 0xFF,
//                                     ( g_sParameters.ulGatewayIP >> 16 ) & 0xFF,
//                                     ( g_sParameters.ulGatewayIP >> 8 ) & 0xFF,
//                                     ( g_sParameters.ulGatewayIP >> 0 ) & 0xFF );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current DNS
//        // servicer IP adress.
//        //
//        case SSI_INDEX_DNSVAR:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     DNS_JAVASCRIPT_VARS,
//                                     ( g_sParameters.ulDNSAddr >> 24 ) & 0xFF,
//                                     ( g_sParameters.ulDNSAddr >> 16 ) & 0xFF,
//                                     ( g_sParameters.ulDNSAddr >> 8 ) & 0xFF,
//                                     ( g_sParameters.ulDNSAddr >> 0 ) & 0xFF,
//                                     g_sParameters.webport );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        case SSI_INDEX_WEBPORT:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input type='text' value='" );
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
//                               g_sParameters.webport );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' name='wpt' maxlength='%d' size='%d'>", 5, 10 );
//            }
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current UPnP port
//        // number.
//        //
//        case SSI_INDEX_PNPINP:
//        {
//            return( usnprintf( pcInsert, iInsertLen,
//                               "%d", g_sParameters.usLocationURLPort ) );
//        }
//        //
//        // Generate an HTML text input field containing the current Sntp enable/disable.
//        //
//        //
//        case SSI_INDEX_SNTPVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     SNTP_JAVASCRIPT_VARS,
////                                     ( g_sParameters.ucFlags &
////                                       CONFIG_EN_SNTP ) ? 1 : 0 );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current SNTP service IP address.
//        //
//        //
//        case SSI_INDEX_SNTPURL:
//        {
//            struct ip_addr ucSntpServiceIP;
//           char* pucurl = NULL;//( char* )g_sParameters.SNTPURL;
//            ucSntpServiceIP.addr = inet_addr( ( char* )pucurl );
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( ucSntpServiceIP.addr != INADDR_NONE )
//            {
//                if( iCount < iInsertLen )
//                {
//                    iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d.%d.%d.%d",
//                                         ( ( ucSntpServiceIP.addr >>  0 ) & 0xFF ),
//                                         ( ( ucSntpServiceIP.addr >>  8 ) & 0xFF ),
//                                         ( ( ucSntpServiceIP.addr >> 16 ) & 0xFF ),
//                                         ( ( ucSntpServiceIP.addr >> 24 ) & 0xFF ) );
//                }
//            }
//            else
//            {
//                if( iCount < iInsertLen )
//                {
//                    iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "%s", pucurl );
//                }
//            }
//            iCount +=
//                usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                           "' maxlength='%d' size='%d' name='ntpurl'>",
//                           ( URL_NAME_LEN - 1 ), 40 );
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current SNTP interval seconds.
//        //
//        //
//        case SSI_INDEX_SNTPINTER:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount, "%d",
////                                     g_sParameters.SNTPInterval );
//            }
//            iCount +=
//                usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                           "' maxlength='%d' size='%d' name='spinter'>",
//                           ( URL_NAME_LEN - 1 ), 40 );
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current snmp trap enable.
//        //
//        //
//        case SSI_INDEX_TRAPVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     SNMP_JAVASCRIPT_VARS,
//                                     ( g_sParameters.ucFlags &
//                                       CONFIG_EN_TRAP ) ? 1 : 0 );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        //
//        //
//        // SNMP trap ip address.
//        //
//        case SSI_INDEX_TRAPIP:
//        {
//            struct ip_addr ucTrapServiceIP;
//            char* pucurl = ( char* )g_sParameters.TrapService;
//            ucTrapServiceIP.addr = inet_addr( ( char* )pucurl );
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%d.%d.%d.%d",
//                                     ( ucTrapServiceIP.addr >> 0 ) & 0xff,
//                                     ( ucTrapServiceIP.addr >> 8 ) & 0xff,
//                                     ( ucTrapServiceIP.addr >> 16 ) & 0xff,
//                                     ( ucTrapServiceIP.addr >> 24 ) & 0xff
//                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "' maxlength='15' size='15' name='trapip'>" );
//            }
//            return( iCount );
//        }
//        //
//        //
//        // SNMP trap port.
//        //
//        case SSI_INDEX_TRAPPORT:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%d",
//                                     g_sParameters.TrapPort
//                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "' maxlength='5' size='15' name='trapt'>" );
//            }
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current swtith fixed time enable.
//        //
//        //
//        case SSI_INDEX_SWENVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     SWITCH_JAVASCRIPT_VARS,
////                                     ( ( g_sParameters.remote_cmd & imark[4] ) ? 1 : 0 ),
////                                     ( ( g_sParameters.remote_cmd & imark[5] ) ? 1 : 0 ),
////                                     ( ( g_sParameters.remote_cmd & imark[6] ) ? 1 : 0 ),
////                                     ( ( g_sParameters.remote_cmd & imark[7] ) ? 1 : 0 )
////                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current hours of swtith boot time.
//        //
//        //
//        case SSI_INDEX_BOOTHVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     BOOTH_JAVASCRIPT_VARS,
////                                     ( g_sParameters.boottime[0].hour ),
////                                     ( g_sParameters.boottime[1].hour ),
////                                     ( g_sParameters.boottime[2].hour ),
////                                     ( g_sParameters.boottime[3].hour )
////                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
        //
        // Generate an HTML text input field containing the current hours of swtith boot time.
        //
        //
//        case SSI_INDEX_BOOTMVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     BOOTM_JAVASCRIPT_VARS,
////                                     ( g_sParameters.boottime[0].minute ),
////                                     ( g_sParameters.boottime[1].minute ),
////                                     ( g_sParameters.boottime[2].minute ),
////                                     ( g_sParameters.boottime[3].minute )
////                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current hours of swtith shut time.
//        //
//        //
//        case SSI_INDEX_SHUTHVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     SHUTH_JAVASCRIPT_VARS,
////                                     ( g_sParameters.shuttime[0].hour ),
////                                     ( g_sParameters.shuttime[1].hour ),
////                                     ( g_sParameters.shuttime[2].hour ),
////                                     ( g_sParameters.shuttime[3].hour )
////                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current hours of swtith shut time.
//        //
//        //
//        case SSI_INDEX_SHUTMVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     SHUTM_JAVASCRIPT_VARS,
////                                     ( g_sParameters.shuttime[0].minute ),
////                                     ( g_sParameters.shuttime[1].minute ),
////                                     ( g_sParameters.shuttime[2].minute ),
////                                     ( g_sParameters.shuttime[3].minute )
////                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing the current regate count.
//        //
//        //
//        case SSI_INDEX_RGVARS:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "%s", JAVASCRIPT_HEADER );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     REGATE_JAVASCRIPT_VARS,
////                                     g_sParameters.rgcnt );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s", JAVASCRIPT_FOOTER );
//            }
//            return( iCount );
//        }
//        //
//        //
//        // regate delay1 first.
//        //
//        case SSI_INDEX_DELAY1:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     "%d",
////                                     g_sParameters.delay1
////                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "' maxlength='5' size='15' name='rgdey1'>" );
//            }
//            return( iCount );
//        }
//        //
//        //
//        // regate delay1 second.
//        //
//        case SSI_INDEX_DELAY2:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     "%d",
////                                     g_sParameters.delay2
////                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "' maxlength='5' size='15' name='rgdey2'>" );
//            }
//            return( iCount );
//        }
//        //
//        //
//        // regate delay3 second.
//        //
//        case SSI_INDEX_DELAY3:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
////                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
////                                     "%d",
////                                     g_sParameters.delay3
////                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "' maxlength='5' size='15' name='rgdey3'>" );
//            }
//            return( iCount );
//        }
//        //
//        // Return the user-editable friendly name for the module.
//        //
//        case SSI_INDEX_MODENAME:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
////                iCount +=
////                    ConfigEncodeFormString( ( char* )g_sParameters.ucModName,
////                                            pcInsert + iCount,
////                                            iInsertLen - iCount );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' maxlength='%d' size='%d' name='modname'>",
//                               ( MOD_NAME_LEN - 1 ), MOD_NAME_LEN );
//            }
//            return( iCount );
//        }
//        //
//        // Return the user-editable model name for the module.
//        //
//        case SSI_INDEX_MODEL:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    ConfigEncodeFormString( ( char* )g_sParameters.Model,
//                                            pcInsert + iCount,
//                                            iInsertLen - iCount );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' maxlength='%d' size='%d' name='model'>",
//                               ( DEVICE_INFO_LEN - 1 ), DEVICE_INFO_LEN );
//            }
//            return( iCount );
//        }
//        //
//        // Return the user-editable friendly manufacture for the module.
//        //
//        case SSI_INDEX_MANUFACT:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    ConfigEncodeFormString( ( char* )g_sParameters.Manufacturer,
//                                            pcInsert + iCount,
//                                            iInsertLen - iCount );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' maxlength='%d' size='%d' name='manu'>",
//                               ( DEVICE_INFO_LEN - 1 ), DEVICE_INFO_LEN );
//            }
//            return( iCount );
//        }
//        //
//        // Generate an HTML text input field containing serial
//        // number.
//        //
//        case SSI_INDEX_SERIAL:
//        {
//            return( usnprintf( pcInsert, iInsertLen,
//                               "<input value='%s' maxlength='12' size='30' "
//                               "name='ser'>", g_sParameters.Serial ) );
//        }
//        //
//        // Return the user-editable friendly install position for the module.
//        //
//        case SSI_INDEX_POSITION:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    ConfigEncodeFormString( ( char* )g_sParameters.InstallPos,
//                                            pcInsert + iCount,
//                                            iInsertLen - iCount );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' maxlength='%d' size='%d' name='pos'>",
//                               ( DEVICE_INFO_LEN - 1 ), DEVICE_INFO_LEN );
//            }
//            return( iCount );
//        }
//        //
//        // Return the user-editable friendly install persion for the module.
//        //
//        case SSI_INDEX_INSTALPERSON:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    ConfigEncodeFormString( ( char* )g_sParameters.InstallPer,
//                                            pcInsert + iCount,
//                                            iInsertLen - iCount );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount +=
//                    usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                               "' maxlength='%d' size='%d' name='insper'>",
//                               ( DEVICE_INFO_LEN - 1 ), DEVICE_INFO_LEN );
//            }
//            return( iCount );
//        }
//        //
//        // The local MAC address tag "macaddr".
//        //
//        case SSI_INDEX_INSTALTIME:
//        {
//            iCount = usnprintf( pcInsert, iInsertLen, "<input value='" );
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "%s",
//                                     g_sParameters.InstallTime
//                                   );
//            }
//            if( iCount < iInsertLen )
//            {
//                iCount += usnprintf( pcInsert + iCount, iInsertLen - iCount,
//                                     "' maxlength='19' size='30' name='instime'>" );
//            }
//            return( iCount );
//        }
//        //
//        // The run time.
//        //
//        case SSI_INDEX_RUNTIME://
//        {
//            unsigned long ucRunDay = g_sParameters.RunSecond / 86400;
//            unsigned long ucRunHour = g_sParameters.RunSecond % 86400 / 3600;
//            unsigned long ucRunMinute = g_sParameters.RunSecond % 86400 % 3600 / 60;
//            unsigned long ucRunSecond = g_sParameters.RunSecond % 86400 % 3600 % 60;
//            if( old_SysRunDay != ucRunDay )
//            {
//                g_sWorkingDefaultParameters = g_sParameters;
//                ConfigSave();
//            }
//            old_SysRunDay = ucRunDay;
//            return( usnprintf( pcInsert, iInsertLen, "%d day %d:%d:%d",
//                               ucRunDay, ucRunHour, ucRunMinute, ucRunSecond ) );
//        }
//        //
//        // The Firmware Version number tag, "revision".
//        //
//        case SSI_INDEX_SOFTVER://
//        {
//            return( usnprintf( pcInsert, iInsertLen, "%s",
//                               "V1.0" ) );
//        }
//        //
//        // The Firmware Version number tag, "revision".
//        //
//        case SSI_INDEX_HARDVER://
//        {
//            return( usnprintf( pcInsert, iInsertLen, "%s",
//                               "T1.0" ) );
//        }
//        case SSI_INDEX_CURTIME:
//        {
//            return( usnprintf( pcInsert, iInsertLen, "%d-%d-%d  %d:%d:%d",
//                               0,0,0,0,0,0) ); //
//        }
        //
        // All other tags are unknown.
        //
        default:
        {
            return( usnprintf( pcInsert, iInsertLen,
                               "<b><i>Tag %d unknown!</i></b>", iIndex ) );
        }
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************