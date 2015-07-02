//////////////////////////////////////////////////////////////////////////////
// * File name: main.c
// *                                                                          
// * Description:  Main function.
// *                                                                          
/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535_i2c.h"
#include "ezdsp5535_uart.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_sar.h"
#include "radio.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)

void uartSend(int voltage);
unsigned long f20; 
int sweepDone = 0;
extern Int16 gMax, gMin;
extern Uint16 gBufPtr;
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
	PLL_init(200000000);		/* CPU clock = 100MHz */
	INTR_init();				/* set Interrupt vector and clean all IRs */
	TIMER_init(1000079);			/* Timer interrupt 0.01s */
	EZDSP5535_I2C_init();		/* Initialize I2C */
	EZDSP5535_UART_init();		/* Initialize UART */
	aic3204_init(2, 43);		/* Initialize codec, fs = 48kHz */
	EZDSP5535_I2S_init(0);		/* Initialize I2S */
	EZDSP5535_LCD_init();
	EZDSP5535_SAR_init();

    LCD_print("K1--START", 0);
    LCD_print("                   ", 1);
	asm("	bit(ST1, #ST1_INTM) = #0");

	while(1) {
		key = EZDSP5535_SAR_getKey();
		if(key == SW1) {
			f20 = 447349;
			gMax = gMin = 0;
			gBufPtr = 0;
			CSL_CPU_REGS->IER0 |= 0xc000;
			CSL_CPU_REGS->IER0 |= 0x0010;
		    LCD_print("Sweep 20--2kHz     ", 1);
		    sweepDone = 0;
		}
		if(sweepDone) {
			LCD_print("Sweep 20--2kHz     ", 1);
			LCD_print("TEST OK            ", 1);
			sweepDone = 0;
			LCD_display_bargraph();
		}
		asm("   idle");
	}
}

