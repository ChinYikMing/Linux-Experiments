BASEINCLUDE ?= /lib/modules/`uname -r`/build

sig-chdev-demo-objs := sig_chdev_demo.o

obj-m := sig-chdev-demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;
