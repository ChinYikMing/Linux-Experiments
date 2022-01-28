#define _GNU_SOURCE
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
#include <signal.h>

static int fd;

void sig_handler(int sig, siginfo_t *info, void *uctx){
	int ret;
	char buf[8];

	if(sig == SIGIO){ // our desired signal
		if(info->si_band & POLLIN){
			printf("FIFO is not empty\n");
			if((ret = read(fd, buf, sizeof(buf))) != -1){
				buf[ret] = 0;
				puts(buf);
			}
		}

		if(info->si_band & POLLOUT){
			printf("FIFO is not full\n");
		}
	}
}

int main(){
	int ret;
	int flag;
	struct sigaction act, oldact;

	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGIO);
	act.sa_flags = SA_SIGINFO; // call sa_sigaction callback which has more info instead of sa_handler
	act.sa_sigaction = sig_handler;
	if(sigaction(SIGIO, &act, &oldact) == -1)
		goto fail;

	fd = open("/dev/demo0", O_RDWR);
	if(fd == -1)
		goto fail;

	// set the device to propagate the signal to current process
	if(fcntl(fd, F_SETOWN, getpid()) == -1)
		goto fail;

	// set which signal is OK(here is SIGIO) to be propagated to current process
	if(fcntl(fd, F_SETSIG, SIGIO) == -1)
		goto fail;

	// enable asynchronous signal(below is the standard way to set the flag: 1. get the old flag 2. old flag | new flag)
	if((flag = fcntl(fd, F_GETFL)) == -1)
		goto fail;
	if(fcntl(fd, F_SETFL, flag | FASYNC) == -1)
		goto fail;

	// waiting for SIGIO
	while(1)
		sleep(1);

fail:
	perror("fail");
	exit(1);
}
