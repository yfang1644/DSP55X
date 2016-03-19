//////////////////////////////////////////////////////////////////////////////
//  File name: main.c
//////////////////////////////////////////////////////////////////////////////

/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535_i2c.h"
#include "ezdsp5535_uart.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_gpio.h"
#include "defs.h"
#include "audio.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)

void clearBuffer();
void main(void)
{
	int status;

	ICR = 0xff2e;	/* IDLE Control Register */
					/* mem port, io port and cpu idle active */

	/* Peripheral_Reset  */
	CSL_SYSCTRL_REGS->PSRCR = 0x0020;
	/* Peripheral Clock Gating Configuration Register1
	   0: active, 1: disable
	   SYSCLKDIS I2S2 TMR2 TMR1   X    TMR0 I2S1 I2S0
	      MMCSD1 I2C  X    MMCSD0 DMA0 UART SPI  I2S3 */
	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	/* Peripheral Clock Gating Configuration Register2
	  X X X X X X X X X X ANAREG DMA3 DMA2 DMA1 USB SAR LCD */
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

	PLL_init(200000000);
	clearBuffer();
	INTR_init();				/* set Interrupt vector and clean all IRs */
	TIMER_init();				/* Timer interrupt */
	EZDSP5535_GPIO_init();		/* Enable GPIO LED */
	EZDSP5535_I2C_init();		/* Initialize I2C */
	status = File_init("song001.mp3");
	Audio_init();				/* Initialize audio */
	EZDSP5535_LCD_init();		/* Initialise LCD */
	if(status) {
		LCD_print("FILE OPEN ERR", 0);
		while(1)	;
	}
	LCD_print("mp3decode", 0);
	LCD_print("start", 1);

	asm("	bit(ST1, #ST1_INTM) = #0");

	audioProcessing();

	clearBuffer();
	LCD_print("finish", 1);

	asm("	bit(ST1, #ST1_INTM) = #1");
	while(1)  asm(" idle");
}

