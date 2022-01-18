BASEINCLUDE ?= /lib/modules/`uname -r`/build

chdev-demo-objs := chdev_demo.o

obj-m := chdev-demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;
