/*************************************************************************
**
**   Filename:     main.c    
**
**   ITU-T G.722.2 (2011-11) - Fixed point implementation @DSP-C5509
**
**   mode   :  (0)  (1)   (2)   (3)   (4)   (5)   (6)   (7)   (8)
**   bitrate: 6.60 8.85 12.65 14.25 15.85 18.25 19.85 23.05 23.85 kbps
**
**
*************************************************************************/

/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535_gpio.h"
#include "ezdsp5535_i2c.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_sar.h"
#include "ezdsp5535_spi.h"
#include "csl_spi.h"
#include "csl_mmcsd.h"
#include "fat.h"
#include "Si446x_api.h"

#include "defs.h"
#include "cnst.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)

volatile Uint16 taskList = 0;
volatile Int16 gpioIsrStatus = 0;
volatile Int16 spiIsrStatus = 0;
volatile Int16 timIsrStatus = 0;

Int16 si4464Func = RECV;

extern int spi_set;
int mark = 0;

extern Int16 stream[];
/*
 *
 *  main( )
 *
 */

void main(void)
{
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
	EZDSP5535_PLL_init(200000000);		// system clock set to 100MHz
	TIMER_init(25000000);
	EZDSP5535_GPIO_init();
	EZDSP5535_I2C_init();		/* Initialize I2C */

	Audio_init();

	EZDSP5535_LCD_init();
	LCD_print("G722.2 Recv", 0);
	LCD_print("16kbps", 1);
	EZDSP5535_SPI_init();
	Si446x_Init();
	EZDSP5535_SAR_init();
	DiskInit();
	FileInit();
	AddFileDriver(SDCammand, NULL);

	asm("	bit(ST1, #ST1_INTM) = #0");

	while(1) {
		if(spi_set) {
			if(mark)    asm("   bit(ST1, #ST1_XF) = #1");
			else        asm("   bit(ST1, #ST1_XF) = #0");
			spi_set = 0;
			mark = 1 - mark;
        }

		switch(si4464Func) {
		case SEND:
			if(gpioIsrStatus) {
				if(taskList & AUDIOPROCESSING) {
					audioEncoding(LEFT);
					taskList &= ~AUDIOPROCESSING;
					gpioIsrStatus = 0;
				}
		    }
			break;
		case RECV:
			if(gpioIsrStatus) {
   			 	gpioIsrStatus = 0;
				audioDecoding();		// decode last frame
				SPI_recvData(stream);
		    }
			break;
		default:
			break;
		}

		asm("	idle");
	}

//	RemoveFileDriver(SDCammand);
}

