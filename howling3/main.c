//////////////////////////////////////////////////////////////////////////////
// * File name: main.c
// *                                                                          
// * Description:  Main function.
// *                                                                          
/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535_i2c.h"
#include "ezdsp5535_gpio.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_sar.h"
#include "radio.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)

/*
 *
 *  main( )
 *
 */

volatile s16 timer = 0;				// timer interrupt
volatile u16 t_cnt = 0;				// timer interrupt counter
Int16 freqShift = 0;
extern Uint16 delta;

void displayFreq(int val)
{
	char *str = "    ";	
	str[0] = val + '0';
	LCD_print(str, 1);
}

void main()
{
	int key = 0;
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
	INTR_init();				/* set Interrupt vector and clean all IRs */
	PLL_init(60000000);				/* CPU clock = 30MHz */
	EZDSP5535_GPIO_init();
    DMA_audio_init(4*BUFFER);
	EZDSP5535_I2C_init();		/* Initialize I2C */
	EZDSP5535_LCD_init();
	aic3204_init(6, 60);		/* Initialize codec, sampling rate = 32kHz */
	EZDSP5535_I2S_init(0);		/* Initialize I2S */
	EZDSP5535_SAR_init();

	asm("	bit(ST1, #ST1_INTM) = #0");
	LCD_print("hello, dsp", 0);
	LCD_print("Hello, DSP", 1);
	while(1) {
		audioProcessing(freqShift);

		key = EZDSP5535_SAR_getKey();
		if(key == SW1) {
			EZDSP5535_waitusec(50000);
			EZDSP5535_waitusec(50000);
			delta++;
			if(delta > 9)	delta = 1;
			displayFreq(delta);
			freqShift = 1;
			CSL_GPIO_REGS->IOOUTDATA2 &= ~0x0002;
		}
		if(key == SW2) {
			freqShift = 0;
			delta = 0;
			displayFreq(delta);
			CSL_GPIO_REGS->IOOUTDATA2 |= 0x0002;
		}

		asm("   idle");
	}
}

