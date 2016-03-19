#include "ezdsp5535_sdcard.h"

#include "fat.h"

int SD_ReadBlock(Uint32 sect, Uint8 *p);
int SD_WriteBlock(Uint32 sect, Uint8 *p);
int SD_ReadBlock_Raw(Uint32 sect, Uint8 *p);
int SD_WriteBlock_Raw(Uint32 sect, Uint8 *p);

/**--------------File Info-------------------------------------------------------------------------------
** File name:			sd.c

** 函数名称: Empty      ---> 使用您自己的函数名称
** 功能描述: 底层驱动程序与上层的接口程序
**
** 输　入: Cammand：define DISK_INIT:                 初始化，必须实现
**                  define DISK_CLOSE:                关闭，必须实现
**                  define DISK_READ_SECTOR:          读扇区，必须实现
**                  define DISK_WRITE_SECTOR:         写扇区，必须实现
**                  define DISK_DRIVER_VER:           查看驱动程序接口版本号，必须实现
**                  define DISK_CHECK_CMD:            查看命令是否实现，必须实现
**                  define DISK_LOW_FORMAT:           低级格式化，可选命令
**                  define DISK_FREE_SECTOR:          释放扇区，可选命令
**                  define DISK_GET_SECTOR_NUMBER:    获得设备总扇区数，可选命令
**                  define DISK_GET_BYTES_PER_SECTOR: 获得每扇区字节数，可选命令
**                  define DISK_CHECK_CHANGE:         查看介质是否改变，可选命令
**                  define DISK_GET_SECTORS_PER_BLOCK:获取每块扇区数，可选命令
**                  define DISK_READ_BLOCK:           读数据块，可选命令
**                  define DISK_WRITE_BLOCK:          写数据块，可选命令
**        Parameter: 剩余参数，其中Parameter->RsvdForLow用于存储驱动程序支持的物理设备相关信息，
**                             如使用的资源等。使用它可以使一个驱动支持多个物理设备。                 
** 输　出: DISK_READ_OK:      读扇区完成
**        DISK_READ_NOT_OK:   读扇区失败
**        DISK_WRITE_OK:      写扇区完成
**        DISK_WRITE_NOT_OK:  写扇区失败
**        DISK_INIT_OK:       初始化完成
**        DISK_INIT_NOT_OK:   初始化失败
**        DISK_FALSE:         真
**        DISK_TRUE:          假
**        BAD_DISK_COMMAND:   无效的命令
** 全局变量: 无
** 调用模块: 无
**
********************************************************************************************************/
Uint16 SDCammand(Uint8 Cammand, void *Parameter) /*使用您自己的函数名称 */
{
    Uint16 rt;
    Disk_RW_Parameter * Dp;
    //FFSDisk *Disk;
    
    Dp = (Disk_RW_Parameter *)Parameter;
    //Disk = (FFSDisk *) Dp->RsvdForLow;
    
    /* 可选命令如果没有实现，则rt =  BAD_DISK_COMMAND*/
    switch (Cammand)
    {
        case DISK_INIT:
			/* Initialize SD card interface */
			EZDSP5535_SDCARD_init();
           	rt = DISK_INIT_OK;
            break;
            
        case DISK_CLOSE:
            /* 关闭驱动程序，必须实现 */
            /* Parameter没有使用 */
		    EZDSP5535_SDCARD_close();
			rt = DISK_RETURN_OK;
            //rt = BAD_DISK_COMMAND;
            break;
            
        case DISK_READ_SECTOR:
        	SD_ReadBlock(Dp->SectorIndex, Dp->Buf);
        	rt = DISK_READ_OK;
            break;
            
        case DISK_WRITE_SECTOR:
        	SD_WriteBlock(Dp->SectorIndex, Dp->Buf);
    		rt = DISK_WRITE_OK;
            break;

        case DISK_READ_RAW:
        	SD_ReadBlock_Raw(Dp->SectorIndex, Dp->Buf);
        	rt = DISK_READ_OK;
            break;
            
        case DISK_WRITE_RAW:
        	SD_WriteBlock_Raw(Dp->SectorIndex, Dp->Buf);
    		rt = DISK_WRITE_OK;
            break;

        case DISK_CHECK_CMD:
            rt = DISK_FALSE;
            /* 以下那个命令没有实现就注释掉相应的语句 */
            if ((Dp->SectorIndex == DISK_INIT)
               || (Dp->SectorIndex == DISK_CLOSE)
               || (Dp->SectorIndex == DISK_READ_SECTOR)
               || (Dp->SectorIndex == DISK_WRITE_SECTOR)
               || (Dp->SectorIndex == DISK_DRIVER_VER)
               || (Dp->SectorIndex == DISK_CHECK_CMD)
             //  || (Dp->SectorIndex == DISK_LOW_FORMAT)
             //  || (Dp->SectorIndex == DISK_FREE_SECTOR)
             //  || (Dp->SectorIndex == DISK_GET_SECTOR_NUMBER)
             //  || (Dp->SectorIndex == DISK_GET_BYTES_PER_SECTOR)
             //  || (Dp->SectorIndex == DISK_CHECK_CHANGE)
             //  || (Dp->SectorIndex == DISK_GET_SECTORS_PER_BLOCK)
             //  || (Dp->SectorIndex == DISK_READ_BLOCK)
             //  || (Dp->SectorIndex == DISK_WRITE_BLOCK)
               )
            {
                rt = DISK_TRUE;
            }
            break;
            
        //case DISK_LOW_FORMAT:
            /* 低级格式化，可选命令 */
            /* Dp->SectorIndex：0:一般低级格式化 */
            /* Dp->SectorIndex：其它:第一次低级格式化 */
        //    rt = DISK_RETURN_OK;
        //    break;
            
        //case DISK_FREE_SECTOR:
            /* 释放扇区，可选命令，ZLG/FFS使用此命令 */
            /* 此命令让驱动程序知道哪些扇区不再保存有用数据 */
            /* Dp->SectorIndex：物理扇区索引 */
            //rt = DISK_RETURN_OK;
        //    break;
            
        case DISK_GET_SECTOR_NUMBER:
            /* 获得设备总扇区数，可选命令 */
            //Dp->SectorIndex = 有效的物理扇区总数;
            Dp->SectorIndex = 5124288;
            rt = DISK_RETURN_OK;
            break;
            
        case DISK_GET_BYTES_PER_SECTOR:
            /* 获得每扇区字节数，可选命令 */
            //Dp->SectorIndex = 每扇区字节数;
            Dp->SectorIndex = 512;
            rt = DISK_RETURN_OK;
            break;
            
        //case DISK_CHECK_CHANGE:
            /* 查看介质是否改变，可选命令 */
            //if (介质改变(如更换了SD卡))
            //{
            //    rt = DISK_TRUE;
            //}
            //else
            //{
            //    rt = DISK_FALSE;
            //}
            
        //case DISK_GET_SECTORS_PER_BLOCK:
            /* 获取每块扇区数，可选命令 */
            //Dp->SectorIndex = 每块扇区数;
        //  rt = DISK_RETURN_OK;
        //  break;
            
        //case DISK_READ_BLOCK:
            /* 读数据块，可选命令 */
            /* Dp->Buf：存储读到的数据 */
            /* Dp->SectorIndex：块的第一个物理扇区索引 */
            /* rt=DISK_READ_OK或DISK_READ_NOT_OK*/
        //  break;
            
        //case DISK_WRITE_BLOCK:
            /* 写数据块，可选命令 */
            /* Dp->Buf：需要写的数据 */
            /* Dp->SectorIndex：块的第一个物理扇区索引 */
            /* rt=DISK_WRITE_OK或DISK_WRITE_NOT_OK*/
        //  break;
            
        default:
            rt = BAD_DISK_COMMAND;
            break;
    }
    return rt;
}

