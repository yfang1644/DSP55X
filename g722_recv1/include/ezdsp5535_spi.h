//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_spi.h
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535.h"

/* ------------------------------------------------------------------------ *
 *  Prototype                                                               *
 * ------------------------------------------------------------------------ */
void  EZDSP5535_SPI_init(void);
Uint8 SpiReadByte(Uint8 reg);
void  SpiWriteByte(Uint8 reg, Uint8 val);
void  SpiSendCommand(int length, Uint8 *str);
void  SpiGetResponse(Uint8 reg, int length, Uint8 *str);
void  SpiWriteTxDataBuffer(int length, Uint8 *str);
void  SpiReadRxDataBuffer(int length, Uint8 *str);
int   SpiWaitforCTS(void);

void  SPI_sendData(Uint8 *buf, Uint16 cnt);
Uint16 SPI_recvData(Uint8 *buf);
