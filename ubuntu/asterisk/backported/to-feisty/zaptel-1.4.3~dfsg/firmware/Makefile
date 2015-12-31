#
# Asterisk -- A telephony toolkit for Linux.
# 
# Makefile for firmware downloading/installation
#
# Copyright (C) 2007, Digium, Inc.
#
# Joshua Colp <jcolp@digium.com>
#
# This program is free software, distributed under the terms of
# the GNU General Public License
#

.PHONY: dist-clean all uninstall have_download install

-include $(ASTTOPDIR)/menuselect.makeopts $(ASTTOPDIR)/makeopts

PWD:=$(shell pwd)

OCT6114_064_VERSION:=1.05.01
OCT6114_128_VERSION:=1.05.01
TC400M_VERSION:=MR5.6

FIRMWARE_URL:=http://ftp.digium.com/pub/telephony/firmware/releases

# Firmware files should use the naming convention: zaptel-fw-<base name>-<sub name>-<version> or zaptel-fw-<base name>-<version>
# First example: zaptel-fw-oct6114-064-1.05.01
# This means this is version 1.05.01 of the oct6114 64 firmware
# Second example: zaptel-fw-tc400m-MR5.6
# This means this is version MR5.6 of the tc400m firmware

# Build a list of firmware package filenames we need
FIRMWARE:=$(MENUSELECT_FIRMWARE:FIRMWARE-OCT6114-064=zaptel-fw-oct6114-064-$(OCT6114_064_VERSION).tar.gz)
FIRMWARE:=$(FIRMWARE:FIRMWARE-OCT6114-128=zaptel-fw-oct6114-128-$(OCT6114_128_VERSION).tar.gz)
FIRMWARE:=$(FIRMWARE:FIRMWARE-TC400M=zaptel-fw-tc400m-$(TC400M_VERSION).tar.gz)

# Build a list of object files if hotplug will not be used
OBJECT_FILES:=$(MENUSELECT_FIRMWARE:FIRMWARE-OCT6114-064=zaptel-fw-oct6114-064.o)
OBJECT_FILES:=$(OBJECT_FILES:FIRMWARE-OCT6114-128=zaptel-fw-oct6114-128.o)
OBJECT_FILES:=$(OBJECT_FILES:FIRMWARE-TC400M=zaptel-fw-tc400m.o)

# If "fetch" is used, --continue is not a valid option.
ifeq ($(WGET),wget)
WGET_ARGS:=--continue
endif

all: $(FIRMWARE)

have_download:
	@if test "$(DOWNLOAD)" = ":" ; then \
		echo "**************************************************"; \
		echo "***                                            ***"; \
		echo "*** You must have either wget or fetch to be   ***"; \
		echo "*** able to automatically download and install ***"; \
		echo "*** the requested firmware packages.           ***"; \
		echo "***                                            ***"; \
		echo "**************************************************"; \
		exit 1; \
	fi

# Download and extract firmware tarballs
zaptel-fw-oct6114-064-%.tar.gz: have_download
ifeq ($(shell if ( [ "$(HOTPLUG_FIRMWARE)" == "no" ] ) || ( [ -d $(INSTALL_PREFIX)/usr/lib/hotplug/firmware ] && ! [ -f $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw-oct6114-064-$(OCT6114_064_VERSION) ] ) || ( [ -d $(INSTALL_PREFIX)/lib/firmware ] && ! [ -f $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw-oct6114-064-$(OCT6114_064_VERSION) ] ); then echo "yes"; else echo "no"; fi),yes)
	@echo "Attempting to download $@"
	@if test ! -f $@; then $(DOWNLOAD) $(WGET_ARGS) $(FIRMWARE_URL)/$@; fi;
	if test ! -f $@; then exit 1; fi; \
	(cat $@ | gzip -d | tar -xf -)
ifeq ($(HOTPLUG_FIRMWARE),yes)
	@echo "Installing zaptel-fw-oct6114-064.bin to hotplug firmware directories"
	if [ -d $(INSTALL_PREFIX)/usr/lib/hotplug/firmware ]; then \
		$(INSTALL) -m 644 zaptel-fw-oct6114-064.bin $(INSTALL_PREFIX)/usr/lib/hotplug/firmware; \
		rm -rf $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw-oct6114-064-*; \
		touch $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw-oct6114-064-$(OCT6114_064_VERSION); \
	fi
	if [ -d $(INSTALL_PREFIX)/lib/firmware ]; then \
		$(INSTALL) -m 644 zaptel-fw-oct6114-064.bin $(INSTALL_PREFIX)/lib/firmware; \
		rm -rf $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw-oct6114-064-*; \
		touch $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw-oct6114-064-$(OCT6114_064_VERSION); \
	fi
endif
else
	@echo "Firmware zaptel-fw-oct6114-064.bin is already installed with required version $(OCT6114_064_VERSION)"
endif

zaptel-fw-oct6114-128-%.tar.gz: have_download
ifeq ($(shell if ( [ "$(HOTPLUG_FIRMWARE)" == "no" ] ) || ( [ -d $(INSTALL_PREFIX)/usr/lib/hotplug/firmware ] && ! [ -f $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw-oct6114-128-$(OCT6114_128_VERSION) ] ) || ( [ -d $(INSTALL_PREFIX)/lib/firmware ] && ! [ -f $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw-oct6114-128-$(OCT6114_128_VERSION) ] ); then echo "yes"; else echo "no"; fi),yes)
	@echo "Attempting to download $@"
	@if test ! -f $@; then $(DOWNLOAD) $(WGET_ARGS) $(FIRMWARE_URL)/$@; fi;
	if test ! -f $@; then exit 1; fi; \
	(cat $@ | gzip -d | tar -xf -)
ifeq ($(HOTPLUG_FIRMWARE),yes)
	@echo "Installing zaptel-fw-oct6114-128.bin to hotplug firmware directories"
	if [ -d $(INSTALL_PREFIX)/usr/lib/hotplug/firmware ]; then \
		$(INSTALL) -m 644 zaptel-fw-oct6114-128.bin $(INSTALL_PREFIX)/usr/lib/hotplug/firmware; \
		rm -rf $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw-oct6114-128-*; \
		touch $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw-oct6114-128-$(OCT6114_128_VERSION); \
	fi
	if [ -d $(INSTALL_PREFIX)/lib/firmware ]; then \
		$(INSTALL) -m 644 zaptel-fw-oct6114-128.bin $(INSTALL_PREFIX)/lib/firmware; \
		rm -rf $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw-oct6114-128-*; \
		touch $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw-oct6114-128-$(OCT6114_128_VERSION); \
	fi
endif
else
	@echo "Firmware zaptel-fw-oct6114-128.bin is already installed with required version $(OCT6114_128_VERSION)"
endif

zaptel-fw-tc400m-%.tar.gz: have_download
ifeq ($(shell if ( [ "$(HOTPLUG_FIRMWARE)" == "no" ] ) || ( [ -d $(INSTALL_PREFIX)/usr/lib/hotplug/firmware ] && ! [ -f $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw-tc400m-$(TC400M_VERSION) ] ) || ( [ -d $(INSTALL_PREFIX)/lib/firmware ] && ! [ -f $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw-tc400m-$(TC400M_VERSION) ] ); then echo "yes"; else echo "no"; fi),yes)
	@echo "Attempting to download $@"
	@if test ! -f $@; then $(DOWNLOAD) $(WGET_ARGS) $(FIRMWARE_URL)/$@; fi;
	if test ! -f $@; then exit 1; fi; \
	(cat $@ | gzip -d | tar -xf -)
ifeq ($(HOTPLUG_FIRMWARE),yes)
	@echo "Installing zaptel-fw-tc400m.bin to hotplug firmware directories"
	if [ -d $(INSTALL_PREFIX)/usr/lib/hotplug/firmware ]; then \
		$(INSTALL) -m 644 zaptel-fw-tc400m.bin $(INSTALL_PREFIX)/usr/lib/hotplug/firmware; \
		rm -rf $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw-tc400m-*; \
		touch $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw-tc400m-$(TC400M_VERSION); \
	fi
	if [ -d $(INSTALL_PREFIX)/lib/firmware ]; then \
		$(INSTALL) -m 644 zaptel-fw-tc400m.bin $(INSTALL_PREFIX)/lib/firmware; \
		rm -rf $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw-tc400m-*; \
		touch $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw-tc400m-$(TC400M_VERSION); \
	fi
endif
else
	@echo "Firmware zaptel-fw-tc400m.bin is already installed with required version $(TC400M_VERSION)"
endif

# Clean up any downloaded/extracted firmware packages
dist-clean: clean
	rm -f zaptel-fw-*.bin
	rm -f zaptel-fw-*.tar.gz

# Clean up anything we built
clean:
	rm -f zaptel-fw-*.o

# Create object files suitable for linking against
object-build: $(FIRMWARE) $(OBJECT_FILES)

# Install all downloaded firmware images for hotplug usage and build headers for inclusion
hotplug-install: $(FIRMWARE)

# Uninstall any installed zaptel firmware images from hotplug firmware directories
hotplug-uninstall:
	if [ -d $(INSTALL_PREFIX)/usr/lib/hotplug/firmware ]; then \
		rm -f $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/zaptel-fw-*.bin; \
		rm -f $(INSTALL_PREFIX)/usr/lib/hotplug/firmware/.zaptel-fw*; \
	fi
	if [ -d $(INSTALL_PREFIX)/lib/firmware ]; then \
		rm -f $(INSTALL_PREFIX)/lib/firmware/zaptel-fw-*.bin; \
		rm -f $(INSTALL_PREFIX)/lib/firmware/.zaptel-fw*; \
	fi

# Build object file of an oct6114 064 firmware image for linking
zaptel-fw-oct6114-064.o: zaptel-fw-oct6114-064-$(OCT6114_064_VERSION).tar.gz ../wct4xxp/base.o zaptel-fw-oct6114-064.bin
	@echo Making firmware object file for zaptel-fw-oct6114-064.bin
	../build_tools/make_firmware_object zaptel-fw-oct6114-064.bin $@ ../wct4xxp/base.o

# Build object file of an oct6114 128 firmware image for linking
zaptel-fw-oct6114-128.o: zaptel-fw-oct6114-128-$(OCT6114_128_VERSION).tar.gz ../wct4xxp/base.o zaptel-fw-oct6114-128.bin
	@echo Making firmware object file for zaptel-fw-oct6114-128.bin
	../build_tools/make_firmware_object zaptel-fw-oct6114-128.bin $@ ../wct4xxp/base.o

# Build object file of a TC400M firmware image for linking
zaptel-fw-tc400m.o:
