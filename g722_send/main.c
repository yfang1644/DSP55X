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

#include "ezdsp5535_gpio.h"
#include "ezdsp5535_i2c.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_sar.h"
#include "ezdsp5535_spi.h"
#include "csl_spi.h"
#include "defs.h"
#include "csl_mmcsd.h"
#include "fat.h"
#include "Si446x_api.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)

volatile Uint16 taskList = 0;

volatile Int16 gpioIsrStatus = 0;
volatile Int16 spiIsrStatus = 0;

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
extern Int16  sent_buffer_cnt;
extern Uint8  *sent_buffer;
extern Uint16 number_of_bytes_per_frame;
void main(void)
{
	HANDLE FHandle1 = Not_Open_FILE;
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
	PLL_init(3658);		// system clock set to 120MHz
	TIMER_init();
	EZDSP5535_GPIO_init();
	EZDSP5535_I2C_init();		/* Initialize I2C */

	Audio_init(7);				/* Initialise to bandwidth=7kHz */

	EZDSP5535_LCD_init();
	LCD_print("G722.1 Send       ", 17, 0);
	LCD_print("16kbps  ", 8, 1);
	EZDSP5535_SPI_init();
	Si446x_Init();

	EZDSP5535_SAR_init();
	DiskInit();
	FileInit();
	AddFileDriver(SDCammand, NULL);
	FHandle1 = FileOpen("A:\\G722_2.COD", FILE_FLAGS_READ);
	asm("	bit(ST1, #ST1_INTM) = #0");

	SpiWriteByte(CMD_GET_INT_STATUS, 0);	// clear interrupt pending
	while(1) {
		audioProcessing();

		if(spiIsrStatus == 1) {
			bApi_Set_Send(number_of_bytes_per_frame + 3);
			spiIsrStatus = 0;
		}
		if(sent_buffer_cnt < 2) {
			if(gpioIsrStatus == 1) {
				SPI_sendData(sent_buffer, number_of_bytes_per_frame+2);
				sent_buffer += number_of_bytes_per_frame+2;
				sent_buffer_cnt++;
				gpioIsrStatus = 0;
				CSL_GPIO_REGS->IOINTFLG1 = 0x0800;
			}
		}

		fileProcessing();

		asm("	idle");
	}

//	RemoveFileDriver(SDCammand);
}

