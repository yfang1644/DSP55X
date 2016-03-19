//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_sdcard.h
//////////////////////////////////////////////////////////////////////////////

#include "tistdtypes.h"
#include "soc.h"
#include "cslr_sysctrl.h"

#define	SD_SECTOR_SIZE	512				// bytes (8bit)

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 EZDSP5535_SDCARD_init();
void DMA_sdcard_init();
Int16 EZDSP5535_SDCARD_write(Uint32 addr, Uint32 len, Uint16* pWriteBuff);
Int16 EZDSP5535_SDCARD_read (Uint32 addr, Uint32 len, Uint16* pReadBuff );
Int16 EZDSP5535_SDCARD_close();
Int16 MMC_dmaRead(Uint32 sector);
Int16 MMC_dmaWrite(Uint32 sector);

