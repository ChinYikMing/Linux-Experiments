BASEINCLUDE ?= /lib/modules/`uname -r`/build

poll-chdev-demo-objs := poll_chdev_demo.o

obj-m := poll-chdev-demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;
