/**************************************************************************
* Filename : fat.c
**************************************************************************/

#define IN_FAT

#include "typedef.h"
#include "fat.h"

extern Disk_Info DiskInfo;

/*********************************************************************************************************
** 函数名称: FATGetNextClus
** 功能描述: 返回FAT表指定簇的下一个簇号
**
** 输　入: Index：簇号
** 输　出: 下一个簇号
**         
** 全局变量: 无
** 调用模块: 无
**
********************************************************************************************************/
Uint32 FATGetNextClus(Uint32 Index)
{
    Uint16 temp, ByteIndex;
    Uint32 SecIndex;
    Uint8 *Buf;
    Uint32 Rt;
    
    if (Index >= (DiskInfo.ClusPerData))
    {
        return BAD_CLUS;
    }
    
    /* 计算扇区号和字节索引 */
    switch (DiskInfo.FATType)
    {
        case FAT12:
            SecIndex = Index * 3 / (2 * DiskInfo.BytsPerSec);
            ByteIndex = ((Index * 3) / 2) - (SecIndex * DiskInfo.BytsPerSec);
            SecIndex += DiskInfo.FATStartSec;
            break;
        case FAT16:
            SecIndex = Index * 2 / DiskInfo.BytsPerSec + DiskInfo.FATStartSec;
            ByteIndex = (Index * 2) & (DiskInfo.BytsPerSec - 1);
            break;
        case FAT32:
            SecIndex = Index * 4 / DiskInfo.BytsPerSec + DiskInfo.FATStartSec;
            ByteIndex = (Index * 4) & (DiskInfo.BytsPerSec - 1);
            break;
        default:
            return BAD_CLUS;
    }

    Buf = OpenSec(SecIndex);
    if (Buf == NULL)
    {
        return BAD_CLUS;
    }
    ReadSec(SecIndex, 0);
    
    /* 读取FAT表数据 */
    switch (DiskInfo.FATType)
    {
        case FAT12:
            temp = Buf[ByteIndex];
            ByteIndex++;
            if (ByteIndex >= DiskInfo.BytsPerSec)          /* 下一个字节是否在下一个扇区 */
            {
                Buf = OpenSec(SecIndex + 1);
                if (Buf == NULL)
                {
                    return BAD_CLUS;
                }
                ReadSec(SecIndex + 1, 0);
                temp = temp | (Buf[0] << 8);
            }
            else
            {
                temp = temp | (Buf[ByteIndex] << 8);
            }
            if ((Index & 0x01) != 0)                /* 判断哪12位有效 */
            {
                temp = temp >> 4;
            }
            else
            {
                temp = temp & 0x0fff;
            }
            Rt = temp;
            if (temp >= (BAD_CLUS & 0x0fff))        /* 是否有特殊意义 */
            {
                Rt = ((Uint32)0x0fffL << 16) | (temp | 0xf000);
            }
            break;
        case FAT16:
            temp = Buf[ByteIndex] | (Buf[ByteIndex + 1] << 8);
            Rt = temp;
            if (temp >= (BAD_CLUS & 0xffff))        /* 是否有特殊意义 */
            {
                Rt = ((Uint32)0x0fffL << 16) | temp;
            }
            break;
        case FAT32:
			Rt = Buf[ByteIndex + 3];
			Rt = (Rt << 8) | Buf[ByteIndex + 2];
			Rt = (Rt << 8) | Buf[ByteIndex + 1];
			Rt = (Rt << 8) | Buf[ByteIndex + 0];
            Rt = Rt & 0x0fffffff;
            break;
        default:
            Rt = BAD_CLUS;
            break;
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: FATSetNextClus
** 功能描述: 设置下一个簇
**
** 输　入:Index：簇号
**        Next：下一个簇号
** 输　出: 无
**         
** 全局变量: 无
** 调用模块: 无
**
********************************************************************************************************/
void FATSetNextClus(Uint32 Index, Uint32 Next)
{
    Uint16 temp;
    Uint16 SecIndex, ByteIndex;
    Uint8 *Buf;
    
    if (Index <= EMPTY_CLUS_1)
    {
        return;
    }

    if (Index >= DiskInfo.ClusPerData)
    {
        return;
    }
    
    /* 计算扇区号和字节索引 */
    switch (DiskInfo.FATType)
    {
        case FAT12:
            SecIndex = Index * 3 / (2 * DiskInfo.BytsPerSec);
            ByteIndex = ((Index * 3) / 2) - (SecIndex * DiskInfo.BytsPerSec);
            SecIndex += DiskInfo.FATStartSec;
            break;
        case FAT16:
            SecIndex = Index * 2 / DiskInfo.BytsPerSec + DiskInfo.FATStartSec;
            ByteIndex = (Index * 2) & (DiskInfo.BytsPerSec - 1);
            break;
        case FAT32:
            SecIndex = Index * 4 / DiskInfo.BytsPerSec + DiskInfo.FATStartSec;
            ByteIndex = (Index * 4) & (DiskInfo.BytsPerSec - 1);
            break;
        default:
            return;
    }

    Buf = OpenSec(SecIndex);
    if (Buf == NULL)
    {
        return;
    }
    ReadSec(SecIndex, 0);

    switch (DiskInfo.FATType)
    {
        case FAT12:
            temp = Next & 0x0fff;
            if ((Index & 0x01) != 0)                /* 判断哪12位有效 */
            {
                temp = temp * 16;
                temp |= (Buf[ByteIndex] & 0x0f);
                Buf[ByteIndex] = temp;
            }
            else
            {
                Buf[ByteIndex] = temp;
            }
            ByteIndex++;
            temp = temp >> 8;
            if (ByteIndex >= DiskInfo.BytsPerSec)          /* 下一个字节是否在下一个扇区 */
            {
                Buf = OpenSec(SecIndex + 1);
                if (Buf == NULL)
                {
                    break;
                }
                ReadSec(SecIndex + 1, 0);
                if ((Index & 0x01) != 0)                /* 判断哪12位有效 */
                {
                    Buf[0] = temp;
                }
                else
                {
                    Buf[0] = (Buf[0] & 0xf0) | temp;
                }
                WriteSec(SecIndex + 1, 0);
            }
            else
            {
                if ((Index & 0x01) != 0)                /* 判断哪12位有效 */
                {
                    Buf[ByteIndex] = temp;
                }
                else
                {
                    Buf[ByteIndex] = (Buf[ByteIndex] & 0xf0) | temp;
                }
            }
            break;
        case FAT16:
            Buf[ByteIndex] = Next;
            Buf[ByteIndex + 1] = Next >> 8;
            break;
        case FAT32:
            Buf[ByteIndex] = Next;
            Buf[ByteIndex + 1] = Next >> 8;
            Buf[ByteIndex + 2] = Next >> 16;
            Buf[ByteIndex + 3] = (Buf[ByteIndex + 3] & 0xf0) | ((Next >> 24) & 0x0f);
            break;
        default:
            break;
    }
    WriteSec(SecIndex, 0);
    return ;
}

/*********************************************************************************************************
** 函数名称: FATAddClus
** 功能描述: 为指定簇链增加一个簇
**
** 输　入: Index：簇链中任意一个簇号，如果为0，则为一个空链增加一个簇
** 输　出: 增加的簇号
**         
** 全局变量: 无
** 调用模块: 无
**
********************************************************************************************************/
Uint32 FATAddClus(Uint32 Index)
{
    Uint32 NextClus,ThisClus,MaxClus;

    if (Index >= BAD_CLUS)
    {
        return BAD_CLUS;
    }

    MaxClus = DiskInfo.ClusPerData;

    /* 查找最后一个簇 */
    ThisClus = Index;
    if (ThisClus != EMPTY_CLUS && ThisClus != EMPTY_CLUS_1)
    {
        while (1)
        {
            NextClus = FATGetNextClus(ThisClus);
            if (NextClus >= EOF_CLUS_1)
            {
                break;
            }
            if (NextClus <= EMPTY_CLUS_1)
            {
                break;
            }
            if (NextClus == BAD_CLUS)
            {
                return BAD_CLUS;
            }
            ThisClus = NextClus;
        }
    }
    else
    {
        ThisClus = EMPTY_CLUS_1;
    }
    
    for (NextClus = ThisClus + 1; NextClus < MaxClus; NextClus++)
    {
        if (FATGetNextClus(NextClus) == EMPTY_CLUS)
        {
            break;
        }
    }
    if (NextClus >= MaxClus)
    {
        for (NextClus = EMPTY_CLUS_1 + 1; NextClus < ThisClus; NextClus++)
        {
            if (FATGetNextClus(NextClus) == EMPTY_CLUS)
            {
                break;
            }
        }
    }
    if (FATGetNextClus(NextClus) == EMPTY_CLUS)
    {
        if (ThisClus > EMPTY_CLUS_1)
        {
            FATSetNextClus(ThisClus, NextClus);
        }
        FATSetNextClus(NextClus, EOF_CLUS_END);
        return NextClus;
    }
    else
    {
        return BAD_CLUS;
    }
}

/******************************************************************************
** 函数名称: FreeClus
** 功能描述: 将指定簇所有数据清零
**
** 输　入: Index：簇号
** 输　出: RETURN_OK:成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: OpenSec,WriteSec,CloseSec
**
*****************************************************************************/
Uint8 FreeClus(Uint32 Index)
{
    Int16 temp;
    Uint32 SecIndex;
    Disk_RW_Parameter parameter;

    if (Index < DiskInfo.ClusPerData)
    {
        temp = DiskInfo.SecPerClus;
        Index -= 2;                     /* 2：保留簇数 */
        SecIndex = DiskInfo.DataStartSec + Index * temp;
        parameter.RsvdForLow = DiskInfo.RsvdForLow;
        do
        {
            parameter.SectorIndex = SecIndex;
            DiskInfo.DiakCommand(DISK_FREE_SECTOR, &parameter);
            FreeCache(SecIndex);
            SecIndex++;
        } while (--temp > 0);
        return RETURN_OK;
    }
    else
    {
        return CLUSTER_NOT_IN_DISK;
    }
}

/******************************************************************************
** 函数名称: FATDelClusChain
** 功能描述: 删除指定簇链
**
** 输　入: Index：簇链中首簇号
** 输　出: 无
**         
** 全局变量: 无
** 调用模块: FATGetNextClus,FATSetNextClus
*****************************************************************************/
void FATDelClusChain(Uint32 Index)
{
    Uint32 NextClus, ThisClus;
    
    if (Index <= EMPTY_CLUS_1)
    {
        return;
    }
    if (Index >= BAD_CLUS)
    {
        return;
    }
    ThisClus = Index;
    while (1)
    {
        NextClus = FATGetNextClus(ThisClus);
        FATSetNextClus(ThisClus, EMPTY_CLUS);
        FreeClus(ThisClus);

        if (NextClus >= BAD_CLUS)
        {
            break;
        }
        if (NextClus <= EMPTY_CLUS_1)
        {
            break;
        }
        ThisClus = NextClus;
    }
}

