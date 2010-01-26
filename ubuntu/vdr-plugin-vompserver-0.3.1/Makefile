#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.17 2009/05/30 14:22:59 christallon Exp $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = vompserver

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
ifdef DEBUG
CXXFLAGS ?= -g -fPIC -Wall -Woverloaded-virtual #-Werror
else
CXXFLAGS ?= -O2 -fPIC -Wall -Woverloaded-virtual #-Werror
endif

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### read standlone settings if there
-include .standalone

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"' -DVOMPSERVER
DEFINES += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE

### The object files (add further files here):

OBJS = $(PLUGIN).o dsock.o mvpserver.o udpreplier.o bootpd.o tftpd.o i18n.o vompclient.o tcp.o \
                   ringbuffer.o mvprelay.o vompclientrrproc.o \
                   config.o log.o thread.o tftpclient.o \
                   media.o responsepacket.o \
                   mediafile.o mediaplayer.o servermediafile.o serialize.o medialauncher.o

OBJS2 = recplayer.o mvpreceiver.o

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: allbase libvdr-$(PLUGIN).so
standalone: standalonebase vompserver-standalone

objectsstandalone: $(OBJS)
objects: $(OBJS) $(OBJS2)

allbase:
	( if [ -f .standalone ] ; then ( rm -f .standalone; make clean ; make objects ) ; else exit 0 ;fi )
standalonebase:
	( if [ ! -f .standalone ] ; then ( make clean; echo "DEFINES+=-DVOMPSTANDALONE" > .standalone; echo "DEFINES+=-D_FILE_OFFSET_BITS=64" >> .standalone; make objectsstandalone ) ; else exit 0 ;fi )

libvdr-$(PLUGIN).so: objects
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(OBJS2) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

vompserver-standalone: objectsstandalone
	$(CXX) $(CXXFLAGS) $(OBJS) -lpthread -o $@
	chmod u+x $@

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	rm -f $(OBJS) $(OBJS2) $(DEPFILE) libvdr*.so.* *.tgz core* *~ .standalone vompserver-standalone

