#
# Makefile for a Video Disk Recorder plugin
#
# $Id:
#
#

# optional
#HAVE_C295 = 1 # uncomment this, if you use c++2.95 and get error "redefinition of 'template <class T> void swap(_Tp &, _Tp &)'"
#HAVE_IMAGEMAGICK = 1 # uncomment this, if you have ImageMagick and want see images of epg
#HAVE_YAEPG_PATCH = 1 # only experimentell - uncomment this, if vdr patched with YAEPG-PATCH and you want see video-window in menu
#VIDEOSTATUS = 1 # only experimentell - uncomment this, if you want some infos of videostatus in DisplayChannel and DisplayReplay
#DEBUG = 1 # only for threads in DisplayMenu.c for screenshot-display of video-window and for epgimages

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = skinelchi

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -Woverloaded-virtual

### The directory environment:

DVBDIR = ../../../../DVB
VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

ifdef HAVE_IMAGEMAGICK
# Add Magick++-linker-info ### from osdimage@brougs78
#LIBS += $(shell Magick++-config --ldflags --libs)
# Add Magick++-linker-info ### from text2skin
#LIBS += -lMagick -lMagick++
# The only needed library ???
	LIBS += -lMagick++
###
	DEFINES += -DHAVE_IMAGEMAGICK

ifdef HAVE_C295
	DEFINES += -DHAVE_C295
endif
endif

ifdef HAVE_YAEPG_PATCH
	DEFINES += -DHAVE_YAEPG_PATCH
endif

ifdef VIDEOSTATUS
	DEFINES += -DVIDEOSTATUS
endif

ifdef DEBUG
	DEFINES += -DDEBUG
endif

### The object files (add further files here):
ifdef HAVE_IMAGEMAGICK
OBJS = bitmap.o
endif
OBJS += i18n.o setup.o DisplayChannel.o DisplayMenu.o DisplayReplay.o DisplayVolume.o DisplayTracks.o DisplayMessage.o $(PLUGIN).o

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: libvdr-$(PLUGIN).so

libvdr-$(PLUGIN).so: $(OBJS)
#	$(CXX) $(CXXFLAGS) -shared $(OBJS) -lncurses -o $@
#	$(CXX) $(CXXFLAGS) -shared $(OBJS) -o $@
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp $@ $(LIBDIR)/$@.$(VDRVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
