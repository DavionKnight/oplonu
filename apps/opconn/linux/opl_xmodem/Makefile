MODULE_DIR = $(shell /bin/pwd)
PWD = $(shell /bin/pwd)
opl_xmodem-objs = ufile.o xmodem.o
obj-m= opl_xmodem.o
MODULE_NAME = opl_xmodem.ko

KERNEL := ../../../../os/linux-2.6.21.5
default:
	#$(MAKE) -C $(KERNEL) M=$(MODULE_DIR) modules
	$(MAKE) -C $(KERNEL) M=$(PWD) ARCH=mips CROSS_COMPILE=mips-linux-uclibc- modules
clean:
	rm .*.cmd *.o *.ko *.mod.c Module.symvers -rf .tmp* ~/opl_xmodem.config
	
#include Makefile.conf


