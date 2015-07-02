#include <math.h>
#include <stdio.h>
#define	PI	3.1415926535897932
main()
{
	short i, x;
	for(i = 0; i < 8192; i++) {
		if((i%8) == 0)
			printf("\n");
		x = 32768.* sin(2*PI*i/32768);
		printf("0x%.04x, ", (unsigned)x);
	}
}
