#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

main()
{
	int fd;
	int i;
	unsigned int type;
	short buf[1000];

	fd=open("/dev/rec722",O_RDWR);
	printf("%d\n", fd);
	
	ioctl(fd, 0x01123344, i);
	// 1st Byte 00: set, 01: get ->bmRequestType (0x40 or 0xc0)
	// 2nd byte bRequest
	// 3rd and 4th byte(16bit word) wValue

	i = read(fd, buf, 512);
	printf("%d\n", i);
	for(i = 0; i < 100; i++)	printf("%x ", buf[i]);
	for(i = 0; i < 100; i++)	buf[i] = i - 100;
	printf("\n");
	i = write(fd, buf, 64);
	printf("%d\n", i);

	close(fd);
}

