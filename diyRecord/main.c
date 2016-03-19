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
 
 WMOPS:
       ---------------------------------------------------------------
              |        7kHz         |              14kHz
              |---------------------+---------------------------------
              |  24kbit  |  32kbit  |  24kbit  |  32kbit  |  48kbit
       -------|----------+----------+----------|----------|-----------
         AVG  |  2.32    |  2.43    |  4.45    |  4.78    |  5.07   
       -------|----------+----------+----------|----------|-----------  
         MAX  |  2.59    |  2.67    |  5.07    |  5.37    |  5.59   
       --------------------------------------------------------------- 

*************************************************************************/

/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535_i2c.h"
#include "ezdsp5535_uart.h"
#include "ezdsp5535_lcd.h"
#include "rtcM41Txx_i2c.h"
#include "diyrecord.h"
#include "csl_mmcsd.h"
#include "fileops.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)

Uint16 taskList = 0;
/*
   bit2 -- uart processing
   bit1 -- file processing
   bit0 -- audio processing
*/

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

	EZDSP5535_PLL_init(160000000);	/* CPU clock = (multipler+4)*32768 */
	INTR_init();				/* set Interrupt vector and clean all IRs */
	TIMER_init();				/* Timer interrupt */
	EZDSP5535_I2C_init();		/* Initialize I2C */
	EZDSP5535_UART_init();		/* Initialize UART */
	Audio_init(7);				/* Initialise set to BandWidth=7kHz */
	File_init();
    EZDSP5535_LCD_init();		/* Initialise LCD */
	LCD_print("G722.1 recorder", 0);
	LCD_print("start", 1);
	asm("	bit(ST1, #ST1_INTM) = #0");

	while(1) {
		while(taskList) {
			audioProcessing();
			fileProcessing();
			uartProcessing();
		}
		asm("	idle");
	}
}

