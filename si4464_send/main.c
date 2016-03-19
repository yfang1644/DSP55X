/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535_gpio.h"
#include "ezdsp5535_i2c.h"
#include "ezdsp5535_spi.h"
#include "csl_spi.h"
#include "si4464send.h"
#include "Si446x_api.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)

volatile Uint16 taskList = 0;

volatile Int16 gpioIsrStatus = 0;
volatile Int16 spiIsrStatus = 0;
volatile Int16 timIsrStatus = 0;

extern Uint16 number_of_16bit_words_per_frame;
extern Int16 rawOutWords[], rawInWords[];
extern int spi_set;
int mark = 0;
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
	EZDSP5535_PLL_init(200000000);		// system clock set to 80MHz
	TIMER_init(25000000);				// when runs at 100MHz, 50000=1ms
	EZDSP5535_GPIO_init();
	EZDSP5535_I2C_init();		/* Initialize I2C */
	Audio_init(7);				/* Initialise to bandwidth=7kHz */

	EZDSP5535_SPI_init();
	Si446x_Init();

	asm("	bit(ST1, #ST1_INTM) = #0");

	while(1) {

		if(spi_set) {
			if(mark)	asm("	bit(ST1, #ST1_XF) = #1");
			else		asm("	bit(ST1, #ST1_XF) = #0");
			spi_set = 0;
			mark = 1 - mark;
		}
/*		if((timIsrStatus <= 0) && (si4464Func == RECV)) {
			SPI_sendData(rawOutWords, number_of_16bit_words_per_frame + 1);
			si4464Func = SEND;
		}
*/
		while((taskList & AUDIOPROCESSING) == 0)	;
		bApi_Set_Receive();
		while(gpioIsrStatus == 0)	;
		gpioIsrStatus = 0;

		audioDecoding();		// decode last frame
		spiIsrStatus = 0;
		SPI_recvData(rawInWords);
		while(spiIsrStatus == 0)	;
//		SpiWriteByte(CMD_CHANGE_STATE, 3);	// go to READY mode

		SPI_sendData(rawOutWords, number_of_16bit_words_per_frame + 1);
		audioEncoding(LEFT);
		bApi_Set_Send(43);
		while(gpioIsrStatus == 0)	;
		gpioIsrStatus = 0;


		asm("	idle");
	}
}

