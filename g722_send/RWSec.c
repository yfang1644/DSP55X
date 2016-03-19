/****************************************************************************
* Filename: rwsec.c
****************************************************************************/
#define IN_RWSEC

#include "tistdtypes.h"
#include "fat.h"

#define BASE_CACHE_NUM      1
#define MAX_DISK_CACHES     16

Disk_cache DiskCache[MAX_DISK_CACHES];      /* 磁盘cache */
extern Disk_Info DiskInfo;

/*****************************************************************************
** 函数名称: CacheInit
** 功能描述: 初始化磁盘cache
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块: 无
**
****************************************************************************/
void CacheInit(void)
{
    Uint16 i;
    
    for (i = 0; i < MAX_DISK_CACHES; i++)
    {
        DiskCache[i].Flag = 0;              /* 扇区无需回写 */
        DiskCache[i].RW_ID = (Uint16)(~0);  /* 访问纪录 */
        DiskCache[i].SecIndex = 0;          /* 缓冲的扇区号  */
    }
}


/*****************************************************************************
** 函数名称: CacheWriteBack2
** 功能描述: 把指定扇区写回逻辑盘
**
** 输　入: Index：cache索引
**
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块:
**
** 说  明: 在回写cache时，判断该扇区是否为FAT表，如果是，将该扇区覆盖到
           各个备份FAT表，实现同步。
****************************************************************************/
void CacheWriteBack2(Uint16 Index)
{
    Disk_RW_Parameter Pa;
    Int16 i;

    if (DiskInfo.DiakCommand != NULL)
    {
        i = 1;
        if(DiskCache[Index].SecIndex >= DiskInfo.FATStartSec)  /* 判断写入扇区是否为主FAT表的空间 */
        if(DiskCache[Index].SecIndex < DiskInfo.FATStartSec + DiskInfo.FATSecCnt)
        {
            i = DiskInfo.NumFATs;                        /* 需要同步的FAT表个数 */
        }
        
        /* 传递参数 */
        Pa.SectorIndex = DiskCache[Index].SecIndex + DiskInfo.SecOffset;
        Pa.RsvdForLow = DiskInfo.RsvdForLow;
        Pa.Buf = DiskCache[Index].buf;              

        do             /* 循环写入各FAT表，如果为普通扇区则只有一次 */
        {
//			if(DiskCache[Index].Flag & CACHE_DATA)
//				DiskInfo.DiakCommand(DISK_WRITE_RAW, &Pa);
//			else
				DiskInfo.DiakCommand(DISK_WRITE_SECTOR, &Pa);
            /* 调用底层驱动写扇区 */ 
			Pa.SectorIndex += DiskInfo.FATSecCnt;
        } while (--i > 0);
        /* cache 不再需要回写 */
        DiskCache[Index].Flag &= ~(CACHE_WRITTEN|CACHE_DATA);
    }
}

/*****************************************************************************
** 函数名称: AllCacheWriteBack
** 功能描述: 把所有已改变的扇区写回逻辑盘
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块: CacheWriteBack2
**
****************************************************************************/
void AllCacheWriteBack(void)
{
    Uint16 i;

    for (i = 0; i < MAX_DISK_CACHES; i++)
    {
        if (DiskCache[i].Flag & CACHE_WRITTEN)
        {
            CacheWriteBack2(i);
        }
    }
}

/*********************************************************************************************************
** 函数名称: GetCache
** 功能描述: 获取一个cache
**
** 输　入: 无
**
** 输　出: cache索引
**         
** 全局变量: DiskCache
** 调用模块: 无
**
********************************************************************************************************/
Uint16 GetCache(Uint32 Index)
{
    Uint16 Max;
    Uint16 i, j;
    Uint16 temp;
    
    Max = 0;
    j = ~0;
    temp = Index & ((1 << BASE_CACHE_NUM) - 1);
    for (i = 0; i < (1 << BASE_CACHE_NUM); i++)
    {
        if (Max <= DiskCache[temp].RW_ID)
        {
            Max = DiskCache[temp].RW_ID;
            j = temp;
        }
        if (Max == ~0)
        {
            break;
        }
        temp += MAX_DISK_CACHES >> BASE_CACHE_NUM;
    }
    if (j < MAX_DISK_CACHES)
    {
        if (DiskCache[j].Flag & CACHE_WRITTEN)
        {
            CacheWriteBack2(j);
        }
    }
    return j;
}

/*********************************************************************************************************
** 函数名称: OpenSec
** 功能描述: 为逻辑盘上的一个扇区打开一个cache并锁定
**
** 输　入: Index：扇区号
** 输　出: 指向指定扇区数据的指针
**         
** 全局变量: DiskCache
** 调用模块: 无
**
********************************************************************************************************/
Uint8 *OpenSec(Uint32 Index)
{
    Uint16 i;
    Uint8 *Rt;
    Uint16 temp;
    
    /* 增加访问间隔 */
    temp = Index & ((1 << BASE_CACHE_NUM) - 1);
    for (i = 0; i < (1 << BASE_CACHE_NUM); i++)
    {   
        if (DiskCache[temp].RW_ID < (Uint16)(~0))
        {
            DiskCache[temp].RW_ID++;
        }

        temp += MAX_DISK_CACHES >> BASE_CACHE_NUM;
    }

    /* 看扇区是否已经缓存 */
    Rt = NULL;
    temp = Index & ((1 << BASE_CACHE_NUM) - 1);
    for (i = 0; i < (1 << BASE_CACHE_NUM); i++)
    {   
        if (DiskCache[temp].SecIndex == Index)
        {
            Rt = DiskCache[temp].buf;
            DiskCache[temp].RW_ID = 0;
            break;
        }
        temp += MAX_DISK_CACHES >> BASE_CACHE_NUM;
    }
    
    /* Rt == NULL则未缓存 */
    if (Rt == NULL)
    {
        if (DiskInfo.SecPerDisk > Index)
        {
            i = GetCache(Index);                     /* 获取cache */
            if (i < MAX_DISK_CACHES)
            {
                /* 初始化cache  */
                DiskCache[i].RW_ID = 0;
                DiskCache[i].Flag = 0;
                DiskCache[i].SecIndex = Index;
                Rt = DiskCache[i].buf;
            }
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: ReadSec
** 功能描述: 从逻辑盘读扇区
**
** 输　入: Index：扇区号
** 输　出: TRUE:成功
**         FALSE:失败
** 全局变量: DiskCache
** 调用模块:
**
********************************************************************************************************/
Uint8 ReadSec(Uint32 Index, Uint16 isData)
{
    Uint16 i;
    Disk_RW_Parameter Pa;
    Uint16 Rt;
    Uint16 temp;
    
    temp = Index & ((1 << BASE_CACHE_NUM) - 1);
    for (i = 0; i < (1 << BASE_CACHE_NUM); i++)
    {
        if (DiskCache[temp].SecIndex == Index)
        {
            if (DiskCache[temp].Flag & CACHE_READ)
            {
                return RETURN_OK;
            }
            break;
        }
        temp += MAX_DISK_CACHES >> BASE_CACHE_NUM;
    }
    Rt = SECTOR_NOT_IN_CACHE;
    if (temp < MAX_DISK_CACHES)
    {
        DiskCache[temp].Flag |= CACHE_READ;

        /* 从磁盘读取扇区数据 */
        Pa.SectorIndex = Index + DiskInfo.SecOffset;
        Pa.RsvdForLow = DiskInfo.RsvdForLow;
        Pa.Buf = DiskCache[temp].buf;
        Rt = NOT_FIND_DISK;
        if (DiskInfo.DiakCommand != NULL)
        {
            if (isData)
            	DiskInfo.DiakCommand(DISK_READ_RAW, &Pa);
            else
            	DiskInfo.DiakCommand(DISK_READ_SECTOR, &Pa);
            return RETURN_OK;
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: WriteSec
** 功能描述: 说明指定逻辑盘的指定一个扇区被改写
**
** 输　入: Index：扇区号
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块: 无
**
********************************************************************************************************/
void WriteSec(Uint32 Index, Uint16 isData)
{
    Uint16 i, temp;

    temp = Index & ((1 << BASE_CACHE_NUM) - 1);
    for (i = 0; i < (1 << BASE_CACHE_NUM); i++)
    {   
        if (DiskCache[temp].SecIndex == Index)
        {
            DiskCache[temp].Flag = (CACHE_WRITTEN | CACHE_READ | isData);
            break;
        }
        temp += MAX_DISK_CACHES >> BASE_CACHE_NUM;
    }
}

/*********************************************************************************************************
** 函数名称: FreeDriveCache
** 功能描述: 释放指定逻辑盘的扇区
**
** 输　入: 
**
** 输　出: 无
**         
********************************************************************************************************/
void FreeDriveCache(void)
{
    Uint16 i;
    
    for (i = 0; i < MAX_DISK_CACHES; i++)
    {
        if (DiskCache[i].Flag & CACHE_WRITTEN) 
        {
            CacheWriteBack2(i);
        }
        DiskCache[i].RW_ID = (Uint16)(~0);
    }
}

/*********************************************************************************************************
** 函数名称: FreeCache
** 功能描述: 释放指定的扇区
**
** 输　入:
** 输　出: 无
**         
********************************************************************************************************/
void FreeCache(Uint32 Index)
{
    Uint16 i, temp;
    
    temp = Index & ((1 << BASE_CACHE_NUM) - 1);
    for (i = 0; i < (1 << BASE_CACHE_NUM); i++)
    {
        temp += MAX_DISK_CACHES >> BASE_CACHE_NUM;

        if (DiskCache[temp].SecIndex == Index)
        {
            if (DiskCache[temp].Flag & CACHE_WRITTEN)
            {
                CacheWriteBack2(temp);
            }
            DiskCache[temp].RW_ID = (Uint16)(~0);
            break;
        }
    }
}

