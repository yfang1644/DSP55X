//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_i2c.c
//////////////////////////////////////////////////////////////////////////////
#include "ezdsp5535_i2c.h"

/*
 *  EZDSP5535_I2C_init( )
 *
 *      Enable and initalize the I2C module.
 *      The I2C clk is set to run at 100 KHz.
 *         prescaled module clock = (120MHz/ICPSC)
 *            prescaled module clock must be 6.7 to 13.3 MHz.
 *         bus clock = prescaled module clock /((CLKH+d)+(CLKL+d))
 *            d = 7 when ICPSC = 0
 *            d = 6 when ICPSC = 1
 *            d = 5 when ICPSC = others
 */
Int16 EZDSP5535_I2C_init( )
{
    CSL_I2C_0_REGS->ICMDR = 0x0400;		// Master mode, Reset I2C
    CSL_I2C_0_REGS->ICPSC = 20;			// Config prescaler for 12MHz
    CSL_I2C_0_REGS->ICCLKL= 5;			// Config clk LOW for 20kHz
    CSL_I2C_0_REGS->ICCLKH= 5;			// Config clk HIGH for 20kHz

    CSL_I2C_0_REGS->ICMDR = 0x0420;		// Release from reset; Master, Transmitter, 7-bit address

	return 0;
}

Int16 EZDSP5535_I2C_close( )
{
	CSL_I2C_0_REGS->ICMDR = 0x0;	// Reset I2C
	return 0;
}

/*
 *  EZDSP5535_I2C_reset( )
 *
 *      Resets I2C module
 */
Int16 EZDSP5535_I2C_reset( )
{
    EZDSP5535_I2C_close( );
    EZDSP5535_I2C_init( );
    return 0;
}

/*
 *  EZDSP5535_I2C_write( i2c_addr, data, len )
 *
 *      I2C write in Master mode: Writes to I2c device with address 
 *      "i2c_addr" from the location of "data" for length "len".
 * 
 *      Uint16 i2c_addr    <- I2C slave address
 *      Uint16* data       <- I2C data ptr
 *      Uint16 len         <- # of bytes to write
 */
Int16 EZDSP5535_I2C_write( Uint16 i2c_addr, Uint8* data, Uint16 len )
{
	Int16 timeout, i;
	Int16 i2c_timeout = 0x7fff;

	//I2C_IER = 0x0000;
	CSL_I2C_0_REGS->ICCNT = len;		// Set length
	CSL_I2C_0_REGS->ICSAR = i2c_addr;	// Set I2C slave address
	CSL_I2C_0_REGS->ICMDR = 0			/* 8bit-word */
						|0x4000			/* FREE */
						|0x2000			/* STT */
						|0x0200			/* TRX */
						|0x0400			/* MST */
						|0x0020;		/* IRS */

#define MDR_STT			0x2000
#define MDR_TRX			0x0200
#define MDR_MST			0x0400
#define MDR_IRS			0x0020
#define MDR_FREE		0x4000
#define STR_XRDY		0x0010
#define STR_RRDY		0x0008
#define MDR_STP 		0x0800

	EZDSP5535_waitusec(1);              // Short delay

	for (i = 0; i < len; i++)
	{
		CSL_I2C_0_REGS->ICDXR = data[i];	// Write
		timeout = i2c_timeout;
		do {
			if ( timeout-- < 0 ) {
				EZDSP5535_I2C_reset( );
				return -1;
			}
		} while (( CSL_I2C_0_REGS->ICSTR & 0x10 ) == 0);	// Wait for Tx Ready
	}

	CSL_I2C_0_REGS->ICMDR |= 0x0800;		// Generate STOP

	EZDSP5535_waitusec(10);

	return 0;
}

/*
 *
 *  EZDSP5535_I2C_read( i2c_addr, data, len )
 *
 *      I2C read in Master mode: Reads from I2c device with address 
 *      "i2c_addr" and stores to the location of "data" for length "len".
 * 
 *      Uint16 i2c_addr    <- I2C slave address
 *      Uint16* data       <- I2C data ptr
 *      Uint16 len         <- # of bytes to write
 */
Int16 EZDSP5535_I2C_read( Uint16 i2c_addr, Uint8* data, Uint16 len )
{
    Int16 timeout, i;
	Int16 i2c_timeout = 0x7fff;

	CSL_I2C_0_REGS->ICCNT = len;		// Set length
	CSL_I2C_0_REGS->ICSAR = i2c_addr;	// Set I2C slave address
	CSL_I2C_0_REGS->ICMDR = 0			/* 8bit-word */
						|0x4000			/* FREE */
						|0x2000			/* STT */
						|0x0400			/* MST */
						|0x0020;		/* IRS */

    EZDSP5535_waitusec(1);            // Short delay

	for (i = 0; i < len; i++) {
		timeout = i2c_timeout;

		do {						//Wait for Rx Ready 
			if ( timeout-- < 0 ) {
				EZDSP5535_I2C_reset( );
				return -1;
			}
		} while (( CSL_I2C_0_REGS->ICSTR & 0x08 ) == 0);// Wait for Rx Ready

		data[i] = CSL_I2C_0_REGS->ICDRR;            // Read
	}

	CSL_I2C_0_REGS->ICMDR |= 0x0800;		// Generate STOP

	EZDSP5535_waitusec(10);
	return 0;
}
