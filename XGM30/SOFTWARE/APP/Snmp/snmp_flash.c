/*****************************************************************************/
/* 文件名:    snmp_flash.c                                                   */
/* 说  明:    flash读写处理                                                  */
/* 创  建:    2018-06-22 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "flash.h"
#include "string.h"
#include "stm32f4xx_flash.h"
#include "stdlib.h"

/*-----------------------------*/
/* 函数声明                    */
/*-----------------------------*/
static uint16_t Flash_GetSector( uint32_t Address );
static void Snmp_Flash_WriteSecondarySector( uint32_t primary_address, uint32_t secondary_address, int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position );
static void Snmp_Flash_Write_Byte( uint32_t address, uint16_t length, int8_t* data );
void Snmp_Flash_Read_Byte( uint32_t address, uint16_t length, int8_t* data );
void Snmp_Flash_Read(int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position);
void Snmp_Flash_Write( int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position );
uint32_t Snmp_Flash_GetDataAddr(void);
/****************************************************************************
* 函    数: Flash_GetSector
* 功    能: 获取地址Address对应的sector编号
* 入口参数：地址
* 出口参数：sector编号
* 返 回 值: 无                                                             
* 创    建: 2018-06-20 changzehai(DTT)                                     
* 更    新: 无                                                             
****************************************************************************/
static uint16_t Flash_GetSector( uint32_t Address )
{
    uint16_t sector = 0;
    if( ( Address < ADDR_FLASH_SECTOR_1 ) && ( Address >= ADDR_FLASH_SECTOR_0 ) )
    {
        sector = FLASH_Sector_0;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_2 ) && ( Address >= ADDR_FLASH_SECTOR_1 ) )
    {
        sector = FLASH_Sector_1;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_3 ) && ( Address >= ADDR_FLASH_SECTOR_2 ) )
    {
        sector = FLASH_Sector_2;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_4 ) && ( Address >= ADDR_FLASH_SECTOR_3 ) )
    {
        sector = FLASH_Sector_3;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_5 ) && ( Address >= ADDR_FLASH_SECTOR_4 ) )
    {
        sector = FLASH_Sector_4;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_6 ) && ( Address >= ADDR_FLASH_SECTOR_5 ) )
    {
        sector = FLASH_Sector_5;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_7 ) && ( Address >= ADDR_FLASH_SECTOR_6 ) )
    {
        sector = FLASH_Sector_6;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_8 ) && ( Address >= ADDR_FLASH_SECTOR_7 ) )
    {
        sector = FLASH_Sector_7;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_9 ) && ( Address >= ADDR_FLASH_SECTOR_8 ) )
    {
        sector = FLASH_Sector_8;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_10 ) && ( Address >= ADDR_FLASH_SECTOR_9 ) )
    {
        sector = FLASH_Sector_9;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_11 ) && ( Address >= ADDR_FLASH_SECTOR_10 ) )
    {
        sector = FLASH_Sector_10;
    }
    else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
    {
        sector = FLASH_Sector_11;
    }
    return sector;
}

/****************************************************************************
* 函    数: Flash_Get_Sector_Start_Addr
* 功    能: 获取指定扇区开始地址
* 入口参数：SectorNum 扇区号
* 出口参数：无
* 返 回 值: 无                                                             
* 创    建: 2018-06-20 changzehai(DTT)                                     
* 更    新: 无  
****************************************************************************/
//uint32_t Flash_Get_Sector_Start_Addr(uint16_t sector)
//{
//    uint32_t addr = 0;
//
//
//    switch(sector)
//    {
//    case FLASH_Sector_0:
//         addr = ADDR_FLASH_SECTOR_0;
//        break;
//    case FLASH_Sector_1:
//         addr = ADDR_FLASH_SECTOR_1;
//        break;
//    case FLASH_Sector_2:
//         addr = ADDR_FLASH_SECTOR_2;
//        break;
//    case FLASH_Sector_3:
//         addr = ADDR_FLASH_SECTOR_3;
//        break;
//    case FLASH_Sector_4:
//         addr = ADDR_FLASH_SECTOR_4;
//        break;
//    case FLASH_Sector_5:
//         addr = ADDR_FLASH_SECTOR_5;
//        break;
//    case FLASH_Sector_6:
//         addr = ADDR_FLASH_SECTOR_6;
//        break;
//    case FLASH_Sector_7:
//         addr = ADDR_FLASH_SECTOR_7;
//        break;
//    case FLASH_Sector_8:
//         addr = ADDR_FLASH_SECTOR_8;
//        break;
//    case FLASH_Sector_9:
//         addr = ADDR_FLASH_SECTOR_9;
//        break;
//    case FLASH_Sector_10:
//         addr = ADDR_FLASH_SECTOR_10;
//        break;
//    case FLASH_Sector_11:
//         addr = ADDR_FLASH_SECTOR_11;
//        break;
//    default:
//        break;
//    }
//
//    return addr;
//
//}

/****************************************************************************
* 函    数: Snmp_Flash_EraseSector
* 功    能: 擦除指定扇区
* 入口参数：SectorNum 扇区号
* 出口参数：无
* 返 回 值: 无                                                             
* 创    建: 2018-06-20 changzehai(DTT)                                     
* 更    新: 无  
****************************************************************************/
//static void Snmp_Flash_EraseSector( uint16_t SectorNum )
//{
//    FLASH_Unlock();
//    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
//                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
//    if( FLASH_EraseSector( SectorNum, VoltageRange_3 ) != FLASH_COMPLETE ) while( 1 );
//    FLASH_Lock();
//}


/****************************************************************************
* 函    数: Snmp_Flash_Read
* 功    能: 读出length字节数据
* 入口参数：data          ：指向读出的数据
*           length        ：数据长度
*           position      ：要读出的数据在第几个4K
*           byte_position : 要的数据位于所在4K的第几byte
* 出口参数：无
* 返 回 值: 无                                                             
* 创    建: 2018-06-20 changzehai(DTT)                                     
* 更    新: 无  
****************************************************************************/
void Snmp_Flash_Read(int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position)
{
    
    uint32_t address;
    uint16_t Flash_Sector_Flag1 = 0; /* 主次Sector标志1 */
    uint16_t Flash_Sector_Flag2 = 0; /* 主次Sector标志2 */    
    
    
    
    /* 获取主次Sector标志  */
    Flash_Sector_Flag1 = *( uint16_t* )ADDR_FLASH_SECTOR_6;
    Flash_Sector_Flag2 = *( uint16_t* )ADDR_FLASH_SECTOR_7;
    
    /* 判断主sector */
    if (Flash_Sector_Flag1 == 0x8000 && Flash_Sector_Flag2 == 0xFFFF)
    { 
         address = ADDR_FLASH_SECTOR_6; /* SECTOR_6为sector */
    } 
    else if (Flash_Sector_Flag1 == 0xFFFF && Flash_Sector_Flag2 == 0x8000)
    {
         
         address = ADDR_FLASH_SECTOR_7; /* SECTOR_7为sector */
    } 
    else
    {
        while(1);
    
    }
        
    /*  地址增加2K，前面2K用于存放主次Sector标志 */
    address = address + 2048;
    
    /* 计算读开始地址 */
    address = address + position * 4096 + byte_position;

    /* 读取数据 */
    Snmp_Flash_Read_Byte( address, length,  data);

        
}

/****************************************************************************
* 函    数: Snmp_Flash_Write
* 功    能: 写入length字节数据
* 入口参数：data          ：要写入的IP数据指针
*           length        ：数据长度
*           position      ：要写入的数据在第几个4K
*           byte_position : 写入的数据位于所在4K的第几byte
* 出口参数：无
* 返 回 值: 无                                                             
* 创    建: 2018-06-20 changzehai(DTT)                                     
* 更    新: 无  
****************************************************************************/
void Snmp_Flash_Write( int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position )
{
    uint32_t address;
    uint16_t Flash_Sector_Flag1 = 0; /* 主次Sector标志1 */
    uint16_t Flash_Sector_Flag2 = 0; /* 主次Sector标志2 */
//    uint16_t i;

    
    
    FLASH_Unlock(); //解锁FLASH后才能向FLASH中写数据。
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    
    /* 获取主次Sector标志  */
    Flash_Sector_Flag1 = *( uint16_t* )ADDR_FLASH_SECTOR_6;
    Flash_Sector_Flag2 = *( uint16_t* )ADDR_FLASH_SECTOR_7;
 

    if( Flash_Sector_Flag1 == 0x8000 && Flash_Sector_Flag2 == 0xFFFF ) /* SECTOR_6为主Sector，SECTOR_7为次Sector */
    {
        /* 将数据写入到SECTOR_7，并将SECTOR_7设置为主Sector，SECTOR_6设置为次Sector */
        Snmp_Flash_WriteSecondarySector( ADDR_FLASH_SECTOR_6, ADDR_FLASH_SECTOR_7,
                                           data, length, position, byte_position );
    }
    else if( Flash_Sector_Flag1 == 0xFFFF && Flash_Sector_Flag2 == 0x8000 ) /* SECTOR_7为主Sector，SECTOR_6为次Sector */
    {
        /* 将数据写入到SECTOR_6，并将SECTOR_6设置为主Sector，SECTOR_7设置为次Sector */
        Snmp_Flash_WriteSecondarySector( ADDR_FLASH_SECTOR_7, ADDR_FLASH_SECTOR_6,
                                           data, length, position, byte_position );
    }
    else /* 第一次写flash或者不正确的时候复位操作 */
    {
        /* 擦除SECTOR_6 */
        FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3);
        
        /* 擦除SECTOR_7 */
        FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3);        
        

       
        /*  地址增加2K，前面2K用于存放主次Sector标志 */
        address = ADDR_FLASH_SECTOR_6 + 2048;

        /* 写入数据 */
        Snmp_Flash_Write_Byte(address, length, data);
//        for( i = 0; i < length; i++ )
//        {
//            if( FLASH_ProgramByte( address, data[i] ) == FLASH_COMPLETE )
//            {
//                address++;
//            }
//            else
//            {
//                
//                while( 1 );
//            }
//        }
        
        /* 设置主Sector标志 */
        if (FLASH_ProgramHalfWord( ADDR_FLASH_SECTOR_6, 0x8000) != FLASH_COMPLETE)
        {
            while(1);
        }             
    }
    
    /* 加锁 */
    FLASH_Lock();
}


/****************************************************************************
* 函    数: Snmp_Flash_Write_Secondary_Sector
* 功    能: 将数据写入原次Sector，并把原次Sector改为主Sector，原主Sector改为次Sector
* 入口参数：primary_address   ：原主分区开始地址
*           secondary_address ：原次分区开始地址
*           data              ：要写入的数据指针
*           length            ：数据长度
*           position      ：要写入的数据在第几个4K
*           byte_position : 写入的数据位于所在4K的第几byte
* 出口参数：无
* 返 回 值: 无                                                             
* 创    建: 2018-06-20 changzehai(DTT)                                     
* 更    新: 无  
****************************************************************************/
static void Snmp_Flash_WriteSecondarySector
(
    uint32_t     primary_address,
    uint32_t     secondary_address,
    int8_t*      data,
    uint16_t     length,
    uint8_t      position,
    uint16_t     byte_position

)
{
    uint8_t      i;
    uint16_t     sector;
    int8_t*      pBuf;
    uint32_t     pri_address;
    uint32_t     sec_address;
    
    pri_address = primary_address;
    sec_address = secondary_address;
    
    /* 创建读写缓冲区 */
    pBuf = calloc(4096, 1);
    if (pBuf == NULL)
    {
        while(1);
    }
    
       
    /*  地址增加2K，前面2K用于存放主次Sector标志 */
    primary_address   += 2048;
    secondary_address += 2048;
    
    /* 将原主Sector所写位置前面的数据写到次Sector */
    for( i = 0; i < position; i++ )
    {
        /* 读写缓冲区清零 */
        memset(pBuf, 0x00, 4096);
        
        /* 从原主Sector读出4K数据 */
        Snmp_Flash_Read_Byte( primary_address, 4096, pBuf );
        
        /* 将读出的4K数据写入到次Sector */
        Snmp_Flash_Write_Byte( secondary_address, 4096, pBuf );
        
        /* 地址增加4K */
        primary_address   += 4096;
        secondary_address += 4096;
    }
    
    /* 读写缓冲区清零 */
    memset(pBuf, 0x00, 4096);
    /* 读出要修改的IP所处4K的数据 */
    Snmp_Flash_Read_Byte( primary_address, 4096, pBuf );

    
    /* 修改对应IP */
    memcpy( &pBuf[byte_position], data, length );
//    char *ip = &pBuf[0];
//    uint16_t j;
//    for (j = 0; j < 256; j++)
//    {
//        ip = ip + 16;
//    }
    
    /* 接着将该4K数据写到次Sector中 */
    Snmp_Flash_Write_Byte( secondary_address, 4096, pBuf );
    

    /* 获取原主Sector编号 */
    sector = Flash_GetSector( pri_address );
    
    /* 擦除原主Sector */
    FLASH_EraseSector(sector, VoltageRange_3);
    
    
    /* 更换主Sector标志，将次Sector改为主Sector */
    if (FLASH_ProgramHalfWord( sec_address, 0x8000) != FLASH_COMPLETE)
    {
        while(1);
    }
        
    /* 释放读写缓冲区 */
    free(pBuf);
    
}

/****************************************************************************
* 函    数: Snmp_Flash_Write_Byte
* 功    能: 写入length字节的数据
* 入口参数：address：地址
*           length： 数据长度
*           data_8  指向写入的数据
* 出口参数：无
* 返 回 值: 无                                                             
* 创    建: 2018-06-20 changzehai(DTT)                                     
* 更    新: 无  
****************************************************************************/
static void Snmp_Flash_Write_Byte( uint32_t address, uint16_t length, int8_t* data )
{
    uint16_t i;

        
    for( i = 0; i < length; i++ )
    {
        
        if( FLASH_ProgramByte( address, data[i] ) == FLASH_COMPLETE )
        {
            address++;
        }
        else
        {
            while( 1 );
        }
    }
}

/****************************************************************************
* 函    数: Snmp_Flash_Read_Byte
* 功    能: 以字节为单位读取length字节的数据
* 入口参数：address：地址
*           length： 数据长度
*           data_8  指向读出的数据
* 出口参数：无
* 返 回 值: 无                                                             
* 创    建: 2018-06-20 changzehai(DTT)                                     
* 更    新: 无  
****************************************************************************/
void Snmp_Flash_Read_Byte( uint32_t address, uint16_t length, int8_t* data )
{
    uint16_t i;
    
    
    for( i = 0; i < length; i++ )
    {
        data[i] = *( __IO int8_t* )address;
        address++;
    }
}


///****************************************************************************
//* 函    数: Snmp_Flash_GetDataAddr
//* 功    能: 获取Flash数据保存位置
//* 入口参数：无
//* 出口参数：无
//* 返 回 值: 主Sector的首地址                                                             
//* 创    建: 2018-06-20 changzehai(DTT)                                     
//* 更    新: 无  
//****************************************************************************/
//uint32_t Snmp_Flash_GetDataAddr(void)
//{
//
//    uint16_t Flash_Sector_Flag1 = 0; /* 主次Sector标志1 */
//    uint16_t Flash_Sector_Flag2 = 0; /* 主次Sector标志2 */
//    
//    
//    /* 获取主次Sector标志  */
//    Flash_Sector_Flag1 = *( uint16_t* )ADDR_FLASH_SECTOR_6;
//    Flash_Sector_Flag2 = *( uint16_t* )ADDR_FLASH_SECTOR_7;
//    
//    if ( Flash_Sector_Flag1 == 0x8000 && Flash_Sector_Flag2 == 0xFFFF ) /* SECTOR_6为主Sector，SECTOR_7为次Sector */
//    {
//        return ADDR_FLASH_SECTOR_6 + 2048;
//    }
//    else if ( Flash_Sector_Flag1 == 0xFFFF && Flash_Sector_Flag2 == 0x8000 ) /* SECTOR_6为次Sector，SECTOR_7为主Sector */
//    {
//        return ADDR_FLASH_SECTOR_7 + 2048;
//    }
//    else
//    {
//        return 0;
//    }
//
//}