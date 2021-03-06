//////////////////////////////////////////////////////////////////////////////
// * File name: aic3204.c
//////////////////////////////////////////////////////////////////////////////

#define AIC3204_I2C_ADDR 0x18

#include "ezdsp5535_i2c.h"

/*
 *
 *  AIC3204_rget( regnum, *regval )
 *
 *      Return value of codec register regnum
 *
 */
Int16 AIC3204_rget( Int8 regnum, Int8* regval )
{
    Int16  retcode = 0;

    retcode |= EZDSP5535_I2C_write( AIC3204_I2C_ADDR, &regnum, 1 );
    retcode |= EZDSP5535_I2C_read( AIC3204_I2C_ADDR, regval, 1 );

    EZDSP5535_waitusec( 1 );
    return retcode;
}

/*
 *
 *  AIC3204_rset( regnum, regval )
 *
 *      Set codec register regnum to value regval
 *
 */
Int16 AIC3204_rset( Uint8 regnum, Uint8 regval )
{
    Int8 cmd[2];
    cmd[0] = regnum;				// 7-bit Device Register
    cmd[1] = regval;				// 8-bit Register Data

    EZDSP5535_waitusec( 300 );

    return EZDSP5535_I2C_write( AIC3204_I2C_ADDR, cmd, 2 );
}

struct codec_param {
	Uint8 PLLJ;
	Uint16 PLLD;
	Uint8 MADC;
	Uint8 NADC;
	Uint8 AOSR;
	Uint8 MDAC;
	Uint8 NDAC;
	Uint8 DOSR;
} ;

/*
 *  AIC3204 init
 * 
 *      Route codec from LINE IN to LINE OUT
 */
void aic3204_init(Uint8 rate, Int8 micGain)
{
	struct codec_param p[2] = {{7,  560, 3, 5, 128, 3, 5, 128},	// 44.1kHz
						{7, 1680, 2, 7, 128, 7, 2, 128}};	// 48kHz
	int idx = 0;

    /* Configure AIC3204 */
    AIC3204_rset( 0,  0x00 );  // Select page 0
    AIC3204_rset( 1,  0x01 );  // Reset codec
    EZDSP5535_waitusec(1000);  // Wait 1ms after reset
    AIC3204_rset( 0,  0x01 );  // Select page 1
    AIC3204_rset( 1,  0x08 );  // Disable crude AVDD generation from DVDD
    AIC3204_rset( 2,  0x01 );  // Enable Analog Blocks, use LDO power
    AIC3204_rset( 123,0x05 );  // Force reference to power up in 40ms
    EZDSP5535_waitusec(40000); // Wait at least 40ms

    AIC3204_rset( 0,  0x00 );  // Select page 0
    /* PLL and Clocks config and Power Up  */
    AIC3204_rset( 27, 0x0d );  // BCLK and WCLK are set as o/p; AIC3204(Master)
    AIC3204_rset( 28, 0x00 );  // Data ofset = 0
    AIC3204_rset( 4,  0x03 );  // PLL setting: PLLCLK <- MCLK, CODEC_CLKIN <-PLL CLK
    AIC3204_rset( 6,  p[idx].PLLJ );  // PLL setting: J=7
    AIC3204_rset( 7,  p[idx].PLLD>>8);  // PLL setting: HI_BYTE(D=1680)
    AIC3204_rset( 8,  p[idx].PLLD&0xff);  // PLL setting: LO_BYTE(D=1680)

 	// For 32 bit clocks per frame in Master mode ONLY
	// BCLK=DAC_CLK/N =(12288000/8) = 1.536MHz = 32*fs
	// MCLK = 12MHz(fangyuang, 2012-06-22)
    AIC3204_rset( 30, 0x88 );

    AIC3204_rset( 5,  0x91 );  // PLL setting: Power up PLL, P=1 and R=1
    EZDSP5535_waitusec(10000); // Wait for PLL to come up    
    AIC3204_rset( 13, 0x00 );  // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
    AIC3204_rset( 14, p[idx].DOSR);  // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
    AIC3204_rset( 20, p[idx].AOSR);  // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
	/* sampling rate = CLOCK/(NADC*MADC) */
    AIC3204_rset( 11, 0x80+p[idx].NDAC);  // Power up NDAC
    AIC3204_rset( 12, 0x80+p[idx].MDAC);  // Power up MDAC
    AIC3204_rset( 18, 0x80+p[idx].NADC);  // Power up NADC
    AIC3204_rset( 19, 0x80+p[idx].MADC);  // Power up MADC

    AIC3204_rset( 64, 0x02 );  // Left vol=right vol
    AIC3204_rset( 65, 0x00 );  // Left DAC gain to 0dB VOL; Right tracks Left
    AIC3204_rset( 63, 0xd4 );  // Power up left,right data paths and set channel
    AIC3204_rset( 81, 0xc0 );  // Powerup Left and Right ADC
    AIC3204_rset( 82, 0x00 );  // Unmute Left and Right ADC, fine gain=0dB
    AIC3204_rset( 83, 0x20 );  // Left ADC gain, -12dB~+20dB, 01100111~00101000, stepsize 0.5dB
    AIC3204_rset( 84, 0x20 );  // Right ADC gain, -12dB~+20dB, 01100111~00101000, stepsize 0.5dB

    /* DAC ROUTING and Power Up */
    AIC3204_rset( 0,  0x01 );  // Select page 1
    AIC3204_rset( 12, 0x08 );  // LDAC AFIR routed to HPL
    AIC3204_rset( 13, 0x08 );  // RDAC AFIR routed to HPR
    AIC3204_rset( 16, 0x00 );  // Unmute HPL , 0dB gain
    AIC3204_rset( 17, 0x00 );  // Unmute HPR , 0dB gain
    AIC3204_rset( 9 , 0x30 );  // Power up HPL,HPR
    EZDSP5535_waitusec(100 );  // Wait
    
    /* ADC ROUTING and Power Up */
    AIC3204_rset( 52, 0x30 );  // STEREO 1 Jack
                               // IN2_L to LADC_P through 40 kohm
    AIC3204_rset( 55, 0x30 );  // IN2_R to RADC_P through 40 kohmm
    AIC3204_rset( 54, 0x03 );  // CM_1 (common mode) to LADC_M through 40 kohm
    AIC3204_rset( 57, 0xc0 );  // CM_1 (common mode) to RADC_M through 40 kohm
    AIC3204_rset( 59, micGain );// MIC_PGA_L 0~47.5dB, 0.5dB stepsize
    AIC3204_rset( 60, micGain );// MIC_PGA_R 0~47.5dB, 0.5dB stepsize
}

void aic3204_set_mic_gain(Int8 gain)
{
    AIC3204_rset( 0, 0x01);	// Select page 1
    AIC3204_rset(59, gain);	// MIC_PGA_L 0~47.5dB, 0.5dB stepsize
    AIC3204_rset(60, gain);	// MIC_PGA_R 0~47.5dB, 0.5dB stepsize
}

void aic3204_set_spk_gain(Int8 gain)
{
    AIC3204_rset( 0,  0x01);	// Select page 1
    AIC3204_rset(16, gain & 0x3f);	// HPL gain -6~+29dB 
    AIC3204_rset(17, gain & 0x3f);	// HPR gain -6~+29dB 
}

void aic3204_set_frequency(Int8 freq)
{
    AIC3204_rset( 0, 0x00);		// Select page 0
    AIC3204_rset(11, 0x80 + freq);// MIC_PGA_L 0~47.5dB, 0.5dB stepsize
    AIC3204_rset(19, 0x80 + freq);// MIC_PGA_R 0~47.5dB, 0.5dB stepsize
}

