#include "ezdsp5535_sdcard.h"
#include "ezdsp5535_uart.h"
#include "g722record.h"
#include "ff.h"
#include "libg722_1.h"
#include "fileops.h"

Int8 fileName[MAX_FILELENGTH] = "20130501/121020lr.raw";

extern Uint8 mix_lr[];

FATFS *fs = NULL;
FIL file;

recordStruct recordFile;

void writeFileProcessing(int chn)
{
	f_write(&file, &mix_lr[2 * MAX_DCT_LENGTH * chn], recordFile.framesize);

	if(--recordFile.frames == 0) {
		f_close(&file);
		recordFile.function = DIRECT_IO;	// make start_record() active
		start_record();
		recordFile.frames = recordFile.split;
	}
}

void readFileProcessing(int chn)
{
	int nsize;

	nsize = f_read(&file,
				   &mix_lr[2 * MAX_DCT_LENGTH * chn],
				   recordFile.framesize);
	if(nsize < recordFile.framesize) {
		f_close(&file);
		recordFile.function = DIRECT_IO;
	}
}

void File_init(void)
{
	recordFile.curFileName = fileName;
	recordFile.split = 3000;		/* file split size 3000frames,        */
									/* 20ms/frame correspond to 1 minute  */
	recordFile.frames = 3000;		/* reload from split and decrements   */
	recordFile.function = DIRECT_IO;

	fs = f_mount(0);
	EZDSP5535_SDCARD_init();

#if	FILE_DMAMODE == 1
	DMA_sdcard_init();
#endif
}

