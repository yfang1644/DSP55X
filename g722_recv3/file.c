/***************************************************************************
* Filename: file.c
**************************************************************************/

#define IN_FILE

#include "tistdtypes.h"
#include "fat.h"

MY_FILE FileInfo[MAX_OPEN_FILES];        /* 打开文件信息表 */
extern Disk_Info DiskInfo;

Uint32 _GetFDTInfo(Int8 *FDTName, Int8 *DirFileName);
Int8 *FsStrCopy(Int8 *source);
Uint8 FS_GetDateTime(SYS_TIME *CurTime);

/*********************************************************************************************************
** 函数名称: FileInit
** 功能描述: 初始化文件指针系统
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
void FileInit(void)
{
    HANDLE i;
    
    for (i = 0; i < MAX_OPEN_FILES; i++)
    {
        FileInfo[i].Flags = 0;
    }
}

/*********************************************************************************************************
** 函数名称: FindOpenFile
** 功能描述: 查找已打开的指定文件的文件句柄，内部使用
**
** 输　入:DirClus:文件所在目录的开始簇号
**        FileName:内部文件名
** 输　出: 文件句柄，Not_Open_FILE为没有打开
**         
** 全局变量: FileInfo
** 调用模块:
**
********************************************************************************************************/
HANDLE FindOpenFile(Uint32 DirClus, Int8 *FileName, Uint16 type)
{
    HANDLE Rt;
    MY_FILE *fp;
    
    fp = FileInfo;
    for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
    {
        if (fp->Flags & type)
        if (fp->DirClus == DirClus)
        if (fp->Name[0] == FileName[0])
        if (fp->Name[1] == FileName[1])
        if (fp->Name[2] == FileName[2])
        if (fp->Name[3] == FileName[3])
        if (fp->Name[4] == FileName[4])
        if (fp->Name[5] == FileName[5])
        if (fp->Name[6] == FileName[6])
        if (fp->Name[7] == FileName[7])
        if (fp->Name[8] == FileName[8])
        if (fp->Name[9] == FileName[9])
        if (fp->Name[10] == FileName[10])
        {
            break;
        }
        fp++;
    }
    if (Rt >= MAX_OPEN_FILES)
    {
        Rt = Not_Open_FILE;
    }
    return Rt;
}

/******************************************************************************
** 函数名称: RemoveFile
** 功能描述: 删除文件
**
** 输　入: DirFileName:用户使用的文件名
**
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: _GetFDTInfo,FindFDTInfo,FATDelClusChain,DelFDT
**
*****************************************************************************/
Uint8 RemoveFile(Int8 *DirFileName)
{
    Uint32 ClusIndex, ClusIndex1;
    Uint8 Rt;
    Int8 DirName[12];
    FDT temp;

    DirFileName = FsStrCopy(DirFileName);
    ClusIndex = _GetFDTInfo(DirName, DirFileName);
    Rt = PATH_NOT_FIND;
    if (ClusIndex != BAD_CLUS)
    {
        Rt = FindFDTInfo(&temp, ClusIndex, DirName);
        if (Rt == RETURN_OK)
        {
            Rt = NOT_FIND_FILE;
            if ((temp.Attr & ATTR_DIRECTORY) == 0)  /* 是文件才删除 */
            {
                Rt = FILE_LOCK;
                if (FindOpenFile(ClusIndex, DirName,
						FILE_FLAGS_READ|FILE_FLAGS_WRITE) == Not_Open_FILE)
                {
                    /* 文件没有打开才删除 */
                    ClusIndex1 = temp.FstClusLO + ((Uint32)temp.FstClusHI << 16);
                    FATDelClusChain(ClusIndex1);
                    Rt = DelFDT(ClusIndex, DirName);
                }
            }
        }
    }
    return Rt;
}

/******************************************************************************
** 函数名称: _FileOpenR
** 功能描述: 只读方式打开文件，内部使用
**
** 输　入: DirFileName:用户使用的文件名
**
** 输　出: 文件句柄，Not_Open_FILE为不能打开
**         
** 全局变量: FileInfo
** 调用模块: _GetFDTInfo,FindFDTInfo
**
*****************************************************************************/
HANDLE _FileOpenR(Int8 *DirFileName)
{
    MY_FILE *fp;
    HANDLE Rt;
    FDT FileFDT;
    
    /* 查找空闲文件登记项 */
    for (Rt = 0; FileInfo[Rt].Flags; Rt++)
    {
		if(Rt >= MAX_OPEN_FILES)
            return Not_Open_FILE;
    }

    fp = FileInfo + Rt;
    
    /* 获取目录开始簇号和文件名 */
    fp->DirClus = _GetFDTInfo(fp->Name, DirFileName);
    if (fp->DirClus < BAD_CLUS)
    {
        /* 获取文件信息 */
        if (FindFDTInfo(&FileFDT, fp->DirClus, fp->Name) == RETURN_OK)
        {
            if ((FileFDT.Attr & ATTR_DIRECTORY) == 0)
            {
                fp->Flags = FILE_FLAGS_READ;
                fp->FileSize = FileFDT.FileSize;
                fp->FstClus = FileFDT.FstClusLO | (Uint32)FileFDT.FstClusHI << 16;
                fp->Clus = fp->FstClus;
                fp->Offset = 0;
                return Rt;
            }
        }
    }
    return Not_Open_FILE;
}

/******************************************************************************
** 函数名称: _FileOpenW
** 功能描述: 只写方式打开文件，内部使用
**
** 输　入: DirFileName:用户使用的文件名
**
** 输　出: 文件句柄，Not_Open_FILE为不能打开
**         
** 全局变量: FileInfo
** 调用模块: _GetFDTInfo,FindFDTInfo
**
*****************************************************************************/
HANDLE _FileOpenW(Int8 *DirFileName)
{
    MY_FILE *fp;
    FDT temp;
    HANDLE Rt;
    Uint8 i;
    SYS_TIME CurTime,CrtTime;	// 保存当前时间日期

    /* 查找空闲文件登记项 */
    for (Rt = 0; FileInfo[Rt].Flags; Rt++)
    {
		if(Rt >= MAX_OPEN_FILES)
            return Not_Open_FILE;
    }
    
    fp = FileInfo + Rt;					// 指向获得的空闲登记项
    
    /* 获取目录开始簇号和文件名 */
    fp->DirClus = _GetFDTInfo(fp->Name, DirFileName);	// 获取该文件的FDT项的首簇号，及文件名
    if (fp->DirClus < BAD_CLUS)
    {
        /* 文件已经以读写方式打开，不能再次以读写方式打开 */
        if (FindOpenFile(fp->DirClus, fp->Name,
					FILE_FLAGS_WRITE) == Not_Open_FILE)
        {
            if(FS_GetDateTime(&CurTime) != RETURN_OK) // 获取当前系统时间
            {
            	return GET_TIME_ERR;
            }
            
            if (FindFDTInfo(&temp, fp->DirClus, fp->Name) == RETURN_OK)
            {
                if ((temp.Attr & ATTR_DIRECTORY) != 0)
                {
                   return Not_Open_FILE;		// 如果该文件为目录，则不能删除
                }
                
                CrtTime.date = temp.CrtDate;    // 保存文件建立时间
                CrtTime.time = temp.CrtTime;
                CrtTime.msec = temp.CrtTimeTenth;
                
                if (RemoveFile(DirFileName) != RETURN_OK)   /* 删除文件 */
                {
                   return Not_Open_FILE;
                }
            }
            else
            {
                CrtTime = CurTime;              // 设置文件建立时间为当前时间
            }                
            
            /* 创建文件 */
            for (i = 0; i < 11; i++)			// 创建将要生成文件的FDT项
            {
                temp.Name[i] = fp->Name[i];		// 复制名字
            }
            temp.Attr = 0;						// 特性为0
            temp.FileSize = 0;

            temp.NTRes = 0;
            
            /*-------在此加入创建时间的程序---------*/
            temp.CrtTimeTenth = CrtTime.msec;	
            temp.CrtTime = CrtTime.time;
            temp.CrtDate = CrtTime.date;
            
            temp.LstAccDate = CurTime.date;
            temp.WrtTime = CurTime.time + 1;
            temp.WrtDate = CurTime.date;
			/*--------------------------------------*/
			
            temp.FstClusLO = 0;
            temp.FstClusHI = 0;
            if (AddFDT(fp->DirClus, &temp) == RETURN_OK)       /* 增加文件 */
            {
                /* 设置文件信息 */
                fp->Flags = FILE_FLAGS_WRITE;
                fp->FileSize = 0;
                fp->FstClus = 0;
                fp->Clus = 0;
                fp->Offset = 0;
                return Rt;
            } // if(Add..)
        } // if(Find..)
    } // if(fp..)
    return Not_Open_FILE;
}

/*********************************************************************************************************
** 函数名称: _FileOpenRW
** 功能描述: 只读写方式打开文件，内部使用
**
** 输　入: DirFileName:用户使用的文件名
**
** 输　出: 文件句柄，Not_Open_FILE为不能打开
**         
** 全局变量: 无
** 调用模块: _FileOpenR,_FileOpenW
**
********************************************************************************************************/
HANDLE _FileOpenRW(char *DirFileName)
{
    HANDLE Rt;
    
    Rt = _FileOpenR(DirFileName);
    if (Rt == Not_Open_FILE)
    {
        Rt = _FileOpenW(DirFileName);
    }
    else
    {
        if (FindOpenFile(FileInfo[Rt].DirClus,
					FileInfo[Rt].Name, FILE_FLAGS_WRITE) == Not_Open_FILE)
        {
            FileInfo[Rt].Flags = (FILE_FLAGS_WRITE | FILE_FLAGS_READ);
        }
        else
        {
            FileInfo[Rt].Flags = 0;
            Rt = Not_Open_FILE;
        }
    }
    return Rt;
}

/******************************************************************************
** 函数名称: FileOpen
** 功能描述: 以指定方式打开文件
**
** 输　入: DirFileName:用户使用的文件名
**        Type:打开方式
** 输　出: 文件句柄，Not_Open_FILE为不能打开
**         
** 全局变量: 无
** 调用模块: _FileOpenR,_FileOpenW,_FileOpenRW
**
*****************************************************************************/
HANDLE FileOpen(Int8 *DirFileName, Uint16 type)
{
	DirFileName = FsStrCopy(DirFileName);

	if (type == (FILE_FLAGS_READ|FILE_FLAGS_WRITE))
        return _FileOpenRW(DirFileName);
	else if (type == FILE_FLAGS_WRITE)
        return _FileOpenW(DirFileName);
	else if(type == FILE_FLAGS_READ)
        return _FileOpenR(DirFileName);
	else
		return Not_Open_FILE;
}

/******************************************************************************
** 函数名称: FileClose
** 功能描述: 关闭指定文件
**
** 输　入: Handle:文件句柄
**
** 输　出: RETURN_OK:成功
**        其它参考fat.h中关于返回值的说明 
** 全局变量: 无
** 调用模块: 无
**
** 说  明: 增加文件的最后访问时间属性和最后修改时间属性
**
** 流  程: 判断文件是否有写状态-Y->取出该文件FDT项-->判断文件大小是否变化-Y->更新FDT中首簇号-->保存FDT项
** 注  意: 关闭文件后，将释放其登记项，并把修改过的扇区回写保存，但其仍占用原来的cache。
****************************************************************************/
Uint8 FileClose(HANDLE Handle)
{
    Uint8 Rt;
    FDT FileFDT;
    MY_FILE *fp;
    SYS_TIME CurTime;
    
    Rt = PARAMETER_ERR;
    if (Handle < MAX_OPEN_FILES)
    {
        if(FS_GetDateTime(&CurTime) != RETURN_OK)       // 获取当前时间
        {
            return GET_TIME_ERR;
        }
                                                        
        fp = FileInfo + Handle;		                    // 获取该文件的FDT项
        Rt = FindFDTInfo(&FileFDT, fp->DirClus, fp->Name);
        if (Rt != RETURN_OK)
        {
            return Rt;
        }
        
        FileFDT.LstAccDate = CurTime.date;              // 更新文件最后访问时间
                                                        // 如果文件不是以只写方式打开就不需要保存
        if (fp->Flags & FILE_FLAGS_WRITE)
        {
            if (FileFDT.FileSize < fp->FileSize)		// 查看文件大小是否发生变化
            {
                FileFDT.FileSize = fp->FileSize;	
                                                        // 更新文件最后修改时间
                FileFDT.WrtTime = CurTime.time + 1;     // 修改时间需要加2秒，因为它不能计算到毫秒戳
                FileFDT.WrtDate = CurTime.date;
                
                if (FileFDT.FstClusLO == 0)
                if (FileFDT.FstClusHI == 0)
                {
                    FileFDT.FstClusLO = fp->FstClus & 0xffff;
                    FileFDT.FstClusHI = fp->FstClus >> 16;
                }
            }
        }
        ChangeFDT(fp->DirClus, &FileFDT);	// 将修改后的FDT项保存
        fp->Flags = 0;
    }
    return Rt;
}


/******************************************************************************
** 函数名称: FileRead
** 功能描述: 读取文件
**
** 输　入: Buf:保存读回的数据
**        Size:要读的字节数
**        Handle:文件句柄
** 输　出: 实际读到的字节数
**         
** 全局变量: 无
** 调用模块: 无
**
*****************************************************************************/
Uint32 FileRead(void *Buf, Uint32 Size, HANDLE Handle)
{
    Uint32 i, j, k, SecIndex;
    MY_FILE *fp;
    Uint32 Rt;
    Uint8 *Buf1, *Buf2;
	Int16 ptr;
    
    Rt = 0;
    fp = FileInfo + Handle;
    Buf2 = (Uint8 *)Buf;
    if (Handle < MAX_OPEN_FILES)     /* Handle是否有效 */
    if (fp->Flags)                   /* 对应的打开文件信息表是否已使用 */
    {
        if (fp->Offset + Size >= fp->FileSize)
        {
            /* 如果读数据超出文件的长度，看一看是否有别的程序正在写这个文件 */
            /* 如果有，则可能文件的长度有变化，更正之*/
            Handle = FindOpenFile(fp->DirClus, fp->Name, FILE_FLAGS_WRITE);
            if (Handle < MAX_OPEN_FILES)
            {
                fp->FileSize = FileInfo[Handle].FileSize;
                if (fp->Offset == 0)
                {
                    fp->FstClus = FileInfo[Handle].FstClus;
                    fp->Clus = fp->FstClus;
                }
            }
        }

        while (Size != 0)
        {
            if (fp->Offset >= fp->FileSize)              /* 是否到文件结束 */
                break;

            /* 计算数据所在扇区 */
            j = fp->Offset % (DiskInfo.SecPerClus * DiskInfo.BytsPerSec);
            i = j / DiskInfo.BytsPerSec;
            j = j % DiskInfo.BytsPerSec;
            SecIndex = (fp->Clus - 2) * DiskInfo.SecPerClus + 
                       DiskInfo.DataStartSec + i;
            /* 打开扇区 */
            Buf1 = OpenSec(SecIndex);
            if (Buf1 == NULL)
                break;

            /* 读取扇区数据 */
            if (ReadSec(SecIndex, CACHE_DATA) != RETURN_OK)
                break;

            /* 读取数据 */
            k = DiskInfo.BytsPerSec - j;
            if (Size < k)
                k = Size;

            if ((fp->Offset + k) > fp->FileSize)
                k = fp->FileSize - fp->Offset;

            Size -= k;
            Rt += k;
            fp->Offset += k;

			for(ptr = 0; ptr < (k); ptr++)
				*Buf2++ = Buf1[j + ptr];

            /* 关闭扇区 */
            j += k;    
            if (j >= DiskInfo.BytsPerSec)
            {
                i++;
                if (i >= DiskInfo.SecPerClus)
                    fp->Clus = FATGetNextClus(fp->Clus);
            }
        }
    }
    return Rt;
}


/*********************************************************************************************************
** 函数名称: FileWrite
** 功能描述: 写文件
**
** 输　入: Buf:要写的数据
**        Size:要写的字节数
**        Handle:文件句柄
** 输　出: 实际写的字节数
**         
** 全局变量: 无
** 调用模块: 无
**
********************************************************************************************************/
Uint32 FileWrite(void *Buf, Uint32 Size, HANDLE Handle)
{
    Uint32 i, j, k, SecIndex;
    MY_FILE *fp;
    Uint32 Rt;
    Uint8 *Buf1, *Buf2;
	Int16 ptr;

    Rt = 0;
    fp = FileInfo + Handle;             // 获得登记项句柄
    Buf2 = (Uint8 *)Buf;                // 数据源
    if (Handle < MAX_OPEN_FILES)        /* Handle是否有效 */
    if (fp->Flags & FILE_FLAGS_WRITE)   /* 文件是否可写 */
    {
        if (fp->Offset > fp->FileSize)  // 偏移超出文件范围，返回
        {
            return 0;
        }

        while (Size != 0)
        {
            /* 计算数据所在扇区 */
            j = fp->Offset % (DiskInfo.SecPerClus * DiskInfo.BytsPerSec);   // 字节偏移量(簇内)
            
            if (j == 0)
            if (fp->Offset == fp->FileSize)  
            {           // 如果指向簇的最后一字节，并且偏移量等于文件大小，即指向文件最后一个字节
                i = FATAddClus(fp->Clus);  // 则需要增加新簇
                if (i >= BAD_CLUS)
                {
                    return DISK_FULL;
                }
                fp->Clus = i;
                if (fp->FstClus == EMPTY_CLUS)
                {
                    fp->FstClus = i;
                }
            }

            i = j / DiskInfo.BytsPerSec;   // 扇区偏移(簇内)
            j = j % DiskInfo.BytsPerSec;   // 字节偏移(扇区内)
            SecIndex = (fp->Clus - 2) * DiskInfo.SecPerClus + 
                       DiskInfo.DataStartSec + i;

            k = DiskInfo.BytsPerSec - j;   // 扇区剩余字节
            if (Size < k)                   // 如果该扇区的空闲字节大于要保存的文件大小
            {
                k = Size;
            }

            /* 打开扇区 */
            Buf1 = OpenSec(SecIndex);
            if (Buf1 == NULL)
            {
                break;
            }
            /* 读取扇区数据 */
            if (fp->Flags & FILE_FLAGS_READ)
            if (ReadSec(SecIndex, CACHE_DATA) != RETURN_OK)
            {
                break;
            }

            // 数据复制，从数据源Buf2复制k字节数据到Buf1+j
            // 数据源指针加k
			for(ptr = 0; ptr < (k); ptr++)
				Buf1[j + ptr] = *Buf2++;

            /* 关闭扇区 */
            WriteSec(SecIndex, CACHE_DATA);  // 设置被修改标记

            Size -= k;                      // 剩余待写入数据量减k
            Rt += k;
            /* 调整文件指针 */
            fp->Offset += k;                // 写入点指针偏移位置+k
            if (fp->Offset > fp->FileSize)  // 如果文件增大，更新文件大小值
            {
                fp->FileSize = fp->Offset;
            }

            if ((j + k) >= DiskInfo.BytsPerSec)
            {
                if ((i + 1) >= DiskInfo.SecPerClus)
                if (fp->Offset < fp->FileSize)
                {
                    fp->Clus = FATGetNextClus(fp->Clus);
                }
            }
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: FileCloseAll
** 功能描述: 关闭所有打开的文件
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: FileInfo
** 调用模块: AllCacheWriteBack
**
********************************************************************************************************/
void FileCloseAll(void)
{
    Uint32 i;

    for (i = 0; i < MAX_OPEN_FILES; i++)
    {
        FileClose(i);
    }
    AllCacheWriteBack();
}


/******************************************************************************
** 函数名称: FileSeek
** 功能描述: 移动文件读\写位置
**
** 输　入: Handle:文件句柄
**        offset:移动偏移量
**        Whence:移动模式SEEK_SET:从文件头计算SEEK_CUR:从当前位置计算SEEK_END:从文件尾计算
** 输　出: 无
**         
** 全局变量: 无
** 调用模块: 无
**
*****************************************************************************/
Uint8 FileSeek(HANDLE Handle, Int32 offset, Uint8 Whence)
{
    Uint8 Rt;
    Uint32 i, Clus;
    MY_FILE *fp;
    
    Rt = PARAMETER_ERR;
    fp = FileInfo + Handle;
    if (Handle < MAX_OPEN_FILES)                    /* Handle是否有效 */
    if (fp->Flags)                               /* 对应的打开文件信息表是否已使用 */
    {
        Rt = RETURN_OK;
        switch (Whence)
        {
            case SEEK_END:             /* 从文件尾计算 */
                fp->Offset = fp->FileSize - offset;
                offset = -offset;
                break;
            case SEEK_SET:
                fp->Offset = offset;
                break;
            case SEEK_CUR:             /* 从当前位置计算 这里不用break是正确的*/
                i = fp->Offset + offset;
                break;
            default:
                Rt = PARAMETER_ERR;
                break;
        }
        /* 改变当前簇号 */
        if (Rt == RETURN_OK)
        {
            if (fp->Offset > fp->FileSize)
            {
                if (offset > 0)
                    fp->Offset = fp->FileSize;
                else
                    fp->Offset = 0;
            }
            Rt = RETURN_OK;
            i = fp->Offset / (DiskInfo.BytsPerSec * DiskInfo.SecPerClus);
            Clus = fp->FstClus;
            for (; i != 0; i--)
            {
                Clus = FATGetNextClus(Clus);
                if (Clus >= BAD_CLUS)
                {
                    Rt = FAT_ERR;
                    break;
                }
            }
            fp->Clus = Clus;
        }
    }
    return Rt;
}

