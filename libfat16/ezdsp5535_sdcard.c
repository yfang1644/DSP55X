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

	/* Open the MMCSD module in POLLED mode */
	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD0_INST,
			CSL_MMCSD_OPMODE_POLLED, &mmcStatus);
	if(mmcStatus != CSL_SOK)		// open failed
	{
		return(mmcStatus);
	}

	/* Send CMD0 to the card */
	mmcStatus = MMC_sendGoIdle(mmcsdHandle);
	if(mmcStatus != CSL_SOK)		// idle failed
	{
		return(mmcStatus);
	}

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

