#
# Makefile for Zaptel driver modules and utilities
#
# Copyright (C) 2001-2007 Digium, Inc.
#
#

CFLAGS+=-DSTANDALONE_ZAPATA -DBUILDING_TONEZONE

ifeq ($(MAKELEVEL),0)
PWD:=$(shell pwd)
endif

ARCH:=$(shell uname -m | sed -e s/i.86/i386/)

ifeq ($(DEB_HOST_GNU_TYPE),)
UNAME_M:=$(shell uname -m)
else
UNAME_M:=$(DEB_HOST_GNU_TYPE)
endif

# If you want to build for a kernel other than the current kernel, set KVERS
ifndef KVERS
KVERS:=$(shell uname -r)
endif
ifndef KSRC
  ifneq (,$(wildcard /lib/modules/$(KVERS)/build))
    KSRC:=/lib/modules/$(KVERS)/build
  else
    KSRC_SEARCH_PATH:=/usr/src/linux-2.4 /usr/src/linux
    KSRC:=$(shell for dir in $(KSRC_SEARCH_PATH); do if [ -d $$dir ]; then echo $$dir; break; fi; done)
  endif
endif
KVERS_MAJ:=$(shell echo $(KVERS) | cut -d. -f1-2)
KINCLUDES:=$(KSRC)/include

ifeq ($(KVERS_MAJ),2.4)
  BUILDVER:=linux24
else
  BUILDVER:=linux26
endif

# Set HOTPLUG_FIRMWARE=no to override automatic building with hotplug support
# if it is enabled in the kernel.
ifeq ($(BUILDVER),linux26)
  ifneq (,$(wildcard $(INSTALL_PREFIX)/etc/udev/rules.d))
    DYNFS=yes
    UDEVRULES=yes
  endif
  HOTPLUG_FIRMWARE:=$(shell if grep CONFIG_FW_LOADER $(KINCLUDES)/linux/autoconf.h | grep -q undef; then echo "no"; else echo "yes"; fi)
else
  # Hotplug firmware loading is not supported on any other version then 2.6
  HOTPLUG_FIRMWARE:=no
endif

ifeq ($(HOTPLUG_FIRMWARE),yes)
  CFLAGS+=-DHOTPLUG_FIRMWARE
endif

ifeq ($(ARCH),i386)
ifneq ($(wildcard $(PWD)/hpec/hpec_x86_32.o_shipped),)
HPEC_PRESENT=yes
endif
endif

ifeq ($(ARCH),x86_64)
ifneq ($(wildcard $(PWD)/hpec/hpec_x86_64.o_shipped),)
HPEC_PRESENT=yes
endif
endif

# If the file .zaptel.makeopts is present in your home directory, you can
# include all of your favorite menuselect options so that every time you download
# a new version of Asterisk, you don't have to run menuselect to set them.
# The file /etc/zaptel.makeopts will also be included but can be overridden
# by the file in your home directory.

GLOBAL_MAKEOPTS=$(wildcard /etc/zaptel.makeopts)
USER_MAKEOPTS=$(wildcard ~/.zaptel.makeopts)

ifeq ($(strip $(foreach var,clean distclean dist-clean update,$(findstring $(var),$(MAKECMDGOALS)))),)
 ifneq ($(wildcard menuselect.makeopts),)
  include menuselect.makeopts
 endif
endif

ifeq ($(strip $(foreach var,clean distclean dist-clean update,$(findstring $(var),$(MAKECMDGOALS)))),)
 ifneq ($(wildcard makeopts),)
  include makeopts
 endif
endif

ifeq ($(BUILDVER),linux24)
MENUSELECT_MODULES+=xpp wctc4xxp zttranscode
endif

ifeq ($(findstring xpp,$(MENUSELECT_MODULES)),)
  BUILD_XPP:=yes
endif

TOPDIR_MODULES:=pciradio tor2 torisa wcfxo wct1xxp wctdm wctdm24xxp wcte11xp wcusb zaptel ztd-eth ztd-loc ztdummy ztdynamic zttranscode wcte12xp
SUBDIR_MODULES:=wct4xxp wctc4xxp xpp
BUILD_TOPDIR_MODULES:=$(filter-out $(MENUSELECT_MODULES),$(TOPDIR_MODULES))
BUILD_SUBDIR_MODULES:=$(filter-out $(MENUSELECT_MODULES),$(SUBDIR_MODULES))
BUILD_MODULES:=$(BUILD_TOPDIR_MODULES) $(BUILD_SUBDIR_MODULES)

MOD_DESTDIR:=zaptel

#NOTE NOTE NOTE
#
# all variables set before the include of Makefile.kernel26 are needed by the 2.6 kernel module build process

ifneq ($(KBUILD_EXTMOD),)

obj-m:=$(BUILD_TOPDIR_MODULES:%=%.o)
obj-m+=$(BUILD_SUBDIR_MODULES:%=%/)

include $(src)/Makefile.kernel26

else

ifeq ($(BUILDVER),linux24)
  INSTALL_MODULES:=$(BUILD_TOPDIR_MODULES:%=%.o)
  INSTALL_MODULES+=$(foreach mod,$(BUILD_SUBDIR_MODULES),$(mod)/$(mod).o)
  ALL_MODULES:=$(TOPDIR_MODULES:%=%.o)
  ALL_MODULES+=$(SUBDIR_MODULES:%=%/%.o)
else
  INSTALL_MODULES:=$(BUILD_TOPDIR_MODULES:%=%.ko)
  INSTALL_MODULES+=$(foreach mod,$(filter-out xpp,$(BUILD_SUBDIR_MODULES)),$(mod)/$(mod).ko)
  ifneq ($(filter xpp,$(BUILD_SUBDIR_MODULES)),)
  INSTALL_MODULES+=$(patsubst %,xpp/%.ko,xpp_usb xpd_fxo xpd_fxs xpp)
  endif

  ALL_MODULES:=$(TOPDIR_MODULES:%=%.ko)
  ALL_MODULES+=$(foreach mod,$(filter-out xpp,$(SUBDIR_MODULES)),$(mod)/$(mod).ko)
  ALL_MODULES+=$(patsubst %,xpp/%.ko,xpp_usb xpd_fxo xpd_fxs xpp)
endif

CFLAGS+=-I. -O4 -g -Wall -DBUILDING_TONEZONE #-DTONEZONE_DRIVER
ifneq (,$(findstring ppc,$(UNAME_M)))
CFLAGS_PPC:=-fsigned-char
endif
ifneq (,$(findstring x86_64,$(UNAME_M)))
CFLAGS_x86_64:=-m64
endif
CFLAGS+=$(CFLAGS_PPC) $(CFLAGS_x86_64)
LCFLAGS=-fPIC $(CFLAGS) -DBUILDING_TONEZONE
KFLAGS=-I$(KINCLUDES) -O6
KFLAGS+=-DMODULE -D__KERNEL__ -DEXPORT_SYMTAB -I$(KSRC)/drivers/net \
	-Wall -I. -Wstrict-prototypes -fomit-frame-pointer -I$(KSRC)/drivers/net/wan -I$(KINCLUDES)/net
ifneq (,$(wildcard $(KINCLUDES)/linux/modversions.h))
  KFLAGS+=-DMODVERSIONS -include $(KINCLUDES)/linux/modversions.h
endif
ifneq (,$(findstring ppc,$(UNAME_M)))
KFLAGS_PPC:=-msoft-float -fsigned-char
endif
KFLAGS+=$(KFLAGS_PPC)
ifeq ($(KVERS_MAJ),2.4)
  ifneq (,$(findstring x86_64,$(UNAME_M)))
    KFLAGS+=-mcmodel=kernel
  endif
endif

#
# Features are now configured in zconfig.h
#

MODULE_ALIASES=wcfxs wctdm8xxp wct2xxp

KFLAGS+=-DSTANDALONE_ZAPATA
CFLAGS+=-DSTANDALONE_ZAPATA
KMAKE  = $(MAKE) -C $(KSRC) SUBDIRS=$(PWD)
KMAKE_INST = $(KMAKE) \
  INSTALL_MOD_PATH=$(INSTALL_PREFIX) INSTALL_MOD_DIR=misc modules_install

ROOT_PREFIX=
INSTALL_PREFIX=$(DESTDIR)

CONFIG_FILE=$(INSTALL_PREFIX)/etc/zaptel.conf
CFLAGS+=-DZAPTEL_CONFIG=\"$(CONFIG_FILE)\"

CHKCONFIG:=$(shell sh -c 'type -p chkconfig' 2> /dev/null)
ifndef CHKCONFIG
CHKCONFIG:=:
endif

ifneq ($(wildcard .version),)
  ZAPTELVERSION:=$(shell cat .version)
else
ifneq ($(wildcard .svn),)
  ZAPTELVERSION=SVN-$(shell build_tools/make_svn_branch_name)
endif
endif

LTZ_A:=libtonezone.a
LTZ_A_OBJS:=zonedata.o tonezone.o
LTZ_SO:=libtonezone.so
LTZ_SO_OBJS:=zonedata.lo tonezone.lo
LTZ_SO_MAJOR_VER:=1
LTZ_SO_MINOR_VER:=0

BIN_DIR:=$(INSTALL_PREFIX)/sbin
LIB_DIR:=$(INSTALL_PREFIX)/usr/lib
INC_DIR:=$(INSTALL_PREFIX)/usr/include
MOD_DIR:=$(INSTALL_PREFIX)/lib/modules/$(KVERS)/misc

BINS:=fxotune fxstest sethdlc-new ztcfg ztdiag ztmonitor ztspeed zttest zttool
UTILS:=$(filter-out zttool fxstest ztcfg,$(BINS))
UTILSO:=$(UTILS:%=%.o)
BINS:=$(filter-out $(MENUSELECT_UTILS),$(BINS))

all: menuselect.makeopts 
	@$(MAKE) _all

_all: $(if $(BUILD_MODULES),modules) programs $(LTZ_SO) $(LTZ_A)

xpp-utils:
ifeq (yes,$(BUILD_XPP))
	@$(MAKE) -C xpp/utils
endif

programs: $(BINS) xpp-utils

modules: prereq
	@if [ -z "$(KSRC)" -o ! -d "$(KSRC)" ]; then echo "You do not appear to have the sources for the $(KVERS) kernel installed."; exit 1 ; fi
ifeq ($(BUILDVER),linux26)
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) modules
else
modules: $(INSTALL_MODULES)
endif

ifeq ($(HPEC_PRESENT),yes)
ifeq ($(ARCH),i386)
ZAPTEL_HPEC:=hpec/hpec_x86_32.o_shipped
endif

ifeq ($(ARCH),x86_64)
ZAPTEL_HPEC:=hpec/hpec_x86_64.o_shipped
endif

KFLAGS+=-DECHO_CAN_HPEC -I$(PWD)/hpec
zaptel-base.o: hpec/hpec_zaptel.h hpec/hpec_user.h
endif

version.h:
	@ZAPTELVERSION="${ZAPTELVERSION}" build_tools/make_version_h > $@.tmp
	@if cmp -s $@.tmp $@ ; then :; else \
		mv $@.tmp $@ ; \
	fi
	@rm -f $@.tmp

tests: patgen pattest patlooptest hdlcstress hdlctest hdlcgen hdlcverify timertest

tor2ee.o: tor2-hw.h

tor2ee: LDLIBS+=-lpci

zonedata.lo: zonedata.c
	$(CC) -c $(LCFLAGS) -o $@ $^

tonezone.lo: tonezone.c
	$(CC) -c $(LCFLAGS) -o $@ $^

torisatool: torisatool.o
	$(CC) -o $@ $^

tones.h: gendigits
	./gendigits > $@

tor2fw.h: tormenta2.rbt makefw 
	./makefw $< tor2fw > $@

radfw.h: pciradio.rbt makefw
	./makefw $< radfw > $@

makefw: makefw.c
	$(HOSTCC) -o $@ $^

gendigits: LDLIBS+=-lm
gendigits: CFLAGS=

prereq: config.status tones.h tor2fw.h radfw.h version.h

zttool.o: zttool.c zaptel.h
	$(CC) -g -c $(NEWT_INCLUDE) $(CFLAGS) -o $@ $<

ztprovision.o: ztprovision.c zaptel.h

ztmonitor.o: ztmonitor.c zaptel.h

ztspeed: CFLAGS=
ztspeed.o: CFLAGS=
zttool: LDLIBS+=$(NEWT_LIB)
sethdlc-new.o: CFLAGS+=-I$(KINCLUDES)

$(LTZ_A): $(LTZ_A_OBJS)
	ar rcs $@ $^
	ranlib $@

$(LTZ_SO): $(LTZ_SO_OBJS)
	$(CC) -shared -Wl,-soname,$(LTZ_SO).$(LTZ_SO_MAJOR_VER).$(LTZ_SO_MINOR_VER) -o $@ $^ -lm

ztcfg.c: ztcfg.h

ztcfg-shared: ztcfg.o $(LTZ_SO)
	$(CC) -o $@ $^ -lm

data:
	$(MAKE) -C datamods datamods

ztcfg: ztcfg.o $(LTZ_A)
	$(CC) -o $@ $^ -lm

ztcfg-dude: ztcfg-dude.o mknotch.o complex.o $(LTZ_SO)
	$(CC) -o $@ $^ -lm -lstdc++

mknotch.o: mknotch.cc
	$(CC) -o $@ -c $^

complex.o: complex.cc
	$(CC) -o $@ -c $^

usbfxstest: LDLIBS+=-lzap
fxstest: $(LTZ_SO)
fxstest: LDLIBS+=-lm
fxotune: LDLIBS+=-lm
fxsdump: LDLIBS+=-lm

devel: tor2ee 

ifeq ($(BUILDVER),linux24)
wct4xxp/wct4xxp.o:
	$(MAKE) -C wct4xxp KFLAGS="$(KFLAGS) -I.." CFLAGS="$(CFLAGS) -I.."

tor2.o: tor2-hw.h tor2fw.h

zaptel-base.o: digits.h arith.h sec.h mec.h sec-2.h mec2.h mec3.h zconfig.h

wcusb.o: wcusb.h

wcfxsusb.o: wcfxsusb.h

wctdm.o: wctdm.h

wctdm24xxp.o: wctdm.h

pciradio.o: radfw.h

ztdummy.o: ztdummy.h

zaptel.o: zaptel-base.o $(ZAPTEL_HPEC)
	$(LD) -r -o $@ $< $(ZAPTEL_HPEC)

$(filter-out zaptel.o,$(BUILD_TOPDIR_MODULES:%=%.o)) zaptel-base.o: %.o: %.c zaptel.h
	$(CC) $(KFLAGS) -o $@ -c $<

zaptel.c: tones.h
endif

stackcheck: checkstack modules
	./checkstack *.ko */*.ko

MISDNVERSION=1_1_3
MISDNUSERVERSION=1_1_3
b410p:
	@if test "$(DOWNLOAD)" = ":" ; then \
		echo "**************************************************"; \
		echo "***                                            ***"; \
		echo "*** You must have either wget or fetch to be   ***"; \
		echo "*** able to automatically download and install ***"; \
		echo "*** b410p support.                             ***"; \
		echo "***                                            ***"; \
		echo "*** Please install one of these.               ***"; \
		echo "***                                            ***"; \
		echo "**************************************************"; \
		exit 1; \
	fi
	[ -f mISDN-$(MISDNVERSION).tar.gz ] || $(DOWNLOAD) ftp://ftp.digium.com/pub/zaptel/b410p/mISDN-$(MISDNVERSION).tar.gz
	tar -zxf mISDN-$(MISDNVERSION).tar.gz
	$(MAKE) -C mISDN-$(MISDNVERSION) install
	[ -f mISDNuser-$(MISDNUSERVERSION).tar.gz ] || $(DOWNLOAD) ftp://ftp.digium.com/pub/zaptel/b410p/mISDNuser-$(MISDNUSERVERSION).tar.gz
	tar -zxf mISDNuser-$(MISDNUSERVERSION).tar.gz
	$(MAKE) -C mISDNuser-$(MISDNUSERVERSION) install

$(UTILS): %: %.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

$(UTILSO): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

install: all devices install-modules install-libs install-include install-xpp
ifeq ($(HOTPLUG_FIRMWARE),yes)
	$(MAKE) -C firmware hotplug-install
endif
	if [ -f ztcfg ]; then \
		$(INSTALL) -D -m 755 ztcfg $(INSTALL_PREFIX)/sbin/ztcfg; \
	fi
	if [ -f sethdlc-new ]; then \
		$(INSTALL) -D -m 755 sethdlc-new $(INSTALL_PREFIX)/sbin/sethdlc; \
	elif [ -f sethdlc ]; then \
		$(INSTALL) -D -m 755 sethdlc $(INSTALL_PREFIX)/sbin/sethdlc; \
	fi
	if [ -f zttool ]; then \
		$(INSTALL) -D -m 755 zttool $(INSTALL_PREFIX)/sbin/zttool; \
	fi
	$(INSTALL) -m 644 doc/ztcfg.8 $(INSTALL_PREFIX)/usr/share/man/man8
	$(INSTALL) -m 644 doc/zttool.8 $(INSTALL_PREFIX)/usr/share/man/man8
	[ `id -u` = 0 ] && /sbin/depmod -a $(KVERS) || :
	[ -f $(CONFIG_FILE) ] || $(INSTALL) -D -m 644 zaptel.conf.sample $(CONFIG_FILE)
	build_tools/genmodconf $(BUILDVER) "$(ROOT_PREFIX)" "$(filter-out zaptel ztdummy xpp zttranscode ztdynamic,$(BUILD_MODULES)) $(MODULE_ALIASES)"
	@if [ -d /etc/modutils ]; then \
		/sbin/update-modules ; \
	fi

install-libs: $(LTZ_SO) $(LTZ_A)
	$(INSTALL) -D -m 755 $(LTZ_A) $(LIB_DIR)/$(LTZ_A)
	$(INSTALL) -D -m 755 $(LTZ_SO) $(LIB_DIR)/$(LTZ_SO).$(LTZ_SO_MAJOR_VER).$(LTZ_SO_MINOR_VER)
	if [ -z "$(INSTALL_PREFIX)" -a `id -u` = 0 ]; then \
		/sbin/ldconfig || : ;\
	fi
	rm -f $(LIB_DIR)$(LTZ_SO)
	$(LN) -sf $(LTZ_SO).$(LTZ_SO_MAJOR_VER).$(LTZ_SO_MINOR_VER) \
		$(LIB_DIR)/$(LTZ_SO).$(LTZ_SO_MAJOR_VER)
	$(LN) -sf $(LTZ_SO).$(LTZ_SO_MAJOR_VER).$(LTZ_SO_MINOR_VER) \
		$(LIB_DIR)/$(LTZ_SO)
	if [ -z "$(INSTALL_PREFIX)" ] && [ -x /usr/sbin/sestatus ] && (/usr/sbin/sestatus | grep "SELinux status:" | grep -q "enabled") ; then /sbin/restorecon -v $(LIB_DIR)/$(LTZ_SO); fi

install-xpp:
ifeq (yes,$(BUILD_XPP))
	@$(MAKE) -C xpp/utils install
endif

install-include:
	$(INSTALL) -D -m 644 zaptel.h $(INSTALL_PREFIX)/usr/include/zaptel/zaptel.h
	$(INSTALL) -D -m 644 tonezone.h $(INSTALL_PREFIX)/usr/include/zaptel/tonezone.h
	rm -f $(INSTALL_PREFIX)/usr/include/linux/zaptel.h
	rm -f $(INSTALL_PREFIX)/usr/include/linux/torisa.h
	rm -f $(INSTALL_PREFIX)/usr/include/zaptel.h
	rm -f $(INSTALL_PREFIX)/usr/include/torisa.h
	rm -f $(INSTALL_PREFIX)/usr/include/tonezone.h

devices:
ifndef DYNFS
	mkdir -p $(INSTALL_PREFIX)/dev/zap
	rm -f $(INSTALL_PREFIX)/dev/zap/ctl
	rm -f $(INSTALL_PREFIX)/dev/zap/channel
	rm -f $(INSTALL_PREFIX)/dev/zap/pseudo
	rm -f $(INSTALL_PREFIX)/dev/zap/timer
	rm -f $(INSTALL_PREFIX)/dev/zap/transcode
	rm -f $(INSTALL_PREFIX)/dev/zap/253
	rm -f $(INSTALL_PREFIX)/dev/zap/252
	rm -f $(INSTALL_PREFIX)/dev/zap/251
	rm -f $(INSTALL_PREFIX)/dev/zap/250
	mknod $(INSTALL_PREFIX)/dev/zap/ctl c 196 0
	mknod $(INSTALL_PREFIX)/dev/zap/transcode c 196 250
	mknod $(INSTALL_PREFIX)/dev/zap/timer c 196 253
	mknod $(INSTALL_PREFIX)/dev/zap/channel c 196 254
	mknod $(INSTALL_PREFIX)/dev/zap/pseudo c 196 255
	N=1; \
	while [ $$N -lt 250 ]; do \
		rm -f $(INSTALL_PREFIX)/dev/zap/$$N; \
		mknod $(INSTALL_PREFIX)/dev/zap/$$N c 196 $$N; \
		N=$$[$$N+1]; \
	done
else # DYNFS
  ifdef UDEVRULES
	install -d $(INSTALL_PREFIX)/etc/udev/rules.d
	build_tools/genudevrules > $(INSTALL_PREFIX)/etc/udev/rules.d/zaptel.rules
  else # !UDEVRULES
	@echo "**** Dynamic filesystem detected -- not creating device nodes"
  endif
endif

install-udev: devices

# make should *fail* and not silently succeed if a program did not build
install-programs: $(BINS)
	$(INSTALL) -d $(BIN_DIR)
	$(INSTALL) $(BINS) $(BIN_DIR)

uninstall-hotplug:
	$(MAKE) -C firmware hotplug-uninstall

uninstall-modules:
ifeq ($(BUILDVER),linux24)
else
	for x in $(ALL_MODULES); do \
		rm -f $(INSTALL_PREFIX)/lib/modules/$(KVERS)/extra/$$x ; \
		rm -f $(INSTALL_PREFIX)/lib/modules/$(KVERS)/misc/$$x ; \
		rm -f $(INSTALL_PREFIX)/lib/modules/$(KVERS)/zaptel/$$x ; \
	done;
endif

install-modules: $(INSTALL_MODULES) uninstall-modules
ifeq ($(BUILDVER),linux24)
	$(INSTALL) -d $(MOD_DIR)
	$(INSTALL) -m 644 $(INSTALL_MODULES) $(MOD_DIR)
else
	$(KMAKE_INST)
	if [ -f datamods/syncppp.ko ]; then \
		$(MAKE) -C datamods install; \
	fi
endif

config:
	if [ -d $(INSTALL_PREFIX)/etc/rc.d/init.d ]; then \
		$(INSTALL) -D -m 755 zaptel.init $(INSTALL_PREFIX)/etc/rc.d/init.d/zaptel; \
		if [ -z "$(INSTALL_PREFIX)" ]; then \
			$(CHKCONFIG) --add zaptel; \
		fi; \
	elif [ -d $(INSTALL_PREFIX)/etc/init.d ]; then \
		$(INSTALL) -D -m 755 zaptel.init $(INSTALL_PREFIX)/etc/init.d/zaptel; \
		if [ -z "$(INSTALL_PREFIX)" ]; then \
			$(CHKCONFIG) --add zaptel; \
		fi; \
	fi 
	if [ -d $(INSTALL_PREFIX)/etc/default ] && [ ! -f $(INSTALL_PREFIX)/etc/default/zaptel ]; then \
			$(INSTALL) -D -m 644 zaptel.sysconfig $(INSTALL_PREFIX)/etc/default/zaptel; \
	fi
	if [ -d $(INSTALL_PREFIX)/etc/sysconfig ] && [ ! -f $(INSTALL_PREFIX)/etc/sysconfig/zaptel ]; then \
		$(INSTALL) -D -m 644 zaptel.sysconfig $(INSTALL_PREFIX)/etc/sysconfig/zaptel; \
	fi
	if [ -d $(INSTALL_PREFIX)/etc/sysconfig/network-scripts ]; then \
		$(INSTALL) -D -m 755 ifup-hdlc $(INSTALL_PREFIX)/etc/sysconfig/network-scripts/ifup-hdlc; \
	fi

update:
	@if [ -d .svn ]; then \
		echo "Updating from Subversion..." ; \
		svn update | tee update.out; \
		rm -f .version; \
		if [ `grep -c ^C update.out` -gt 0 ]; then \
			echo ; echo "The following files have conflicts:" ; \
			grep ^C update.out | cut -b4- ; \
		fi ; \
		rm -f update.out; \
	else \
		echo "Not under version control";  \
	fi

clean:
	-@$(MAKE) -C menuselect clean
	rm -f torisatool makefw tor2fw.h radfw.h
	rm -f $(BINS)
	rm -f *.o ztcfg tzdriver sethdlc sethdlc-new
	rm -f $(LTZ_SO) $(LTZ_A) *.lo
ifeq ($(BUILDVER),linux26)
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) clean
	$(MAKE) -C xpp/utils clean
else
	$(MAKE) -C wct4xxp clean
endif
	$(MAKE) -C firmware clean
	rm -rf .tmp_versions
	rm -f gendigits tones.h
	rm -f libtonezone*
	rm -f tor2ee
	rm -f fxotune
	rm -f core
	rm -f ztcfg-shared fxstest
	rm -rf misdn*
	rm -rf mISDNuser*

distclean: dist-clean

dist-clean: clean
	@$(MAKE) -C menuselect dist-clean
	@$(MAKE) -C firmware dist-clean
	rm -f makeopts menuselect.makeopts menuselect-tree
	rm -f config.log config.status

config.status: configure
	@CFLAGS="" ./configure
	@echo "****"
	@echo "**** The configure script was just executed, so 'make' needs to be"
	@echo "**** restarted."
	@echo "****"
	@exit 1

menuselect.makeopts: menuselect/menuselect menuselect-tree
	@menuselect/menuselect --check-deps ${GLOBAL_MAKEOPTS} ${USER_MAKEOPTS} $@

menuconfig: menuselect

menuselect: menuselect/menuselect menuselect-tree
	-@menuselect/menuselect $(GLOBAL_MAKEOPTS) $(USER_MAKEOPTS) menuselect.makeopts && echo "menuselect changes saved!" || echo "menuselect changes NOT saved!"

menuselect/menuselect: menuselect/menuselect.c menuselect/menuselect_curses.c menuselect/menuselect_stub.c menuselect/menuselect.h menuselect/linkedlists.h config.status
	@CFLAGS="" $(MAKE) -C menuselect

menuselect-tree: zaptel.xml firmware/firmware.xml
	@echo "Generating input for menuselect ..."
	@build_tools/make_tree > $@

.EXPORT_ALL_VARIABLES:

.PHONY: menuselect distclean dist-clean clean version.h all _all install b410p devices programs modules tests devel data stackcheck install-udev config update install-programs install-modules install-include install-libs install-xpp xpp-utils uninstall-modules

endif
