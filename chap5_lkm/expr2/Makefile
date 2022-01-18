BASEINCLUDE ?= /lib/modules/`uname -r`/build

simple_lkm_param-objs := simlkm_param.o

obj-m := simple_lkm_param.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;
