#include <stdio.h>
#define	BITRATE	16 			// BITRATE in kbps
#define MAX_BITS_PER_FRAME 960

unsigned short bufr[MAX_BITS_PER_FRAME];
unsigned short bufw[MAX_BITS_PER_FRAME];

int main(int argc, char *argv[])
{
	FILE *fpr, *fpw1, *fpw2;
	int i, n;
	int number_of_bits_per_frame = (BITRATE * 20);		// bits in 20ms/frame 
	int number_of_bytes_per_frame = number_of_bits_per_frame>>3;
	int number_of_16bit_words_per_frame = number_of_bits_per_frame>>4;
	unsigned short c1, c2;

	fpr = fopen(argv[1], "rb");
	fpw1 = fopen(argv[2], "wb");
	fpw2 = fopen(argv[3], "wb");

	for(;;) {
		n = fread(bufr, 2, number_of_16bit_words_per_frame, fpr);
		if(n == 0)	break;
		for(i = 0; i < n; i++) {
			c1 = (bufr[i] >> 8) & 0x00ff;
			c2 = (bufr[i] << 8) & 0xff00;
			bufw[i] = c1 | c2;
//			bufw[i] = bufr[i];
		}
		fwrite(bufw, 2, n, fpw1);
		n = fread(bufr, 2, number_of_16bit_words_per_frame, fpr);
		if(n == 0)	break;
		for(i = 0; i < n; i++) {
			c1 = (bufr[i] >> 8) & 0x00ff;
			c2 = (bufr[i] << 8) & 0xff00;
			bufw[i] = c1 | c2;
//			bufw[i] = bufr[i];
		}
		fwrite(bufw, 2, n, fpw2);
	}
	fcloseall();

	return 0;
}

