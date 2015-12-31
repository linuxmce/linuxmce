PEDANTIC	= -ansi -pedantic -std=c99

RANLIB		= ranlib
INSTALL		= install
INSTALL_DATA	= install -m 644

ZAPTEL_DIR	?= ../..

-include $(ZAPTEL_DIR)/makeopts

INSTALL_DATA	= $(INSTALL) -m 644

# In 1.4 those are provided by autoconf through makeopts
prefix		?= /usr
datadir		?= $(prefix)/share
mandir		?= $(datadir)/man
INSTALL		?= install

INSTALL_DATA	= $(INSTALL) -m 644

SBINDIR		= $(prefix)/sbin
DATADIR		= $(datadir)/zaptel
MANDIR		= $(mandir)/man8
HOTPLUG_USB_DIR	= /etc/hotplug/usb
UDEV_RULES_DIR	= /etc/udev/rules.d
# Perl disabled by default, until we see it is safe:
#PERLLIBDIR	= $(shell eval `perl -V:sitelib`; echo "$$sitelib")
PERL_DIRS	:= Zaptel Zaptel/Hardware Zaptel/Xpp
PERL_MODS_PAT	:= *.pm $(PERL_DIRS:%=%/*.pm)
PERL_MODS	:= $(shell cd zconf; echo $(PERL_MODS_PAT))

XPD_FIRMWARE	= $(wildcard ../firmwares/*.hex)
XPD_INIT_DATA	= $(XPD_FIRMWARE) init_fxo_modes
XPD_INIT	= $(wildcard ../init_card_?_*) ../calibrate_slics xpp_fxloader

# Variables that should be defined above, but need sane defaults:
# FIXME: Are those values really sane?
HOSTCC		?= $(CC)

ifeq	(,$(PBX_LIBUSB))
# No PBX_LIBUSB? Maybe we compile against zaptel-1.2
# Let's make a poor man detection of libusb
PBX_LIBUSB	= $(shell if [ -r /usr/include/usb.h ]; then echo 1; else echo 0; fi)
endif

WCTDM=$(ZAPTEL_DIR)/wctdm.c

CFLAGS		= -g -Wall $(EXTRA_CFLAGS)

%.8: %
	pod2man --section 8 $^ > $@ || $(RM) $@
PERL_SCRIPTS	= zt_registration xpp_sync lszaptel xpp_blink
PERL_MANS	= $(PERL_SCRIPTS:%=%.8)

TARGETS	= init_fxo_modes print_modes
PROG_INSTALL	= genzaptelconf
MAN_INSTALL	= $(PROG_INSTALL:%=%.8)
ifeq	(1,$(PBX_LIBUSB))
TARGETS	+= libhexfile.a fpga_load test_parse
PROG_INSTALL	+= fpga_load
endif
ifneq	(,$(PERLLIBDIR))
PROG_INSTALL	+= $(PERL_SCRIPTS)
TARGETS		+= $(PERL_MANS)
endif

all: $(TARGETS)

docs: $(PERL_MANS)

install: all
	$(INSTALL) -d $(DESTDIR)$(SBINDIR)
	$(INSTALL) $(PROG_INSTALL) $(DESTDIR)$(SBINDIR)/
	$(INSTALL) -d $(DESTDIR)$(DATADIR)
	$(INSTALL_DATA) $(XPD_INIT_DATA) $(DESTDIR)$(DATADIR)/
	$(INSTALL) $(XPD_INIT) $(DESTDIR)$(DATADIR)/
	$(INSTALL) -d $(DESTDIR)$(MANDIR)
	$(INSTALL_DATA) $(MAN_INSTALL) $(DESTDIR)$(MANDIR)/
	$(INSTALL) -d $(DESTDIR)$(HOTPLUG_USB_DIR)
	$(INSTALL_DATA) xpp_fxloader.usermap $(DESTDIR)$(HOTPLUG_USB_DIR)/
	# for backward compatibility and for hotplug users:
	ln -sf $(DATADIR)/xpp_fxloader $(DESTDIR)$(HOTPLUG_USB_DIR)/
	$(INSTALL) -d $(DESTDIR)$(UDEV_RULES_DIR)
	$(INSTALL_DATA) xpp.rules $(DESTDIR)$(UDEV_RULES_DIR)/
ifneq	(,$(PERLLIBDIR))
	$(INSTALL) -d $(DESTDIR)$(PERLLIBDIR)
	for i in $(PERL_DIRS); \
	do \
		$(INSTALL) -d "$(DESTDIR)$(PERLLIBDIR)/$$i"; \
	done
	for i in $(PERL_MODS); \
	do \
		$(INSTALL_DATA) "zconf/$$i" "$(DESTDIR)$(PERLLIBDIR)/$$i"; \
	done
endif

libhexfile.a: hexfile.o
	$(AR) cru $@ $^
	$(RANLIB) $@

fpga_load: fpga_load.o libhexfile.a
	$(CC) -L. -o $@ $@.o $(EXTRA_LIBS) -lhexfile -lusb

fpga_load.o: CFLAGS+=-D_GNU_SOURCE	# We use memrchr()

hexfile.o: hexfile.c hexfile.h
	$(CC) $(CFLAGS) $(PEDANTIC) -c $<

test_parse.o: test_parse.c hexfile.h
	$(CC) $(CFLAGS) $(PEDANTIC) -c $<

test_parse: test_parse.o libhexfile.a
	$(CC) -L. -o $@ $@.o $(EXTRA_LIBS) -lhexfile -lusb

print_modes: print_modes.c wctdm_fxomodes.h
	$(HOSTCC) -o $@ $(CFLAGS) $<

wctdm_fxomodes.h: $(WCTDM)
	perl -n -e 'print if (/^static struct fxo_mode {$$/ .. /};$$/)' $(WCTDM) >$@

init_fxo_modes: print_modes
	./$< >$@

clean:
	$(RM) *.o $(TARGETS)
