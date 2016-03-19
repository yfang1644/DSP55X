#include <stdio.h>

#define	DCT_LENGTH	640
unsigned short buf1[DCT_LENGTH], buf2[DCT_LENGTH];
unsigned short bufw[DCT_LENGTH*2];

int main(int argc, char *argv[])
{
	FILE *fpr, *fpw;
	int i, n;
	unsigned short c1, c2;

	fpr = fopen(argv[1], "rb");
	fpw = fopen(argv[2], "wb");

	for(;;) {
		n = fread(buf1, 2, DCT_LENGTH, fpr);
		n = fread(buf2, 2, DCT_LENGTH, fpr);
		for(i = 0; i < n; i++) {
			c1 = (buf1[i] >> 8) & 0x00ff;
			c2 = (buf1[i] << 8) & 0xff00;
			bufw[2 * i + 0] = c1 | c2;

			c1 = (buf2[i] >> 8) & 0x00ff;
			c2 = (buf2[i] << 8) & 0xff00;
			bufw[2 * i + 1] = c1 | c2;
		}
		fwrite(bufw, 2, 2*n, fpw);
		if(n != DCT_LENGTH)
			break;
	}
	fcloseall();

	return 0;
}
