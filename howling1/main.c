//////////////////////////////////////////////////////////////////////////////
// * File name: main.c
// *                                                                          
// * Description:  Main function.
// *                                                                          
/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535_i2c.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_gpio.h"
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
extern Uint16 delay;
extern Int16 rcv_l[], rcv_r[];

void displayFrequency()
{
}

void displayDelay()
{
	Uint16 s1, s2;
	char *str = "Delay    ";
	s1 = (delay >> 12) & 0xf;
	s2 = (delay >> 8) & 0xf;
	if (s1 > 9)
		str[7] = s1 + 'A' - 10;
	else
		str[7] = s1 + '0';
	if (s2 > 9)
		str[8] = s2 + 'A' - 10;
	else
		str[8] = s2 + '0';
	LCD_print(str, 0);
}

void main()
{
	int led = 0;
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
	PLL_init(200000000);				/* CPU clock = 100MHz */
	INTR_init();				/* set Interrupt vector and clean all IRs */
	TIMER_init();				/* Timer interrupt */
	DMA_audio_init(FFT2*4);
	EZDSP5535_GPIO_init();
	EZDSP5535_I2C_init();		/* Initialize I2C */
	aic3204_init(6, 64);		/* Initialize codec, sampling rate = 16kHz */
	EZDSP5535_I2S_init(0);		/* Initialize I2S */

	EZDSP5535_LCD_init();
	EZDSP5535_SAR_init();

    LCD_print("Function", 0);
    LCD_print("        ", 1);
	asm("	bit(ST1, #ST1_INTM) = #0");

	while(1) {
		audioProcessing( );

		if(timer > 0) {
			if(t_cnt++ > 100) {
				if(led)
					asm("	bit(ST1, #ST1_XF) = #1");
				else
					asm("	bit(ST1, #ST1_XF) = #0");
				t_cnt = 0;
				led = 1 - led;
				displayFrequency();
			}
			timer = 0;
		}
		key = EZDSP5535_SAR_getKey();
		if(key == SW1) {
			EZDSP5535_waitusec(50000);
			DSP_zero(rcv_l, 0x2000);
			DSP_zero(rcv_r, 0x2000);
			delay += 0x100;
			delay &= 0x1fff;
			displayDelay();
		}
		if(key == SW2) {
			EZDSP5535_waitusec(50000);
			delay = 0;
			displayDelay();
		}

		asm("   idle");
	}
}

