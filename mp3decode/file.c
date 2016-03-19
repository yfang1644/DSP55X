#include "ezdsp5535_sdcard.h"
#include "defs.h"
#include "ff.h"


FATFS *fs = NULL;
FIL file;

int File_init(char *filename)
{
	int res = 0;
	fs = f_mount(0);
	EZDSP5535_SDCARD_init();

#if	FILE_DMAMODE == 1
	DMA_sdcard_init();
#endif

	res = f_open(&file, filename, FA_READ);
	return res;
}

