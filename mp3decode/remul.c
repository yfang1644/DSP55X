/****************************************************************************/
/*  remul.c          v4.3.8                                                 */
/*  Copyright (c) 2002-2010  Texas Instruments Incorporated                 */
/****************************************************************************/
 
unsigned long _remul (unsigned long x1, unsigned long x2)
{
    register int i;
    register unsigned long num;
    register unsigned long den;
    register int shift;
    unsigned long num32;
 
    shift = _llnorm(x2) - _llnorm(x1);
 
    if (x1 < x2) return x1;
    if (x1 == 0) return 0;
    if (x2 == 0) return 0;
 
    num = x1;
    den = x2 << shift;
 
    num32 = (_llnorm(x1) == -1);
 
    if (num32) {
        if(den > num) { den >>= 1; num -= den; }
        else          { num -= den; den >>= 1; }
    } else
        shift++;
 
    for (i = 0; i < shift; i++) {
    	if (num < den)	num <<= 1;
    	else			num = ((num - den) << 1) | 1;
    }
    return num >> shift;
}

