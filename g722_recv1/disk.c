/************************************************************************
* Filename: disk.c
************************************************************************/
#define IN_DISK

#include "tistdtypes.h"
#include "fat.h"

Disk_Info DiskInfo;     //逻辑盘信息

/*********************************************************************************************************
** 函数名称: DiskInit
** 功能描述: 初始化逻辑盘管理模块
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: DiskInfo
** 调用模块: 无
**
********************************************************************************************************/
void DiskInit(void)
{
    DiskInfo.FATType = 0xff;         // 无效的文件系统
    DiskInfo.SecPerClus = 0;         // 每簇扇区数
    DiskInfo.NumFATs = 0;            // FAT表数
    DiskInfo.SecPerDisk = 0xffffffff;// 逻辑驱动器包含扇区数
    DiskInfo.BytsPerSec = 0;         // 每扇区字节数
    DiskInfo.RootDirTable = 0;       // 根目录开始扇区号（FAT32为开始簇号）
    DiskInfo.RootSecCnt = 0;         // 根目录占用扇区数
    DiskInfo.FATStartSec = 0;        // FAT表开始扇区号
    DiskInfo.FATSecCnt = 0;          // 每个FAT占用扇区数
    DiskInfo.DataStartSec = 0;       // 数据区开始扇区号
    DiskInfo.PathClusIndex = 0;      // 当前目录
    DiskInfo.DiakCommand = NULL;     // 驱动程序

    CacheInit();                //初始化磁盘cache
}

/*********************************************************************************************************
** 函数名称: FsMount
** 功能描述: 内部加载卷
**
** 输　入: Disk：逻辑盘信息指针
**         Buf：扇区0内容
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
**
********************************************************************************************************/
Uint8 FsMount(Uint8 *Buf)
{
    Uint32 temp1;
    
    if (Buf[510] != 0x55 || Buf[511] != 0xaa)      /* 有效标志 */
    {
        return DISK_NO_FORMAT;
    }
    if (Buf[21] != 0xF0)
    if (Buf[21] < 0xF8)
    {
        return DISK_NO_FORMAT;
    }
    
    /* 每扇区字节数 */
    DiskInfo.BytsPerSec = Buf[11] | (Buf[12] << 8);
    /* 每簇扇区数 */
    DiskInfo.SecPerClus = Buf[13];
    /* FAT开始扇区号 */
    DiskInfo.FATStartSec = Buf[14] | (Buf[15] << 8);
    /* FAT表个数 */
    DiskInfo.NumFATs = Buf[16];
    /* 根目录占扇区数 */ 
    DiskInfo.RootSecCnt = ((Buf[17] | (Buf[18] << 8)) * 32 + 
                               DiskInfo.BytsPerSec - 1) /
                               DiskInfo.BytsPerSec;

    /* 逻辑盘（卷）占扇区数 */
    temp1 = Buf[19] | (Buf[20] << 8);
    if (temp1 == 0)
    {
        temp1 = Buf[35];
        temp1 = (temp1 << 8) | Buf[34];
        temp1 = (temp1 << 8) | Buf[33];
        temp1 = (temp1 << 8) | Buf[32];
    }
    DiskInfo.SecPerDisk = temp1;

    /* FAT表占用扇区数 */
    temp1 = Buf[22] | (Buf[23] << 8);
    if (temp1 == 0)
    {
        temp1 = Buf[39];
        temp1 = (temp1 << 8) | Buf[38];
        temp1 = (temp1 << 8) | Buf[37];
        temp1 = (temp1 << 8) | Buf[36];
    }
    DiskInfo.FATSecCnt = temp1;
                
    /* 当前目录为根目录 */
    DiskInfo.PathClusIndex = 0;
    /* 根目录开始扇区号 */
    DiskInfo.RootDirTable = DiskInfo.NumFATs * DiskInfo.FATSecCnt + 
                               DiskInfo.FATStartSec;
    /* 数据区开始扇区号 */
    DiskInfo.DataStartSec = DiskInfo.RootDirTable + DiskInfo.RootSecCnt;
    temp1 = DiskInfo.SecPerDisk - DiskInfo.DataStartSec;
    temp1 = temp1 / DiskInfo.SecPerClus;
    DiskInfo.ClusPerData = temp1;
    /* 判断是FAT12、FAT16还是FAT32 */
    if (temp1 < 4085)
    {
        DiskInfo.FATType = FAT12;
    }
    else if (temp1 < 65525)
    {
        DiskInfo.FATType = FAT16;
    }
    else
    {
        DiskInfo.FATType = FAT32;
        /* FAT32 RootDirTable为开始簇号 */

        DiskInfo.RootDirTable = Buf[47];
        DiskInfo.RootDirTable = (DiskInfo.RootDirTable << 8) | Buf[46];
        DiskInfo.RootDirTable = (DiskInfo.RootDirTable << 8) | Buf[45];
        DiskInfo.RootDirTable = (DiskInfo.RootDirTable << 8) | Buf[44];
        DiskInfo.PathClusIndex = DiskInfo.RootDirTable;
    }

    return RETURN_OK;
}


/*********************************************************************************************************
** 函数名称: FsUMount
** 功能描述: 卸载卷
**
** 输　入: Disk：逻辑盘信息指针
**         Buf：逻辑扇区0内容
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
**
********************************************************************************************************/
Uint8 FsUMount(void)
{
    FreeDriveCache();

    return RETURN_OK;
}

/*********************************************************************************************************
** 函数名称: FloppyDiskInit
** 功能描述: 软盘格式硬件初始化
**
** 输　入: DiakCommand：驱动程序接口函数
**         RsvdForLow：保留给底层的指针
**         Buf：逻辑扇区0内容
**         SecOffset：逻辑扇区偏移
** 输　出: 无
**         
********************************************************************************************************/
void FloppyDiskInit(Uint16  (* DiakCommand)(Uint8 Cammand, void *Parameter),
                       void *RsvdForLow , Uint8 *Buf, Uint32 SecOffset)
{
   	DiskInfo.DiakCommand = DiakCommand;
   	DiskInfo.SecOffset = SecOffset;
   	DiskInfo.RsvdForLow = RsvdForLow;

    FsMount(Buf);
}

/*********************************************************************************************************
** 函数名称: HardDiskInit
** 功能描述: 硬盘格式硬件初始化
**
** 输　入: DiakCommand：驱动程序接口函数
**         RsvdForLow：保留给底层的指针
**         Buf：扇区0内容
** 输　出: 无
**         
********************************************************************************************************/

void _HardDiskInit(Uint16  (* DiakCommand)(Uint8 Cammand, void *Parameter),
      void *RsvdForLow , Uint8 *Buf, Uint32 SecOffset, Uint16 offset)
{
    Uint32 temp1;
    Disk_RW_Parameter Pa;
    
    Pa.SectorIndex = SecOffset;
    Pa.RsvdForLow = RsvdForLow;
    Pa.Buf = Buf;
    if(DiakCommand(DISK_READ_SECTOR, &Pa) != DISK_READ_OK)
        return;

    if (Buf[510] != 0x55 || Buf[511] != 0xaa)       /* 有效标志 */
    {
        FloppyDiskInit(DiakCommand, RsvdForLow, Buf, SecOffset);
        return;
    }
    
    if ((Buf[0x1be] & 0xff7f) == 0x00)                /* 第1分区表有效 */
    if ((Buf[0x1ce] & 0xff7f) == 0x00)                /* 第2分区表有效 */
    if ((Buf[0x1de] & 0xff7f) == 0x00)                /* 第3分区表有效 */
    if ((Buf[0x1ee] & 0xff7f) == 0x00)                /* 第4分区表有效 */
    {
        temp1 = Buf[0x1be] + Buf[0x1ce] + Buf[0x1de] + Buf[0x1ee];
        if (temp1 != 0 && temp1 != 0x80)
        {
            FloppyDiskInit(DiakCommand, RsvdForLow, Buf, SecOffset);
            return;
        }
    }

    switch (Buf[offset + 4])    /* 第1分区 */
    {
        case 0x01:              /* FAT12*/
        case 0x04:              /* FAT16*/
        case 0x06:              /* FAT16*/
        case 0x0b:              /* FAT32*/
        case 0x0c:              /* FAT32*/
        case 0x0e:              /* FAT16*/
        case 0x1b:              /* FAT32*/
        case 0x1c:              /* FAT32*/
			temp1 = Buf[offset + 11];
			temp1 = (temp1 << 8) | Buf[offset + 10];
			temp1 = (temp1 << 8) | Buf[offset + 9];
			temp1 = (temp1 << 8) | Buf[offset + 8];
            
            SecOffset = temp1 + SecOffset;

            Pa.SectorIndex = SecOffset;
            Pa.RsvdForLow = RsvdForLow;
            Pa.Buf = Buf;
            if(DiakCommand(DISK_READ_SECTOR, &Pa) != DISK_READ_OK)
            {
                return;
            }

            FloppyDiskInit(DiakCommand, RsvdForLow, Buf, SecOffset);
            break;
        default:
            break;
    }
}

/******************************************************************************
** 函数名称: AddFileDriver
** 功能描述: 增加一个底层驱动程序
**
** 输　入: DiakCommand：驱动程序接口函数
**
** 输　出: 无
**         
** 全局变量: DiskInfo
** 调用模块: 无
**
*****************************************************************************/
void AddFileDriver(Uint16 (* DiakCommand)(Uint8 Cammand, void *Parameter), void *RsvdForLow)
{
    Uint8 Buf[DISK_CACHE_SIZE];
    Disk_RW_Parameter Pa;

    DiakCommand(DISK_INIT, &Pa);

    Pa.SectorIndex = 0;
    Pa.RsvdForLow = RsvdForLow;
    Pa.Buf = Buf;
    if(DiakCommand(DISK_READ_SECTOR, &Pa) != DISK_READ_OK)
        return;

    if ((Buf[510] != 0x55) || (Buf[511] != 0xaa))        /* 有效标志 */
		return;
	if ((Buf[0x1be] & 0xff7f) == 0)    /* 第1分区表有效 */
		_HardDiskInit(DiakCommand, RsvdForLow, Buf, 0, 0x1be);
	if ((Buf[0x1ce] & 0xff7f) == 0)    /* 第2分区表有效 */
		_HardDiskInit(DiakCommand, RsvdForLow, Buf, 0, 0x1ce);
	if ((Buf[0x1de] & 0xff7f) == 0)    /* 第3分区表有效 */
		_HardDiskInit(DiakCommand, RsvdForLow, Buf, 0, 0x1de);
	if ((Buf[0x1ee] & 0xff7f) == 0)    /* 第4分区表有效 */
		_HardDiskInit(DiakCommand, RsvdForLow, Buf, 0, 0x1ee);
}

/******************************************************************************
** 函数名称: RemoveFileDriver
** 功能描述: 删除一个底层驱动程序
**
** 输　入: DiakCommand:驱动程序函
**
** 输　出: 无
**         
** 全局变量: DiskInfo
** 调用模块: 无
**
*****************************************************************************/
void RemoveFileDriver(Uint16 (* DiakCommand)(Uint8 Cammand, void *Parameter))
{
    Disk_RW_Parameter Pa;
    
    if (DiskInfo.DiakCommand != DiakCommand)
    {
        FsUMount();
        Pa.RsvdForLow = DiskInfo.RsvdForLow;
        DiskInfo.DiakCommand(DISK_CLOSE, &Pa);

        DiskInfo.FATType = 0xff;         // 无效的文件系统
        DiskInfo.SecPerClus = 0;         // 每簇扇区数
        DiskInfo.NumFATs = 0;            // FAT表数
        DiskInfo.SecPerDisk = 0xffffffff;// 逻辑驱动器包含扇区数
        DiskInfo.BytsPerSec = 0;         // 每扇区字节数
        DiskInfo.RootDirTable = 0;       // 根目录开始扇区号（FAT32为开始簇号）
        DiskInfo.RootSecCnt = 0;         // 根目录占用扇区数
        DiskInfo.FATStartSec = 0;        // FAT表开始扇区号
        DiskInfo.FATSecCnt = 0;          // 每个FAT占用扇区数
        DiskInfo.DataStartSec = 0;       // 数据区开始扇区号
        DiskInfo.PathClusIndex = 0;      // 当前目录
        DiskInfo.DiakCommand = NULL;     // 驱动程序
    }
}

