#include "flash.h"

/****************************************************************************
* 功    能: 获取地址Address对应的sector编号
* 入口参数：地址
* 出口参数：sector编号
* 说    明：无
* 调用方法：无
****************************************************************************/
uint16_t Flash_GetSector( uint32_t Address )
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
* 功    能: 获取指定扇区开始地址
* 入口参数：SectorNum 扇区号
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
uint32_t Flash_Get_Sector_Start_Addr(uint16_t sector)
{
    uint32_t addr = 0;
    
 
    switch(sector)
    {
    case FLASH_Sector_0:  
         addr = ADDR_FLASH_SECTOR_0;
        break;
    case FLASH_Sector_1:
         addr = ADDR_FLASH_SECTOR_1;
        break;
    case FLASH_Sector_2:
         addr = ADDR_FLASH_SECTOR_2;
        break;
    case FLASH_Sector_3:
         addr = ADDR_FLASH_SECTOR_3;
        break;
    case FLASH_Sector_4:
         addr = ADDR_FLASH_SECTOR_4;
        break;
    case FLASH_Sector_5:
         addr = ADDR_FLASH_SECTOR_5;
        break;
    case FLASH_Sector_6:
         addr = ADDR_FLASH_SECTOR_6;
        break;
    case FLASH_Sector_7:
         addr = ADDR_FLASH_SECTOR_7;
        break;
    case FLASH_Sector_8:
         addr = ADDR_FLASH_SECTOR_8;
        break;
    case FLASH_Sector_9:
         addr = ADDR_FLASH_SECTOR_9;
        break;
    case FLASH_Sector_10:
         addr = ADDR_FLASH_SECTOR_10;
        break;
    case FLASH_Sector_11:
         addr = ADDR_FLASH_SECTOR_11;
        break;
    default:
        break;
    }
    
    return addr;

}

/****************************************************************************
* 功    能: 擦除指定扇区
* 入口参数：SectorNum 扇区号
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
void Flash_EraseSector( uint16_t SectorNum )
{
    FLASH_Unlock();
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    if( FLASH_EraseSector( SectorNum, VoltageRange_3 ) != FLASH_COMPLETE ) while( 1 );
    FLASH_Lock();
}

/****************************************************************************
* 功    能: 写入长度为length的32位数据
* 入口参数：address：地址
            length： 数据长度
            data_32：要写入的数据指针
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
void Flash_Write32BitDatas( uint32_t address, uint16_t length, int32_t* data_32 )
{
    uint16_t StartSector, EndSector, i;
    FLASH_Unlock(); //解锁FLASH后才能向FLASH中写数据。
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    StartSector = Flash_GetSector( address ); //获取FLASH的Sector编号
    EndSector = Flash_GetSector( address + 4 * length );
    for( i = StartSector; i < EndSector; i += 8 ) //每次FLASH编号增加8，可参考上边FLASH Sector的定义。
    {
        if( FLASH_EraseSector( i, VoltageRange_3 ) != FLASH_COMPLETE ) while( 1 );
    }
    for( i = 0; i < length; i++ )
    {
        if( FLASH_ProgramWord( address, data_32[i] ) == FLASH_COMPLETE ) //将DATA_32写入相应的地址。
        {
            address = address + 4;
        }
        else
        {
            while( 1 );
        }
    }
    FLASH_Lock();  //读FLASH不需要FLASH处于解锁状态。
}

/****************************************************************************
* 功    能: 读取长度为length的32位数据
* 入口参数：address：地址
            length： 数据长度
            data_32  指向读出的数据
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
void Flash_Read32BitDatas( uint32_t address, uint16_t length, int32_t* data_32 )
{
    uint8_t i;
    for( i = 0; i < length; i++ )
    {
        data_32[i] = *( __IO int32_t* )address;
        address = address + 4;
    }
}

/****************************************************************************
* 功    能: 写入长度为length的16位数据
* 入口参数：address：地址
            length： 数据长度
            data_16：要写入的数据指针
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
void Flash_Write16BitDatas( uint32_t address, uint16_t length, int16_t* data_16 )
{
    uint16_t StartSector, EndSector, i;
    FLASH_Unlock(); //解锁FLASH后才能向FLASH中写数据。
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    StartSector = Flash_GetSector( address ); //获取FLASH的Sector编号
    EndSector = Flash_GetSector( address + 2 * length );
    for( i = StartSector; i < EndSector; i += 8 ) //每次FLASH编号增加8，可参考上边FLASH Sector的定义。
    {
        if( FLASH_EraseSector( i, VoltageRange_3 ) != FLASH_COMPLETE ) while( 1 );
    }
    for( i = 0; i < length; i++ )
    {
        if( FLASH_ProgramHalfWord( address, data_16[i] ) == FLASH_COMPLETE )
        {
            address = address + 2;
        }
        else
        {            while( 1 );
        }
    }
    FLASH_Lock();  //读FLASH不需要FLASH处于解锁状态。
}

/****************************************************************************
* 功    能: 读取长度为length的16位数据
* 入口参数：address：地址
            length： 数据长度
            data_16  指向读出的数据
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
void Flash_Read16BitDatas( uint32_t address, uint16_t length, int16_t* data_16 )
{
    uint8_t i;
    for( i = 0; i < length; i++ )
    {
        data_16[i] = *( __IO int16_t* )address;
        address = address + 2;
    }
}

/****************************************************************************
* 功    能: 写入length字节数据
* 入口参数：address：地址
*           length： 数据长度
*           data_8：要写入的数据指针
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
uint8_t Snmp_Flash_Write_Byte(int8_t* data_8, uint16_t length)
{
    uint16_t StartSector, EndSector, i;
    uint32_t Current;
    uint32_t Flash_Primary_Addr;
    uint32_t Flash_Secondary_Addr;
    uint8_t  Flash_Use_Flag1;
    uint8_t  Flash_Use_Flag2;
    AnalogPropertyTable *data;
    uint8_t  index;
    uint8_t  ipIndex;
    uint8_t  ipLength;
    uint8_t i;
    char ip[256][16];
    uint32_t address;
    
    uint16_t sector;
    
    
    FLASH_Unlock(); //解锁FLASH后才能向FLASH中写数据。
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    
    
    /* 使用标识  */
    Flash_Use_Flag1 = *(uint8_t *)ADDR_FLASH_SECTOR_6;
    Flash_Use_Flag2 = *(uint8_t *)ADDR_FLASH_SECTOR_7;
    
    if (Flash_Use_Flag1 != 0x80 && Flash_Use_Flag1 != 0x01)
    {
        /* 获取FLASH的Sector编号 */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_6 );
        /* 擦除该Sector编号 */
        FLASH_EraseSector(sector, VoltageRange_3); 
        
        /* 根据IP指示计算*/
        data =  (AnalogPropertyTable *)data_8;
        index = data-> analogIpIndication / 256; /* 第几个4K */
        ipIndex = data-> analogIpIndication % 256;     /* 该4K当中的第几个IP */
        
        address = ADDR_FLASH_SECTOR_6;
        
        ipLength = length /sizeof(AnalogPropertyTable) * 16;
               
        for (i = 0; i < ipLength; i++)
        {
            if ( FLASH_ProgramByte( address, data->ip)== FLASH_COMPLETE)
            {
               address++;  
            }
            else
            {
                while(1);
            }
        }
      
        *(uint8_t *)ADDR_FLASH_SECTOR_6 = 0x80;
        *(uint8_t *)ADDR_FLASH_SECTOR_7 = 0x01;
         
        
    } 
    else if(Flash_Use_Flag1 == 0x80 && Flash_Use_Flag2 == 0x01)
    {
        
        /* 获取FLASH的Sector编号 */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_7 );
        /* 擦除该Sector编号 */
        FLASH_EraseSector(sector, VoltageRange_3);         
        
        /* 根据IP指示计算*/
        data =  (AnalogPropertyTable *)data_8;
        index = data-> analogIpIndication / 256; /* 第几个4K */
        ipIndex = data-> analogIpIndication % 256;     /* 该4K当中的第几个IP */

        address = ADDR_FLASH_SECTOR_7;
         
        /* 将前面的数据保存到次Sector */
        for (i = 0; i < index; i++)
        {
            Flash_Read_Byte(address, 4096, ip);
            for (i = 0; i < 4096; i++)
            {
                if ( FLASH_ProgramByte( address, ip)== FLASH_COMPLETE)
                {
                   address++;  
                }
                else
                {
                    while(1);
                }
            }
            
        }
        
        
        /* 修改IP */
        Flash_Read_Byte(address, 4096, ip);
        for(i = 0; i < length; i++)
        
        
        
        
        
        
        
    
    }
    else if(Flash_Use_Flag1 == 0 && Flash_Use_Flag2 == 1)
    {
        
    
    }
    else
    {
    
    }
    
    

    FLASH_Lock();  //读FLASH不需要FLASH处于解锁状态。
    
    return SNMP_OK;
}


/****************************************************************************
* 功    能: 写入length字节数据
* 入口参数：data：要写入的IP数据指针
*           length： 数据长度
*           index： 要写入的第一个IP在第几个4K
*           Indication: 写入的第一个IP为所在4K共256个IP中的第几个IP
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
uint8_t Snmp_Flash_Write(int8_t* data_8, uint16_t length, uint8_t index, uint8_t Indication)
{

    char ip[256][16];
    uint32_t address;
    
    uint16_t sector;
    
    
    FLASH_Unlock(); //解锁FLASH后才能向FLASH中写数据。
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    
    
    /* 使用标识  */
    Flash_Use_Flag1 = *(uint8_t *)ADDR_FLASH_SECTOR_6;
    Flash_Use_Flag2 = *(uint8_t *)ADDR_FLASH_SECTOR_7;
    
    if (Flash_Use_Flag1 != 0x80 && Flash_Use_Flag1 != 0x01) /* 系统第一次写flash */
    {
        /* 获取FLASH的Sector编号 */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_6 );
        /* 擦除该Sector编号 */
        FLASH_EraseSector(sector, VoltageRange_3); 
    
        address = ADDR_FLASH_SECTOR_6;
        
        for (i = 0; i < ipLength; i++)
        {
            if ( FLASH_ProgramByte( address, data_8)== FLASH_COMPLETE)
            {
               address++;  
            }
            else
            {
                while(1);
            }
        }

        /* 设置主次Sector标志 */        
        *(uint8_t *)ADDR_FLASH_SECTOR_6 = 0x80; /* 主Sector */
        *(uint8_t *)ADDR_FLASH_SECTOR_7 = 0x01; /* 次Sector */      
        
    }
    else if (Flash_Use_Flag1 == 0x80 && Flash_Use_Flag2 == 0x01) 
    {
    
        /* 获取FLASH的Sector编号 */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_7 );
        /* 擦除该Sector编号 */
        FLASH_EraseSector(sector, VoltageRange_3);     
        
        address = ADDR_FLASH_SECTOR_7;
         
        /* 将前面的数据保存到次Sector */
        for (i = 0; i < index; i++)
        {
            Flash_Read_Byte(address, 4096, ip);
            Flash_Write_Byte(address, 4096, ip);
            address += 4096；
            
        }
        
                
        /* 读出要修改的IP所处4K的数据 */
        Flash_Read_Byte(ADDR_FLASH_SECTOR_7 + index * 4096, 4096, ip);
        /* 修改对应IP */
        memcpy(&ip[Indication][0], data_8, length);
        
        /* 将该4K数据接着写到次Sector中 */
        Flash_Write_Byte(address, 4096, ip);

        /* 更改主次Sector标志 */ 
        *(uint8_t *)ADDR_FLASH_SECTOR_6 = 0x01; /* 次Sector */
        *(uint8_t *)ADDR_FLASH_SECTOR_7 = 0x80; /* 主Sector */  
        
    
    }
    else if(Flash_Use_Flag1 == 0x01 && Flash_Use_Flag2 == 0x80)
    {
        /* 获取FLASH的Sector编号 */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_6 );
        /* 擦除该Sector编号 */
        FLASH_EraseSector(sector, VoltageRange_3);
        
        address = ADDR_FLASH_SECTOR_6;
        
        /* 将前面的数据保存到次Sector */
        for (i = 0; i < index; i++)
        {
            Flash_Read_Byte(address, 4096, ip);
            Flash_Write_Byte(address, 4096, ip);
            address += 4096；
        }  
        
        /* 读出要修改的IP所处4K的数据 */
        Flash_Read_Byte(ADDR_FLASH_SECTOR_7 + index * 4096, 4096, ip);
        /* 修改对应IP */
        memcpy(&ip[Indication][0], data_8, length);        
        /* 将该4K数据接着写到次Sector中 */
        Flash_Write_Byte(address, 4096, ip);


        /* 更改主次Sector标志 */ 
        *(uint8_t *)ADDR_FLASH_SECTOR_6 = 0x80; /* 次Sector */
        *(uint8_t *)ADDR_FLASH_SECTOR_7 = 0x01; /* 主Sector */          
         
    } 
    else
    {
       while(1);
    }




}


/****************************************************************************
* 函    数: Snmp_Flash_Write_Secondary_Sector
* 功    能: 将数据写入原次Sector，并把原次Sector改为主Sector，原主Sector改为次Sector
* 入口参数：primary_address   ：原主分区开始地址
*           secondary_address ：原次分区开始地址
*           data              ：要写入的IP数据指针
*           length            ：数据长度 
*           index             ： 要写入的第一个IP在第几个4K
*           Indication        : 写入的第一个IP为所在4K共256个IP中的第几个IP
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
uint8_t Snmp_Flash_Write_Secondary_Sector
(
    uint32_t     primary_address, 
    uint32_t     secondary_address, 
    int8_t*      data,
    uint8_t      index,
    uint8_t      Indication,
    
)
{
        
        uint16_t     sector;
        char         ip[256][16];
        
    
        /* 获取原次Sector编号 */
        sector = Flash_GetSector( secondary_address );
        /* 擦除原次Sector */
        FLASH_EraseSector(sector, VoltageRange_3);
        
        
        /* 将原主Sector所写位置前面的数据写到次Sector */
        for (i = 0; i < index; i++)
        {
            /* 从原主Sector读出4K数据 */
            Flash_Read_Byte(primary_address, 4096, ip);
            
            /* 将读出的4K数据写入到次Sector */
            Flash_Write_Byte(secondary_address, 4096, ip);
            
            /* 地址增加4K */
            primary_address += 4096；
            secondary_address += 4096；
        }  
        
        /* 读出要修改的IP所处4K的数据 */
        Flash_Read_Byte(primary_address, 4096, ip);
        
        /* 修改对应IP */
        memcpy(&ip[Indication][0], data_8, length); 
        
        /* 接着将该4K数据写到次Sector中 */
        Flash_Write_Byte(secondary_address, 4096, ip);


        /* 更换主次Sector标志 */ 
        *(uint8_t *)secondary_address = 0x80; /* 主Sector */
        *(uint8_t *)primary_address = 0x01;   /* 次Sector */ 


}

/****************************************************************************
* 功    能: 写入length字节的数据
* 入口参数：address：地址
*           length： 数据长度
*           data_8  指向写入的数据
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
void Snmp_Flash_Write_Byte( uint32_t address, uint16_t length, int8_t* data )
{
    uint16_t i;
    
   
        for(i = 0; i < length; i++)
        {
            
                if ( FLASH_ProgramByte( address, data)== FLASH_COMPLETE)
                {
                   address++;  
                }
                else
                {
                    while(1);
                }
        
        }



}

/****************************************************************************
* 功    能: 读取length字节的数据
* 入口参数：address：地址
*           length： 数据长度
*           data_8  指向读出的数据
* 出口参数：无
* 说    明：无
* 调用方法：无
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