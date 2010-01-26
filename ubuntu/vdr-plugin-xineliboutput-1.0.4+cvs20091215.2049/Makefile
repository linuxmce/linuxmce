#
# Makefile for a Video Disk Recorder plugin
#
# See the main source file 'xineliboutput.c' for copyright information and
# how to reach the author.
#
# $Id: Makefile,v 1.86 2009/06/02 08:49:32 phintuka Exp $
#

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = xineliboutput

_default: all


# check for Apple Darwin
ARCH_APPLE_DARWIN = no
ifeq ($(shell gcc -dumpmachine | grep -q 'apple-darwin' && echo "1" || echo "0"), 1)
    ARCH_APPLE_DARWIN = yes
endif

#
# Override configuration here or in ../../../Make.config
#

#NOSIGNAL_IMAGE_FILE=/usr/share/vdr/xineliboutput/nosignal.mpv
#STARTUP_IMAGE_FILE=/usr/share/vdr/xineliboutput/logodisplay.mpv
XINELIBOUTPUT_CONFIGURE_OPTS =


###
### The version number of this plugin (taken from the main source file):
###

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | cut -d'"' -f2)


###
### The C++ compiler and options:
###

CXX      ?= g++
CC       ?= gcc 
OPTFLAGS ?= 

ifeq ($(ARCH_APPLE_DARWIN), yes)
    CXXFLAGS   ?= -O3 -pipe -Wall -Woverloaded-virtual -fPIC -g -fno-common -bundle -flat_namespace -undefined suppress
    CFLAGS     ?= -O3 -pipe -Wall -fPIC -g -fno-common -bundle -flat_namespace -undefined suppress
    LDFLAGS_SO ?= -fvisibility=hidden
else
    CXXFLAGS   ?= -O3 -pipe -Wall -Woverloaded-virtual -fPIC -g
    CFLAGS     ?= -O3 -pipe -Wall -fPIC -g
    LDFLAGS_SO ?= -shared -fvisibility=hidden
endif

###
### The directory environment:
###

VDRDIR  ?= ../../..
LIBDIR  ?= ../../lib
TMPDIR  ?= /tmp
BINDIR  ?= /usr/bin
DESTDIR ?= /

INSTALL ?= install

VDRINCDIR ?= $(VDRDIR)/include

###
### Allow user defined options to overwrite defaults:
###

-include $(VDRDIR)/Make.config
-include Make.config


###
### check for VDR
###

ifeq ($(ARCH_APPLE_DARWIN), yes)
    VDRVERSION = $(shell sed -ne '/define VDRVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)
    APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)
else
    VDRVERSION = $(shell sed -ne '/define VDRVERSION/ { s/^.*"\(.*\)".*$$/\1/; p }' $(VDRDIR)/config.h)
    APIVERSION = $(shell sed -ne '/define APIVERSION/ { s/^.*"\(.*\)".*$$/\1/; p }' $(VDRDIR)/config.h)
endif

ifeq ($(strip $(VDRVERSION)),)
    $(warning ********************************************************)
    $(warning VDR not detected ! VDR plugin will not be compiled.     )
    $(warning ********************************************************)
    CONFIGURE_OPTS += --disable-vdr
else
    ifeq ($(strip $(APIVERSION)),)
        $(warning VDR APIVERSION missing, using VDRVERSION $(VDRVERSION) )
        APIVERSION = $(VDRVERSION)
    endif
    CONFIGURE_OPTS += --add-cflags=-I$(VDRDIR)
endif


###
### run configure script
###

config.mak: Makefile configure
	@echo Running configure
	@sh configure --cc=$(CC) --cxx=$(CXX) $(CONFIGURE_OPTS) $(XINELIBOUTPUT_CONFIGURE_OPTS)
-include config.mak

###
### The name of the distribution archive:
###

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)


###
### The name of executable and libraries
###

VDRPLUGIN            = libvdr-$(PLUGIN).so
VDRPLUGIN_SXFE       = lib$(PLUGIN)-sxfe.so
VDRPLUGIN_FBFE       = lib$(PLUGIN)-fbfe.so
VDRSXFE              = vdr-sxfe
VDRFBFE              = vdr-fbfe
XINEINPUTVDR         = xineplug_inp_xvdr.so
XINEPOSTAUTOCROP     = xineplug_post_autocrop.so
XINEPOSTSWSCALE      = xineplug_post_swscale.so
XINEPOSTAUDIOCHANNEL = xineplug_post_audiochannel.so

###
### which programs and libs to build
###

TARGETS_VDR  =
TARGETS_FE   =
TARGETS_XINE =
ifeq ($(XINELIBOUTPUT_VDRPLUGIN), yes)
    TARGETS_VDR += $(VDRPLUGIN)
endif
ifeq ($(XINELIBOUTPUT_XINEPLUGIN), yes)
    TARGETS_XINE += $(XINEINPUTVDR) $(XINEPOSTAUTOCROP) $(XINEPOSTSWSCALE) $(XINEPOSTAUDIOCHANNEL)
endif
ifeq ($(XINELIBOUTPUT_X11), yes)
    TARGETS_FE += $(VDRSXFE)
    ifeq ($(XINELIBOUTPUT_VDRPLUGIN), yes)
        TARGETS_VDR += $(VDRPLUGIN_SXFE)
    endif
endif
ifeq ($(XINELIBOUTPUT_FB), yes)
    TARGETS_FE += $(VDRFBFE)
    ifeq ($(XINELIBOUTPUT_VDRPLUGIN), yes)
        TARGETS_VDR += $(VDRPLUGIN_FBFE)
    endif
endif


###
### Includes and Defines (add further entries here):
###

INCLUDES  += -I$(VDRINCDIR)

ifeq ($(ARCH_APPLE_DARWIN), yes)
    INCLUDES  += -I/sw/include
    LIBDIRS   += -L/sw/lib
    LIBS      += $(LIBDIRS)
else
    LIBS      += -lrt
endif

DEFINES   += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"' \
             -D_REENTRANT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	     -DXINELIBOUTPUT_VERSION='"$(VERSION)"'

ifdef NOSIGNAL_IMAGE_FILE
  DEFINES += -DNOSIGNAL_IMAGE_FILE='"$(NOSIGNAL_IMAGE_FILE)"'
endif
ifdef STARTUP_IMAGE_FILE
  DEFINES += -DSTARTUP_IMAGE_FILE='"$(STARTUP_IMAGE_FILE)"'
endif


###
### The object files (add further files here):
###

# VDR plugin
OBJS = $(PLUGIN).o device.o frontend.o osd.o config.o menu.o setup_menu.o \
       menuitems.o media_player.o equalizer.o \
       frontend_local.o frontend_svr.o \
       tools/cxsocket.o tools/udp_pes_scheduler.o \
       tools/backgroundwriter.o tools/playlist.o tools/http.o \
       tools/vdrdiscovery.o tools/time_pts.o tools.o \
       tools/metainfo_menu.o logdefs.o tools/rle.o
OBJS_MPG = black_720x576.o nosignal_720x576.o vdrlogo_720x576.o

# frontends
OBJS_FE_SO = xine_frontend.o logdefs.o \
             xine/post.o xine/vo_hook.o xine/vo_osdscaler.o xine/vo_osdreorder.o \
             tools/rle.o
OBJS_FE    = $(OBJS_FE_SO) tools/vdrdiscovery.o xine_frontend_main.o xine_frontend_lirc.o

OBJS_SXFE_SO = xine_sxfe_frontend.o $(OBJS_FE_SO)
OBJS_SXFE    = xine_sxfe_frontend.o $(OBJS_FE)
OBJS_FBFE_SO = xine_fbfe_frontend.o $(OBJS_FE_SO)
OBJS_FBFE    = xine_fbfe_frontend.o $(OBJS_FE)

ifneq ($(HAVE_DBUS_GLIB_1), no)
OBJS_SXFE    += tools/gnome_screensaver.o
OBJS_SXFE_SO += tools/gnome_screensaver.o
endif

# xine plugins
OBJS_XINEINPUTVDR = xine_input_vdr.o xine/demux_xvdr.o \
                    xine/ts2es.o xine/demux_xvdr_tsdata.o \
                    xine/adjustable_scr.o xine/xvdr_metronom.o xine/osd_manager.o \
                    tools/rle.o tools/ts.o tools/pes.o tools/mpeg.o tools/h264.o

OBJS_XINE = $(OBJS_XINEINPUTVDR) xine_post_autocrop.o xine_post_swscale.o xine_post_audiochannel.o

###
### Implicit rules:
###

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $(CFLAGS_VDR) -o $@ $<


###
### Dependencies:
###

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile config.mak
	@rm -f $@
	@for i in $(OBJS:%.o=%.c) $(OBJS_SXFE:%.o=%.c) $(OBJS_FBFE:%.o=%.c) $(OBJS_XINE:%.o=%.c) ; do \
	  $(MAKEDEP) $(DEFINES) $(INCLUDES) -MT "`dirname $$i`/`basename $$i .c`.o" $$i >>$@ ; \
	done

-include $(DEPFILE)

DEFINES += -Wall


###
### Rules:
###

mpg2c: mpg2c.c
	$(CC) mpg2c.c -o $@

# data
black_720x576.c: mpg2c black_720x576.mpg
	@./mpg2c black black_720x576.mpg black_720x576.c
nosignal_720x576.c: mpg2c nosignal_720x576.mpg
	@./mpg2c nosignal nosignal_720x576.mpg nosignal_720x576.c
vdrlogo_720x576.c: mpg2c vdrlogo_720x576.mpg
	@./mpg2c vdrlogo vdrlogo_720x576.mpg vdrlogo_720x576.c

# C code (xine plugins and frontends)
$(sort $(OBJS_SXFE) $(OBJS_FBFE) $(OBJS_XINE)):
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(CFLAGS_X11) $(OPTFLAGS) -o $@ $<

### Internationalization (I18N):

PODIR     = po
LOCALEDIR ?= $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmsgs  = $(addprefix $(LOCALEDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --msgid-bugs-address='<phintuka@users.sourceforge.net>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

.PHONY: i18n
i18n: $(I18Nmsgs)

###
### targets
###

XINELIBOUTPUT_INSTALL_MSG =  \
	    $(warning *********************** xineliboutput ***************************) \
	    $(warning  Xine plugins and frontends will not be installed automatically. ) \
	    $(warning  To install files execute "make install" in                      ) \
	    $(warning  $(shell echo `pwd`)) \
	    $(warning *****************************************************************)

install : XINELIBOUTPUT_INSTALL_MSG =

.PHONY: all
all: config $(TARGETS_VDR) frontends i18n

frontends: config $(TARGETS_FE) $(TARGETS_XINE)
	$(XINELIBOUTPUT_INSTALL_MSG)

config: config.mak

.PHONY: config 

.PHONY: frontends install dist clean

#
# VDR plugin
#

$(VDRPLUGIN): $(OBJS) $(OBJS_MPG)
	$(CXX) $(CXXFLAGS) $(LDFLAGS_SO) $(OBJS) $(OBJS_MPG) $(LIBS) $(LIBS_VDR) -o $@
	@-rm -rf $(LIBDIR)/$@.$(APIVERSION)
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

#
# vdr-sxfe
#

$(VDRPLUGIN_SXFE): $(OBJS_SXFE_SO)
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(OBJS_SXFE_SO) $(LIBS_X11) $(LIBS_XINE) -o $@
	@-rm -rf $(LIBDIR)/$(VDRPLUGIN_SXFE).$(VERSION)
	@cp $@ $(LIBDIR)/$(VDRPLUGIN_SXFE).$(VERSION)
$(VDRSXFE): $(OBJS_SXFE)
	$(CC) -g $(OBJS_SXFE) $(LIBS_X11) $(LIBS_XINE) -o $@

#
# vdr-fbfe
#

$(VDRPLUGIN_FBFE): $(OBJS_FBFE_SO)
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(OBJS_FBFE_SO) $(LIBS_XINE) -o $@
	@-rm -rf $(LIBDIR)/$(VDRPLUGIN_FBFE).$(VERSION)
	@cp $@ $(LIBDIR)/$(VDRPLUGIN_FBFE).$(VERSION)
$(VDRFBFE): $(OBJS_FBFE)
	$(CC) -g $(OBJS_FBFE) $(LIBS_XINE) -o $@

#
# xine plugins
#

$(XINEINPUTVDR): $(OBJS_XINEINPUTVDR)
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LIBS_XINE) -o $@ $(OBJS_XINEINPUTVDR)
$(XINEPOSTAUTOCROP): xine_post_autocrop.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LIBS_XINE) -o $@ $<
$(XINEPOSTSWSCALE): xine_post_swscale.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LIBS_XINE) -o $@ $<
$(XINEPOSTAUDIOCHANNEL): xine_post_audiochannel.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LIBS_XINE) -o $@ $<

#
# install
#

install: all
ifeq ($(XINELIBOUTPUT_XINEPLUGIN), yes)
	@mkdir -p $(DESTDIR)/$(XINEPLUGINDIR)/post
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTVDR)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTVDR)
	@$(INSTALL) -m 0644 $(XINEINPUTVDR) $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTVDR)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTAUTOCROP)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTAUTOCROP)
	@$(INSTALL) -m 0644 $(XINEPOSTAUTOCROP) $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTAUTOCROP)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTSWSCALE)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTSWSCALE)
	@$(INSTALL) -m 0644 $(XINEPOSTSWSCALE) $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTSWSCALE)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTAUDIOCHANNEL)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTAUDIOCHANNEL)
	@$(INSTALL) -m 0644 $(XINEPOSTAUDIOCHANNEL) $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTAUDIOCHANNEL)
endif
ifeq ($(XINELIBOUTPUT_FB), yes)
	@echo Installing $(DESTDIR)/$(BINDIR)/vdr-fbfe
	@mkdir -p $(DESTDIR)/$(BINDIR)
	@-rm -rf $(DESTDIR)/$(BINDIR)/vdr-fbfe
	@$(INSTALL) -m 0755 vdr-fbfe $(DESTDIR)/$(BINDIR)/vdr-fbfe
endif
ifeq ($(XINELIBOUTPUT_X11), yes)
	@echo Installing $(DESTDIR)/$(BINDIR)/vdr-sxfe
	@mkdir -p $(DESTDIR)/$(BINDIR)
	@-rm -rf $(DESTDIR)/$(BINDIR)/vdr-sxfe
	@$(INSTALL) -m 0755 vdr-sxfe $(DESTDIR)/$(BINDIR)/vdr-sxfe
endif

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz --exclude=CVS -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz


clean:
	@-rm -f $(DEPFILE) *.so* *.o *.tgz core* *~ *.flc *.bak \
		tools/*.o tools/*~ tools/*.flc xine/*.o xine/*~ \
		xine/*.flc $(VDR_FBFE) $(VDR_SXFE) mpg2c black_720x576.c \
		nosignal_720x576.c vdrlogo_720x576.c vdr-sxfe vdr-fbfe \
		$(PODIR)/*.mo $(PODIR)/*.pot \
		features.h config.mak configure.log

