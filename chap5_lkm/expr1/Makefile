BASEINCLUDE ?= /lib/modules/`uname -r`/build

simple_lkm-objs := simlkm.o

obj-m := simple_lkm.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;
