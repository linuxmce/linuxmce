# if define KERNELRELEASE means the release version

ifneq ($(KERNELRELEASE),)
	obj-m := spcp8x5.o
	module-objs := spcp8x5.o	
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

install:
	Dir=$(DESTDIR)/lib/modules/$(shell uname -r)/kernel/drivers/usb/serial; \
	mkdir -p $$Dir; \
	cp spcp8x5.ko $$Dir
