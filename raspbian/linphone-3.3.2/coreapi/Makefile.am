
SUBDIRS=help

EXTRA_DIST=linphonecore_jni.cc

## Process this file with automake to produce Makefile.in
linphone_includedir=$(includedir)/linphone

linphone_include_HEADERS=linphonecore.h ../config.h lpconfig.h sipsetup.h

INCLUDES = \
	-I$(top_srcdir)\
	-I$(top_srcdir)/mediastreamer2/include


lib_LTLIBRARIES=liblinphone.la

liblinphone_la_SOURCES=\
	linphonecore.c linphonecore.h private.h\
	offeranswer.c offeranswer.h\
	sal.c sal.h \
	sal_eXosip2.c sal_eXosip2.h\
	sal_eXosip2_sdp.c \
	sal_eXosip2_presence.c \
	callbacks.c \
	misc.c  \
	address.c \
	enum.c enum.h \
	presence.c \
	proxy.c \
	friend.c \
	authentication.c \
	lpconfig.c lpconfig.h \
	chat.c \
	general_state.c \
	sipsetup.c sipsetup.h \
	siplogin.c


liblinphone_la_LDFLAGS= -version-info $(LIBLINPHONE_SO_VERSION) -no-undefined

liblinphone_la_LIBADD= \
		$(EXOSIP_LIBS) \
		$(top_builddir)/mediastreamer2/src/libmediastreamer.la \
		$(ORTP_LIBS)

if BUILD_WIN32
liblinphone_la_LIBADD+=$(top_builddir)/oRTP/src/libortp.la
endif


AM_CFLAGS=$(STRICT_OPTIONS)  -DIN_LINPHONE \
	$(ORTP_CFLAGS) \
	$(OSIP_CFLAGS) \
	$(EXOSIP_CFLAGS) \
	-DENABLE_TRACE 	\
	-DLOG_DOMAIN=\"LinphoneCore\" \
	 $(IPV6_CFLAGS) \
	 -DORTP_INET6 \
	 $(VIDEO_CFLAGS) 
