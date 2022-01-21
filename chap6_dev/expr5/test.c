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
	char buf[10] = {0};
	int devfd;
	int ret;
	size_t len;
	char data[20] = "apple123456";

	len = sizeof(data) - 1;

	devfd = open(DEV_NAME, O_RDWR | O_NONBLOCK);
	if(devfd == -1)
		err_exit("open");

	ret = read(devfd, buf, len);
	if(ret != len)
		perror("read");

	ret = write(devfd, data, len);
	if(ret != len){
		printf("have write %d bytes\n", ret);
		perror("write");
	}

	ret = write(devfd, data, len);
	if(ret != len){
		printf("have write %d bytes\n", ret);
		perror("write");
	}

	ret = read(devfd, buf, len);
	printf("read bytes: %d\n", ret);
	printf("buf: %s\n", buf);

	close(devfd);
	exit(0);
}
