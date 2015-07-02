#include "ezdsp5535.h"
#include "radio.h"

extern biquad filterCoeffs[];
extern Int16 dbuffer_l[], dbuffer_r[];

void InitFilter(void) {
	int i;

	for(i = 0; i < EQBAND; i++) {
		filterCoeffs[i].a1 = 0;
		filterCoeffs[i].a2 = 0;
		filterCoeffs[i].b0 = 2048;
		filterCoeffs[i].b1 = 0;
		filterCoeffs[i].b2 = 0;
	}
}

void clearBuf(void) {
	int i;

	for(i = 0; i < 2*EQBAND; i++)
		dbuffer_l[i] = dbuffer_r[i] = 0;
}


/*          2              2 
 *         s  + g*a*s0*s + s0
 * H(s) = -----------------------
 *          2              2 
 *         s  +   a*s0*s + s0
 *
 *
 *                  -1      -2
 *         b0 + b1*z  + b2*z
 * H(z) = -------------------------
 *                  -1      -2 
 *          1 + a1*z  + a2*z
 */

float freq[] = {	/* 500, 1k, 2k, 4k, 8k */
	0.03273661041,	/* w = tan(PI*f/Fs)    */
	0.06554346282,	/* FS = 48kHz          */
	0.13165249759,
	0.26794919243,
	0.57735026919};

void equalizer(Int16 gain, int band)	// gain (dB, -2,-1,0,1,2=-6dB,-3dB...)
{
	biquad *coeff = (biquad *)&filterCoeffs[band];
	float a0, a1, a2;
	float b0, b1, b2;
	float w2, aw;
	float g, a = 1.6, s2 = 1.0;

	if(gain & 1)	s2 = 1.4142;
	gain >>= 1;
	if(gain > 0)
		g = (float)(1 << gain) * s2;
	else
		g = 1.0 / ((float)(1 << (-gain)) * s2);

	w2 = freq[band] * freq[band];
	aw = a * freq[band];
	a0 = w2 + aw + 1;
	a1 = 2 * (w2 - 1) / a0;
	a2 = (w2 - aw + 1) / a0;
	b0 = (w2 + g * aw + 1) / a0;
	b1 = a1;
	b2 = (w2 - g * aw + 1) / a0;

	coeff->a1 = (Int16)(a1 * 16384);		/* 2>|a1| */
	coeff->a2 = (Int16)(a2 * 32768);	/*  a2<1   */
	coeff->b0 = (Int16)(b0 * 2048);     /* Max. 8.2 */
	coeff->b1 = (Int16)(b1 * 16384);
	coeff->b2 = (Int16)(b2 * 2048);     /* Max. -7.3 */
	clearBuf();
}
