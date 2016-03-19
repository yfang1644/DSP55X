#include "soc.h"
#include "diyrecord.h"
#include "ff.h"
#include "fileops.h"
#include "libg722_1.h"
#include "ezdsp5535_sdcard.h"

Int8 fileName[64] = "20130501/121020lr.raw";

extern Uint16 taskList;
extern Uint8 mix_lr[];
extern Int16 *wrFilePtrl, *wrFilePtrr;

Uint16 start_record(void);

FATFS *fs = NULL;
FIL file;

recordStruct recordFile;

void fileProcessing(void)
{
	int nsize;
	Uint16 chn;

	if((taskList & FILEPROCESSING) == 0)	return;

	switch(recordFile.function) {
	case FILE_RECORD_RAW:
	case FILE_RECORD_COD1:
		if(recordFile.channel & (1 << LEFT))		// left
			f_write(&file, wrFilePtrl, recordFile.framesize);
		if(recordFile.channel & (1 << RIGHT))		// right
			f_write(&file, wrFilePtrr, recordFile.framesize);

		if(--recordFile.frames <= 0) {
			f_close(&file);
			recordFile.function = DIRECT_IO;
			start_record();
			recordFile.frames = recordFile.split;
		}
		break;

	case FILE_REPLAY_RAW:
	case FILE_REPLAY_COD1:
		for(chn = 0; chn < STEREO; chn++) {
			if(recordFile.channel & (1 << chn))	
				nsize = f_read(&file,
							   &mix_lr[2 * MAX_DCT_LENGTH * chn],
							   recordFile.framesize);
			if(nsize < recordFile.framesize) {
				f_close(&file);
				recordFile.function = DIRECT_IO;
			}
		}
		break;

	default:

		break;
	}

	taskList &= ~FILEPROCESSING;
}

void File_init(void)
{
	recordFile.curFileName = fileName;
	recordFile.channel = 3;
	recordFile.code = 0x0116;
	recordFile.framesize = 40;		// 16kbps, 20ms/frame, 40bytes/frame
#if	BANDWIDTH == 14
	recordFile.sampleRate = 3;	/* fs = 32kHz */
#else
	recordFile.sampleRate = 6;	/* fs = 16kHz */
#endif
	recordFile.split = 3000;
	recordFile.frames = 3000;
	recordFile.function = DIRECT_IO;

	fs = f_mount(0);
	EZDSP5535_SDCARD_init();
	DMA_sdcard_init();
	taskList &= ~FILEPROCESSING;
}

