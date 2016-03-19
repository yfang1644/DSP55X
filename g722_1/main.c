//////////////////////////////////////////////////////////////////////////////
// * File name: main.c
// *                                                                          
// * Description:  Main function.
// *                                                                          
/*************************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**

 Description:  Contains the main function for the G.722.1 Annex C encoder

 Design Notes:
 
 WMOPS:     7kHz |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    2.32      |     2.43
          -------|--------------|----------------  
            MAX  |    2.59      |     2.67
          -------|--------------|---------------- 
                                                                        
           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    4.45      |     4.78       |     5.07   
          -------|--------------|----------------|----------------
            MAX  |    5.07      |     5.37       |     5.59   
          -------|--------------|----------------|----------------

*************************************************************************/

/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535.h"
#include "ezdsp5535_gpio.h"
#include "ezdsp5535_i2c.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_sar.h"
#include "csl_mmcsd.h"
#include "g722_1.h"
#include "libg722_1.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)

//volatile Int16 timer = 0;              // timer interrupt
volatile Uint16 t_cnt = 0;             // timer interrupt counter
volatile Uint16 s_cnt = 0;             // DMA interrupt counter

/*
 *
 *  main( )
 *
 */

/***************************************************************************
 Function:     G.722.1 Annex C main encoder function 

            bit rate    16, 24, 32, and 48 kbps
            bandwidth   7 or 14 kHz (defined in codec.h)
                        
 Description:  Main processing loop for the G.722.1 Annex C encoder/decoder

 Design Notes: 16kbit/sec is only supported for bandwidth of 7kHz

****************************************************************************/
void main(void)
{
	HANDLE FHandle = Not_Open_FILE;
	Int16 key;

	ICR = 0xff2e;	/* IDLE Control Register */
					/* mem port, io port and cpu idle active */

	/* Peripheral_Reset  */
	CSL_SYSCTRL_REGS->PSRCR = 0x0020;

	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;
	CSL_SYSCTRL_REGS->PRCR = 0x00bf;

/* VERY IMPORTANT !!! */
/* PPMODE1 (SPI, GPIO, UART, and I2S2):
	7 signals of the SPI module,
	6 GPIO signals(GP[17:12]),
	4 signals of the UART module,
	4 signals of the I2S2 module

   SP1MODE2 (GP[11:6]). 6 GPIO signals (GP[11:6])
   SP0MODE0 (MMC/SD0). All 6 signals of the MMC/SD0
*/
	CSL_SYSCTRL_REGS->EBSR = (CSL_SYS_EBSR_PPMODE_MODE1 << 12)
							|(CSL_SYS_EBSR_SP1MODE_MODE2 << 10)
							|(CSL_SYS_EBSR_SP0MODE_MODE0 << 8);

	asm("	bit(ST1, #ST1_INTM) = #1");

	INTR_init();
	PLL_init(40000000);	/* system clock set to  50MHz */

	TIMER_init();
	EZDSP5535_GPIO_init();

	EZDSP5535_I2C_init();		/* Initialize I2C */
	Audio_init(7);
	EZDSP5535_LCD_init();
	LCD_print("G722.1 TEST       ", 17, 0);
	LCD_print("16kbps  ", 8, 1);
	LCD_scroll();
	EZDSP5535_SAR_init();

	DiskInit();
	FileInit();

	AddFileDriver(SDCammand, NULL);

	asm("	bit(ST1, #ST1_INTM) = #0");

	while(1) {
		audioProcessing(FHandle);

		key = EZDSP5535_SAR_getKey();
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

		asm("	idle");
	}

//	RemoveFileDriver(SDCammand);
}
