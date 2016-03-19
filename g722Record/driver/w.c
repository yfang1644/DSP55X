#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

main(int argc, char *argv[])
{
	int fd;
	int i, c;
	unsigned int type;
	short buf[1000];

	c = atoi(argv[1]);
	fd=open("/dev/rec722",O_RDWR);
	printf("%d\n", fd);

	for(i = 0; i < 100; i++)	buf[i] = c*i;
	i = write(fd, buf, 512);
	printf("%d\n", i);
	printf("\n");
	close(fd);
}

