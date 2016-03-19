/****************************************************************************
 * File name: fileproc.c
 *                                                                          
 * Description:  file syetem READ/ WRITE
 *
 ***************************************************************************/

/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "defs.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_sar.h"
#include "fat.h"

extern volatile Uint16 taskList;

extern Uint8  RfByte[];
extern Uint16 number_of_bytes_per_frame;

void fileProcessing(void)
{
	static HANDLE FHandle = Not_Open_FILE;
	Int16  key;

	if((taskList & FILEPROCESSING) == 0)	return;

//	key = EZDSP5535_SAR_getKey();
	key = -1;
	if(key == SW1)
	if(FHandle == Not_Open_FILE) {
		LCD_print("RECORDING......", 15, 0);
		FHandle = FileOpen("A:\\VOICE000.TXT", FILE_FLAGS_WRITE);
	}

	if(key == SW2)
	if(FHandle != Not_Open_FILE) {
		LCD_print("SAVE VOICE000.DAT", 17, 0); 
		FileClose(FHandle);
		AllCacheWriteBack();
		FHandle = Not_Open_FILE;
	}

	if (FHandle != Not_Open_FILE)
		FileWrite(&RfByte[2], number_of_bytes_per_frame, FHandle);

	taskList &= ~FILEPROCESSING;
}

