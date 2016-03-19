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
	CSL_FSET(CSL_SPI_REGS->SPICCR, CSL_SPI_SPICCR_CLKEN_SHIFT, 
		CSL_SPI_SPICCR_CLKEN_SHIFT, CSL_SPI_SPICCR_CLKEN_DISABLED);
	//CSL_FINST(CSL_SPI_REGS->SPICCR, SPI_SPICCR_CLKEN, DISABLED);
	CSL_FINST(CSL_SPI_REGS->SPICCR, SPI_SPICCR_RST, RELEASE);

	CSL_FINS(CSL_SPI_REGS->SPICDR, SPI_SPICDR_CLKDV, 0x001f);	// 100KHz clock (12MHz / 120)

	/* Enable the serial Data clock */
	CSL_SPI_REGS->SPICCR =
	      (Uint16)(CSL_SPI_SPICCR_CLKEN_ENABLED << CSL_SPI_SPICCR_CLKEN_SHIFT);

	/* Set Data delay, cs pol, clk pol and clpck pkase bit as per chip select */
	CSL_FINS(CSL_SPI_REGS->SPIDCR2, SPI_SPIDCR2_DD2, SPI_DATA_DLY_0);
	CSL_FINS(CSL_SPI_REGS->SPIDCR2, SPI_SPIDCR2_CSP2, 0);
	CSL_FINS(CSL_SPI_REGS->SPIDCR2, SPI_SPIDCR2_CKP2, SPI_CLKP_LOW_AT_IDLE);
	CSL_FINS(CSL_SPI_REGS->SPIDCR2, SPI_SPIDCR2_CKPH2, 0);

	CSL_CPU_REGS->IER1 |= 0x0008;		// SPI interrupt
}

int SpiBusStatus(void)
{
	Uint16  spiStatusReg;
	volatile Uint16	spiBusyStatus;
	volatile Uint16	spiWcStaus;

	while(!(CSL_SPI_REGS->SPISTAT1 & CSL_SPI_SPISTAT1_BSY_MASK));
	do {
		spiStatusReg = CSL_SPI_REGS->SPISTAT1;
		spiBusyStatus = (spiStatusReg & CSL_SPI_SPISTAT1_BSY_MASK);
		spiWcStaus = (spiStatusReg & CSL_SPI_SPISTAT1_CC_MASK);
	}while(spiBusyStatus && (!spiWcStaus));
	return 0;
}

Uint8 SpiReadByte(Uint8 reg)
{
	asm("	bit(ST1, #ST1_INTM) = #1");

	CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;
	CSL_SPI_REGS->SPIDR2 = (Uint16)reg << 8;
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_READ_CMD;

	SpiBusStatus();

	asm("	bit(ST1, #ST1_INTM) = #0");
	return (CSL_SPI_REGS->SPIDR1 & 0xFF);
}

int SpiWaitforCTS(void)
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
	asm("	bit(ST1, #ST1_INTM) = #1");
	CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;

	CSL_SPI_REGS->SPIDR2 = ((Uint16)reg << 8) | val;
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_WRITE_CMD;

	SpiBusStatus();
	asm("	bit(ST1, #ST1_INTM) = #0");
	SpiWaitforCTS();
}

void SpiSendCommand(int length, Uint8 *str, int waitcts)
{
	int i;

	asm("	bit(ST1, #ST1_INTM) = #1");
	CSL_SPI_REGS->SPICMD1 = 0x0000 | (length - 1);		// bytes

	for(i = 0; i < length; i++) {
		CSL_SPI_REGS->SPIDR2 = (Uint16)(*str++) << 8;
		CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_WRITE_CMD;

		SpiBusStatus();
	}
	asm("	bit(ST1, #ST1_INTM) = #0");
	if(waitcts)
		SpiWaitforCTS();
}

void SpiGetResponse(Uint8 reg, int length, Uint8 *str)
{
	int i;

	asm("	bit(ST1, #ST1_INTM) = #1");
	CSL_SPI_REGS->SPICMD1 = 0x0000 | length;
	CSL_SPI_REGS->SPIDR2 = (Uint16)reg << 8;
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_READ_CMD;

	SpiBusStatus();

	for(i = 0; i < length; i++)
	{
		CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_READ_CMD;
		SpiBusStatus();
		*str++ = CSL_SPI_REGS->SPIDR1 & 0xFF;
	}
	asm("	bit(ST1, #ST1_INTM) = #0");
}

void SpiWriteTxDataBuffer(int length, Uint8 *str)
{
    CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;
	CSL_SPI_REGS->SPIDR2 = CMD_WRITE_TX_FIFO << 8;
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_WRITE_CMD;

	SpiBusStatus();
	SpiSendCommand(length, str, 1);
}

void SpiReadRxDataBuffer(int length, Uint8 *str)
{
	SpiGetResponse(CMD_READ_RX_FIFO, length, str);
}

Int16 recv_cnt;
Int16 sent_cnt, sent_cntRes = 40;
Int16 *buffer;
extern Uint16 si4464Func;
int spi_set = 0;
interrupt void spi_isr(void)
{
	static Int16 spiCnt = 1000;
	if(spiCnt-- <= 0) {
		spiCnt = 1000;
		spi_set = 1;
	}

	if(si4464Func == RECV) {
		if(--recv_cnt >= 0) {
			*buffer++ = CSL_SPI_REGS->SPIDR1;
			CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_READ_CMD;
		} else {
			*buffer++ = CSL_SPI_REGS->SPIDR1;
			CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;
			SpiWriteByte(CMD_GET_INT_STATUS, 0);
			bApi_Set_Receive();
		}
	} else {
		if(--sent_cnt >= 0) {
			CSL_SPI_REGS->SPIDR2 = (Uint16)(*buffer++);
			CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_WRITE_CMD;
		} else {
			CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;	// Frame over, disable interrupt
			bApi_Set_Send(sent_cntRes);
		}
	}
}

Uint16 SPI_recvData(Int16 *buf)
{
	Uint8 info[4];
	int odd = 0;
	buffer = buf;
//	SpiGetResponse(CMD_FRR_C_READ, 1, info);		// read out latched RSSI
	SpiWriteByte(CMD_FIFO_INFO, 0);
	SpiGetResponse(CMD_READ_CMD_BUFF, 2, info);
	recv_cnt = info[1];
	odd = recv_cnt & 1;
	recv_cnt >>= 1;

	CSL_SPI_REGS->SPICMD1 = 0x4000 | (recv_cnt);
	CSL_SPI_REGS->SPIDR2 = CMD_READ_RX_FIFO << 8;
	if(odd)
		CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_READ_CMD;
	else
		CSL_SPI_REGS->SPICMD2 = (2 << 12)|(7 << 3)|SPI_READ_CMD;
	return recv_cnt;
}

void SPI_sendData(Int16 *buf, Uint16 cnt)
{
	buffer = buf;							// send first byte as length
	sent_cnt = cnt;
	CSL_SPI_REGS->SPICMD1 = 0x4000 | cnt;			// enable interrupt
	CSL_SPI_REGS->SPIDR2 = (CMD_WRITE_TX_FIFO << 8) | (cnt << 1);
	CSL_SPI_REGS->SPICMD2 = (2 << 12)|(15 << 3)|SPI_WRITE_CMD;  // send first
	sent_cntRes =  2 * cnt + 1;
}

