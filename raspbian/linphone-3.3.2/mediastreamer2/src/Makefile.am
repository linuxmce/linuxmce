
EXTRA_DIST= winsnd2.c winsnd.c winvideo.c  \
	winvideods.c wincevideods.c dxfilter.h dxfilter.cpp \
	msfileplayer_win.c msfilerec_win.c winsndds.cpp nowebcamCIF.jpg winsnd3.c vfw-missing.h \
	winvideo2.c

BUILT_SOURCES=alldescs.h

CLEANFILES=alldescs.h filterdescs.txt

INCLUDES=-I$(top_srcdir)/include/

lib_LTLIBRARIES=libmediastreamer.la

libmediastreamer_la_SOURCES=	mscommon.c     \
				msfilter.c     \
				msqueue.c      \
				msticker.c     \
				eventqueue.c \
				alaw.c 	       \
				ulaw.c         \
				mssndcard.c    \
				msrtp.c	       \
				dtmfgen.c      \
				ice.c          \
				tee.c          \
				msconf.c       \
				msjoin.c       \
				g711common.h \
				msvolume.c \
				mswebcam.c \
				mtu.c \
				void.c \
				dsptools.c \
				kiss_fft.c  \
				_kiss_fft_guts.h \
				kiss_fft.h \
				kiss_fftr.c \
				kiss_fftr.h \
				equalizer.c \
				chanadapt.c \
				audiomixer.c \
				itc.c

#dummy c++ file to force libtool to use c++ linking (because of msdscap-mingw.cc)
nodist_EXTRA_libmediastreamer_la_SOURCES = dummy.cxx

libmediastreamer_la_SOURCES+=audiostream.c


if BUILD_SPEEX
libmediastreamer_la_SOURCES+=msspeex.c speexec.c
endif

if BUILD_GSM
libmediastreamer_la_SOURCES+=gsm.c
endif

if BUILD_WIN32
libmediastreamer_la_SOURCES+=	winsnd3.c \
				msfileplayer_win.c msfilerec_win.c
else
libmediastreamer_la_SOURCES+=	msfileplayer.c \
				msfilerec.c    
endif

if BUILD_RESAMPLE
libmediastreamer_la_SOURCES+=msresample.c
endif

if BUILD_ALSA
libmediastreamer_la_SOURCES+=alsa.c
endif

if BUILD_OSS
libmediastreamer_la_SOURCES+=oss.c
endif

if BUILD_ARTS
libmediastreamer_la_SOURCES+=arts.c
endif

if BUILD_PORTAUDIO
libmediastreamer_la_SOURCES+=pasnd.c
endif

if BUILD_MACSND
libmediastreamer_la_SOURCES+=macsnd.c
endif

if BUILD_MACAQSND
libmediastreamer_la_SOURCES+=aqsnd.c
endif

if BUILD_PULSEAUDIO
libmediastreamer_la_SOURCES+=pulseaudio.c
endif


if BUILD_VIDEO

if BUILD_MACOSX
libmediastreamer_la_SOURCES+=msv4m.m
endif

if BUILD_V4L
libmediastreamer_la_SOURCES+=msv4l.c msv4l2.c
endif

if BUILD_WIN32
libmediastreamer_la_SOURCES+=msdscap-mingw.cc drawdib-display.c
endif

if BUILD_THEORA
libmediastreamer_la_SOURCES+=theora.c
endif

if BUILD_FFMPEG
libmediastreamer_la_SOURCES+=	videoenc.c \
				videodec.c \
				pixconv.c  \
				sizeconv.c \
				nowebcam.c nowebcam.h \
				videoout.c \
				swscale.h ffmpeg-priv.h \
				h264dec.c
endif

libmediastreamer_la_SOURCES+= rfc2429.h \
				msvideo.c \
				rfc3984.c \
				mire.c \
				extdisplay.c

libmediastreamer_la_SOURCES+=videostream.c

endif




alldescs.h:	Makefile.am $(libmediastreamer_la_SOURCES)
	builddir=`pwd` && cd $(srcdir) && \
	awk 'BEGIN { FS="[()]" ; }; /^\t*MS_FILTER_DESC_EXPORT/{ printf("%s\n", $$2) } '  > $$builddir/filterdescs.txt $(libmediastreamer_la_SOURCES) && \
	awk 'BEGIN { print("#include \"mediastreamer2/msfilter.h\"\n") } { printf("extern MSFilterDesc %s;\n",$$1) } ' $$builddir/filterdescs.txt > $$builddir/$@ && \
	awk 'BEGIN { print("MSFilterDesc * ms_filter_descs[]={") } { printf("&%s,\n",$$1) } END{ print("NULL\n};\n") } ' $$builddir/filterdescs.txt >> $$builddir/$@


libmediastreamer_la_LIBADD=	$(ORTP_LIBS) \
				$(PORTAUDIO_LIBS) \
				$(ALSA_LIBS) \
				$(ARTS_LIBS) \
				$(LIBPULSE_LIBS) \
				$(SPEEX_LIBS) \
				$(GSM_LIBS) \
				$(LIBV4L1_LIBS) \
				$(LIBV4L2_LIBS)
if !BUILD_WIN32
libmediastreamer_la_LDFLAGS=-rdynamic
else
libmediastreamer_la_LDFLAGS=-no-undefined
endif

if BUILD_VIDEO
libmediastreamer_la_LIBADD+=	$(VIDEO_LIBS) \
				$(THEORA_LIBS)


if BUILD_WIN32
libmediastreamer_la_LIBADD+=	-lole32 \
				-loleaut32\
				-lwinmm \
				-luuid

endif

endif
if BUILD_WIN32_WCE
libmediastreamer_la_LIBADD+=	-lmmtimer
endif

AM_CFLAGS=	-I$(top_srcdir)  \
                $(ORTP_CFLAGS)   \
		$(SPEEX_CFLAGS)  \
		$(GSM_CFLAGS)    \
		$(STRICT_OPTIONS) \
		$(LIBPULSE_CFLAGS)

AM_CXXFLAGS= 	-I$(top_srcdir)  \
                $(ORTP_CFLAGS)   \
		$(STRICT_OPTIONS)

if BUILD_VIDEO
AM_CFLAGS+=$(VIDEO_CFLAGS)
endif

AM_OBJCFLAGS=$(AM_CFLAGS)

imgdir=$(datadir)/images/

img_DATA=nowebcamCIF.jpg

