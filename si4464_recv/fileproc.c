/****************************************************************************
 * File name: fileproc.c
 *                                                                          
 * Description:  file syetem READ/ WRITE
 *
 ***************************************************************************/

/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535_lcd.h"
#include "ezdsp5535_sar.h"
#include "si4464recv.h"
#include "fat.h"

extern volatile Uint16 taskList;

extern Int16 rawOutWords[];
extern Uint16 number_of_16bit_words_per_frame;

HANDLE FHandle = Not_Open_FILE;

void fileProcessing(void)
{
	Int16  key;

	if((taskList & FILEPROCESSING) == 0)	return;

//	key = EZDSP5535_SAR_getKey();
	key = -1;
	if(key == SW1)
	if(FHandle == Not_Open_FILE) {
		LCD_print("RECORDING......", 0);
		FHandle = FileOpen("A:\\VOICE000.TXT", FILE_FLAGS_WRITE);
	}

	if(key == SW2)
	if(FHandle != Not_Open_FILE) {
		LCD_print("SAVE VOICE000.DAT", 0); 
		FileClose(FHandle);
		AllCacheWriteBack();
		FHandle = Not_Open_FILE;
	}

//	if (FHandle != Not_Open_FILE)
//		FileWrite(&RfByte[2], number_of_bytes_per_frame, FHandle);

	taskList &= ~FILEPROCESSING;
}

