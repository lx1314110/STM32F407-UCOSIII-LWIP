#ifndef __SNMP_MIB_H__
#define __SNMP_MIB_H__



#define XsysDescr 0
#define XsysObjectID 1
#define XsysUptime 2
#define XsysContact 3
#define XsysName 4
#define XsysLocation 5
#define XsysServices 6
#define XanalogIpIndex 7
#define XanalogIpColIndication 8
#define XanalogIp 9

#define FLASH_SNMP_START_ADDR ADDR_FLASH_SECTOR_6   //用户起始地址，暂定为第十个扇区的起始地址

/* IP数据最大个数 */
#define SNMP_ANALOG_IP_MAX    1024

/* 每个IP数据长度 */
#define SNMP_ANALOG_IP_SIZE   16

/* 表中存放的IP数据个数 */
#define SNMP_ANALOGIPTABLE_IP_MAX 64


/* 正常结果 */
#define SNMP_OK    1
/* 错误结果 */
#define SNMP_NG   0


/*---------------------------*/
/* 声明MIB表格结构           */
/*---------------------------*/
/* 模拟参数属性表 */
typedef struct
{
    u8_t    analogIpIndex;                  /* 索引(行指示)：1~64 */
    u16_t    analogIpColIndication;         /* 列指示      : 1~16 */
    char    analogIp[SNMP_ANALOG_IP_SIZE];  /* IP信息             */
} AnalogPropertyTable;





#endif
