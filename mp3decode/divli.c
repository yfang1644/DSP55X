/****************************************************************************/
/*  divli.c          v3.3.2                                                 */
/*  Copyright (c) 2002-2006  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <c55x.h>

long _divli(long x1, long x2)
{
    int negative_quotient = (long)(x1 ^ x2) < 0;
    unsigned long t1, t2;
    long result;

	t1 = _labss(x1);
	t2 = _labss(x2);

	result = t1 / t2;
	return (negative_quotient)? -result : result;
}
