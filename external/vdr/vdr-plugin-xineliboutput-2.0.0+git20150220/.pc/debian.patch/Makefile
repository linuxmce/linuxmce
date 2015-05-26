#
# Makefile for a Video Disk Recorder plugin
#
# See the main source file 'xineliboutput.c' for copyright information and
# how to reach the author.
#
# $Id: Makefile,v 1.119 2014-06-17 11:22:15 phintuka Exp $
#

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = xineliboutput

_default: all

# Keep VDR Makefile happy - it requires $(LIBDIR)/.$(APIVERSION) somewhere in this file ...

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
    LDFLAGS_SO ?= -fvisibility=hidden
    LDFLAGS_SO += -shared
endif

###
### The directory environment:
###

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell PKG_CONFIG_PATH="$$PKG_CONFIG_PATH:../../.." pkg-config --variable=$(1) vdr))
LIBDIR = $(call PKGCFG,libdir)
LOCDIR = $(call PKGCFG,locdir)
PLGCFG = $(call PKGCFG,plgcfg)
VIDEODIR = $(call PKGCFG,videodir)

TMPDIR  ?= /tmp
BINDIR  ?= /usr/bin

INSTALL ?= install

###
### Allow user defined options to overwrite defaults:
###

-include Make.config


###
### check for VDR
###

APIVERSION = $(call PKGCFG,apiversion)

VDR_TREE = no
ifeq ($(strip $(APIVERSION)),)
    $(warning ********************************************************)
    $(warning VDR not detected ! VDR plugins will not be compiled.    )
    $(warning ********************************************************)
    CONFIGURE_OPTS += --disable-vdr
else
    export CFLAGS   = $(call PKGCFG,cflags)
    export CXXFLAGS = $(call PKGCFG,cxxflags)

    ifeq ($(VDRDIR),)
        $(warning Building outside VDR source tree)
    else
        $(warning Building inside VDR source tree)
        VDR_TREE = yes
    endif
endif

### Allow user defined options to overwrite defaults:
-include $(PLGCFG)

###
### run configure script
###

config.mak: Makefile configure
	@echo Running configure
	@sh configure --cc="$(CC)" --cxx="$(CXX)" $(CONFIGURE_OPTS) $(XINELIBOUTPUT_CONFIGURE_OPTS)
-include config.mak

###
### The name of the distribution archive:
###

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)


###
### The name of executable and libraries
###

VDRPLUGIN            = libvdr-$(PLUGIN).so.$(APIVERSION)
VDRPLUGIN_SXFE       = lib$(PLUGIN)-sxfe.so.$(VERSION)
VDRPLUGIN_FBFE       = lib$(PLUGIN)-fbfe.so.$(VERSION)
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

ifeq ($(ARCH_APPLE_DARWIN), yes)
    INCLUDES  += -I/sw/include
    LIBDIRS   += -L/sw/lib
    LIBS      += $(LIBDIRS)
else
    LIBS      += -lrt
endif

DEFINES   += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"' \
             -D_REENTRANT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	     -DVIDEODIR='"$(VIDEODIR)"' -DXINELIBOUTPUT_VERSION='"$(VERSION)"'

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
ifeq ($(XINELIBOUTPUT_VDRPLUGIN), yes)
OBJS = $(PLUGIN).o device.o frontend.o osd.o config.o menu.o setup_menu.o \
       menuitems.o media_player.o equalizer.o \
       frontend_local.o frontend_svr.o \
       tools/cxsocket.o tools/udp_pes_scheduler.o \
       tools/backgroundwriter.o tools/playlist.o tools/http.o \
       tools/vdrdiscovery.o tools/time_pts.o tools.o \
       tools/metainfo_menu.o logdefs.o tools/rle.o
OBJS_MPG = black_720x576.o nosignal_720x576.o vdrlogo_720x576.o
endif

# frontends
OBJS_FE_SO = xine_frontend.o logdefs.o \
             xine/post.o xine/vo_hook.o xine/vo_osdscaler.o xine/vo_osdreorder.o xine/vo_lastpts.o \
             xine/vo_frameoutput.o \
             tools/rle.o
OBJS_FE    = $(OBJS_FE_SO) \
             xine_frontend_main.o \
             xine_frontend_lirc.o xine_frontend_kbd.o xine_frontend_cec.o \
             tools/vdrdiscovery.o
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

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile config.mak
	@rm -f $@
	@for i in $(OBJS:%.o=%.c) $(OBJS_SXFE:%.o=%.c) $(OBJS_FBFE:%.o=%.c) $(OBJS_XINE:%.o=%.c) ; do \
	  $(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -MT "`dirname $$i`/`basename $$i .c`.o" $$i >>$@ ; \
	done

-include $(DEPFILE)

DEFINES += -Wall


###
### Rules:
###

mpg2c: mpg2c.c
	$(CC) $(CFLAGS) $(LDFLAGS) mpg2c.c -o $@

# data
black_720x576.c: mpg2c black_720x576.mpg
	@./mpg2c black black_720x576.mpg black_720x576.c
nosignal_720x576.c: mpg2c nosignal_720x576.mpg
	@./mpg2c nosignal nosignal_720x576.mpg nosignal_720x576.c
vdrlogo_720x576.c: mpg2c vdrlogo_720x576.mpg
	@./mpg2c vdrlogo vdrlogo_720x576.mpg vdrlogo_720x576.c

# C code (xine plugins and frontends)
$(sort $(OBJS_SXFE) $(OBJS_FBFE) $(OBJS_XINE)): %.o: %.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(CFLAGS_X11) $(CFLAGS_AVUTIL) $(OPTFLAGS) -o $@ $<

### Internationalization (I18N):
PODIR     = po
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Nmsgs  = $(addprefix $(DESTDIR)$(LOCDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --package-name=vdr-$(PLUGIN) --package-version=$(VERSION) --msgid-bugs-address='<phintuka@users.sourceforge.net>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q -N $@ $<
	@touch $@

$(I18Nmsgs): $(DESTDIR)$(LOCDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
ifeq ($(XINELIBOUTPUT_VDRPLUGIN), yes)
	@echo Installing $^
	install -D -m644 $< $@
endif

.PHONY: i18n
i18n: $(I18Nmo) $(I18Npot)

install-i18n: $(I18Nmsgs)

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
	$(CXX) $(CXXFLAGS) $(LDFLAGS_SO) $(LDFLAGS) -shared $(OBJS) $(OBJS_MPG) $(LIBS) $(LIBS_VDR) -o $@
ifeq ($(VDR_TREE), yes)
	$(INSTALL) $@ $(LIBDIR)/
endif

install-lib: $(TARGETS_VDR)
ifeq ($(XINELIBOUTPUT_VDRPLUGIN), yes)
	@echo Installing $^
	install -D $^ $(DESTDIR)$(LIBDIR)/
endif

install: install-lib install-i18n

#
# vdr-sxfe
#

$(VDRPLUGIN_SXFE): $(OBJS_SXFE_SO)
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LDFLAGS) $(OBJS_SXFE_SO) $(LIBS_X11) $(LIBS_XINE) $(LIBS_JPEG) -o $@
ifeq ($(VDR_TREE), yes)
	$(INSTALL) $@ $(LIBDIR)/
endif
$(VDRSXFE): $(OBJS_SXFE)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS_SXFE) $(LIBS_X11) $(LIBS_XINE) $(LIBS_JPEG) $(LIBS_CEC) $(LIBS_PTHREAD) -o $@

#
# vdr-fbfe
#

$(VDRPLUGIN_FBFE): $(OBJS_FBFE_SO)
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LDFLAGS) $(OBJS_FBFE_SO) $(LIBS_XINE) $(LIBS_JPEG) -o $@
ifeq ($(VDR_TREE), yes)
	$(INSTALL) $@ $(LIBDIR)/
endif
$(VDRFBFE): $(OBJS_FBFE)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS_FBFE) $(LIBS_XINE) $(LIBS_JPEG) $(LIBS_CEC) $(LIBS_PTHREAD) -o $@

#
# xine plugins
#

$(XINEINPUTVDR): $(OBJS_XINEINPUTVDR)
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LDFLAGS) $(OBJS_XINEINPUTVDR) $(LIBS_XINE) $(LIBS_AVUTIL) $(LIBS_PTHREAD) -o $@
$(XINEPOSTAUTOCROP): xine_post_autocrop.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LDFLAGS) $< -o $@ $(LIBS_XINE)
$(XINEPOSTSWSCALE): xine_post_swscale.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LDFLAGS) $< -o $@ $(LIBS_XINE)
$(XINEPOSTAUDIOCHANNEL): xine_post_audiochannel.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(LDFLAGS) $< -o $@ $(LIBS_XINE)

#
# install
#

install: all
ifeq ($(XINELIBOUTPUT_XINEPLUGIN), yes)
	@mkdir -p $(DESTDIR)$(XINEPLUGINDIR)/post
	@echo Installing $(DESTDIR)$(XINEPLUGINDIR)/$(XINEINPUTVDR)
	@-rm -rf $(DESTDIR)$(XINEPLUGINDIR)/$(XINEINPUTVDR)
	@$(INSTALL) -m 0644 $(XINEINPUTVDR) $(DESTDIR)$(XINEPLUGINDIR)/$(XINEINPUTVDR)
	@echo Installing $(DESTDIR)$(XINEPLUGINDIR)/post/$(XINEPOSTAUTOCROP)
	@-rm -rf $(DESTDIR)$(XINEPLUGINDIR)/post/$(XINEPOSTAUTOCROP)
	@$(INSTALL) -m 0644 $(XINEPOSTAUTOCROP) $(DESTDIR)$(XINEPLUGINDIR)/post/$(XINEPOSTAUTOCROP)
	@echo Installing $(DESTDIR)$(XINEPLUGINDIR)/post/$(XINEPOSTSWSCALE)
	@-rm -rf $(DESTDIR)$(XINEPLUGINDIR)/post/$(XINEPOSTSWSCALE)
	@$(INSTALL) -m 0644 $(XINEPOSTSWSCALE) $(DESTDIR)$(XINEPLUGINDIR)/post/$(XINEPOSTSWSCALE)
	@echo Installing $(DESTDIR)$(XINEPLUGINDIR)/post/$(XINEPOSTAUDIOCHANNEL)
	@-rm -rf $(DESTDIR)$(XINEPLUGINDIR)/post/$(XINEPOSTAUDIOCHANNEL)
	@$(INSTALL) -m 0644 $(XINEPOSTAUDIOCHANNEL) $(DESTDIR)$(XINEPLUGINDIR)/post/$(XINEPOSTAUDIOCHANNEL)
endif
ifeq ($(XINELIBOUTPUT_FB), yes)
	@echo Installing $(DESTDIR)$(BINDIR)/vdr-fbfe
	@mkdir -p $(DESTDIR)$(BINDIR)
	@-rm -rf $(DESTDIR)$(BINDIR)/vdr-fbfe
	@$(INSTALL) -m 0755 vdr-fbfe $(DESTDIR)$(BINDIR)/vdr-fbfe
endif
ifeq ($(XINELIBOUTPUT_X11), yes)
	@echo Installing $(DESTDIR)$(BINDIR)/vdr-sxfe
	@mkdir -p $(DESTDIR)$(BINDIR)
	@-rm -rf $(DESTDIR)$(BINDIR)/vdr-sxfe
	@$(INSTALL) -m 0755 vdr-sxfe $(DESTDIR)$(BINDIR)/vdr-sxfe
endif

dist: $(I18Npo) clean
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
		features.h config.mak configure.log
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
