include lirc-modules-source.conf

# An space should be left at the end of each CFLAGS variable for the
# sanity-check to work.
IT87_CFLAGS     := $(subst ",,$(LIRC_IT87_CFLAGS) )
PARALLEL_CFLAGS	:= $(subst ",,-DLIRC_PORT=$(LIRC_PARALLEL_PORT) \
				-DLIRC_TIMER=$(LIRC_PARALLEL_TIMER) \
				-DLIRC_IRQ=$(LIRC_PARALLEL_IRQ) )
SERIAL_CFLAGS   := $(subst ",,$(LIRC_SERIAL_CFLAGS) \
				-DLIRC_PORT=$(LIRC_SERIAL_PORT) \
				-DLIRC_IRQ=$(LIRC_SERIAL_IRQ) ) 
SIR_CFLAGS		:= $(subst ",,$(LIRC_SIR_CFLAGS) \
				-DLIRC_PORT=$(LIRC_SIR_PORT) \
				-DLIRC_IRQ=$(LIRC_SIR_IRQ) )

ifeq "$(KVERS)" ""
        KVERS:=$(shell uname -r)
endif
ifeq "$(KSRC)" ""
	KSRC:=/lib/modules/linux-$(KVERS)/build
endif
KERNEL_LOCATION = $(KSRC)
CC = gcc -D__KERNEL__ -I $(KSRC)/include
CFLAGS= -O2 -g -Wall
DEFS=-DHAVE_CONFIG_H -I. -I../..

KEXT=ko

export KERNEL_LOCATION CC

DRIVERS=atiusb bt829 gpio i2c igorplugusb imon it87 ite8709 mceusb parallel sasem serial sir streamzap ttusbir ene0100
all: $(DRIVERS)

test:
	@echo '$(IT87_CFLAGS)'
	@echo '$(PARALLEL_CFLAGS)'
	@echo '$(LIRC_SERIAL_IRQ)'
	@echo '$(LIRC_SIR_IRQ)'
	@echo '$(LIRC_PARALLEL_IRQ)'
	@echo '$(SERIAL_CFLAGS)'
	@echo '$(SIR_CFLAGS)'
	@echo '$(KERNEL_LOCATION)'
	@echo '$(KVERS)'

UNCONFIGURED: #Do nothing if the user didn't configure which drivers to build

dkms: $(subst ", ,$(LIRC_MODULES:,=))

modules:
	mkdir modules

sanity-check:
	@if \
		expr "$(DEFS)" : '.* -DLIRC_IRQ= ' > /dev/null\
		|| expr "$(DEFS)" : '.* -DLIRC_PORT= ' > /dev/null \
		|| expr "$(DEFS)" : '.* -DLIRC_TIMER= '> /dev/null \
		|| expr "$(DEFS)" : '.*UNCONFIGURED'> /dev/null;\
	then \
		echo ;\
		echo "##########################";\
		echo "## CONFIGURATION ERROR: ##";\
		echo "##########################";\
		echo ;\
		echo "You should reconfigure lirc-modules-source and make";\
		echo "sure you don't leave blank any one of IRQ, IO Port or ";\
		echo "Timer (parallel only)";\
		echo ;\
		echo "Hint1: use \"dpkg-reconfigure lirc-modules-source\"";\
		echo "Hint2: If you selected \"automagical\" configuration of";\
		echo "	kernel modules you should probably reconfigure lirc";\
		echo "	instead.";\
		echo "Hint3: you may instead edit /etc/lirc/lirc-modules-source.conf";\
		exit 1;\
	fi

dev: modules sanity-check
	$(MAKE) -C drivers SUBDIRS="lirc_dev"
	cp drivers/lirc_dev/lirc_dev.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_dev.$(KEXT).KVERS

atiusb: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_atiusb"
	cp drivers/lirc_atiusb/lirc_atiusb.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_atiusb.$(KEXT).KVERS

bt829: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_bt829"
	cp drivers/lirc_bt829/lirc_bt829.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_bt829.$(KEXT).KVERS

gpio: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_gpio"
	cp drivers/lirc_gpio/lirc_gpio.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_gpio.$(KEXT).KVERS

i2c: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_i2c"
	cp drivers/lirc_i2c/lirc_i2c.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_i2c.$(KEXT).KVERS

igorplugusb: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_igorplugusb"
	cp drivers/lirc_igorplugusb/lirc_igorplugusb.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_igorplugusb.$(KEXT).KVERS

imon: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_imon"
	cp drivers/lirc_imon/lirc_imon.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_imon.$(KEXT).KVERS

it87: DEFS += $(IT87_CFLAGS)
it87: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_it87" DEFS="$(DEFS)"
	cp drivers/lirc_it87/lirc_it87.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_it87.$(KEXT).KVERS

ite8709: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_ite8709"
	cp drivers/lirc_ite8709/lirc_ite8709.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_ite8709.$(KEXT).KVERS

mceusb: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_mceusb"
	cp drivers/lirc_mceusb/lirc_mceusb.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_mceusb.$(KEXT).KVERS

parallel: DEFS += $(PARALLEL_CFLAGS)
parallel: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_parallel" DEFS="$(DEFS)"
	cp drivers/lirc_parallel/lirc_parallel.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_parallel.$(KEXT).KVERS

sasem: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_sasem"
	cp drivers/lirc_sasem/lirc_sasem.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_sasem.$(KEXT).KVERS

serial: DEFS += $(SERIAL_CFLAGS)
serial: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_serial" DEFS="$(DEFS)"
	cp drivers/lirc_serial/lirc_serial.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_serial.$(KEXT).KVERS

sir: DEFS += $(SIR_CFLAGS)
sir: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_sir" DEFS="$(DEFS)"
	cp drivers/lirc_sir/lirc_sir.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_sir.$(KEXT).KVERS

streamzap: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_streamzap"
	cp drivers/lirc_streamzap/lirc_streamzap.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_streamzap.$(KEXT).KVERS

ttusbir: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_ttusbir"
	cp drivers/lirc_ttusbir/lirc_ttusbir.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_ttusbir.$(KEXT).KVERS

ene0100: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_ene0100"
	cp drivers/lirc_ene0100/lirc_ene0100.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_ene0100.$(KEXT).KVERS

wpc8769l: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_wpc8769l"
	cp drivers/lirc_wpc8769l/lirc_wpc8769l.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_wpc8769l.$(KEXT).KVERS


install:
	@for file in modules/*.$(KEXT); \
	do \
		kvers=`cut -d" " -f1 $$file.KVERS` && \
		ksrc=`cut -d" " -f2 $$file.KVERS` && \
		dir="$(prefix)/lib/modules/$$kvers/misc/" && \
		( [ -d $$dir ] || mkdir -p $$dir ) && \
		cp -v $$file $$dir ; \
	done
depmod:
	@for file in modules/*.$(KEXT); \
	do \
		kvers=`cut -d" " -f1 $$file.KVERS` && \
		ksrc=`cut -d" " -f2 $$file.KVERS` && \
		dir="$(prefix)/lib/modules/$$kvers/misc/" && \
		if [ -f $$ksrc/System.map ];then \
			/sbin/depmod -r -b $(prefix)/ -a $$kvers \
					-F $$ksrc/System.map ; \
		elif [ -f /boot/System.map-$$kvers ];then \
			/sbin/depmod -r -b $(prefix)/ -a $$kvers \
					-F /boot/System.map-$$kvers; \
		else \
			/sbin/depmod -a ;\
		fi ;\
	done

clean:
	$(MAKE) clean -C drivers SUBDIRS="$(patsubst %,lirc_%,$(DRIVERS))"
	rm -rf modules
