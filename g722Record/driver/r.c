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
	

	i = read(fd, buf, 512);
	printf("%d\n", i);
	for(i = 0; i < 100; i++)	printf("%x ", buf[i]);
	printf("\n");
	close(fd);
}

