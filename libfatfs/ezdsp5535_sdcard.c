//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_sd_card.c
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535_sdcard.h"
#include "csl_mmcsd.h"

/* MMCSD Data structures */
CSL_MMCControllerObj 	pMmcsdContObj;
CSL_MmcsdHandle 		mmcsdHandle;
CSL_MMCCardObj			mmcCardObj;
CSL_MMCCardIdObj 		sdCardIdObj;
CSL_MMCCardCsdObj 		sdCardCsdObj;

CSL_Status	 mmcStatus;

/*
 *  EZDSP5535_SDCARD_init( )
 *
 *      Initalize SD card interface
 */
Int16 EZDSP5535_SDCARD_init()
{
	Uint16		 actCard;
	Uint16       rca;

	/* Initialize the CSL MMCSD module */
//	mmcStatus = MMC_init();		// never failed

	/* Open the MMCSD module in POLLED mode. never failed */
	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD0_INST,
			CSL_MMCSD_OPMODE_POLLED, &mmcStatus);
	CSL_MMCSD0_REGS->MMCCTL |= 0x0600;		// big-endian for read/write

	/* Send CMD0 to the card. never failed */
	CSL_MMCSD0_REGS->MMCARG1 = 0;
	CSL_MMCSD0_REGS->MMCARG2 = 0;
	CSL_MMCSD0_REGS->MMCCMD1 = 0xc000;	// data clear, init clock, go idle state

	/* Check for the card */
    mmcStatus = MMC_selectCard(mmcsdHandle, &mmcCardObj);
	if((mmcStatus == CSL_ESYS_BADHANDLE) ||
	   (mmcStatus == CSL_ESYS_INVPARAMS))		// select failed
	{
		return(mmcStatus);
	}

	/* Set the init clock */
    mmcStatus = MMC_sendOpCond(mmcsdHandle, 70);
	if(mmcStatus != CSL_SOK)	// send op cond failed
	{
		return(mmcStatus);
	}

	/* Send the card identification Data */
	mmcStatus = SD_sendAllCID(mmcsdHandle, &sdCardIdObj);
	if(mmcStatus != CSL_SOK)	// send all CID failed
	{
		return(mmcStatus);
	}

	/* Set the Relative Card Address */
	mmcStatus = SD_sendRca(mmcsdHandle, &mmcCardObj, &rca);
	if(mmcStatus != CSL_SOK)
	{
		return(mmcStatus);
	}

	/* Read the SD Card Specific Data */
	mmcStatus = SD_getCardCsd(mmcsdHandle, &sdCardCsdObj);
	if(mmcStatus != CSL_SOK)
	{
		return(mmcStatus);
	}

	/* Set bus width - Optional */
	mmcStatus = SD_setBusWidth(mmcsdHandle, 1);
	if(mmcStatus != CSL_SOK)
	{
		return(mmcStatus);
	}

	/* Disable SD card pull-up resistors - Optional */
	mmcStatus = SD_configurePullup(mmcsdHandle, 0);
	if(mmcStatus != CSL_SOK)
	{
		return(mmcStatus);
	}

	/* Set the card type in internal data structures */
	mmcStatus = MMC_setCardType(&mmcCardObj, mmcCardObj.cardType);
	if(mmcStatus != CSL_SOK)
	{
		return(mmcStatus);
	}

	/* Set the card pointer in internal data structures */
	mmcStatus = MMC_setCardPtr(mmcsdHandle, &mmcCardObj);
	if(mmcStatus != CSL_SOK)
	{
		return(mmcStatus);
	}

	/* Get the number of cards */
	mmcStatus = MMC_getNumberOfCards(mmcsdHandle, &actCard);
	if(mmcStatus != CSL_SOK)
	{
		return(mmcStatus);
	}

	/* Set clock for read-write access */
    mmcStatus = MMC_sendOpCond(mmcsdHandle, 2);
	if(mmcStatus != CSL_SOK)
	{
		return(mmcStatus);
	}

	/* Set block length for the memory card
	 * For high capacity cards setting the block length will have
	 * no effect
	 */
	mmcStatus = MMC_setBlockLength(mmcsdHandle, CSL_MMCSD_BLOCK_LENGTH);
	if(mmcStatus != CSL_SOK)
		return(mmcStatus);
	
	return 0;
}

/*
 *  EZDSP5535_SDCARD_close()
 *
 *      Close SD card interface
 */
Int16 EZDSP5535_SDCARD_close()
{
	/* Deselect the SD card */
	mmcStatus = MMC_deselectCard(mmcsdHandle, &mmcCardObj);
	if(mmcStatus != CSL_SOK)
		return(mmcStatus);

	/* Clear the MMCSD card response registers */
	mmcStatus = MMC_clearResponse(mmcsdHandle);
	if(mmcStatus != CSL_SOK)
		return(mmcStatus);

	/* Send CMD0 to the SD card */
	mmcStatus = MMC_sendCmd(mmcsdHandle, 0x00, 0x00, 0xFFFF);
	if(mmcStatus != CSL_SOK)
		return(mmcStatus);

	/* Close the MMCSD module */
	mmcStatus = MMC_close(mmcsdHandle);
	if(mmcStatus != CSL_SOK)
		return(mmcStatus);
	
	return 0;
}

#define	SD_SECTOR_SIZE	512				// bytes (8bit)
#pragma DATA_SECTION(dmaOutBuff, ".cio")
#pragma DATA_SECTION(dmaInBuff, ".cio")
#pragma DATA_ALIGN (dmaOutBuff, 4)
#pragma DATA_ALIGN (dmaInBuff, 4)
Uint16 dmaOutBuff[SD_SECTOR_SIZE/2], dmaInBuff[SD_SECTOR_SIZE/2];
extern volatile Uint16 dmaIsrStatus;
void DMA_sdcard_init( )
{
	CSL_DmaRegsOvly dma_0 = CSL_DMA0_REGS;
	CSL_SysRegsOvly regs = CSL_SYSCTRL_REGS;
	CSL_MmcsdRegsOvly mmcRegs = CSL_MMCSD0_REGS;
	Uint32 addr;

/********************** DMA0 ch0 : mmcsd -> daram *************************/
	dma_0->DMACH0TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0000 |		/* DST address inc. */
						0x0080 |		/* SRC address constant */
						0x0018 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2 */
						0x0000;			/* no Ping Pong mode */

	dma_0->DMACH0SSAL = (Uint16)&mmcRegs->MMCDRR2;
	dma_0->DMACH0SSAU = 0;
	addr = ((Uint32)dmaInBuff << 1) + 0x010000;	/* DARAM OFFSET = 0x010000 */
	dma_0->DMACH0DSAL = addr & 0xffff;
	dma_0->DMACH0DSAU = (Uint16)(addr >> 16);
	dma_0->DMACH0TCR1 = SD_SECTOR_SIZE;

/********************** DMA0 ch1 : daram -> mmcsd *************************/
	dma_0->DMACH1TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0200 |		/* DST address constant */
						0x0000 |		/* SRC address inc. */
						0x0018 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2*/
						0x0000;			/* no Ping Pong mode */

	addr = ((Uint32)dmaOutBuff << 1) + 0x010000;/* DARAM OFFSET = 0x010000 */
	dma_0->DMACH1SSAL = addr & 0xffff;
	dma_0->DMACH1SSAU = (Uint16)(addr >> 16);
	dma_0->DMACH1DSAL = (Uint16)&mmcRegs->MMCDXR1;
	dma_0->DMACH1DSAU = 0;
	dma_0->DMACH1TCR1 = SD_SECTOR_SIZE;

	regs->DMA0CESR1 =	0x0006 |		/* mmc/sd0 receive */
						(0x0005 << 8);	/* mmc/sd0 transmitt  */
	regs->DMAIER |= 0x0003;

	/* Enable DMA */
	dma_0->DMACH0TCR2 |= 0x8004;			/* Enable */
	dma_0->DMACH1TCR2 |= 0x8004;			/* Enable */

	dmaIsrStatus = 0x0003;
	mmcRegs->MMCBLEN = SD_SECTOR_SIZE;
	mmcRegs->MMCNBLK = 1;

	/* Enabling Interrupt */
	CSL_CPU_REGS->IER0 |= 0x0100;
}

Int16 MMC_dmaRead(Uint32 sector)
{
	CSL_MmcsdRegsOvly mmcRegs = CSL_MMCSD0_REGS;
	Uint32 response;

//	while((dmaIsrStatus & 1) == 0)		;

	mmcRegs->MMCARG1 = (Uint16)(sector << 9);
	mmcRegs->MMCARG2 = (Uint16)(sector >> 7);
//	SD_getCardCsd(mmcsdHandle, &sdCardCsdObj);
//	MMC_read(mmcsdHandle, 0, 512, rcv);

	mmcRegs->MMCFIFOCTL = 0x0005;	// reset, READ,
									// 4BYTES ACCWD, 256bitFIFO level

//	MMC_setRca(mmcsdHandle, &mmcCardObj,0);

//	mmcRegs->MMCARG1 = 0;
//	mmcRegs->MMCARG2 = 0;
//	mmcRegs->MMCCMD1 = CSL_MMCSD_SET_BLKLEN_CMD;

	mmcRegs->MMCCMD1 = CSL_MMCSD_READ_BLOCK_CMD;
	while((mmcRegs->MMCST0 & 0x0001) == 0)	;
//	dmaIsrStatus &= ~1;
	response = mmcRegs->MMCRSP7;
	response = (response << 16) | mmcRegs->MMCRSP6;

	if((response & 0xFFFF00FF) != 0)
		return CSL_EMMCSD_READ_ERROR;
	else
		return CSL_SOK;
}

Int16 MMC_dmaWrite(Uint32 sector)
{
	CSL_MmcsdRegsOvly mmcRegs = CSL_MMCSD0_REGS;
	Uint32 response;

	mmcRegs->MMCARG1 = (Uint16)(sector << 9);
	mmcRegs->MMCARG2 = (Uint16)(sector >> 7);

	mmcRegs->MMCFIFOCTL = 0x0007;	// reset, WRITE,
									// 4BYTES ACCWD, 256bitFIFO level

	mmcRegs->MMCCMD1 = CSL_MMCSD_WRITE_BLOCK_CMD;
	mmcRegs->MMCCMD2 = 1;			//Trigger a DMA transfer event
	while((mmcRegs->MMCST0 & 0x0001) == 0)	;
//	while((dmaIsrStatus & 2) == 0)		;
//	dmaIsrStatus &= ~2;
	response = mmcRegs->MMCRSP7;
	response = (response << 16) | mmcRegs->MMCRSP6;

	if((response & 0xFFFF00FF) != 0)
		return CSL_EMMCSD_WRITE_ERROR;
	else
		return CSL_SOK;
}

