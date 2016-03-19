/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "ezdsp5535_sdcard.h"
#include "ff.h"
#include "diskio.h"
#include <csl_mmcsd.h>

extern CSL_MmcsdHandle 		mmcsdHandle;
extern Uint16 dmaInBuff[], dmaOutBuff[];

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

Uint16 disk_initialize (
	Uint16 drv				/* Physical drive number (0..) */
)
{
    if(drv)
    {
        return STA_NOINIT;  //仅支持磁盘0的操作
    }

//    state = SD_Init();
    return 0;           //初始化成功
}


/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

Uint16 disk_status (
	Uint16 drv		/* Physical drive number (0..) */
)
{
    if(drv)
    {
        return STA_NOINIT;  //仅支持磁盘0操作
    }

/*       检查SD卡是否插入
    if(!SD_DET())
    {
        return STA_NODISK;
    }
*/

    return 0;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	Uint16 drv,			/* Physical drive number (0..) */
	Uint8 *buff,		/* Data buffer to store read data */
	Uint32 sector		/* Sector address (LBA) */
)
{
	Uint16 res = 0;

#if	FILE_DMAMODE == 1
	res = MMC_dmaRead(sector << 9);
#else
	res = MMC_read(mmcsdHandle, (sector<<9), SD_SECTOR_SIZE, dmaInBuff);
#endif
	DSP_word2byte(buff, dmaInBuff, SD_SECTOR_SIZE/2);

    return (DRESULT)res;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	Uint16 drv,			/* Physical drive number (0..) */
	const Uint8 *buff,	/* Data to be written */
	Uint32 sector		/* Sector address (LBA) */
)
{
	Uint16 res = 0;

	DSP_byte2word(dmaOutBuff, buff, SD_SECTOR_SIZE/2);

#if	FILE_DMAMODE == 1
	res = MMC_dmaWrite(sector << 9);
#else
	res = MMC_write(mmcsdHandle, (sector<<9), SD_SECTOR_SIZE, dmaOutBuff);
#endif

	return (DRESULT)res;
}
#endif /* _READONLY */


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	Uint16 drv,		/* Physical drive number (0..) */
	Uint16 ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res;

    if (drv)
    {    
        return RES_PARERR;  //仅支持单磁盘操作，否则返回参数错误
    }
    
    //FATFS目前版本仅需处理CTRL_SYNC，GET_SECTOR_COUNT，GET_BLOCK_SIZ三个命令
    switch(ctrl)
    {
    case CTRL_SYNC:
        res = RES_OK;
        break;
        
    case GET_BLOCK_SIZE:
        *(Uint16*)buff = 512;
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(Uint32*)buff = 0xffffffff;	// get sd capacity
        res = RES_OK;
        break;
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}

