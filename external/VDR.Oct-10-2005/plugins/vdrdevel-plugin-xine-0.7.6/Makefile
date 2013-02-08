#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = xine

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -O2 -g -Wall -Woverloaded-virtual
#CXXFLAGS ?= -O0 -g3 -Wall -Woverloaded-virtual

### The directory environment:

DVBDIR = ../../../../DVB
VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

INCLUDES += `xine-config --cflags` 

# where to create fifos (xine expects them at /tmp/vdr-xine)
VDR_XINE_FIFO_DIR ?= /tmp/vdr-xine

# enable to fully support yaepg plugin
#VDR_XINE_SET_VIDEO_WINDOW = 1

# where are these utilities for image grabbing? (default: anywhere on your PATH)
#VDR_XINE_Y4MSCALER = /usr/bin/y4mscaler
#VDR_XINE_Y4MTOPPM  = /usr/bin/y4mtoppm
#VDR_XINE_PNMCUT    = /usr/bin/pnmcut
#VDR_XINE_PNMTOJPEG = /usr/bin/pnmtojpeg

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

DEFINES += -DFIFO_DIR=\"$(VDR_XINE_FIFO_DIR)\"

ifdef VDR_XINE_SET_VIDEO_WINDOW
DEFINES += -DSET_VIDEO_WINDOW
endif

ifdef VDR_XINE_Y4MSCALER
DEFINES += -DY4MSCALER=\"$(VDR_XINE_Y4MSCALER)\"
endif

ifdef VDR_XINE_Y4MTOPPM
DEFINES += -DY4MTOPPM=\"$(VDR_XINE_Y4MTOPPM)\"
endif

ifdef VDR_XINE_PNMCUT
DEFINES += -DPNMCUT=\"$(VDR_XINE_PNMCUT)\"
endif

ifdef VDR_XINE_PNMTOJPEG
DEFINES += -DPNMTOJPEG=\"$(VDR_XINE_PNMTOJPEG)\"
endif

### The object files (add further files here):

OBJS = $(PLUGIN).o xineDevice.o xineLib.o xineOsd.o xineSettings.o xineSetupPage.o xineRemote.o xineExternal.o

### Implicit rules:

%.o: %.c Makefile
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) xineplayer.c > $@

-include $(DEPFILE)

### Targets:

all: libvdr-$(PLUGIN).so xineplayer

libvdr-$(PLUGIN).so: $(OBJS) Makefile
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -o $@
	@cp $@ $(LIBDIR)/$@.$(VDRVERSION)

xineplayer: xineplayer.o Makefile
	$(CXX) $(CXXFLAGS) xineplayer.o -o $@

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~ xineplayer xineplayer.o
