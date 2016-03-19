/**************************************************************************
* Filename:  dir.c
**************************************************************************/
#define IN_DIR

#include "tistdtypes.h"
#include "fat.h"

extern Disk_Info DiskInfo;
Int8 FsPathFileName[MAX_PATHFILE_LENGTH + 1];
Uint32 GetDirClusIndex(Int8 *Path);

/******************************************************************************
** 函数名称: FsStrCopy
** 功能描述: 把字符串拷贝内部缓存，小写变大写
**
** 输　入: source:字符串
**        
** 输　出: 内部缓存地址
**
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
Int8 *FsStrCopy(Int8 *source)
{
    Uint16 i;
    Int8 *destin;
    
    if (source == NULL)
    {
        return NULL;
    }
    destin = FsPathFileName;
    for(i = 0; i < MAX_PATHFILE_LENGTH; i++)
    {
        destin[i] = source[i];
        if (destin[i] == 0)
            break;
		if(destin[i] >= 'a' && destin[i] <= 'z')
			destin[i] &= ~0x20;
	}

    return FsPathFileName;
}

/******************************************************************************
** 函数名称: FS_GetDateTime
** 功能描述: 时间格式转换函数，将DATE_TIME格式的数据转换为SYS_TIME格式的时间数据
**
** 输　入: CurTime  指向保存结果
**
** 输　出: 错误代码，RETURN_OK为正确
**         
** 全局变量: 无
** 调用模块: GetDataTime
**
*****************************************************************************/
Uint8 FS_GetDateTime(SYS_TIME *CurTime)
{
   Uint8       temp8;
   Uint16      temp16 = 0;
   DATE_TIME   Time;
   
//   temp8 = GetDateTime(&Time);            // 获取当前时间


   /* 时间格式转换 */
   Time.da_year = 2012;
   Time.da_mon =     2;
   Time.da_day =    29;
    Time.ti_hour  = 0;
    Time.ti_min   = 0;
    Time.ti_sec   = 0;
    Time.ti_hund  = 0;
   if(Time.da_year > 1980)
   {
      temp16  = Time.da_year - 1980;      // 年，如果当前年小于1980年，设为0
   }
   temp16  = (temp16 & 0x7F) << 9;        // 年：9～15位
   temp16 |= (Time.da_mon & 0x0F) << 5;   // 月：5～8位
   temp16 |= Time.da_day & 0x1F ;         // 日：0～4位
   CurTime->date = temp16;
	
   temp16  = (Time.ti_hour & 0x1F) << 11; // 时：11～15位
   temp16 |= (Time.ti_min  & 0x3F) << 5;  // 分：5～10位
   temp16 |= (Time.ti_sec  & 0x3F) >> 1;  // 秒,变为2秒的倍数：0～4位
   CurTime->time = temp16; 
   
   if((Time.ti_sec & 0x01) == 0x01)
   {
      temp8   = 100;                   // 如果当前秒为奇数，毫秒戳加上100
   } 
   temp8   += Time.ti_hund;            // 加上百分之一秒数
   CurTime->msec = temp8;
   
   return RETURN_OK;
}

/******************************************************************************
** 函数名称: StrToFDTName
** 功能描述: 用户文件\目录名转换为系统名
**
** 输　入: Str:用户名称
**        
** 输　出: 驱动器号
**
** 全局变量: 无
** 调用模块: 无
**
*****************************************************************************/
Uint8 StrToFDTName(Int8 *FDTName, Int8 *Str)
{
    Uint8 i, temp;
    
    /* 文件\目录名是否有效 */
    if (Str[0] == ' ' || Str[0] == 0)
        return FILE_NAME_ERR; 

    for (i = 0; i < 11; i++)
        FDTName[i] = ' ';

    /* 是否为"." */
    if (Str[0] == '.')
    if (Str[1] == 0 || Str[1] == '\\')
    {
        FDTName[0] = '.';
        return RETURN_OK;
    }

    /* 是否为".." */
    if (Str[0] == '.')
    if (Str[1] == '.' )
    if (Str[2] == 0 || Str[2] == '\\')
    {
        FDTName[0] = '.';
        FDTName[1] = '.';
        return RETURN_OK;
    }

    /* 获取主文件/目录名 */
    for (i = 0; i < 8; i++)
    {
        temp = *Str;
        if ((temp == 0) || (temp == '\\'))
            break;
        Str++;
        if (temp == '.')
            break;
        FDTName[i] = temp;
    }

    if (*Str == '.')
        Str++;

    /* 获取文件\目录扩展名 */
    for (i = 8; i < 11; i++)
    {
        temp = *Str++;
        if (temp == 0 || temp == '\\')
            break;
        FDTName[i] = temp;
    }
    return RETURN_OK;
}

/******************************************************************************
** 函数名称: _GetFDTInfo
** 功能描述: 获取FDT所在的目录的开始簇号及系统内名称，内部使用
**
** 输　入: DirFileName:用户使用的FDT名（包括路径）
**         FDTName:用于返回系统使用的FDT名（不包括路径）
** 输　出: FDT所在的目录的开始簇号，BAD_CLUS表示找不到路径
**         
** 全局变量: 无
** 调用模块: GetDirClusIndex,StrToFDTName
**
*****************************************************************************/
Uint32 _GetFDTInfo(Int8 *FDTName, Int8 *DirFileName)
{
    Uint32 Rt;
    Int8 *temp;
    Int8 ch;
    
    /* 获取字符串结束位置 */
    temp = DirFileName;
    while (*temp++ != 0)	;
    temp--;
    if (*(--temp) == '\\')  /* 最后字符为\不是有效文件/目录名 */
    {
        return BAD_CLUS;
    }
    
    /* 获取目录开始簇号 */
    Rt = BAD_CLUS;
    while (1)
    {
        if (*temp == '\\' || *temp == ':')
        {
            /* 找到目录分割符号'\' 或 */
            /* 找到逻辑盘分割符号':'，表明是指定逻辑盘当前目录 */
            ch = *(++temp);
            *temp = 0;
            Rt = GetDirClusIndex(DirFileName);
            *temp = ch;
            break;
        }
        if (temp == DirFileName)
        {
            /* 只有文件\目录名，表明是当前逻辑盘当前目录 */
            //Rt = GetDirClusIndex(".");
            Rt = DiskInfo.PathClusIndex;
            break;
        }
        temp--;
    }
    /* 获取系统内文件\目录名 */
    if (StrToFDTName(FDTName, temp) != RETURN_OK)
    {
        Rt = BAD_CLUS;
    }
    return Rt;
}

/******************************************************************************
** 函数名称: GetDirClusIndex
** 功能描述: 获取指定目录开始簇号
**
** 输　入: Path:路径名
**        
** 输　出: 开始簇号，EMPTY_CLUS：为根目录
**
** 全局变量: 无
** 调用模块: FindFDTInfo
**
*****************************************************************************/
Uint32 GetDirClusIndex(Int8 *Path)
{
    Int8 DirName[12];
    Uint32 Rt;
    FDT temp;
    
    Rt = BAD_CLUS;
    if (Path != NULL)
    {
        Path = FsStrCopy(Path);
        if (Path[1] == ':')
            Path += 2;

        Rt = 0;
        if (DiskInfo.FATType == FAT32)         /* FAT32 根目录 */
            Rt = DiskInfo.RootDirTable;

        if (Path[0] != '\\')       /* 不是目录分隔符号，表明起点是当前路径 */
            Rt = DiskInfo.PathClusIndex;
        else
            Path++;

        if (Path[0] == '.')        /* '\.'表明起点是当前路径 */
        {
            Rt = DiskInfo.PathClusIndex;
            if (Path[1] == 0 || Path[1] == '\\')
            {
                Path++;
            }
        }
        if (Path[0] == '\\')
        {
            Path++;
        }
        
        DirName[11] = 0;
        while (Path[0] != 0)
        {
            /* 获取子目录名 */
            StrToFDTName(DirName , Path);

            /* 子目录名开始簇号 */
            if (DirName[0] == 0x20)
            {
                Rt = BAD_CLUS;
                break;
            }
            /* 获取FDT信息 */
            if (FindFDTInfo(&temp, Rt, DirName) != RETURN_OK)
            {
                Rt = BAD_CLUS;
                break;
            }
            /* FDT是否是目录 */
            if ((temp.Attr & ATTR_DIRECTORY) == 0)
            {
                Rt = BAD_CLUS;
                break;
            }
            Rt = temp.FstClusLO + ((Uint32)temp.FstClusHI << 16);
            /* 字符串到下一个目录 */
            while (*Path != 0)
            {
                if (*Path++ == '\\')
                    break;
            }
        }

        if (DiskInfo.FATType == FAT32)
        if (Rt != BAD_CLUS)
        if (Rt == DiskInfo.RootDirTable)
        {
            Rt = 0;
        }
    }
    return Rt;
}

/******************************************************************************
** 函数名称: MakeDir
** 功能描述: 建立目录
**
** 输　入: Path:路径名
**
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: ClearClus,AddFDT
**
** 说  明: 增加文件夹建立时的"建立时间"属性
*****************************************************************************/
Uint8 MakeDir(Int8 *Path)
{
    Uint32 ClusIndex, Temp1;
    Uint8 Rt;
    FDT temp;
    SYS_TIME CurTime;
    
    Path = FsStrCopy(Path);

    ClusIndex = _GetFDTInfo(temp.Name, Path);
    if (ClusIndex == BAD_CLUS)
    {
        return PATH_NOT_FIND;
    }

    /* FDT是否存在 */
    Rt = FDTIsLie(ClusIndex, temp.Name);	// 查看要建立的目录是否已经存在
    if (Rt != NOT_FIND_FDT)
    {
        return Rt;								// 如果存在同名目录，则错误返回
    }
    
    /* 不存在 */
    Temp1 = FATAddClus(0);                      /* 获取目录所需磁盘空间 */
    if ((Temp1 <= EMPTY_CLUS_1) || (Temp1 >= BAD_CLUS))
    {
        /* 没有空闲空间 */
        return  DISK_FULL;
    }

    ClearClus(Temp1);                    /* 清空簇 */
    
    if(FS_GetDateTime(&CurTime) != RETURN_OK)   // 获取当前时间
    {
        return GET_TIME_ERR;    
    }    
        
        /* 设置FDT属性 */
    temp.Attr = ATTR_DIRECTORY;      		    // FDT属性为文件夹       
    temp.FileSize = 0;						    // 大小为0

    temp.NTRes = 0;

    /*-------在此加入创建时间的程序---------*/
    temp.CrtTimeTenth = CurTime.msec;
    temp.CrtTime = CurTime.time;
    temp.CrtDate = CurTime.date;
    
    temp.LstAccDate = CurTime.date;
    temp.WrtTime = CurTime.time + 1;            // 修改时间需要加2秒，因为它不能计算到毫秒戳
    temp.WrtDate = CurTime.date;
	/*--------------------------------------*/
  
    temp.FstClusLO = Temp1 & 0xffff;            // 簇链第一簇的低16位
    temp.FstClusHI = Temp1 >> 16;               // 高16位

    Rt = AddFDT(ClusIndex, &temp);       /* 增加目录项 */
    if (Rt == RETURN_OK)
    {
        /* 建立'.'目录 */
        temp.Name[0] = '.';
        temp.Name[1] = 0x20;
        temp.Name[2] = 0x20;
        temp.Name[3] = 0x20;
        temp.Name[4] = 0x20;
        temp.Name[5] = 0x20;
        temp.Name[6] = 0x20;
        temp.Name[7] = 0x20;
        temp.Name[8] = 0x20;
        temp.Name[9] = 0x20;
        temp.Name[10] = 0x20;
        
        AddFDT(Temp1, &temp);		// 增加文件名为'.'的目录项，指向当前目录

        /* 建立'..'目录 */
        temp.Name[1] = '.';					// 增加文件名为'..'的目录项，指向上一级目录
        
        temp.FstClusLO = ClusIndex & 0xffff;
        temp.FstClusHI = ClusIndex >> 16;
        Rt = AddFDT(Temp1, &temp);
    }
    else
    {
        FATDelClusChain(Temp1);		// 如果目录建立失败，释放刚获得的簇链
    }
    
    return Rt;
}

/*********************************************************************************************************
** 函数名称: RemoveDir
** 功能描述: 删除目录
**
** 输　入: Path:路径名
**
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: DelFDT
**
********************************************************************************************************/
Uint8 RemoveDir(Int8 *Path)
{
    Uint32 ClusIndex, ClusIndex1;
    Uint8 Rt;
    Int8 DirName[12];
    FDT temp;
    
    Path = FsStrCopy(Path);

    ClusIndex = _GetFDTInfo(DirName, Path);
    if (ClusIndex == BAD_CLUS)
    {
        return PATH_NOT_FIND;
    }

    /* 获取FDT其信息 */
    Rt = FindFDTInfo(&temp, ClusIndex, DirName);
    if (Rt == RETURN_OK)
    {
        /* 是否是目录 */
        if ((temp.Attr & ATTR_DIRECTORY) != 0)
        {
            /* 是 */
            ClusIndex1 = temp.FstClusLO + ((Uint32)temp.FstClusHI << 16);
            /* 是否是空目录 */
            Rt = DirIsEmpty(ClusIndex1);
            if (Rt == DIR_EMPTY)
            {
                /* 是，删除 */
                FATDelClusChain(ClusIndex1);
                Rt = DelFDT(ClusIndex, DirName);
            }
        }
        else
        {
            return PATH_NOT_FIND;
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: ChangeDir
** 功能描述: 改变当前目录
**
** 输　入: Path:路径名
**
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDirClusIndex
**
********************************************************************************************************/
Uint8 ChangeDir(Int8 *Path)
{
    Uint32 ClusIndex;
    Uint8 Rt;

    Rt = PATH_NOT_FIND;
    ClusIndex = GetDirClusIndex(Path);
    if (ClusIndex != BAD_CLUS)
    {
        DiskInfo.PathClusIndex = ClusIndex;
        Rt = RETURN_OK;
    }
    return Rt;
}

