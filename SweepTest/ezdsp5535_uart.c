//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_uart.c
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535_uart.h"
#include "radio.h"

Int8 tx_buffer[64];
Int8 rx_buffer[64];
static int rcvCounter = 0;
static int xmtCounter = 0, XmtSize = 0;

Uint16 taskList;

void interrupt uart_isr(void)
{
	Uint8 c;
	Uint16 int_type;

	int_type = CSL_UART_REGS->FCR;		// same address as IIR

	if(int_type & 2) {					//transmitter holding register empty
		CSL_UART_REGS->THR = tx_buffer[xmtCounter++];
		if(xmtCounter >= XmtSize) {
			CSL_UART_REGS->IER &= ~0x0002;
		}
	} else if(int_type & 4) {			// receiver data available
		c = CSL_UART_REGS->THR;
		if(c == '#')	rcvCounter = 0;
		rx_buffer[rcvCounter++] = c;
		rcvCounter &= 63;
		if(c == '!') {
//			CSL_UART_REGS->IER &= ~0x0001;        // RX interrupt disabled
			taskList |= UARTPROCESSING;
		}
	}
}


/*
 *  UART_init( )
 *    Open UART handle and configure it for 115200bps, 8N1
 * 
 */
void EZDSP5535_UART_init( )
{
    CSL_GPIO_REGS->IODIR2 |= 0x0001;		// red LED

	CSL_GPIO_REGS->IODIR1 = 0x8000;
	CSL_GPIO_REGS->IOOUTDATA1 = 0x0000; /* FT2232 enable */

//	Transmitter and receiver are disabled and in reset state.
	CSL_UART_REGS->PWREMU_MGMT = 0;

    /*  
     * Configuring for baud rate of 115200
     * Divisor = UART input clock frequency / (Desired baud rate*16)
     * = 100MHz / 115200 / 16
     * = 54 = 0x36
     */
	CSL_UART_REGS->LCR = 0x80;			// DLAB=1
	CSL_UART_REGS->DLL = 0x36;
	CSL_UART_REGS->DLH = 0x00;
	CSL_UART_REGS->FCR = 0x08;			// Clear UART TX & RX FIFOs, DMAMODE1
	CSL_UART_REGS->LCR = 0x03;			// 8-bit words,
										// 1 STOP bit generated,
										// No Parity, No Stick paritiy,
										// No Break control

	CSL_UART_REGS->MCR = 0x0000;		// RTS & CTS disabled,
										// Loopback mode disabled,
										// Autoflow disabled

	CSL_UART_REGS->IER = 0x0001;		// RX interrupt enabled
//	Transmitter and receiver are enabled
	CSL_UART_REGS->PWREMU_MGMT = 0x6000;

	CSL_CPU_REGS->IER0 |= 0x0040;		// UART interrupt enabled

	taskList &= ~UARTPROCESSING;
}

void uartSend(int voltage)
{
	int ptr = 0;
	int c, i;

	tx_buffer[ptr++] = '#';
	
	for(i = 0; i < 4; i++) {
		c = voltage;
		voltage <<= 4;
		c = (c >> 12) & 0x0f;
		if(c < 10)	c = c + '0';
		else c = c - 10 + 'A';
		tx_buffer[ptr++] = c;
	}
	tx_buffer[ptr++] = '$';

	XmtSize = ptr;
	xmtCounter = 0;

	CSL_UART_REGS->IER |= 0x0003;
	taskList &= ~UARTPROCESSING;
}

