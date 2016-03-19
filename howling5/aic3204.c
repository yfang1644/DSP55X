//////////////////////////////////////////////////////////////////////////////
// * File name: aic3204.c
//////////////////////////////////////////////////////////////////////////////

#define AIC3204_I2C_ADDR 0x18

#include "ezdsp5535_i2c.h"


/*
 *
 *  AIC3204_rset( regnum, regval )
 *
 *      Set codec register regnum to value regval
 *
 */
Int16 AIC3204_rset(Uint8 regnum, Uint8 regval)
{
	Uint8 cmd[2];
	cmd[0] = regnum;				// 7-bit Device Register
	cmd[1] = regval;				// 8-bit Register Data

	EZDSP5535_waitusec(300);

	return EZDSP5535_I2C_write( AIC3204_I2C_ADDR, cmd, 2);
}

/*
 *  AIC3204 init
 *
 *      Route codec from LINE IN to LINE OUT
 */
void aic3204_init(Uint8 rate, Int8 micGain)
{
	/* Configure AIC3204 */
	AIC3204_rset( 0,  0x00 );  // Select page 0
	AIC3204_rset( 1,  0x01 );  // Reset codec
//	EZDSP5535_waitusec(1000);  // Wait 1ms after reset
	AIC3204_rset( 0,  0x01 );  // Select page 1
	AIC3204_rset( 1,  0x08 );  // Disable crude AVDD generation from DVDD
	AIC3204_rset( 2,  0x01 );  // Enable Analog Blocks, use LDO power
	AIC3204_rset( 123,0x05 );  // Force reference to power up in 40ms
//	EZDSP5535_waitusec(40000); // Wait at least 40ms

	AIC3204_rset( 0,  0x00 );  // Select page 0
	/* PLL and Clocks config and Power Up  */
	AIC3204_rset(27, 0x0d);		// BCLK and WCLK are set as o/p; AIC3204(Master)
	AIC3204_rset(28, 0x00);		// Data ofset = 0
	AIC3204_rset( 4, 0x03);		// PLL setting: PLLCLK <- MCLK, CODEC_CLKIN <-PLL CLK
	AIC3204_rset( 6, 0x07);		// PLL setting: J=7
	AIC3204_rset( 7, 0x06);		// PLL setting: HI_BYTE(D=1680)
	AIC3204_rset( 8, 0x90);		// PLL setting: LO_BYTE(D=1680)

	// For 32 bit clocks per frame in Master mode ONLY
	// BCLK=DAC_CLK/N =(12288000/8) = 1.536MHz = 32*fs
	// MCLK = 12MHz(fangyuang, 2012-06-22)
	AIC3204_rset( 30, 0x88 );

	AIC3204_rset( 5,  0x91 );  // PLL setting: Power up PLL, P=1 and R=1
//	EZDSP5535_waitusec(10000); // Wait for PLL to come up
	AIC3204_rset( 13, 0x00 );  // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
	AIC3204_rset( 14, 0x80 );  // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
	AIC3204_rset( 20, 0x80 );  // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
	/* sampling rate = CLOCK/(NADC*MADC) */
	AIC3204_rset( 11, 0x80+rate);  // Power up NDAC and set NDAC value to rate
	AIC3204_rset( 12, 0x87 );      // Power up MDAC and set MDAC value to 7
	AIC3204_rset( 18, 0x87 );      // Power up NADC and set NADC value to 7
	AIC3204_rset( 19, 0x80+rate);  // Power up MADC and set MADC value to rate

	AIC3204_rset( 64, 0);		// Left, right vol independent
	AIC3204_rset( 65, 30);  // Left DAC gain to 10dB VOL
	AIC3204_rset( 66, 30);  // Right DAC gain to 10dB VOL
	AIC3204_rset( 63, 0xd4 );  // Power up left,right data paths and set channel
	AIC3204_rset( 81, 0xc0 );  // Powerup Left and Right ADC
	AIC3204_rset( 82, 0x00 );  // Unmute Left and Right ADC, fine gain=0dB
	AIC3204_rset( 83, 30 );  // Left ADC gain, -12dB~+20dB, 01100111~00101000, stepsize 0.5dB
	AIC3204_rset( 84, 30 );  // Right ADC gain, -12dB~+20dB, 01100111~00101000, stepsize 0.5dB

	/* DAC ROUTING and Power Up */
	AIC3204_rset( 0,  0x01 );	// Select page 1
	AIC3204_rset( 12, 0x08 );	// LDAC AFIR routed to HPL
	AIC3204_rset( 13, 0x08 );	// RDAC AFIR routed to HPR
	AIC3204_rset( 16, 0x00 );	// Unmute HPL , 0dB gain
	AIC3204_rset( 17, 0x00 );	// Unmute HPR , 0dB gain
	AIC3204_rset( 9 , 0x30 );	// Power up HPL,HPR
//	EZDSP5535_waitusec(100 );	// Wait

	/* ADC ROUTING and Power Up */
	AIC3204_rset( 51, 0x40 );	// MIC BIAS

	AIC3204_rset( 52, 0x30 );	// STEREO 1 Jack
								// IN2_L to LADC_P through 40 kohm
//	AIC3204_rset( 55, 0x30 );	// IN2_R to RADC_P through 40 kohmm
	AIC3204_rset( 55, 0x03 );	// IN2_L to RADC_P through 40 kohmm
	AIC3204_rset( 54, 0x03 );	// CM_1 (common mode) to LADC_M through 40 kohm
	AIC3204_rset( 57, 0xc0 );	// CM_1 (common mode) to RADC_M through 40 kohm
	AIC3204_rset( 59, micGain );// MIC_PGA_L 0~47.5dB, 0.5dB stepsize
	AIC3204_rset( 60, micGain );// MIC_PGA_R 0~47.5dB, 0.5dB stepsize
}

void beep()
{
	AIC3204_rset(0, 0);					// page 0
	AIC3204_rset(60, 25);
	AIC3204_rset(73, 0x01);				// length(23:16)
	AIC3204_rset(74, 0x77);				// length(15:8)
	AIC3204_rset(75, 0x00);				// length(7:0)

	AIC3204_rset(76, 0x10);				// 32768*sin(2 pi F/Fs)  (15:8)
	AIC3204_rset(77, 0xb5);				//  (7:0)

	AIC3204_rset(78, 0x7e);				// 32768*cos(2 pi F/Fs)  (15:8)
	AIC3204_rset(79, 0xe7);				//  (7:0)

	AIC3204_rset(71, 0x80);				// left volume
	AIC3204_rset(72, 0x00);				// master and right volume
}

long inputFilter1[] = {			// hilbert
	       0,   -46448,        0,  -127415,        0,
	 -321140,        0,  -703919,        0, -1540281,
	       0, -5256648,        0,  5256648,        0,
	 1540281,        0,   703919,        0,   321140,
	       0,   127415,        0,    46448,        0
};

long inputFilter2[] = {			// high pass
	-170962, -120952, -158197, -198677, -240740,
	-283329, -324663, -362985, -397045, -425337,
	-446369, -459312, 7924907, -459312, -446369,
	-425337, -397045, -362985, -324663, -283329,
	-240740, -198677, -158197, -120952, -170962
};

void FIR_filters(int active)
{
	int i, j;
	Uint32 p;
	Uint8 val;

	AIC3204_rset(0, 0);				// page 0
	AIC3204_rset(61, 3);			// RBR_R3
	AIC3204_rset(81, 0);			// ADC power down

	AIC3204_rset(0, 8);				// page 8
	if(active == 0) {
		AIC3204_rset(1, 4);			// disable
		AIC3204_rset(0, 0);			// page 0
		AIC3204_rset(81, 0xc0);		// ADC power up
		return;
	}

	AIC3204_rset(1, 0);
	j = 36;							// fir0 left at register 36,37,38
	for(i = 0; i < 23; i++) {
		p = inputFilter1[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	AIC3204_rset(0, 9);				// page 9
	j = 8;							// fir23 left at register 8,9,10
	for(i = 23; i < 25; i++) {
		p = inputFilter1[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	j = 44;							// fir0 right at register 44,45,46
	for(i = 0; i < 21; i++) {
		p = inputFilter2[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	AIC3204_rset(0, 10);			// page 10
	j = 8;							// fir21 right at register 8,9,10
	for(i = 21; i < 25; i++) {
		p = inputFilter2[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	AIC3204_rset(0, 0);				// page 0
	AIC3204_rset(81, 0xc0);			// ADC power up
}


/*
                     -1       -2
        N0 + 2 * N1 z   + N2 z
H(z) = ----------------------------
         23          -1       -2
        2  - 2 * D1 z   - D2 z
*/
// N0,N1,N2,D1,d2 left, right
long inputFilter[] = {
	0x0498b6, 0x0498b6, 0x0498b6, 0x6f93b8, 0x8e75b4,
	0x04268e, 0x04268e, 0x04268e, 0x64c09d, 0xa5e48d,
	0x03d58d, 0x03d58d, 0x03d58d, 0x5d1253, 0xb68524,
	0x03a20a, 0x03a20a, 0x03a20a, 0x582fe7, 0xc1180b,
	0x038911, 0x038911, 0x038911, 0x55d1bd, 0xc63841,

	0x742c6e, 0x8bd392, 0x742c6e, 0x6f93b8, 0x8e75b5,
	0x68e72a, 0x9718d6, 0x68e72a, 0x64c09c, 0xa5e48e,
	0x60e7e0, 0x9f1820, 0x60e7e0, 0x5d1253, 0xb68524,
	0x5bd1f0, 0xa42e10, 0x5bd1f0, 0x582fe6, 0xc1180b,
	0x595ace, 0xa6a532, 0x595ace, 0x55d1bd, 0xc63841
};

void adcFilters()
{
	int i, j;
	Uint32 p;
	Uint8 val;

	AIC3204_rset(0, 0);				// page 0
	AIC3204_rset(61, 2);			// RBR_R2
	AIC3204_rset(81, 0);			// ADC power down

	AIC3204_rset(0, 8);				// page 8
	AIC3204_rset(1, 0);

	j = 36;							// biquad A left at register 36,37,38
	for(i = 0; i < 23; i++) {
		p = inputFilter[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	AIC3204_rset(0, 9);				// page 9
	j = 8;							// biquad E left D1 at register 8,9,10
	for(i = 23; i < 25; i++) {
		p = inputFilter[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	j = 44;							// biquad A right N0 at register 44,45,46
	for(i = 25; i < 46; i++) {
		p = inputFilter[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	AIC3204_rset(0, 10);			// page 10
	j = 8;							// biquad E right N1 at register 8,9,10
	for(i = 46; i < 50; i++) {
		p = inputFilter[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	AIC3204_rset(0, 0);				// page 0
	AIC3204_rset(81, 0xc0);			// ADC power up
}

// N0,N1,N2,D1,d2 left, right
long outputFilter[] = {
	0x7fffff, 0x000000, 0x000000, 0x000000, 0x000000,
	0x0498b6, 0x0498b6, 0x0498b6, 0x6f93b8, 0x8e75b4,	// lowpass 3000Hz
	0x04268e, 0x04268e, 0x04268e, 0x64c09d, 0xa5e48d,
	0x03d58d, 0x03d58d, 0x03d58d, 0x5d1253, 0xb68524,
	0x03a20a, 0x03a20a, 0x03a20a, 0x582fe7, 0xc1180b,
	0x038911, 0x038911, 0x038911, 0x55d1bd, 0xc63841,

	0x7fffff, 0x000000, 0x000000, 0x000000, 0x000000,
	0x742c6e, 0x8bd392, 0x742c6e, 0x6f93b8, 0x8e75b5,	// highpass 3000Hz
	0x68e72a, 0x9718d6, 0x68e72a, 0x64c09c, 0xa5e48e,
	0x60e7e0, 0x9f1820, 0x60e7e0, 0x5d1253, 0xb68524,
	0x5bd1f0, 0xa42e10, 0x5bd1f0, 0x582fe6, 0xc1180b,
	0x595ace, 0xa6a532, 0x595ace, 0x55d1bd, 0xc63841
};

void dacFilters()
{
	int i, j;
	Uint32 p;
	Uint8 val;

	AIC3204_rset(0, 0);				// page 0
	AIC3204_rset(60, 25);			// RBR_P25
	AIC3204_rset(63, 0);			// DAC power down

	AIC3204_rset(0, 44);			// page 44
	AIC3204_rset(1, 0);

	j = 12;							// biquad A left at register 12,13,14
	for(i = 0; i < 29; i++) {
		p = outputFilter[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	AIC3204_rset(0, 45);			// page 45
	j = 8;							// biquad F left D2 at register 8,9,10
	p = outputFilter[29];
	val = (p >> 16) & 0xff;
	AIC3204_rset( j++, val);
	val = (p >> 8) & 0xff;
	AIC3204_rset( j++, val);
	val = p & 0xff;
	AIC3204_rset( j++, val);

	j = 20;							// biquad A right N0 at register 20,21,22
	for(i = 30; i < 57; i++) {
		p = outputFilter[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}

	AIC3204_rset(0, 46);			// page 46
	j = 8;							// biquad F right N2 at register 8,9,10
	for(i = 57; i < 60; i++) {
		p = outputFilter[i];
		val = (p >> 16) & 0xff;
		AIC3204_rset( j++, val);
		val = (p >> 8) & 0xff;
		AIC3204_rset( j++, val);
		val = p & 0xff;
		AIC3204_rset( j++, val);
		j++;
	}
	AIC3204_rset(0, 0);				// page 0
	AIC3204_rset(63, 0xd4);			// DAC power up
}

