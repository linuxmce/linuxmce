#
# Makefile for IPTV plugin
#

# Default shell for EXT protocol

#IPTV_EXTSHELL = /bin/bash

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = iptv

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'const char VERSION\[\] *=' $(PLUGIN).c | awk '{ print $$5 }' | sed -e 's/[";]//g')
GITTAG  = $(shell git describe --always 2>/dev/null)

### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell PKG_CONFIG_PATH="$$PKG_CONFIG_PATH:../../.." pkg-config --variable=$(1) vdr))
LIBDIR = $(call PKGCFG,libdir)
LOCDIR = $(call PKGCFG,locdir)
PLGCFG = $(call PKGCFG,plgcfg)
RESDIR = $(call PKGCFG,resdir)
CFGDIR = $(call PKGCFG,configdir)
#
TMPDIR ?= /tmp

### The compiler options:

export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)
STRIP           ?= /bin/true

### The version number of VDR's plugin API:

APIVERSION = $(call PKGCFG,apiversion)

### Allow user defined options to overwrite defaults:

-include $(PLGCFG)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### The name of the shared object file:

SOFILE = libvdr-$(PLUGIN).so

### Libraries

LIBS = $(shell curl-config --libs)

### Includes and Defines (add further entries here):

INCLUDES +=

DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

ifdef IPTV_EXTSHELL
DEFINES += -DEXTSHELL='"$(IPTV_EXTSHELL)"'
endif

ifneq ($(strip $(GITTAG)),)
DEFINES += -DGITVERSION='"-GIT-$(GITTAG)"'
endif

.PHONY: all all-redirect
all-redirect: all

### The object files (add further files here):

OBJS = $(PLUGIN).o common.o config.o device.o pidscanner.o \
	protocolcurl.o protocolext.o protocolfile.o protocolhttp.o \
	protocoludp.o sectionfilter.o setup.o sidscanner.o socket.o \
	source.o statistics.o streamer.o

### The main target:

all: $(SOFILE) i18n

### Implicit rules:

%.o: %.c Makefile
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Nmsgs  = $(addprefix $(DESTDIR)$(LOCDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --package-name=vdr-$(PLUGIN) --package-version=$(VERSION) --msgid-bugs-address='<see README>' -o $@ `ls $^`

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q -N $@ $<
	@touch $@

$(I18Nmsgs): $(DESTDIR)$(LOCDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	install -D -m644 $< $@

.PHONY: i18n
i18n: $(I18Nmo) $(I18Npot)

install-i18n: $(I18Nmsgs)

### Targets:

$(SOFILE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@$(STRIP) $@

install-lib: $(SOFILE)
	install -D $^ $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)

install-conf:
	@mkdir -p $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)
	@mkdir -p $(DESTDIR)$(RESDIR)/plugins/$(PLUGIN)
	@cp -pn $(PLUGIN)/* $(DESTDIR)$(RESDIR)/plugins/$(PLUGIN)/

install: install-lib install-i18n install-conf

dist: $(I18Npo) clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~

.PHONY: cppcheck
cppcheck:
	@cppcheck --language=c++ --enable=all -v -f $(OBJS:%.o=%.c)
