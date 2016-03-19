#include "soc.h"

#include "defs.h"
#include "cnst.h"
#include "cod_main.h"
#include "dec_main.h"
#include "bits.h"

#pragma DATA_SECTION(rcv_l, ".cio")
#pragma DATA_SECTION(xmt_l, ".cio")
#pragma DATA_SECTION(rcv_r, ".cio")
#pragma DATA_SECTION(xmt_r, ".cio")
#pragma DATA_ALIGN (rcv_l, 4)
#pragma DATA_ALIGN (xmt_l, 4)
#pragma DATA_ALIGN (rcv_r, 4)
#pragma DATA_ALIGN (xmt_r, 4)
Int16 rcv_l[L_FRAME16k*2], xmt_l[L_FRAME16k*2];
Int16 rcv_r[L_FRAME16k*2], xmt_r[L_FRAME16k*2];

volatile Int16 pingpong = -1;
extern volatile Uint16 taskList;
// fs=16000Hz
// BUFSIZE=512x2channels
// HALF buffer interrupt
// every 512/16000/2 seconds issue an interrupt
void interrupt dma_isr(void)
{
	int ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
	static int cnt = 50;
	if(--cnt <= 0)
	{
		CSL_GPIO_REGS->IOOUTDATA2 ^= 0x0001;	// green LED
		cnt = 50;
	}
	CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

	pingpong = CSL_DMA1_REGS->DMACH1TCR2;
	pingpong &= 2;
}

// DMA1 channel 0 for I2S receive left
// DMA1 channel 1 for I2S transmit left
// DMA1 channel 2 for I2S receive right
// DMA1 channel 3 for I2S transmit right
void DMA_audio_init(Uint16 size)
{
	CSL_DmaRegsOvly dma_1 = CSL_DMA1_REGS;
	CSL_SysRegsOvly regs = CSL_SYSCTRL_REGS;
	CSL_I2sRegsOvly i2s_2 = CSL_I2S2_REGS;
	Uint32 addr;

/*ADC left***********************************************************/
	// DMA1 ch0 : ADC--  i2s.left->daram
	dma_1->DMACH0TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0000 |		/* DST address inc. */
						0x0080 |		/* SRC address constant */
						0x0000 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2 */
						0x0001;			/* Ping Pong mode */

	dma_1->DMACH0SSAL = (Uint16)&i2s_2->I2SRXLT0;
	dma_1->DMACH0SSAU = 0;
	addr = ((Uint32)rcv_l << 1) + 0x010000;		/* DARAM OFFSET = 0x010000 */
	dma_1->DMACH0DSAL = addr & 0xffff;
	dma_1->DMACH0DSAU = (Uint16)(addr >> 16);
	dma_1->DMACH0TCR1 = 4 * size;

/*DAC left***********************************************************/
	// DMA1 ch1 : DAC--  daram->i2s.left
	dma_1->DMACH1TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0200 |		/* DST address constant */
						0x0000 |		/* SRC address inc. */
						0x0000 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2*/
						0x0001;			/* Ping Pong mode */

	addr = ((Uint32)xmt_l << 1) + 0x010000;
	dma_1->DMACH1SSAL = addr & 0xffff;
	dma_1->DMACH1SSAU = (Uint16)(addr >> 16);
	dma_1->DMACH1DSAL = (Uint16)&i2s_2->I2STXLT0;
	dma_1->DMACH1DSAU = 0;
	dma_1->DMACH1TCR1 = 4 * size;

/*ADC right**********************************************************/
	// DMA1 ch2 : ADC--  i2s.right->daram
	dma_1->DMACH2TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0000 |		/* DST address inc. */
						0x0080 |		/* SRC address constant */
						0x0000 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2 */
						0x0001;			/* Ping Pong mode */

	dma_1->DMACH2SSAL = (Uint16)&i2s_2->I2SRXRT0;
	dma_1->DMACH2SSAU = 0;
	addr = ((Uint32)rcv_r << 1) + 0x010000;		/* DARAM OFFSET = 0x010000 */
	dma_1->DMACH2DSAL = addr & 0xffff;
	dma_1->DMACH2DSAU = (Uint16)(addr >> 16);
	dma_1->DMACH2TCR1 = 4 * size;

/*DAC right**********************************************************/
	// DMA1 ch3 : DAC--  daram->i2s.right
	dma_1->DMACH3TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0200 |		/* DST address constant */
						0x0000 |		/* SRC address inc. */
						0x0000 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2*/
						0x0001;			/* Ping Pong mode */

	addr = ((Uint32)xmt_r << 1) + 0x010000;
	dma_1->DMACH3SSAL = addr & 0xffff;
	dma_1->DMACH3SSAU = (Uint16)(addr >> 16);
	dma_1->DMACH3DSAL = (Uint16)&i2s_2->I2STXRT0;
	dma_1->DMACH3DSAU = 0;
	dma_1->DMACH3TCR1 = 4 * size;

	regs->DMA1CESR1 =	0x0002 |		/* I2S rcv_l */
						(0x0001 << 8);	/* I2S xmt_l */
	regs->DMA1CESR2 =	0x0002 |		/* I2S rcv_r */
						(0x0001 << 8);	/* I2S xmt_r */
	regs->DMAIER |= 0x0010;					/* DMA1 ch0 interrupt enable */

	/* Enable DMA */
	dma_1->DMACH0TCR2 |= 0x8000 |		/* Enable */
						0x0004;			/* synchronized to CHnEVT */
	dma_1->DMACH2TCR2 |= 0x8000 |		/* Enable */
						0x0004;			/* synchronized to CHnEVT */
	dma_1->DMACH1TCR2 |= 0x8000 |		/* Enable */
						0x0004;			/* synchronized to CHnEVT */
	dma_1->DMACH3TCR2 |= 0x8000 |		/* Enable */
						0x0004;			/* synchronized to CHnEVT */

	/* Enabling Interrupt */
	CSL_CPU_REGS->IER0 |= 0x0100;
}

Coder_State ste;
TX_State tx_state;

Decoder_State std;
RX_State rx_state;

void audioEncoding(int chn)
{
	Int16  *signal;
	Uint16 offset;
	UWord8 packet[64];

	Word16 prms[NB_BITS_MAX];  /* Buffer for bit stream                 */

	Word16 coding_mode, mode_c, mode_d, nb_bits, allow_dtx;
	Word16 bitstreamformat;
	Word16 frame_type;

	if(pingpong < 0)	return;

	mode_c = 4;			// 15.85kbps

	nb_bits = nb_of_bits[mode_c];
	allow_dtx = 1;				// DTX ON
	allow_dtx = 0;				// DTX OFF -- default

	bitstreamformat = 0;		// default
	bitstreamformat = 1;		// ITU
	bitstreamformat = 2;		// MIME

//	n = fread(packet, sizeof(UWord8), 1 + packed_size[mode_c], fp);
}

void audioDecoding(void)
{
	Int16  *signal;
	Uint16 offset;

	Word16 prms[NB_BITS_MAX];  /* Buffer for bit stream                 */

	Word16 coding_mode, mode_c, mode_d, nb_bits, allow_dtx;
	Word16 bitstreamformat;
	Word16 frame_type;

	if(pingpong < 0)	return;

	offset = (pingpong >> 1) * L_FRAME16k;
	mode_c = 4;			// 15.85kbps

	nb_bits = nb_of_bits[mode_c];
	allow_dtx = 1;				// DTX ON
	allow_dtx = 0;				// DTX OFF -- default

	bitstreamformat = 0;		// default
	bitstreamformat = 1;		// ITU
	bitstreamformat = 2;		// MIME

/**************************************************************************
*                       Decoder                                           *
***************************************************************************/
	Read_serial0(prms, &frame_type, &mode_d, &rx_state, bitstreamformat);

	signal = &xmt_l[offset];	/* Buffer for speech @ 16kHz             */
	decoder(rx_state.prev_mode, prms, signal, &std, frame_type);

	/* copy right channel without coding/decoding */
	DSP_memcpy(&xmt_r[offset], &rcv_r[offset], L_FRAME16k);
}

/*----------------------------------------------------------------------*
 * Initialisation                                                       *
 *----------------------------------------------------------------------*/
void Audio_init(void)
{
	aic3204_init(6, 0);						/* Initialize codec */
	Init_coder(&ste);                       /* Initialize the coder */
	Init_write_serial(&tx_state);
	Init_decoder(&std);
	Init_read_serial(&rx_state);

	DMA_audio_init(L_FRAME16k);
	EZDSP5535_I2S_init();					/* Initialize I2S */
}

