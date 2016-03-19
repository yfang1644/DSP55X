/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file  R0.06   (C)ChaN, 2007
/-----------------------------------------------------------------------*/

#ifndef _DISKIO

#define _READONLY	0	/* 1: Read-only mode */
#define _USE_IOCTL	1

struct tm {
	int tm_sec;		/* seconds */
	int tm_min;		/* minutes */
	int tm_hour;	/* hours */
	int tm_mday;	/* day of the month */
	int tm_mon;		/* month */
	int tm_year;	/* year */
	int tm_wday;	/* day of the week */
	int tm_yday;	/* day in the year */
	int tm_isdst;	/* daylight saving time */
};


/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

Uint16 disk_initialize (Uint16 drv);
Uint16 disk_status (Uint16 drv);
DRESULT disk_read (
	Uint16 drv,
	Uint8 *buff,
	Uint32 sector
);

#if	_READONLY == 0
DRESULT disk_write(
	Uint16 drv,
	const Uint8 *buff,
	Uint32 sector
);
#endif
DRESULT disk_ioctl(
	Uint16 drv,
	Uint16 ctrl,
	void *buff
);
void	disk_timerproc (void);


/* Disk Status Bits (BYTE) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl() */

/* Generic command */
#define CTRL_SYNC			0	/* Mandatory for read/write configuration */
#define GET_SECTOR_COUNT	1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE		2
#define GET_BLOCK_SIZE		3	/* Mandatory for only f_mkfs() */
#define CTRL_POWER			4
#define CTRL_LOCK			5
#define CTRL_EJECT			6
/* MMC/SDC command */
#define MMC_GET_TYPE		10
#define MMC_GET_CSD			11
#define MMC_GET_CID			12
#define MMC_GET_OCR			13
#define MMC_GET_SDSTAT		14
/* ATA/CF command */
#define ATA_GET_REV			20
#define ATA_GET_MODEL		21
#define ATA_GET_SN			22


#define	FILE_DMAMODE	1
#undef	FILE_DMAMODE

void DSP_byte2word(void *dst, const void *src, Int16 cnt);
void DSP_word2byte(void *dst, const void *src, Int16 cnt);

#define _DISKIO
#endif

