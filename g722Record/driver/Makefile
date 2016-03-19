##########################################
# Makefile (!not makefile)
# ifneq ($(KERNELRELEASE),)
obj-m       := c5535drv.o
# else
KDIR        := /lib/modules/`uname -r`/build
PWD         := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
# endif

usb: usb.c
	gcc -o usb usb.c
# ²Î¿¼£ºhttp://lwn.net/Articles/driver-porting/
