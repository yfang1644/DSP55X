//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_sdcard.h
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535.h"

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 EZDSP5535_SDCARD_init();
Int16 EZDSP5535_SDCARD_write(Uint32 addr, Uint32 len, Uint16* pWriteBuff);
Int16 EZDSP5535_SDCARD_read (Uint32 addr, Uint32 len, Uint16* pReadBuff );
Int16 EZDSP5535_SDCARD_close();

