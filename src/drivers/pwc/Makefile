#
# Makefile for the Linux Philips USB Webcam driver
#
# NOTE: This make file can serve as both an external Makefile (launched
#       directly by the user), or as the sub-dir Makefile used by the kernel
#       build system.

# If CONFIG_USB_PWC isn't set, we'll assume the user want to build this driver has a module

ifndef CONFIG_USB_PWC
CONFIG_USB_PWC=m
# Comment the next line, if you don't want debug message and a smaller binary
CONFIG_PWC_DEBUG=y
endif

ifneq ($(KERNELRELEASE),)

pwc-objs	:= pwc-if.o pwc-misc.o pwc-ctrl.o pwc-v4l.o pwc-uncompress.o
pwc-objs	+= pwc-dec1.o pwc-dec23.o pwc-kiara.o pwc-timon.o

obj-$(CONFIG_USB_PWC) += pwc.o

EXTRA_CFLAGS=-Wall -I$(PWD)

ifeq ($(CONFIG_PWC_DEBUG),y)
EXTRA_CFLAGS += -DCONFIG_PWC_DEBUG=1
else
EXTRA_CFLAGS += -DCONFIG_PWC_DEBUG=0
endif

else
# KVER  := $(shell uname -r)
KLINK := $(shell test -e /lib/modules/${KVER}/source/ && echo source || echo "build")
KSRC  := /lib/modules/$(KVER)/$(KLINK)
KMISC := /lib/modules/$(KVER)/kernel/drivers/usb/media
PWD := $(shell pwd)

# Fix some problem with suse < 9.2 and suse >= 9.2
is_suse := $(shell test -e /etc/SuSE-release && echo 1 || echo 0)
ifeq ($(is_suse),1)
  suse_version := $(shell grep VERSION /etc/SuSE-release | cut -f 3 -d " "| tr -d .)
  is_suse_92_or_greater := $(shell test $(suse_version) -ge 92 && echo 1)
  ifeq ($(is_suse_92_or_greater),1)
    KSRC := /lib/modules/$(KVER)/build
  endif
endif



all default:
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) modules

install: default
	install -d $(KMISC)
	install -m 644 -c pwc.ko $(KMISC)
	-/sbin/depmod -a

uninstall:
	-rm -rf $(KMISC)/pwc.ko

endif

clean:
	rm -f *.[oas] .*.flags *.ko .*.cmd .*.d .*.tmp *.mod.c
	rm -rf .tmp_versions

