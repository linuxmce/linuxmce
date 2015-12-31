KINCLUDES = /usr/src/linux/include
BRISTUFFBASE = $(shell dirname `pwd`)

ZAP = $(shell [ -f $(BRISTUFFBASE)/zaptel/zaptel.h ] && echo "-I$(BRISTUFFBASE)/zaptel")

CFLAGS+=-I. $(ZAP) -DRELAXED_LOCKING -O2 -g -Wall #-DBLINKYBLINK
CFLAGS+=$(shell if uname -m | grep -q ppc; then echo "-fsigned-char"; fi)

KFLAGS=-D__KERNEL__ -DMODULE -DEXPORT_SYMTAB -DRELAXED_LOCKING -fomit-frame-pointer -O2 -Wall -I$(KINCLUDES) $(ZAP) 
KFLAGS+=$(shell [ -f $(KINCLUDES)/linux/modversions.h ] && echo "-DMODVERSIONS -include $(KINCLUDES)/linux/modversions.h")
KFLAGS+=$(shell if uname -m | grep -q ppc; then echo "-msoft-float -fsigned-char"; fi)

OBJS=cwain.o

BUILDVER=$(shell if uname -r | grep -q ^2.6; then echo "linux26"; else echo "linux24"; fi)

MODCONF=$(shell if [ -d $(INSTALL_PREFIX)/etc/modprobe.d ]; then echo "$(INSTALL_PREFIX)/etc/modprobe.d/zaptel"; elif [ -d $(INSTALL_PREFIX)/etc/modutils ]; then echo "$(INSTALL_PREFIX)/etc/modutils/zaptel"; elif [ -f $(INSTALL_PREFIX)/etc/modprobe.conf ]; then echo "$(INSTALL_PREFIX)/modprobe.conf"; elif [ -f $(INSTALL_PREFIX)/etc/modules.conf ]; then echo "$(INSTALL_PREFIX)/etc/modules.conf"; else echo $(INSTALL_PREFIX)/etc/conf.modules ; fi)

MODULES=cwain

MODULESO=$(shell for x in $(MODULES); do echo "$$x.o "; done )
MODULESKO=$(shell for x in $(MODULES); do echo "$$x.ko "; done )

PWD=$(shell pwd)

obj-m := $(MODULESO)

all: $(BUILDVER)

linux24: $(OBJS)
	sync

linux26:
	@if ! [ -d /usr/src/linux-2.6 ]; then echo "Link /usr/src/linux-2.6 to your kernel sources first!"; exit 1 ; fi
	make -C /usr/src/linux-2.6 SUBDIRS=$(PWD) ZAP=$(ZAP) modules
obj-m := $(OBJS)

cwain.o: cwain.c cwain.h 
	$(CC) -c cwain.c $(KFLAGS)

clean:	
	rm -f $(OBJS) *.ko *.mod.c *.mod.o .*o.cmd *~
	
testlinux24: all
	modprobe zaptel
	insmod ./cwain.o
	ztcfg -v
	cat /proc/interrupts
	sleep 1
	cat /proc/interrupts
	rmmod cwain zaptel

testlinux26: all
	modprobe zaptel
	insmod ./cwain.ko
	ztcfg -v
	cat /proc/interrupts
	sleep 5
	cat /proc/interrupts
	rmmod cwain zaptel

reload:	unload load
load:	load$(BUILDVER)

test:	test$(BUILDVER)


loadlinux24: linux24
	modprobe zaptel
	insmod ./cwain.o
	ztcfg -v

loadlinux26: linux26
	sync
	modprobe zaptel
	insmod ./cwain.ko debug=1
	ztcfg -v

install:	install$(BUILDVER)

installlinux26:
	install -D -m 644 cwain.ko $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/cwain.ko

installlinux24:
	install -D -m 644 cwain.o $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/cwain.o

unload: 
	rmmod cwain zaptel
