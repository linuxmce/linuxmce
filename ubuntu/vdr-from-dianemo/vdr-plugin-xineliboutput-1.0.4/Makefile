#
# Makefile for a Video Disk Recorder plugin
#
# See the main source file 'xineliboutput.c' for copyright information and
# how to reach the author.
#
# $Id: Makefile,v 1.38.2.2 2008/11/17 17:01:41 phintuka Exp $
#

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = xineliboutput

_default: all

###
### check for xine-lib and X11
###

XINELIBOUTPUT_FB  = 0
XINELIBOUTPUT_X11 = 0
HAVE_XRENDER      = 0
HAVE_XDPMS        = 0
HAVE_XINERAMA     = 0
HAVE_EXTRACTOR_H  = 0
APPLE_DARWIN      = 0
XINELIBOUTPUT_XINEPLUGIN = 0

# check for xine-lib
ifeq ($(shell (pkg-config libxine && echo 1 || echo 0)), 1)
    XINELIBOUTPUT_XINEPLUGIN = 1
else
    ifeq ($(shell (xine-config --cflags >/dev/null 2>&1 && echo "1") || echo "0"), 1)
        XINELIBOUTPUT_XINEPLUGIN = 1
    endif
endif

# check for X11 and Xrender extension
ifeq ($(XINELIBOUTPUT_XINEPLUGIN), 1)
    XINELIBOUTPUT_FB  = $(XINELIBOUTPUT_XINEPLUGIN)
    ifeq ($(shell (((echo "\#include <X11/Xlib.h>";echo "int main(int c,char* v[]) {return 0;}") > testx.c && gcc -c testx.c -o testx.o >/dev/null 2>&1) && echo "1") || echo "0" ; rm -f testx.* >/dev/null), 1)
        XINELIBOUTPUT_X11 = 1
        ifeq ($(shell (((echo "\#include <X11/extensions/Xrender.h>";echo "int main(int c,char* v[]) {return 0;}") > testx.c && gcc -c testx.c -o testx.o >/dev/null 2>&1) && echo "1") || echo "0" ; rm -f testx.* >/dev/null), 1)
            HAVE_XRENDER = 1
        else
            $(warning ********************************************************)
            $(warning XRender extension not detected ! HUD OSD disabled.      )
            $(warning ********************************************************)
        endif
        ifeq ($(shell (((echo "\#include <X11/Xlib.h>";echo "\#include <X11/extensions/dpms.h>";echo "int main(int c,char* v[]) {return 0;}") > testx.c && gcc -c testx.c -o testx.o >/dev/null 2>&1) && echo "1") || echo "0" ; rm -f testx.* >/dev/null), 1)
            HAVE_XDPMS = 1
        else
            $(warning XDPMS extension not detected. )
        endif
        ifeq ($(shell (((echo "\#include <X11/extensions/Xinerama.h>";echo "int main(int c,char* v[]) {return 0;}") > testx.c && gcc -c testx.c -o testx.o >/dev/null 2>&1) && echo "1") || echo "0" ; rm -f testx.* >/dev/null), 1)
            HAVE_XINERAMA = 1
        else
            $(warning Xinerama extension not detected. )
        endif
    else
        $(warning ********************************************************)
        $(warning X11 not detected ! X11 frontends will not be compiled.  )
        $(warning ********************************************************)
    endif
else
    $(warning ********************************************************)
    $(warning xine-lib not detected ! frontends will not be compiled. )
    $(warning ********************************************************)
endif

# check for Apple Darwin
ifeq ($(shell gcc -dumpmachine | grep -q 'apple-darwin' && echo "1" || echo "0"), 1)
    APPLE_DARWIN = 1
endif

#
# Override configuration here or in ../../../Make.config
#

USE_ICONV = 1
#XINELIBOUTPUT_X11        = 1
#HAVE_XRENDER             = 1
#HAVE_XDPMS               = 1
#HAVE_XINERAMA            = 1
#HAVE_EXTRACTOR_H         = 1
#XINELIBOUTPUT_FB         = 1
#XINELIBOUTPUT_XINEPLUGIN = 1
#XINELIBOUTPUT_VDRPLUGIN  = 1
#ENABLE_TEST_POSTPLUGINS  = 1
#NOSIGNAL_IMAGE_FILE=/usr/share/vdr/xineliboutput/nosignal.mpv
#STARTUP_IMAGE_FILE=/usr/share/vdr/xineliboutput/logodisplay.mpv


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

ifeq ($(APPLE_DARWIN), 1)
    CXXFLAGS   ?= -O3 -pipe -Wall -Woverloaded-virtual -fPIC -g -fno-common -bundle -flat_namespace -undefined suppress
    CFLAGS     ?= -O3 -pipe -Wall -fPIC -g -fno-common -bundle -flat_namespace -undefined suppress
    LDFLAGS_SO ?= -fvisibility=hidden
else
    CXXFLAGS   ?= -O3 -pipe -Wall -Woverloaded-virtual -fPIC -g 
    CFLAGS     ?= -O3 -pipe -Wall -fPIC -g
    LDFLAGS_SO ?= -shared -fvisibility=hidden 
endif
LIBS_VDR ?= 

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

ifeq ($(APPLE_DARWIN), 1)
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
    XINELIBOUTPUT_VDRPLUGIN = 0
else
    ifeq ($(strip $(APIVERSION)),)
        $(warning VDR APIVERSION missing, using VDRVERSION $(VDRVERSION) )
        APIVERSION = $(VDRVERSION)
    endif
    XINELIBOUTPUT_VDRPLUGIN = 1
    ifeq ($(shell pkg-config libextractor && echo "1"), 1)
        HAVE_EXTRACTOR_H = 1
    else
        $(warning libextractor not found.)
    endif
endif


###
### The name of the distribution archive:
###

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)


###
### The name of executable and libraries
###

VDRSXFE      = vdr-sxfe
VDRFBFE      = vdr-fbfe
XINEINPUTVDR = xineplug_inp_xvdr.so
XINEPOSTAUTOCROP  = xineplug_post_autocrop.so
XINEPOSTSWSCALE = xineplug_post_swscale.so
XINEPOSTAUDIOCHANNEL = xineplug_post_audiochannel.so
XINEPOSTHEADPHONE = xineplug_post_headphone.so

###
### which programs and libs to build
###

VDRSXFE_EXEC =
VDRPLUGIN_SXFE_SO =
VDRFBFE_EXEC =
VDRPLUGIN_FBFE_SO =
XINEPOSTHEADPHONE_SO =
XINEINPUTVDR_SO =
XINEPLUGINDIR = ./
XINEPOSTAUTOCROP_SO =
XINEPOSTSWSCALE_SO =
XINEPOSTAUDIOCHANNEL_SO =
XINEPOSTHEADPHONE_SO =
VDRPLUGIN_SO =

ifeq ($(XINELIBOUTPUT_X11), 1)
    VDRSXFE_EXEC = $(VDRSXFE)
    ifeq ($(XINELIBOUTPUT_VDRPLUGIN), 1)
        VDRPLUGIN_SXFE_SO = lib$(PLUGIN)-sxfe.so
    endif
endif
ifeq ($(XINELIBOUTPUT_FB), 1)
    VDRFBFE_EXEC = $(VDRFBFE)
    ifeq ($(XINELIBOUTPUT_VDRPLUGIN), 1)
        VDRPLUGIN_FBFE_SO = lib$(PLUGIN)-fbfe.so
    endif
endif
ifeq ($(XINELIBOUTPUT_XINEPLUGIN), 1)
    XINEINPUTVDR_SO = $(XINEINPUTVDR)
    XINEPLUGINDIR   = $(shell (pkg-config libxine --atleast-version=1.1.90 && pkg-config libxine --variable=plugindir) || xine-config --plugindir)
    XINEPOSTAUTOCROP_SO = $(XINEPOSTAUTOCROP)
    XINEPOSTSWSCALE_SO = $(XINEPOSTSWSCALE)
    XINEPOSTAUDIOCHANNEL_SO = $(XINEPOSTAUDIOCHANNEL)
    ifeq ($(ENABLE_TEST_POSTPLUGINS), 1)
        XINEPOSTHEADPHONE_SO = $(XINEPOSTHEADPHONE)
    endif
endif
ifeq ($(XINELIBOUTPUT_VDRPLUGIN), 1)
    VDRPLUGIN_SO = libvdr-$(PLUGIN).so
endif


###
### Includes and Defines (add further entries here):
###

INCLUDES  += -I$(VDRINCDIR)
LIBS_XINE += $(shell (pkg-config libxine --atleast-version=1.1.90 && pkg-config libxine --libs) || xine-config --libs)
LIBS_X11  += -L/usr/X11R6/lib -lX11 -lXv -lXext
ifeq ($(HAVE_XRENDER), 1)
# need -lm for ceil/floor in HUD OSD
    LIBS_X11  += -lXrender -lm
endif

ifeq ($(APPLE_DARWIN), 1)
    INCLUDES  += -I/sw/include
    LIBDIRS   += -L/sw/lib
    LIBS      += $(LIBDIRS) -ljpeg -liconv
else
    LIBS      += -lrt
endif

DEFINES   += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"' \
             -D_REENTRANT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	     -DXINELIBOUTPUT_VERSION='"$(VERSION)"'
ifeq ($(HAVE_XRENDER), 1)
    DEFINES += -DHAVE_XRENDER=1
endif
ifeq ($(HAVE_XDPMS), 1)
    DEFINES += -DHAVE_XDPMS=1
endif
ifeq ($(HAVE_XINERAMA), 1)
    DEFINES += -DHAVE_XINERAMA=1
endif
ifeq ($(HAVE_EXTRACTOR_H), 1)
    DEFINES  += -DHAVE_EXTRACTOR_H=1
    INCLUDES += $(shell pkg-config libextractor --cflags-only-I)
    LIBS_VDR += $(shell pkg-config libextractor --libs-only-L)
    LIBS_VDR += $(shell pkg-config libextractor --libs-only-l)
endif

# check for yaegp patch
#YAEPG = $(shell grep -q 'vidWin' \$(VDRINCDIR)/vdr/osd.h && echo "1")
#ifeq ($(YAEPG), 1)
#  DEFINES += -DYAEGP_PATCH
#endif
#DEFINES += $(shell grep -q 'vidWin' \$(VDRINCDIR)/vdr/osd.h && echo "-DYAEGP_PATCH")

ifeq ($(XINELIBOUTPUT_XINEPLUGIN), 1)
    CFLAGS += $(shell (pkg-config libxine --atleast-version=1.1.90 && pkg-config libxine --cflags) || xine-config --cflags) 
endif

ifeq ($(ENABLE_TEST_POSTPLUGINS), 1)
    DEFINES += -DENABLE_TEST_POSTPLUGINS
endif

ifdef USE_ICONV
  DEFINES += -DUSE_ICONV=$(USE_ICONV)
endif
ifdef NOSIGNAL_IMAGE_FILE
  DEFINES += -DNOSIGNAL_IMAGE_FILE='"$(NOSIGNAL_IMAGE_FILE)"'
endif
ifdef STARTUP_IMAGE_FILE
  DEFINES += -DSTARTUP_IMAGE_FILE='"$(STARTUP_IMAGE_FILE)"'
endif


###
### configuration
###

#DEFINES += -DHAVE_XV_FIELD_ORDER


###
### The object files (add further files here):
###

ifeq ($(XINELIBOUTPUT_VDRPLUGIN), 1)
  OBJS = $(PLUGIN).o device.o frontend.o osd.o config.o menu.o setup_menu.o \
         i18n.o menuitems.o media_player.o equalizer.o \
         frontend_local.o frontend_svr.o \
         tools/cxsocket.o tools/udp_pes_scheduler.o \
         tools/backgroundwriter.o tools/playlist.o tools/http.o \
         tools/vdrdiscovery.o tools/time_pts.o tools.o \
         tools/metainfo_menu.o
  OBJS_MPG  = black_720x576.o nosignal_720x576.o vdrlogo_720x576.o
else
  OBJS = 
  OBJS_MPG = 
endif

ifeq ($(XINELIBOUTPUT_X11), 1)
  OBJS_SXFE_SO = xine_sxfe_frontend.o xine/post.o
  OBJS_SXFE = xine_sxfe_frontend_standalone.o xine/post.o tools/vdrdiscovery_standalone.o
else
  OBJS_SXFE_SO = 
  OBJS_SXFE = 
endif

ifeq ($(XINELIBOUTPUT_FB), 1)
  OBJS_FBFE_SO = xine_fbfe_frontend.o xine/post.o
  OBJS_FBFE = xine_fbfe_frontend_standalone.o xine/post.o tools/vdrdiscovery_standalone.o
else
  OBJS_FBFE_SO = 
  OBJS_FBFE = 
endif


###
### Implicit rules:
###

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<


###
### Dependencies:
###

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) $(OBJS_SXFE_SO:%.o=%.c) $(OBJS_FBFE_SO:%.o=%.c) > $@

-include $(DEPFILE)

DEFINES += -Wall


###
### Rules:
###

mpg2c: mpg2c.c
	$(CC) mpg2c.c -o $@

black_720x576.c: mpg2c black_720x576.mpg
	@./mpg2c black black_720x576.mpg black_720x576.c
nosignal_720x576.c: mpg2c nosignal_720x576.mpg
	@./mpg2c nosignal nosignal_720x576.mpg nosignal_720x576.c
vdrlogo_720x576.c: mpg2c vdrlogo_720x576.mpg
	@./mpg2c vdrlogo vdrlogo_720x576.mpg vdrlogo_720x576.c

xine_input_vdr.o: xine_input_vdr.c xine_input_vdr.h xine_osd_command.h nosignal_720x576.c logdefs.h
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) xine_input_vdr.c
xine_input_http.o: xine_input_http.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) xine_input_http.c
xine/post.o: xine/post.c xine/post.h
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) xine/post.c -o $@
tools/vdrdiscovery.o: tools/vdrdiscovery.c tools/vdrdiscovery.h
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) tools/vdrdiscovery.c -o $@
tools/vdrdiscovery_standalone.o: tools/vdrdiscovery.c tools/vdrdiscovery.h
	$(CC) $(CFLAGS) -c $(DEFINES) -DFE_STANDALONE $(INCLUDES) $(OPTFLAGS) tools/vdrdiscovery.c -o $@
xine_post_autocrop.o: xine_post_autocrop.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) xine_post_autocrop.c
xine_post_swscale.o: xine_post_swscale.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) xine_post_swscale.c
xine_post_audiochannel.o: xine_post_audiochannel.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) xine_post_audiochannel.c
xine_post_headphone.o: xine_post_headphone.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) xine_post_headphone.c

xine_sxfe_frontend.o: xine_sxfe_frontend.c xine_frontend.c xine_frontend.h \
		xine_input_vdr.h xine_osd_command.h xine/post.h logdefs.h \
		xineliboutput.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) xine_sxfe_frontend.c
xine_fbfe_frontend.o: xine_fbfe_frontend.c xine_frontend.c xine_frontend.h \
		xine_input_vdr.h xine_osd_command.h xine/post.h logdefs.h \
		xineliboutput.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $(OPTFLAGS) xine_fbfe_frontend.c
xine_sxfe_frontend_standalone.o: xine_sxfe_frontend.c xine_frontend.c \
		xine_frontend.h xine_input_vdr.h xine_osd_command.h \
		xine/post.h logdefs.h xine_frontend_main.c xine_frontend_lirc.c \
		xineliboutput.c tools/vdrdiscovery.h
	$(CC) $(CFLAGS) -c $(DEFINES) -DFE_STANDALONE $(INCLUDES) $(OPTFLAGS) xine_sxfe_frontend.c -o $@
xine_fbfe_frontend_standalone.o: xine_fbfe_frontend.c xine_frontend.c \
		xine_frontend.h xine_input_vdr.h xine_osd_command.h \
		xine/post.h logdefs.h xine_frontend_main.c xine_frontend_lirc.c \
		xineliboutput.c tools/vdrdiscovery.h
	$(CC) $(CFLAGS) -c $(DEFINES) -DFE_STANDALONE $(INCLUDES) $(OPTFLAGS) xine_fbfe_frontend.c -o $@

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
	    $(warning *****************************************************************) \

install : XINELIBOUTPUT_INSTALL_MSG =

all: $(VDRPLUGIN_SO) $(VDRPLUGIN_SXFE_SO) $(VDRPLUGIN_FBFE_SO) \
	    $(VDRSXFE_EXEC) $(VDRFBFE_EXEC) $(XINEINPUTVDR_SO) \
	    $(XINEPOSTAUTOCROP_SO) $(XINEPOSTSWSCALE_SO) $(XINEPOSTHEADPHONE_SO) \
	    $(XINEPOSTAUDIOCHANNEL_SO) i18n
	$(XINELIBOUTPUT_INSTALL_MSG)

frontends: $(VDRSXFE_EXEC) $(VDRFBFE_EXEC) $(XINEINPUTVDR_SO) \
	    $(XINEPOSTAUTOCROP_SO) $(XINEPOSTSWSCALE_SO) $(XINEPOSTHEADPHONE_SO) \
	    $(XINEPOSTAUDIOCHANNEL_SO)

.PHONY: all


ifeq ($(XINELIBOUTPUT_VDRPLUGIN), 1)
$(VDRPLUGIN_SO): $(OBJS) $(OBJS_MPG)
	$(CXX) $(CXXFLAGS) $(LDFLAGS_SO) $(OBJS) $(OBJS_MPG) $(LIBS) $(LIBS_VDR) -o $@
	@-rm -rf $(LIBDIR)/$@.$(APIVERSION)
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)
endif

ifeq ($(XINELIBOUTPUT_X11), 1)
$(VDRPLUGIN_SXFE_SO): $(OBJS_SXFE_SO)
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(OBJS_SXFE_SO) $(LIBS_X11) $(LIBS_XINE) -o $@
	@-rm -rf $(LIBDIR)/$(VDRPLUGIN_SXFE_SO).$(VERSION)
	@cp $@ $(LIBDIR)/$(VDRPLUGIN_SXFE_SO).$(VERSION)
$(VDRSXFE): $(OBJS_SXFE)
	$(CC) -g $(OBJS_SXFE) $(LIBS_X11) -ljpeg $(LIBS_XINE) -o $@
endif

ifeq ($(XINELIBOUTPUT_FB), 1)
$(VDRPLUGIN_FBFE_SO): $(OBJS_FBFE_SO)
	$(CC) $(CFLAGS) $(LDFLAGS_SO) $(OBJS_FBFE_SO) $(LIBS_XINE) -o $@
	@-rm -rf $(LIBDIR)/$(VDRPLUGIN_FBFE_SO).$(VERSION)
	@cp $@ $(LIBDIR)/$(VDRPLUGIN_FBFE_SO).$(VERSION)
$(VDRFBFE): $(OBJS_FBFE)
	$(CC) -g $(OBJS_FBFE) $(LIBS_XINE) -ljpeg -o $@
endif

ifeq ($(XINELIBOUTPUT_XINEPLUGIN), 1)
$(XINEINPUTVDR_SO): xine_input_vdr.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) xine_input_vdr.o $(LIBS_XINE) -o $@
$(XINEPOSTAUTOCROP_SO): xine_post_autocrop.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) xine_post_autocrop.o $(LIBS_XINE) -o $@
$(XINEPOSTSWSCALE_SO): xine_post_swscale.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) xine_post_swscale.o $(LIBS_XINE) -o $@
$(XINEPOSTAUDIOCHANNEL_SO): xine_post_audiochannel.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) xine_post_audiochannel.o $(LIBS_XINE) -o $@
endif
ifeq ($(ENABLE_TEST_POSTPLUGINS), 1)
$(XINEPOSTHEADPHONE_SO): xine_post_headphone.o
	$(CC) $(CFLAGS) $(LDFLAGS_SO) xine_post_headphone.o $(LIBS_XINE) -o $@
endif

install: all
ifeq ($(XINELIBOUTPUT_XINEPLUGIN), 1)
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
ifeq ($(ENABLE_TEST_POSTPLUGINS), 1)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTHEADPHONE)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTHEADPHONE)
	@$(INSTALL) -m 0644 $(XINEPOSTHEADPHONE) $(DESTDIR)/$(XINEPLUGINDIR)/post/$(XINEPOSTHEADPHONE)
endif
ifeq ($(XINELIBOUTPUT_FB), 1)
	@echo Installing $(DESTDIR)/$(BINDIR)/vdr-fbfe
	@mkdir -p $(DESTDIR)/$(BINDIR)
	@-rm -rf $(DESTDIR)/$(BINDIR)/vdr-fbfe
	@$(INSTALL) -m 0755 vdr-fbfe $(DESTDIR)/$(BINDIR)/vdr-fbfe
endif
ifeq ($(XINELIBOUTPUT_X11), 1)
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
		$(PODIR)/*.mo $(PODIR)/*.pot

