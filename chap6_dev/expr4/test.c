#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define DEV_NAME "/dev/miscdev_demo"
#define err_exit(syscall) { \
	perror(syscall); \
	exit(1); \
}

int main(){
	char buf[8] = {0};
	int devfd;
	int ret;
	size_t len;
	char data[] = "apple1254";

	len = sizeof(data) - 1;

	devfd = open(DEV_NAME, O_RDWR);
	if(devfd == -1)
		err_exit("open");

	// write data to device
	ret = write(devfd, data, len);
	if(ret == -1)
		err_exit("write");

	ret = read(devfd, buf, len);
	if(ret == -1)
		err_exit("read");
	printf("read byte: %d, read data: %s\n", ret, buf);

	close(devfd);

	exit(0);
}
