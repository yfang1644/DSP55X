//////////////////////////////////////////////////////////////////////////////
// * File name: main.c
//////////////////////////////////////////////////////////////////////////////

#include "project.h"
#include "ezdsp5535_i2c.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_sar.h"
#include "ezdsp5535_gpio.h"

#define ICR     (*(volatile ioport Uint16 *)0x0001)

/*
 *
 *  main( )
 *
 */

int timer_st = -1;
Int16 period = 48;

void displayPeriod( )
{
	char *str = "    ";
	Int16 i;
	Int16 pv = period;

	for(i = 0; (i < 10) && (pv >= 100); i++) {
		pv -= 100;
	}
	str[0] = i + '0';
	for(i = 0; (i < 10) && (pv >= 10); i++) {
		pv -= 10;
	}
	str[1] = i + '0';
	str[2] = pv + '0';

	LCD_print(str, 1);
}

void main(void)
{
	Uint16 key;

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
	PLL_init(160*1000000L);
	TIMER_init();

	EZDSP5535_I2C_init();
	EZDSP5535_LCD_init();
	aic3204_init(2, 0);
	EZDSP5535_GPIO_init();
	EZDSP5535_I2S_init(1);			// interrupt mode
	EZDSP5535_SAR_init();
	asm("	bit(ST1, #ST1_INTM) = #0");

	displayPeriod();
	while(1) {
		key = EZDSP5535_SAR_getKey();
		if(key == SW1) {
			if(period > 10)
				period--;
			CSL_GPIO_REGS->IOOUTDATA2 ^= 2;
			displayPeriod();
		}
		if(key == SW2) {
			if(period < 900)
				period++;
			CSL_GPIO_REGS->IOOUTDATA2 ^= 1;
			displayPeriod();
		}
		asm("   idle");
	}
}

