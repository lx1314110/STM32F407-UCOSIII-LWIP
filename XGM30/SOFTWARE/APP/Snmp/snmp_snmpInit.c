#include <includes.h>
#include "arch/cc.h"
#include "Snmp\snmp_SNMP.h"
#include "Snmp\snmp_mib.h"
#include "config.h"
/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD Start -*/
#include "string.h"
/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD End   -*/
//-------------------------------------------------------------
extern   MIB mib_ace;

/* The following MIBs must be in lexicographical order */
static   const MIB* mibs[] =
{
    &mib_ace,
};

/* This structure is defined as external to SNMPAgentTask() */
AGENT_CONTEXT snmp_ac =
{
    mibs, ( sizeof( mibs ) / sizeof( MIB* ) ), 0
};
//------------------------------------------------------------

u8_t  snmp_Context[16];                         //SNMP社区串
u32_t Snmp_Enterprise = 40172;                  //企业标识
u8_t* Snmp_Context = snmp_Context;          //SNMP社区串
//
//! analogProperty table parameters.
//
/* 2018-06-13 changzehai(DTT) ----------------------------------- MOD Start -*/
AnalogPropertyTable analogPropertyTable[SNMP_ANALOGIPTABLE_IP_MAX];

/*---------------------------*/
/* 函数声明                  */
/*---------------------------*/
static u8_t snmp_ipmac_init();
void Snmp_Flash_Read(int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position);
void Snmp_Flash_Write( int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position );
/* 2018-06-13 changzehai(DTT) ----------------------------------- MOD End   -*/


/*********************************************************************/
/* the common groud paramters.*/
/*********************************************************************/
//
//! logical id.
//
u8_t    commonNELogicalID[16];
//
//! the manufature of device.
//
u8_t    commonNEVendor[DEVICE_INFO_LEN];
//
//! the model of device.
//
u8_t    commonNEModelNumber[DEVICE_INFO_LEN];
//
//! the serial of device.
//
u8_t    commonNESerialNumber[16];
//
//! the alarm detection control .
//
s16_t   commonAlarmDetectionControl = 2;
//
//! the network address of the device.
//
u8_t    commonNetworkAddress[4];
//
//! the port of the device.
//
u16_t   commonNetworkPort;
//
//! the physical address.
//
u8_t    commonPhysicalAddress[6];
//
//! the safe status of device.
//
s16_t   commonTamperStatus = 1;
//
//! the temperature of conditon.
//
s16_t   commonInternalTemperature = 26;
//
//! the humidity of the conditon.
//
u16_t   commonInternalHumidity = 0;
//------------------------------------------------------------------------------
u8_t fnSntpServiceAddr[4];
//
////! sntp inter  time.
////
//u16_t fnSntpInter;
//------------------------------------------------------------------------------------------
//s16_t analogPropertyNO = 3;                     //模拟属性表数目
s16_t analogPropertyNO = 64;                      //模拟属性表数目
//s16_t currentAlarmNO = 0;                        //当前报警表数目
//s16_t discretePropertyNO = 7;                   //离散属性表数目
////------------------------------------------------------------------------------------------
//const char* Str_fnChannelName[4] =
//{
//    "CH1",
//    "CH2",
//    "CH3",
//    "CH4"
//};
////
////! extern defintion from config.c.
//extern tConfigParameters g_sParameters;
//--------------------------------


/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD Start -*/


//}
/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD End   -*/

extern u8_t Oid_Len;

void snmpInit( void )
{

//    u8_t* buffzz;
    u16_t i;
    char buf[64][16];
//    u16_t temp;
//    u16_t configTemp;
//    u8_t i_dc = 1;
//    u8_t i_op = 1;
//    u8_t temp_bit = 0;
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR cpu_sr;
#endif

    //
    //!snmp_context.
    //
    memcpy( snmp_Context, &g_sParameters.CommunityStr, 16 );                    //社区串
    //
    //! commonNESerialNumber.
    //
    memcpy( commonNESerialNumber, &g_sParameters.Serial, 12 );              //设备序列号
    //
    //! manufacture.
    //
    memcpy( commonNEVendor, &g_sParameters.Manufacturer, DEVICE_INFO_LEN );             //生产商
    //
    //! model.
    memcpy( commonNEModelNumber, &g_sParameters.Model, DEVICE_INFO_LEN );               //设备型号
    //
    //! logical ID /MODEL.
    //
    memcpy( commonNELogicalID, &g_sParameters.Model, 16 ); //逻辑ID
    //
    //! common network address.
    //
    commonNetworkAddress[0] = ( g_sParameters.ulStaticIP >> 24 ) & 0xff;
    commonNetworkAddress[1] = ( g_sParameters.ulStaticIP >> 16 ) & 0xff;
    commonNetworkAddress[2] = ( g_sParameters.ulStaticIP >> 8 ) & 0xff;
    commonNetworkAddress[3] =   g_sParameters.ulStaticIP & 0xff;
    //
    //! alarm detection control enable
    //
    commonAlarmDetectionControl = 2;
    //
    //  OSTimeDlyHMSM(0,0,1,500);//等待保证采样到一次完整的轮询数据
    //
    commonNetworkPort = g_sParameters.TrapPort;

    //  //初始化模拟属性表、离散属性表--------------------------
    /* 2018-06-13 changzehai(DTT) ----------------------------------- MOD Start -*/


    u8_t ret;
    
    /* 做成初始化IP数据 */
    ret = snmp_ipmac_init();
    if (ret == SNMP_NG)
    {
//        exit(-1);
    }
    
//    for (i = 3; i < 4; i++)
//    { 
//        Snmp_Flash_Read((int8_t *)p, 4096, i, 0);
//        for (j = 0; j < 256; j++)
//        {
//            p = p + 16;
//        }
//        
//    }
    
//    uint16_t j;
//    char buf[16];
//    char *p = &buf[0];
//    for (i = 0; i < 4; i++)
//    {
//        for (j = 0; j < 256; j++)
//        {
//            memset(buf, 0x00, sizeof(p));
//            Snmp_Flash_Read((int8_t *)buf, 16, i, j * 16);
//            p = &buf[0];
//        }
//    
//    }
    

    /* 从flash取出第1列的数据用于初始化 */
    Snmp_Flash_Read((int8_t *)buf, 64*16, 0, 0);
    
    
    
    /* 初始化模拟参数属性表数据 */
    for( i = 0; i < analogPropertyNO; i++ )
    {
 
        /* 索引 */
        analogPropertyTable[i].analogIpIndex = i + 1;
        
        /* 列指示 */
        analogPropertyTable[i].analogIpColIndication = 1;
        
        /* IP信息 */
        strncpy(analogPropertyTable[i].analogIp, buf[i], sizeof(analogPropertyTable[i].analogIp));
          
    }

    

    /* 2018-06-13 changzehai(DTT) ----------------------------------- MOD End   -*/
    //-------------------------------------------------------
    Oid_Len = 3;
    ussSNMPAgentInit();

}


/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD Start -*/
/*****************************************************************************/
/* 函  数:    snmp_ipmac_init                                                */
/* 功  能:    做成IP和MAC初始化数据并保存到flash里                           */
/* 输  入:    无                                                             */
/* 输  出:    无                                                             */
/* 返回值:    无                                                             */
/* 创  建:    2018-06-13 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/*****************************************************************************/
static u8_t snmp_ipmac_init()
{

  u16_t cnt = 0;
  u8_t i = 0;
  u8_t j = 0;
  u8_t wcnt = 0; 
  char ipmac_buf[SNMP_ANALOG_IP_SIZE];
  char *pflash_wbuf;
  char *pflash_wbuf_start; /* 写缓冲区开始地址 */
  
  
  /* 创建4K的写缓冲区 */
  pflash_wbuf = calloc(4096, 1);
  pflash_wbuf_start = pflash_wbuf;
  
  /*---------------------------------*/
  /* 写入1024个IP                    */
  /*---------------------------------*/
  for (i = 1; i < 6; i++)  /* 1024 = 4 * 254 + 8， 所以i最大值为5 */
  {
      for (j = 1; j < 255; j++)  /* 有效IP范围为1~254 */
      {
          
         /* 作成IP地址 */
          memset(ipmac_buf, 0x00, sizeof(ipmac_buf));
          sprintf(ipmac_buf, "172.18.%d.%d", i, j);
          
          /* 写入IP地址 */
          strcpy(pflash_wbuf, ipmac_buf);
          
          /* 每次偏移一个IP数据长度 */
          pflash_wbuf = pflash_wbuf + SNMP_ANALOG_IP_SIZE;
          
          cnt++;  
          
//          /* 已做成所有IP*/
//          if (cnt >= SNMP_ANALOG_IP_MAX)
//          {
//              /* 将flash_wbuf的数据写进flash */
//              Snmp_Flash_Write(flash_wbuf, (uint16_t)(pflash_wbuf - flash_wbuf), wcnt, 0);
//              
//              break;
//          }
          
          /* 4K大小的flash_wbuf的数据已经写满 */
          if (cnt % 256 == 0) /* 256 = 4096 / 16 */
          {
              /* 将flash_wbuf的数据写进flash */
              Snmp_Flash_Write((int8_t *)pflash_wbuf_start, 4096, wcnt, 0);
              
              /* 写4K的计数加1 */
              wcnt++;
              if (wcnt == 4) /* 4 = 1024 / 256 */
              {
                  break;
              }
              
              
              /* pflash_wbuf重新指向写缓冲区开始地址 */
              pflash_wbuf = pflash_wbuf_start;
          }
          

          
      }
  
  }
  
  /* 释放写缓冲区空间 */
  free(pflash_wbuf_start);
  
  return SNMP_OK;
}


/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD End   -*/