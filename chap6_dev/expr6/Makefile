BASEINCLUDE ?= /lib/modules/`uname -r`/build

miscdev-demo-objs := miscdev_demo.o

obj-m := miscdev-demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;
