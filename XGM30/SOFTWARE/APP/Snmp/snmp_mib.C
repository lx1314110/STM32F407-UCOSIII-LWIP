#include <includes.h>
#include "arch/cc.h"
#include "Snmp\snmp_SNMP.h"
#include "Snmp\snmp_mib.h"
#include "Config\config.h"

#include "lwip/api.h"
#include "lwip/inet.h"

//-----------------------------------------------------------
unsigned long SysTime = 0;
u8_t sys_name[16] = {"MTFS_30"};
u8_t sys_contact[16] = {"MTFS_30"};
u8_t sys_location[16] = {"MTFS_30"};
u8_t sys_descr[16] = {"MTFS_30"};
u8_t sysObjectID[16] = {0x2b, 6, 1, 4, 1, 0x82, 0xb9, 0x6c, 1};
//--------------------------------------------------
extern AnalogPropertyTable analogPropertyTable[];
//--------------------------------------------------
extern s16_t analogPropertyNO;

/*----------------------*/
/* 函数声明             */
/*----------------------*/
extern void Snmp_Flash_Read(int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position);
extern void Snmp_Flash_Write( int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position );
//---------------------
static MIBVAR mibvar_ace[] =
{
    {{8, {0x2b, 6, 1, 2, 1, 1, 1, 0}}, 0, String, 16, sys_descr}, //1.3-->1*40+3==0x2b
    {{8, {0x2b, 6, 1, 2, 1, 1, 2, 0}}, 0, Identifier, 9, sysObjectID},
    {{8, {0x2b, 6, 1, 2, 1, 1, 3, 0}}, 0, Ticks, 4, &SysTime},
    {{8, {0x2b, 6, 1, 2, 1, 1, 4, 0}}, W, String, 16, sys_contact},
    {{8, {0x2b, 6, 1, 2, 1, 1, 5, 0}}, W, String, 16, sys_name},
    {{8, {0x2b, 6, 1, 2, 1, 1, 6, 0}}, W, String, 16, sys_location},
    {{8, {0x2b, 6, 1, 2, 1, 1, 7, 0}}, IMMED, Integer, 0x4f, 0},

/* 2018-06-13 changzehai(DTT) ----------------------------------- MOD Start -*/
    /* 64个IP结点 */
    {{13, {0x2b, 6, 1, 4, 1, 0x82, 0xb9, 0x6c, 1, 1, 1, 1, 1}}, 0, Integer, 2, &analogPropertyTable[0].analogIpIndex},  
    {{13, {0x2b, 6, 1, 4, 1, 0x82, 0xb9, 0x6c, 1, 1, 1, 1, 2}}, W+CAW, Integer, 2, &analogPropertyTable[0].analogIpColIndication},
//    {{13, {0x2b, 6, 1, 4, 1, 0x82, 0xb9, 0x6c, 1, 1, 1, 1, 3}},CAR+W+CAW, String, 17, &analogPropertyTable[0].analogIp},
    {{13, {0x2b, 6, 1, 4, 1, 0x82, 0xb9, 0x6c, 1, 1, 1, 1, 3}},W+CAW, String, 17, &analogPropertyTable[0].analogIp},

/* 2018-06-13 changzehai(DTT) ----------------------------------- MOD End   -*/

};

static s16_t mibvarsize_ace( void )
{
    return sizeof( mibvar_ace ) / sizeof( MIBVAR );
}

static MIBTAB mibtab_ace[] =
{
        {{11,{0x2b,6,1,4,1,0x82,0xb9,0x6c,1,1,1}}, 1,{XanalogIpIndex}, sizeof(AnalogPropertyTable),&analogPropertyNO},

};

static s16_t mibtabsize_ace( void )
{
    return sizeof( mibtab_ace ) / sizeof( MIBTAB );
}

static s16_t mibset_ace( s16_t varix, s16_t tabix ) //可写的
{
   // static u8_t  s_colIndication = 1; /* 列指示(前次)         */
//    u8_t         colIndication;       /* 列指示(本次)         */
//    u8_t         i;
//    u8_t         postion;             /* 所在的是第几个4K     */
//    u16_t        byte_postion;        /* 从该4K的第几个byte开始读 */
//    char         rbuf[64][16];        /* 读缓冲区             */
    
    
    switch(varix)
    {  
        
    case XanalogIpColIndication:    /* 列指示 */
  
        /* 获取列指示 */
//        colIndication = analogPropertyTable[tabix].analogIpColIndication;
//        /* 本次与前次不一样 */
////        if (colIndication != s_colIndication)
////        { 
//           
//            
//            /* 计算该列数据处在第几个4K */
//            postion = (colIndication - 1) / 4; /* 1列为1K, 每4列为一个4k */
//            
//            /* 该列数据从所在4k中的第几byte开始 */
//            byte_postion = ((colIndication - 1) % 4) * 64 * 16;
//         
//             /* 从flash中读出该列的64个IP信息 */
//            Snmp_Flash_Read((int8_t *)rbuf, 64*16, postion, byte_postion);
//            
//            /* 更新表数据 */
//            for (i = 0; i < analogPropertyNO; i++)
//            {
//                 /* 更新列指示 */
//                 analogPropertyTable[i].analogIpColIndication = colIndication;
//                 
//                 /* 更新IP信息 */
//                 strncpy(analogPropertyTable[i].analogIp, (char const *)rbuf[i], 16);
//            }
                
//            /* 保存列指示 */
//            s_colIndication = colIndication;
//        }
         
        break;
        
    case XanalogIp:    /* IP */         
     
         /* 获取该IP所处的列 */
//         colIndication = analogPropertyTable[tabix].analogIpColIndication;
//         int b = colIndication;
//         int c = b + 1;
//         
//         char buff[16];
//         strncpy(buff, analogPropertyTable[tabix].analogIp, 16);
//         printf(buff);
//         
//         /* 计算该IP所在列数据处在第几个4K */
//         postion = (colIndication - 1) / 4; /* 1列为1K, 每4列为一个4k */
//         
//         int d = postion;
//         int e = d + 1;
//         
//         /* 该IP所在列数据从所在4k中的第几byte开始 */
//         byte_postion = ((colIndication - 1) % 4) * 64 * 16;
//         
//         /* 该IP数据从所在4k中的第几byte开始 */
//         byte_postion = byte_postion + (tabix % 64) * 16; 
//         
//         int f = byte_postion;
//         int g = f + 1;
//         
//         /* 将新的IP数据写入flash */
//         Snmp_Flash_Write((int8_t *)analogPropertyTable[tabix].analogIp, 16, postion, byte_postion);
      
       
        break;
        

        
    default:
        break;
    
    }
    
    
    return 0;
}



static void mibget_ace( s16_t varix, s16_t tabix, u8_t** vvptr )        //读
{ 
        
    switch(varix)
    {   
    case XanalogIpIndex: /* 索引 */
         *vvptr = (u8_t*)&analogPropertyTable[tabix].analogIpIndex;
        
        break;
        
    case XanalogIpColIndication:    /* 列指示 */
        *vvptr = (u8_t*)&analogPropertyTable[tabix].analogIpColIndication;
        break;    
        
    case XanalogIp:    /* IP */              
         *vvptr = (u8_t*)analogPropertyTable[tabix].analogIp;
        
        break;
        

        
    default:
        break;
    
    }
      
 
}

static s16_t mibindex_ace( s16_t varix, s16_t tabix )
{
          //属性表
          if (varix >= XanalogIpIndex  && varix <= XanalogIp )
          {
              if (tabix >= analogPropertyNO) return -1;
              return 1;
          }
   
    
    return -1;
}

//extern void UpdateSysConfigBuf(uint8 *temp, uint8 len, uint16 pos);

MIB mib_ace =
{
    mibvar_ace,
    mibvarsize_ace,
    mibtab_ace,
    mibtabsize_ace,
    mibget_ace,               // mibget
    mibset_ace,           // mibset
    mibindex_ace,
    0,                        // mibinit
};

