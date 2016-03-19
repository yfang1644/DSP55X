//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_spi.h
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535.h"

#define	NONE	0
#define	RECV	1
#define	SEND	2

/* ------------------------------------------------------------------------ *
 *  Prototype                                                               *
 * ------------------------------------------------------------------------ */
void  EZDSP5535_SPI_init(void);
Uint8 SpiReadByte(Uint8 reg);
void  SpiWriteByte(Uint8 reg, Uint8 val);
void  SpiSendCommand(int length, Uint8 *str, int waitcts);
void  SpiGetResponse(Uint8 reg, int length, Uint8 *str);
void  SpiWriteTxDataBuffer(int length, Uint8 *str);
void  SpiReadRxDataBuffer(int length, Uint8 *str);

void  SPI_sendData(Int16 *buf, Uint16 cnt);
Uint16 SPI_recvData(Int16 *buf);
