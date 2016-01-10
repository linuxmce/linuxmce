KINCLUDES = /usr/src/linux/include
BRISTUFFBASE = $(shell dirname `pwd`)

ZAP = $(shell [ -f $(BRISTUFFBASE)/zaptel/zaptel.h ] && echo "-I$(BRISTUFFBASE)/zaptel")
RTAI = $(shell [ -f /usr/realtime/include/rtai.h ] && echo "-DRTAITIMING -I/usr/realtime/include")

CFLAGS+=-I. $(ZAP) $(RTAI) -O2 -g -Wall -DBUILDING_TONEZONE 
CFLAGS+=$(shell if uname -m | grep -q ppc; then echo "-fsigned-char"; fi)

KFLAGS=-D__KERNEL__ -DMODULE -DEXPORT_SYMTAB -fomit-frame-pointer -O2 -Wall -I$(KINCLUDES) $(ZAP) $(RTAI) -Wall
KFLAGS+=$(shell [ -f $(KINCLUDES)/linux/modversions.h ] && echo "-DMODVERSIONS -include $(KINCLUDES)/linux/modversions.h")
KFLAGS+=$(shell if uname -m | grep -q ppc; then echo "-msoft-float -fsigned-char"; fi)


BUILDVER=$(shell if uname -r | grep -q ^2.6; then echo "linux26"; else echo "linux24"; fi)

MODCONF=$(shell if [ -d $(INSTALL_PREFIX)/etc/modprobe.d ]; then echo "$(INSTALL_PREFIX)/etc/modprobe.d/zaptel"; elif [ -d $(INSTALL_PREFIX)/etc/modutils ]; then echo "$(INSTALL_PREFIX)/etc/modutils/zaptel"; elif [ -f $(INSTALL_PREFIX)/etc/modprobe.conf ]; then echo "$(INSTALL_PREFIX)/modprobe.conf"; elif [ -f $(INSTALL_PREFIX)/etc/modules.conf ]; then echo "$(INSTALL_PREFIX)/etc/modules.conf"; else echo $(INSTALL_PREFIX)/etc/conf.modules ; fi)

OBJS=zaphfc.o

MODULES=zaphfc

MODULESO=$(shell for x in $(MODULES); do echo "$$x.o "; done )
MODULESKO=$(shell for x in $(MODULES); do echo "$$x.ko "; done )

PWD=$(shell pwd)

obj-m := $(MODULESO)

all: $(BUILDVER)

linux24: $(OBJS)
	sync


zaphfc.o: zaphfc.c zaphfc.h
	$(CC) -c zaphfc.c $(KFLAGS)

clean:	
	rm -f $(OBJS) *.ko *.mod.c *.mod.o .*o.cmd *~
	rm -rf .tmp_versions

test: all
	modprobe zaptel
	insmod ./zaphfc.o
	cat /proc/interrupts
	sleep 1
	cat /proc/interrupts
	rmmod zaphfc
	rmmod zaptel

load:	load$(BUILDVER)

loadNT:	load$(BUILDVER)NT

load-debug:	load$(BUILDVER)-debug

loadNT-debug:	load$(BUILDVER)NT-debug

loadlinux24: all
	modprobe zaptel
	insmod ./zaphfc.o
	ztcfg -v

loadlinux24-debug: all
	modprobe zaptel
	insmod ./zaphfc.o debug=1
	ztcfg -v

loadlinux26: linux26
	modprobe zaptel
	insmod ./zaphfc.ko
	ztcfg -v

loadlinux26-debug: linux26
	modprobe zaptel
	insmod ./zaphfc.ko debug=1
	ztcfg -v

loadlinux24NT: all
	modprobe zaptel
	insmod ./zaphfc.o modes=1
	ztcfg -v

loadlinux24NT-debug: all
	modprobe zaptel
	insmod ./zaphfc.o modes=1 debug=1
	ztcfg -v

loadlinux26NT: linux26
	modprobe zaptel
	insmod ./zaphfc.ko modes=1
	ztcfg -v

loadlinux26NT-debug: linux26
	modprobe zaptel
	insmod ./zaphfc.ko modes=1 debug=1
	ztcfg -v

unload: 
	-rmmod zaphfc zaptel

zaphfc.ko: zaphfc.c zaphfc.h

linux26: 
	@if ! [ -d /usr/src/linux-2.6 ]; then echo "Link /usr/src/linux-2.6 to your kernel sources first!"; exit 1 ; fi
	make -C /usr/src/linux-2.6 SUBDIRS=$(PWD) ZAP=$(ZAP) modules

install:	install$(BUILDVER)

installlinux26:
	install -D -m 644 zaphfc.ko $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/zaphfc.ko

installlinux24:
	install -D -m 644 zaphfc.o $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/zaphfc.o

