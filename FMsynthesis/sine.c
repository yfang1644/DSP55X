#include "tistdtypes.h"
#include "sine_tbl.h"

#define TBLNUM          13      /* sine table size = 2^13 */
#define SINETBLSIZE     (1 << TBLNUM)       /* 1/4 of one sine cycle */

Int16 sine(Int16 arc)		// arc = -32768(-PI) 32767(+PI)
{
	Int16 quad, ptr;
	Int16 ret;
	Int16 sign = arc & 0x8000;

	arc  = _abss(arc);
	quad = arc & 0x6000;
	ptr = arc & (SINETBLSIZE-1);
	switch(quad) {
		case 0x0000:
			ret = sine_table[ptr];
			break;
		case 0x2000:
			ret = (ptr == 0) ? 0x7fff : sine_table[SINETBLSIZE - ptr];
			break;
		case 0x4000:
			ret = -sine_table[ptr];
			break;
		case 0x6000:
			ret = (ptr == 0) ? 0x8000 : -sine_table[SINETBLSIZE - ptr];
			break;
		default:
			break;
	}
	if(sign)
		ret = _sneg(ret);
	return ret;
}

