//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_uart.h
//////////////////////////////////////////////////////////////////////////////
#include "ezdsp5535.h"

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */

void EZDSP5535_UART_init( );
void uartProcessing();
Int16 start_record(void);
Int16 checkToday(Int8 bcd[]);

