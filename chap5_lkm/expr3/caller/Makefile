BASEINCLUDE ?= /lib/modules/`uname -r`/build

caller_mod-objs := caller.o

obj-m := caller_mod.o

KBUILD_EXTRA_SYMBOLS += /mnt/mylab/chap5_lkm/expr3/callee/Module.symvers

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;
