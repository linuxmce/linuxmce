#
# Makefile for the linux smb-filesystem routines.
#

## Using Kernel Make
ifneq ($(KERNELRELEASE),)
	via-objs    := via_drv.o via_ds.o via_mm.o via_map.o
	obj-$(CONFIG_DRM_VIA)   += via.o

## Using Pluto Make
else
        KVER  := $(shell uname -r)
        KLINK := $(shell test -e /lib/modules/${KVER}/source/ && echo source || echo build)
        KSRC  := /lib/modules/$(KVER)/$(KLINK)
        PWD   := $(shell pwd)
        DEST  := /lib/modules/$(KVER)/kernel/drivers/misc
default: 
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) modules

clean:
	rm -f .*.cmd *.o *.mod.c *.ko .v* *~ core
	rm -rf .tmp_versions/
endif

