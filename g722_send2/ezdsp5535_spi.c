//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_spi.c
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535_spi.h"
#include "csl_spi.h"
#include "Si446x_api.h"

/*
 *  EZDSP5535_SPI_init( )
 *
 *      Enables and configures SPI for the SPIFLASH
 *      ( CS0, EBSR Mode 1, 8-bit, 100KHz clock )
 */
void EZDSP5535_SPI_init(void)
{
    /* Configuration for SPI
    SPI_Config      hwConfig;

    hwConfig.wLen       = SPI_WORD_LENGTH_8;    // 8-bit
    hwConfig.spiClkDiv  = 0x0041;               // 100KHz clock (12MHz / 120)
    hwConfig.csNum      = SPI_CS_NUM_2;         // Select CS2
    hwConfig.frLen      = 1;
    hwConfig.dataDelay  = SPI_DATA_DLY_0;
    hwConfig.clkPol     = SPI_CLKP_LOW_AT_IDLE;
*/
	/* Disable the serial Data clock */
	CSL_SPI_REGS->SPICCR = 0x0000;
	
	CSL_SPI_REGS->SPICDR = 0x002f;	// 100KHz clock (12MHz / 120)

	EZDSP5535_waitusec(1);
	/* Enable the serial Data clock */
	CSL_SPI_REGS->SPICCR = 0x8000;

	/* Set Data delay, cs pol, clk pol and clpck pkase bit as per chip select */
	CSL_FINS(CSL_SPI_REGS->SPIDCR2, SPI_SPIDCR2_DD2, SPI_DATA_DLY_0);
	CSL_FINS(CSL_SPI_REGS->SPIDCR2, SPI_SPIDCR2_CSP2, 0);
	CSL_FINS(CSL_SPI_REGS->SPIDCR2, SPI_SPIDCR2_CKP2, SPI_CLKP_LOW_AT_IDLE);
	CSL_FINS(CSL_SPI_REGS->SPIDCR2, SPI_SPIDCR2_CKPH2, 0);

	CSL_CPU_REGS->IER1 |= 0x0008;		// SPI interrupt
}

Uint8 SpiReadByte(Uint8 reg)
{
	Uint16 	spiStatusReg;
	volatile Uint16 	spiBusyStatus;
	volatile Uint16 	spiWcStaus;
	Uint8 temp8;

	CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;
	CSL_SPI_REGS->SPIDR2 = (Uint16)(reg << 8);
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_READ_CMD;

	while(!(CSL_SPI_REGS->SPISTAT1 & CSL_SPI_SPISTAT1_BSY_MASK));
	do {
		spiStatusReg = CSL_SPI_REGS->SPISTAT1;
		spiBusyStatus = (spiStatusReg & CSL_SPI_SPISTAT1_BSY_MASK);
		spiWcStaus = (spiStatusReg & CSL_SPI_SPISTAT1_CC_MASK);
	}while(spiBusyStatus && (!spiWcStaus));

	temp8 = (CSL_SPI_REGS->SPIDR1 & 0xFF);
	return temp8;
}

Int16 SpiWaitforCTS(void)
{
	Uint8 ctsValue = 0;
	Uint16 errCnt = 0;

	while (ctsValue != 0xFF) {		// Wait until radioIC is ready
		ctsValue = SpiReadByte(CMD_READ_CMD_BUFF);
		if (++errCnt > MAX_CTS_RETRY)
			return -1;     // Error handling; CTS read exceeds a limit
	}
	return 0;
}

void SpiWriteByte(Uint8 reg, Uint8 val)
{
	Uint16 	spiStatusReg;
	volatile Uint16 	spiBusyStatus;
	volatile Uint16 	spiWcStaus;

	CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;

	CSL_SPI_REGS->SPIDR2 = ((Uint16)reg << 8) | val;
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_WRITE_CMD;

	while(!(CSL_SPI_REGS->SPISTAT1 & CSL_SPI_SPISTAT1_BSY_MASK));
	do {
		spiStatusReg = CSL_SPI_REGS->SPISTAT1;
		spiBusyStatus = (spiStatusReg & CSL_SPI_SPISTAT1_BSY_MASK);
		spiWcStaus = (spiStatusReg & CSL_SPI_SPISTAT1_CC_MASK);
	}while(spiBusyStatus && (!spiWcStaus));

	SpiWaitforCTS();
}

void SpiSendCommand(int length, Uint8 *str)
{
	int i;
	Uint16 	spiStatusReg;
	volatile Uint16 	spiBusyStatus;
	volatile Uint16 	spiWcStaus;

    CSL_SPI_REGS->SPICMD1 = 0x0000 | (length - 1);		// bytes

	for(i = 0; i < length; i++)
	{
		CSL_SPI_REGS->SPIDR2 = (Uint16)(str[i] << 8);
		CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_WRITE_CMD;

		while(!(CSL_SPI_REGS->SPISTAT1 & CSL_SPI_SPISTAT1_BSY_MASK));
		do {
			spiStatusReg = CSL_SPI_REGS->SPISTAT1;
			spiBusyStatus = (spiStatusReg & CSL_SPI_SPISTAT1_BSY_MASK);
			spiWcStaus = (spiStatusReg & CSL_SPI_SPISTAT1_CC_MASK);
		}while(spiBusyStatus && (!spiWcStaus));
	}
	SpiWaitforCTS();
}

void SpiGetResponse(Uint8 reg, int length, Uint8 *str)
{
	int i;
	Uint16 	spiStatusReg;
	volatile Uint16 	spiBusyStatus;
	volatile Uint16 	spiWcStaus;

	CSL_SPI_REGS->SPICMD1 = 0x0000 | length;
	CSL_SPI_REGS->SPIDR2 = (Uint16)reg << 8;
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_WRITE_CMD;

	while(!(CSL_SPI_REGS->SPISTAT1 & CSL_SPI_SPISTAT1_BSY_MASK));
	do {
		spiStatusReg = CSL_SPI_REGS->SPISTAT1;
		spiBusyStatus = (spiStatusReg & CSL_SPI_SPISTAT1_BSY_MASK);
		spiWcStaus = (spiStatusReg & CSL_SPI_SPISTAT1_CC_MASK);
	}while(spiBusyStatus && (!spiWcStaus));

	for(i = 0; i < length; i++)
	{
		CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_READ_CMD;
		while(!(CSL_SPI_REGS->SPISTAT1 & CSL_SPI_SPISTAT1_BSY_MASK));
		do {
			spiStatusReg = CSL_SPI_REGS->SPISTAT1;
			spiBusyStatus = (spiStatusReg & CSL_SPI_SPISTAT1_BSY_MASK);
			spiWcStaus = (spiStatusReg & CSL_SPI_SPISTAT1_CC_MASK);
		}while(spiBusyStatus && (!spiWcStaus));
		str[i] = CSL_SPI_REGS->SPIDR1 & 0xFF;
	}
}

void SpiWriteTxDataBuffer(int length, Uint8 *str)
{
	int i;
	Uint16 	spiStatusReg;
	volatile Uint16 	spiBusyStatus;
	volatile Uint16 	spiWcStaus;

    CSL_SPI_REGS->SPICMD1 = 0x0000 | length;
	CSL_SPI_REGS->SPIDR2 = CMD_WRITE_TX_FIFO << 8;
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_WRITE_CMD;

	while(!(CSL_SPI_REGS->SPISTAT1 & CSL_SPI_SPISTAT1_BSY_MASK));
	do {
		spiStatusReg = CSL_SPI_REGS->SPISTAT1;
		spiBusyStatus = (spiStatusReg & CSL_SPI_SPISTAT1_BSY_MASK);
		spiWcStaus = (spiStatusReg & CSL_SPI_SPISTAT1_CC_MASK);
	}while(spiBusyStatus && (!spiWcStaus));

	for(i = 0; i < length; i++)
	{
		CSL_SPI_REGS->SPIDR2 = (Uint16)str[i] << 8;
		CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_WRITE_CMD;

		while(!(CSL_SPI_REGS->SPISTAT1 & CSL_SPI_SPISTAT1_BSY_MASK));
		do {
			spiStatusReg = CSL_SPI_REGS->SPISTAT1;
			spiBusyStatus = (spiStatusReg & CSL_SPI_SPISTAT1_BSY_MASK);
			spiWcStaus = (spiStatusReg & CSL_SPI_SPISTAT1_CC_MASK);
		}while(spiBusyStatus && (!spiWcStaus));
	}
	SpiWaitforCTS();
}

void SpiReadRxDataBuffer(int length, Uint8 *str)
{
	SpiGetResponse(CMD_READ_RX_FIFO, length, str);
}

static Int16 sent_cnt;
static Uint8 *sent_buffer;
extern Int16 spiIsrStatus;

interrupt void spi_isr(void)
{
	if(--sent_cnt >= 0) {
		CSL_SPI_REGS->SPIDR2 = (CMD_WRITE_TX_FIFO << 8) | ((Uint16)(*sent_buffer++));
		CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_WRITE_CMD;
	} else {
		CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;
		spiIsrStatus = 1;
//		CSL_SPI_REGS->SPIDR2 = 0;
//		CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_WRITE_CMD;
	}
}

void SPI_sendData(Uint8 *buf, Uint16 cnt)
{
	sent_buffer = buf;							// send first byte as length
	CSL_SPI_REGS->SPICMD1 = 0x4000 | 0;			// enable interrupt
	CSL_SPI_REGS->SPIDR2 = (CMD_WRITE_TX_FIFO << 8) | cnt;
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_WRITE_CMD;  // send first
	sent_cnt = cnt;
}

