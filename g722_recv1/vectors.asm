*****************************************************************************
* Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2008
*
*   Use of this software is controlled by the terms and conditions found in the
*   license agreement under which this software has been supplied.
*****************************************************************************
;Module: INTC 
;File: vectors.asm - vector table
;**************************************************************************
;;**************************************************************************

		.C54CM_off
		.CPL_off
		.ARMS_off

;**************************************************************************
;        predefined stack operation modes  
;**************************************************************************
;        USE_RETA     : 2x16-bit fast return mode (RETA used)
;        NO_RETA      : 2x16-bit slow return mode (RETA not used)
;        C54X_STK     : 32-bit fast return mode 

;************************************************************************** 
		.ref _c_int00

		.ref	_tim_isr
		.ref	_i2s2_tx_isr
		.ref	_i2s2_rx_isr
		.ref	_dma_isr
		.ref	_spi_isr
		.ref	_gpio_isr
;**************************************************************************
		.sect ".vectors"
		.global _Reset
;**************************************************************************
_Reset:
RST:		.ivec	reset_isr, USE_RETA   ; Reset / Software Interrupt #0
NMI:		.ivec	no_handler			; Nonmaskable Interrupt
INT0:		.ivec	no_handler			; External User Interrupt #0 
INT1:		.ivec	no_handler			; External User Interrupt #1
TINT:		.ivec	_tim_isr			; Timer / Software Interrupt #4
PROG0:		.ivec	no_handler			; Programmable 0 Interrupt
UART:		.ivec	no_handler			; IIS #1 Receive Interrupt
PROG1:		.ivec	no_handler			; Programmable 1 Interrupt
DMA:		.ivec	_dma_isr			; DMA Interrupt
PROG2:		.ivec	no_handler			; Programmable 2 Interrupt
COPROCFFT:	.ivec	no_handler			; Coprocessor FFT Module Interrupt
PROG3:		.ivec	no_handler			; Programmable 3 Interrupt
LCD:		.ivec	no_handler			; LCD Interrupt
SARADC:		.ivec	no_handler			; SAR ADC Interrupt
XMT2:		.ivec	_i2s2_tx_isr		; I2S2 Tx Interrupt
RCV2:		.ivec	_i2s2_rx_isr		; I2S2 Rx Interrupt
XMT3:		.ivec	no_handler			; I2S3 Tx Interrupt
RCV3:		.ivec	no_handler			; I2S3 Rx Interrupt
RTC:		.ivec	no_handler			; RTC interrupt
SPI:		.ivec	_spi_isr			; SPI Receive Interrupt
USB:		.ivec	no_handler			; USB Transmit Interrupt
GPIO:		.ivec	_gpio_isr			; GPIO Interrupt
EMIF:		.ivec	no_handler			; EMIF Error Interrupt
I2C:		.ivec	no_handler			; IIC interrupt
BERR:		.ivec	no_handler			; Bus Error Interrupt
DLOG:		.ivec	no_handler			; Emulation Interrupt - DLOG
RTOS:		.ivec	no_handler			; Emulation Interrupt - RTOS
RTDXRCV:	.ivec	no_handler			; Emulation Interrupt - RTDX receive
RTDXXMT:	.ivec	no_handler			; Emulation Interrupt - RTDX transmit
EMUINT:		.ivec	no_handler			; Emulation monitor mode interrupt
SINT30:		.ivec	no_handler			; Software Interrupt #30
SINT31:		.ivec	no_handler			; Software Interrupt #31

		.text
reset_isr:
	;	*port(#0x1C01) = #0x0				; Clear idles
		bit (ST1, #11) = #1					; Disable interrupts
		@IVPD_L = #(RST >> 8) || mmap()
		@IVPH_L = #(RST >> 8) || mmap()
		bit(ST3,#7) = #0					; Clear bus error interrupts

		@#IFR0_L = #0xFFFF   || mmap()      ; clear all pending interrupts
		@#IFR1_L = #0xFFFF   || mmap()

;**************************************************************************
;     Reset all peripherals
;**************************************************************************
		*port(#0x1C04) = 0x20
		nop_16
		*port(#0x1C05) = 0x00FF	; Reset all peripherals
		nop_16
		goto	_c_int00

no_handler:
		goto	no_handler

