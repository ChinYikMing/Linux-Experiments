#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

int main(){
	int ret;
	struct pollfd fds[2];
	char buf0[8];
	char buf1[8];

	fds[0].fd = open("/dev/demo0", O_RDWR);
	if(fds[0].fd == -1)
		goto fail;
	fds[0].events = POLLIN;
	fds[0].revents = 0;


	fds[1].fd = open("/dev/demo1", O_RDWR);
	if(fds[1].fd == -1)
		goto fail;
	fds[1].events = POLLIN;
	fds[1].revents = 0;

	while(1){
		ret = poll(fds, 2, -1);
		if(ret == -1)
			goto fail;

		if(fds[0].revents & POLLIN){
			ret = read(fds[0].fd, buf0, 8);
			if(ret == -1)
				goto fail;
			printf("read from /dev/demo0: %s\n", buf0);
		}

		if(fds[1].revents & POLLIN){
			ret = read(fds[1].fd, buf1, 8);
			if(ret == -1)
				goto fail;
			printf("read from /dev/demo1: %s\n", buf1);
		}
	}

fail:
	perror("error");
	exit(1);
}
