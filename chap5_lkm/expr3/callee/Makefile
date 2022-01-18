BASEINCLUDE ?= /lib/modules/`uname -r`/build

callee_mod-objs := callee.o

obj-m := callee_mod.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;
