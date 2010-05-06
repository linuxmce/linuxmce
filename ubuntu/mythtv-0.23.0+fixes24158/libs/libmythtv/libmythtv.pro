include ( ../../settings.pro )
  
QT += network xml sql

TEMPLATE = lib
TARGET = mythtv-$$LIBVERSION
CONFIG += thread dll
target.path = $${LIBDIR}
INSTALLS = target

POSTINC =

contains(INCLUDEPATH, /usr/include) {
  POSTINC += /usr/include
  INCLUDEPATH -= /usr/include
}
contains(INCLUDEPATH, /usr/local/include) {
  POSTINC += /usr/local/include
  INCLUDEPATH -= /usr/local/include
}

DEPENDPATH  += .
DEPENDPATH  += ../libmyth ../libavcodec ../libavformat ../libavutil ../libswscale
DEPENDPATH  += ../libmythmpeg2 ../libmythdb ../libmythhdhomerun
DEPENDPATH  += ../libmythdvdnav/
DEPENDPATH  += ./dvbdev ./mpeg ./iptv ./channelscan
DEPENDPATH  += ../libmythlivemedia/BasicUsageEnvironment/include
DEPENDPATH  += ../libmythlivemedia/BasicUsageEnvironment
DEPENDPATH  += ../libmythlivemedia/groupsock/include
DEPENDPATH  += ../libmythlivemedia/groupsock
DEPENDPATH  += ../libmythlivemedia/liveMedia/include
DEPENDPATH  += ../libmythlivemedia/liveMedia
DEPENDPATH  += ../libmythlivemedia/UsageEnvironment/include
DEPENDPATH  += ../libmythlivemedia/UsageEnvironment
DEPENDPATH  += ../libmythdb ../libmythui

INCLUDEPATH += .. ../.. # for avlib headers
INCLUDEPATH += $$DEPENDPATH
INCLUDEPATH += $$POSTINC

LIBS += -L../libmyth -L../libavutil -L../libavcodec -L../libavformat
LIBS += -L../libswscale
LIBS += -L../libmythui -L../libmythupnp
LIBS += -L../libmythmpeg2 -L../libmythdvdnav
LIBS += -L../libmythdb
LIBS += -lmyth-$$LIBVERSION         -lmythavutil-$$LIBVERSION
LIBS += -lmythavcodec-$$LIBVERSION  -lmythavformat-$$LIBVERSION
LIBS += -lmythswscale-$$LIBVERSION
LIBS += -lmythui-$$LIBVERSION       -lmythupnp-$$LIBVERSION
LIBS += -lmythmpeg2-$$LIBVERSION    -lmythdvdnav-$$LIBVERSION
LIBS += -lmythdb-$$LIBVERSION
using_mheg: LIBS += -L../libmythfreemheg -lmythfreemheg-$$LIBVERSION
using_live: LIBS += -L../libmythlivemedia -lmythlivemedia-$$LIBVERSION
using_hdhomerun: LIBS += -L../libmythhdhomerun -lmythhdhomerun-$$LIBVERSION
using_backend: LIBS += -lmp3lame
LIBS += -lz $$EXTRA_LIBS $$QMAKE_LIBS_DYNLOAD

TARGETDEPS += ../libmyth/libmyth-$${MYTH_SHLIB_EXT}
TARGETDEPS += ../libavutil/libmythavutil-$${MYTH_SHLIB_EXT}
TARGETDEPS += ../libavcodec/libmythavcodec-$${MYTH_SHLIB_EXT}
TARGETDEPS += ../libavformat/libmythavformat-$${MYTH_SHLIB_EXT}
TARGETDEPS += ../libmythmpeg2/libmythmpeg2-$${MYTH_LIB_EXT}
TARGETDEPS += ../libmythdvdnav/libmythdvdnav-$${MYTH_LIB_EXT}
using_mheg: TARGETDEPS += ../libmythfreemheg/libmythfreemheg-$${MYTH_SHLIB_EXT}
using_live: TARGETDEPS += ../libmythlivemedia/libmythlivemedia-$${MYTH_SHLIB_EXT}
using_hdhomerun: TARGETDEPS += ../libmythhdhomerun/libmythhdhomerun-$${MYTH_SHLIB_EXT}
TARGETDEPS += ../libswscale/libmythswscale-$${MYTH_SHLIB_EXT}

DEFINES += _LARGEFILE_SOURCE
QMAKE_CXXFLAGS += $${FREETYPE_CFLAGS}
QMAKE_LFLAGS_SHLIB += $${FREETYPE_LIBS}

macx {
    # Mac OS X Frameworks
    FWKS = AGL ApplicationServices Carbon Cocoa OpenGL QuickTime IOKit
    PFWKS = DVD

    using_firewire:using_backend: FWKS += IOKit

    # The following trick shortens the command line, but depends on
    # the shell expanding Csh-style braces. Luckily, Bash and Zsh do.
    FC = $$join(FWKS,",","{","}")
    PFC = $$join(PFWKS,",","{","}")

    QMAKE_CXXFLAGS += -F/System/Library/Frameworks/$${FC}.framework/Frameworks
    QMAKE_CXXFLAGS += -F/System/Library/PrivateFrameworks/$${PFC}.framework/Frameworks
    LIBS           += -framework $$join(FWKS," -framework ")
    LIBS           += -F/System/Library/PrivateFrameworks
    LIBS           += -framework $$join(PFWKS," -framework ")

    using_firewire:using_backend {
        QMAKE_CXXFLAGS += -F$${CONFIG_MAC_AVC}
        LIBS += -F$${CONFIG_MAC_AVC} -framework AVCVideoServices
        # Recent versions of this framework use /usr/lib/libstdc++.6.dylib
        # which may clash with symbols in /usr/lib/gcc/darwin/3.3/libstdc++.a
        # In that case, rebuild the framework with your (old) Xcode version
    }

    using_dvdv {
        # DVDUtils uses Objective-C++, activated by .mm suffix
        QMAKE_EXT_CPP += .mm
    }
}

cygwin:QMAKE_LFLAGS_SHLIB += -Wl,--noinhibit-exec
cygwin:DEFINES += _WIN32

# Enable Valgrind, i.e. disable some timeouts
using_valgrind:DEFINES += USING_VALGRIND

# old libvbitext (Caption decoder)
!win32 {
    HEADERS += vbitext/cc.h vbitext/dllist.h vbitext/hamm.h vbitext/lang.h
    HEADERS += vbitext/vbi.h vbitext/vt.h
    SOURCES += vbitext/cc.cpp vbitext/vbi.c vbitext/hamm.c vbitext/lang.c
}

# mmx macros from avlib
contains( HAVE_MMX, yes ) {
    HEADERS += ../../libs/libavcodec/x86/mmx.h ../../libs/libavcodec/dsputil.h
}

QMAKE_CLEAN += $(TARGET) $(TARGETA) $(TARGETD) $(TARGET0) $(TARGET1) $(TARGET2)

##########################################################################
# libmythtv proper

# Headers needed by frontend & backend
HEADERS += filter.h                 format.h
HEADERS += frame.h                  compat.h

# LZO / RTjpegN, used by NuppelDecoder & NuppelVideoRecorder
HEADERS += lzoconf.h
HEADERS += minilzo.h                RTjpegN.h
SOURCES += minilzo.cpp              RTjpegN.cpp

# Misc. needed by backend/frontend
HEADERS += recordinginfo.h          recordinglist.h
HEADERS += RingBuffer.h             avfringbuffer.h
HEADERS += ThreadedFileWriter.h
HEADERS += dbcheck.h
HEADERS += tvremoteutil.h           tv.h
HEADERS += jobqueue.h
HEADERS += filtermanager.h          recordingprofile.h
HEADERS += remoteencoder.h          videosource.h
HEADERS += cardutil.h               sourceutil.h
HEADERS += cc608decoder.h
HEADERS += cc708decoder.h           cc708window.h
HEADERS += scheduledrecording.h
HEADERS += signalmonitorvalue.h     signalmonitorlistener.h
HEADERS += livetvchain.h            playgroup.h
HEADERS += channelsettings.h        previewgenerator.h
HEADERS += transporteditor.h        listingsources.h
HEADERS += myth_imgconvert.h
HEADERS += channelgroup.h           channelgroupsettings.h
HEADERS += recordingrule.h          programdetail.h
HEADERS += iso639.h
HEADERS += mythsystemevent.h

SOURCES += recordinginfo.cpp        recordinglist.cpp
SOURCES += RingBuffer.cpp           avfringbuffer.cpp
SOURCES += ThreadedFileWriter.cpp
SOURCES += dbcheck.cpp
SOURCES += tvremoteutil.cpp         tv.cpp
SOURCES += jobqueue.cpp
SOURCES += filtermanager.cpp        recordingprofile.cpp
SOURCES += remoteencoder.cpp        videosource.cpp
SOURCES += cardutil.cpp             sourceutil.cpp
SOURCES += cc608decoder.cpp
SOURCES += cc708decoder.cpp         cc708window.cpp
SOURCES += scheduledrecording.cpp
SOURCES += signalmonitorvalue.cpp
SOURCES += livetvchain.cpp          playgroup.cpp
SOURCES += channelsettings.cpp      previewgenerator.cpp
SOURCES += transporteditor.cpp
SOURCES += channelgroup.cpp         channelgroupsettings.cpp
SOURCES += myth_imgconvert.cpp
SOURCES += recordingrule.cpp        programdetail.cpp
SOURCES += iso639.cpp
SOURCES += mythsystemevent.cpp

# DiSEqC
HEADERS += diseqc.h                 diseqcsettings.h
SOURCES += diseqc.cpp               diseqcsettings.cpp

# Listings downloading classes
HEADERS += datadirect.h
SOURCES += datadirect.cpp

# Teletext stuff
HEADERS += teletextdecoder.h        vbilut.h
SOURCES += teletextdecoder.cpp      vbilut.cpp

# MPEG parsing stuff
HEADERS += mpeg/tspacket.h          mpeg/pespacket.h
HEADERS += mpeg/mpegtables.h        mpeg/atsctables.h
HEADERS += mpeg/dvbtables.h         mpeg/premieretables.h
HEADERS += mpeg/mpegstreamdata.h    mpeg/atscstreamdata.h
HEADERS += mpeg/dvbstreamdata.h     mpeg/scanstreamdata.h
HEADERS += mpeg/mpegdescriptors.h   mpeg/atscdescriptors.h
HEADERS += mpeg/dvbdescriptors.h    mpeg/dishdescriptors.h
HEADERS += mpeg/premieredescriptors.h
HEADERS += mpeg/atsc_huffman.h
HEADERS += mpeg/freesat_huffman.h   mpeg/freesat_tables.h
HEADERS += mpeg/iso6937tables.h
HEADERS += mpeg/tsstats.h           mpeg/streamlisteners.h
HEADERS += mpeg/H264Parser.h

SOURCES += mpeg/tspacket.cpp        mpeg/pespacket.cpp
SOURCES += mpeg/mpegtables.cpp      mpeg/atsctables.cpp
SOURCES += mpeg/dvbtables.cpp       mpeg/premieretables.cpp
SOURCES += mpeg/mpegstreamdata.cpp  mpeg/atscstreamdata.cpp
SOURCES += mpeg/dvbstreamdata.cpp   mpeg/scanstreamdata.cpp
SOURCES += mpeg/mpegdescriptors.cpp mpeg/atscdescriptors.cpp
SOURCES += mpeg/dvbdescriptors.cpp  mpeg/dishdescriptors.cpp
SOURCES += mpeg/premieredescriptors.cpp
SOURCES += mpeg/atsc_huffman.cpp
SOURCES += mpeg/freesat_huffman.cpp
SOURCES += mpeg/iso6937tables.cpp
SOURCES += mpeg/H264Parser.cpp

# Channels, and the multiplexes that transmit them
HEADERS += frequencies.h            frequencytables.h
SOURCES += frequencies.c            frequencytables.cpp

HEADERS += channelutil.h            dbchannelinfo.h
SOURCES += channelutil.cpp          dbchannelinfo.cpp

HEADERS += dtvmultiplex.h
HEADERS += dtvconfparser.h          dtvconfparserhelpers.h
SOURCES += dtvmultiplex.cpp
SOURCES += dtvconfparser.cpp        dtvconfparserhelpers.cpp

HEADERS += channelscan/scaninfo.h   channelscan/channelimporter.h
SOURCES += channelscan/scaninfo.cpp channelscan/channelimporter.cpp

using_frontend {
    # Recording profile stuff
    HEADERS += profilegroup.h
    SOURCES += profilegroup.cpp

    # Video playback
    HEADERS += tv_play.h                NuppelVideoPlayer.h
    HEADERS += DVDRingBuffer.h          playercontext.h
    HEADERS += tv_play_win.h
    SOURCES += tv_play.cpp              NuppelVideoPlayer.cpp
    SOURCES += DVDRingBuffer.cpp        playercontext.cpp
    SOURCES += tv_play_win.cpp

    # Text subtitle parser
    HEADERS += textsubtitleparser.h     xine_demux_sputext.h
    SOURCES += textsubtitleparser.cpp   xine_demux_sputext.cpp

    # A/V decoders
    HEADERS += decoderbase.h
    HEADERS += nuppeldecoder.h          avformatdecoder.h
    SOURCES += decoderbase.cpp
    SOURCES += nuppeldecoder.cpp        avformatdecoder.cpp

    # On screen display (video output overlay)
    using_fribidi:DEFINES += USING_FRIBIDI
    HEADERS += osd.h                    osdtypes.h
    HEADERS += osdsurface.h             osdlistbtntype.h
    HEADERS += osdimagecache.h          osdtypeteletext.h
    HEADERS += udpnotify.h                  tvosdmenuentry.h
    SOURCES += osd.cpp                  osdtypes.cpp
    SOURCES += osdsurface.cpp           osdlistbtntype.cpp
    SOURCES += osdimagecache.cpp        osdtypeteletext.cpp
    SOURCES += udpnotify.cpp              tvosdmenuentry.cpp

    # Video output
    HEADERS += videooutbase.h           videoout_null.h
    HEADERS += videobuffers.h           vsync.h
    HEADERS += jitterometer.h           yuv2rgb.h
    HEADERS += videodisplayprofile.h    mythcodecid.h
    HEADERS += videoouttypes.h
    HEADERS += videooutwindow.h
    SOURCES += videooutbase.cpp         videoout_null.cpp
    SOURCES += videobuffers.cpp         vsync.cpp
    SOURCES += jitterometer.cpp         yuv2rgb.cpp
    SOURCES += videodisplayprofile.cpp  mythcodecid.cpp
    SOURCES += videooutwindow.cpp

    macx:HEADERS +=               videoout_dvdv.h
    macx:HEADERS +=               videoout_quartz.h
    macx:SOURCES +=               videoout_quartz.cpp

    using_dvdv:DEFINES +=         USING_DVDV
    using_dvdv:HEADERS +=         videoout_dvdv_private.h
    using_dvdv:HEADERS +=         util-osx-cocoa.h
    using_dvdv:SOURCES +=         videoout_dvdv.mm
    using_dvdv:SOURCES +=         util-osx-cocoa.mm

    using_directfb:HEADERS +=     videoout_directfb.h
    using_directfb:SOURCES +=     videoout_directfb.cpp
    using_directfb:DEFINES +=     USING_DIRECTFB

    using_x11:DEFINES += USING_X11

    using_xv:HEADERS += videoout_xv.h   XvMCSurfaceTypes.h
    using_xv:HEADERS += osdxvmc.h       osdchromakey.h
    using_xv:HEADERS += util-xvmc.h     util-xv.h
    using_xv:SOURCES += videoout_xv.cpp XvMCSurfaceTypes.cpp
    using_xv:SOURCES += osdxvmc.cpp     osdchromakey.cpp
    using_xv:SOURCES += util-xvmc.cpp   util-xv.cpp

    using_xv:DEFINES += USING_XV

    using_xvmc:DEFINES += USING_XVMC
    using_xvmcw:DEFINES += USING_XVMCW
    using_xvmc_vld:DEFINES += USING_XVMC_VLD

    using_vdpau {
        DEFINES += USING_VDPAU
        HEADERS += videoout_vdpau.h
        SOURCES += videoout_vdpau.cpp
        LIBS += -lvdpau
    }

    using_opengl {
        CONFIG += opengl
        DEFINES += USING_OPENGL
        HEADERS += util-opengl.h        openglcontext.h
        SOURCES += util-opengl.cpp      openglcontext.cpp
    }
    using_opengl_vsync:DEFINES += USING_OPENGL_VSYNC

    using_opengl_video:DEFINES += USING_OPENGL_VIDEO
    using_opengl_video:HEADERS += openglvideo.h   videoout_opengl.h
    using_opengl_video:SOURCES += openglvideo.cpp videoout_opengl.cpp

    using_glx_proc_addr_arb:DEFINES += USING_GLX_PROC_ADDR_ARB

    # Misc. frontend
    HEADERS += ttfont.h
    SOURCES += ttfont.cpp
    HEADERS += DetectLetterbox.h
    SOURCES += DetectLetterbox.cpp

    using_mheg {
        # DSMCC stuff
        HEADERS += dsmcc.h                  dsmcccache.h
        HEADERS += dsmccbiop.h              dsmccobjcarousel.h
        HEADERS += dsmccreceiver.h
        SOURCES += dsmcc.cpp                dsmcccache.cpp
        SOURCES += dsmccbiop.cpp            dsmccobjcarousel.cpp

        # MHEG/MHI stuff
        HEADERS += interactivetv.h          mhi.h
        SOURCES += interactivetv.cpp        mhi.cpp
        DEFINES += USING_MHEG
    }

    # C stuff
    HEADERS += blend.h
    SOURCES += blend.c

    DEFINES += USING_FRONTEND
}

using_backend {
    # Channel stuff
    HEADERS += channelbase.h               dtvchannel.h
    HEADERS += signalmonitor.h             dtvsignalmonitor.h
    HEADERS += inputinfo.h                 inputgroupmap.h
    SOURCES += channelbase.cpp             dtvchannel.cpp
    SOURCES += signalmonitor.cpp           dtvsignalmonitor.cpp
    SOURCES += inputinfo.cpp               inputgroupmap.cpp

    # Channel scanner stuff
    HEADERS += scanwizard.h
    SOURCES += scanwizard.cpp

    HEADERS += channelscan/channelscan_sm.h
    HEADERS += channelscan/channelscanner.h
    HEADERS += channelscan/channelscanner_gui.h
    HEADERS += channelscan/channelscanner_gui_scan_pane.h
    HEADERS += channelscan/channelscanner_cli.h
    HEADERS += channelscan/frequencytablesetting.h
    HEADERS += channelscan/inputselectorsetting.h
    HEADERS += channelscan/loglist.h
    HEADERS += channelscan/channelscanmiscsettings.h
    HEADERS += channelscan/modulationsetting.h
    HEADERS += channelscan/multiplexsetting.h
    HEADERS += channelscan/paneanalog.h
    HEADERS += channelscan/paneatsc.h
    HEADERS += channelscan/panedvbc.h
    HEADERS += channelscan/panedvbs.h
    HEADERS += channelscan/panedvbs2.h
    HEADERS += channelscan/panedvbt.h
    HEADERS += channelscan/panedvbutilsimport.h
    HEADERS += channelscan/panesingle.h
    HEADERS += channelscan/scanmonitor.h
    HEADERS += channelscan/scanwizardconfig.h

    SOURCES += channelscan/channelscan_sm.cpp
    SOURCES += channelscan/channelscanner.cpp
    SOURCES += channelscan/channelscanner_gui.cpp
    SOURCES += channelscan/channelscanner_gui_scan_pane.cpp
    SOURCES += channelscan/channelscanner_cli.cpp
    SOURCES += channelscan/frequencytablesetting.cpp
    SOURCES += channelscan/inputselectorsetting.cpp
    SOURCES += channelscan/loglist.cpp
    SOURCES += channelscan/multiplexsetting.cpp
    SOURCES += channelscan/paneanalog.cpp
    SOURCES += channelscan/scanmonitor.cpp
    SOURCES += channelscan/scanwizardconfig.cpp

    # EIT stuff
    HEADERS += eithelper.h                 eitscanner.h
    HEADERS += eitfixup.h                  eitcache.h
    SOURCES += eithelper.cpp               eitscanner.cpp
    SOURCES += eitfixup.cpp                eitcache.cpp

    # non-EIT EPG stuff
    HEADERS += programdata.h
    SOURCES += programdata.cpp

    # TVRec & Recorder base classes
    HEADERS += tv_rec.h
    HEADERS += recorderbase.h              DeviceReadBuffer.h
    HEADERS += dtvrecorder.h
    SOURCES += tv_rec.cpp
    SOURCES += recorderbase.cpp            DeviceReadBuffer.cpp
    SOURCES += dtvrecorder.cpp

    # Import recorder
    HEADERS += importrecorder.h
    SOURCES += importrecorder.cpp

    # Simple NuppelVideo Recorder
    using_ffmpeg_threads:DEFINES += USING_FFMPEG_THREADS
    HEADERS += NuppelVideoRecorder.h       fifowriter.h        audioinput.h
    SOURCES += NuppelVideoRecorder.cpp     fifowriter.cpp      audioinput.cpp
    using_alsa {
        HEADERS += audioinputalsa.h
        SOURCES += audioinputalsa.cpp
        DEFINES += USING_ALSA
        LIBS += $$ALSA_LIBS
    }
    using_oss {
        HEADERS += audioinputoss.h
        SOURCES += audioinputoss.cpp
        DEFINES += USING_OSS
        LIBS += $$OSS_LIBS
    }

    # Support for Video4Linux devices
    using_v4l {
        HEADERS += v4lchannel.h                analogsignalmonitor.h
        SOURCES += v4lchannel.cpp              analogsignalmonitor.cpp

        DEFINES += USING_V4L
    }

    # Support for cable boxes that provide Firewire out
    using_firewire {
        HEADERS += firewirechannel.h           firewirerecorder.h
        HEADERS += firewiresignalmonitor.h     firewiredevice.h
        HEADERS += avcinfo.h
        SOURCES += firewirechannel.cpp         firewirerecorder.cpp
        SOURCES += firewiresignalmonitor.cpp   firewiredevice.cpp
        SOURCES += avcinfo.cpp

        macx {
            HEADERS += darwinfirewiredevice.h   darwinavcinfo.h
            SOURCES += darwinfirewiredevice.cpp darwinavcinfo.cpp
            DEFINES += USING_OSX_FIREWIRE
        }

        !macx {
            HEADERS += linuxfirewiredevice.h   linuxavcinfo.h
            SOURCES += linuxfirewiredevice.cpp linuxavcinfo.cpp
            DEFINES += USING_LINUX_FIREWIRE
        }

        DEFINES += USING_FIREWIRE
        using_libavc_5_3:DEFINES += USING_LIBAVC_5_3
    }

    # Support for MPEG2 TS streams (including FreeBox http://adsl.free.fr/)
    using_iptv {
        HEADERS += iptvchannel.h              iptvrecorder.h
        HEADERS += iptvsignalmonitor.h
        HEADERS += iptv/iptvchannelfetcher.h  iptv/iptvchannelinfo.h
        HEADERS += iptv/iptvmediasink.h
        HEADERS += iptv/iptvfeeder.h          iptv/iptvfeederwrapper.h
        HEADERS += iptv/iptvfeederrtsp.h      iptv/iptvfeederudp.h
        HEADERS += iptv/iptvfeederfile.h      iptv/iptvfeederlive.h
        HEADERS += iptv/iptvfeederrtp.h       iptv/timeoutedtaskscheduler.h

        SOURCES += iptvchannel.cpp            iptvrecorder.cpp
        SOURCES += iptvsignalmonitor.cpp
        SOURCES += iptv/iptvchannelfetcher.cpp
        SOURCES += iptv/iptvmediasink.cpp
        SOURCES += iptv/iptvfeeder.cpp        iptv/iptvfeederwrapper.cpp
        SOURCES += iptv/iptvfeederrtsp.cpp    iptv/iptvfeederudp.cpp
        SOURCES += iptv/iptvfeederfile.cpp    iptv/iptvfeederlive.cpp
        SOURCES += iptv/iptvfeederrtp.cpp     iptv/timeoutedtaskscheduler.cpp

        DEFINES += USING_IPTV
    }

    # Support for HDHomeRun box
    using_hdhomerun {
        # MythTV HDHomeRun glue
        HEADERS += hdhrsignalmonitor.h   hdhrchannel.h
        HEADERS += hdhrrecorder.h        hdhrstreamhandler.h

        SOURCES += hdhrsignalmonitor.cpp hdhrchannel.cpp
        SOURCES += hdhrrecorder.cpp      hdhrstreamhandler.cpp

        DEFINES += USING_HDHOMERUN
    }

    # Support for PVR-150/250/350/500, etc. on Linux
    using_ivtv:HEADERS += mpegrecorder.h
    using_ivtv:SOURCES += mpegrecorder.cpp
    using_ivtv:DEFINES += USING_IVTV

    # Support for HD-PVR on Linux
    using_hdpvr:HEADERS *= mpegrecorder.h
    using_hdpvr:SOURCES *= mpegrecorder.cpp
    using_hdpvr:DEFINES += USING_HDPVR

    # Support for Linux DVB drivers
    using_dvb {
        # Basic DVB types
        HEADERS += dvbtypes.h
        SOURCES += dvbtypes.cpp

        # Channel stuff
        HEADERS += dvbchannel.h           dvbsignalmonitor.h
        HEADERS += dvbcam.h
        SOURCES += dvbchannel.cpp         dvbsignalmonitor.cpp
        SOURCES += dvbcam.cpp

        # DVB Recorder
        HEADERS += dvbrecorder.h          dvbstreamhandler.h
        SOURCES += dvbrecorder.cpp        dvbstreamhandler.cpp

        # Misc
        HEADERS += dvbdev/dvbci.h
        SOURCES += dvbdev/dvbci.cpp

        DEFINES += USING_DVB
    }

    DEFINES += USING_BACKEND
}

use_hidesyms {
    QMAKE_CXXFLAGS += -fvisibility=hidden
}

mingw {
    DEFINES -= USING_OPENGL_VSYNC
    DEFINES += USING_MINGW

    HEADERS += videoout_d3d.h
    SOURCES -= NuppelVideoRecorder.cpp
    SOURCES += videoout_d3d.cpp

    LIBS += -lpthread
    LIBS += -lws2_32
}

include ( ../libs-targetfix.pro )

LIBS += $$LATE_LIBS
