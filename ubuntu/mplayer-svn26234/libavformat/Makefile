#
# libavformat Makefile
# (c) 2000-2003 Fabrice Bellard
#
include ../config.mak

NAME = avformat
FFLIBS = avcodec avutil

OBJS = allformats.o cutils.o os_support.o sdp.o utils.o

HEADERS = avformat.h avio.h rtsp.h rtspcodes.h

# muxers/demuxers
OBJS-$(CONFIG_AAC_DEMUXER)               += raw.o
OBJS-$(CONFIG_AC3_DEMUXER)               += raw.o
OBJS-$(CONFIG_AC3_MUXER)                 += raw.o
OBJS-$(CONFIG_ADTS_MUXER)                += adtsenc.o
OBJS-$(CONFIG_AIFF_DEMUXER)              += aiff.o riff.o raw.o
OBJS-$(CONFIG_AIFF_MUXER)                += aiff.o riff.o
OBJS-$(CONFIG_AMR_DEMUXER)               += amr.o
OBJS-$(CONFIG_AMR_MUXER)                 += amr.o
OBJS-$(CONFIG_APC_DEMUXER)               += apc.o
OBJS-$(CONFIG_APE_DEMUXER)               += ape.o
OBJS-$(CONFIG_ASF_DEMUXER)               += asf.o asfcrypt.o riff.o
OBJS-$(CONFIG_ASF_MUXER)                 += asf-enc.o riff.o
OBJS-$(CONFIG_ASF_STREAM_MUXER)          += asf-enc.o riff.o
OBJS-$(CONFIG_AU_DEMUXER)                += au.o raw.o
OBJS-$(CONFIG_AU_MUXER)                  += au.o
OBJS-$(CONFIG_AVI_DEMUXER)               += avidec.o riff.o
OBJS-$(CONFIG_AVI_MUXER)                 += avienc.o riff.o
OBJS-$(CONFIG_AVISYNTH)                  += avisynth.o
OBJS-$(CONFIG_AVM2_MUXER)                += swf.o
OBJS-$(CONFIG_AVS_DEMUXER)               += avs.o vocdec.o voc.o
OBJS-$(CONFIG_BETHSOFTVID_DEMUXER)       += bethsoftvid.o
OBJS-$(CONFIG_C93_DEMUXER)               += c93.o vocdec.o voc.o
OBJS-$(CONFIG_CRC_MUXER)                 += crcenc.o
OBJS-$(CONFIG_DAUD_DEMUXER)              += daud.o
OBJS-$(CONFIG_DSICIN_DEMUXER)            += dsicin.o
OBJS-$(CONFIG_DTS_DEMUXER)               += raw.o
OBJS-$(CONFIG_DV_DEMUXER)                += dv.o
OBJS-$(CONFIG_DV_MUXER)                  += dvenc.o
OBJS-$(CONFIG_DXA_DEMUXER)               += dxa.o riff.o
OBJS-$(CONFIG_EA_CDATA_DEMUXER)          += eacdata.o
OBJS-$(CONFIG_EA_DEMUXER)                += electronicarts.o
OBJS-$(CONFIG_FFM_DEMUXER)               += ffm.o
OBJS-$(CONFIG_FFM_MUXER)                 += ffm.o
OBJS-$(CONFIG_FLAC_DEMUXER)              += raw.o
OBJS-$(CONFIG_FLAC_MUXER)                += raw.o
OBJS-$(CONFIG_FLIC_DEMUXER)              += flic.o
OBJS-$(CONFIG_FLV_DEMUXER)               += flvdec.o
OBJS-$(CONFIG_FLV_MUXER)                 += flvenc.o
OBJS-$(CONFIG_FOURXM_DEMUXER)            += 4xm.o
OBJS-$(CONFIG_FRAMECRC_MUXER)            += framecrcenc.o
OBJS-$(CONFIG_GIF_MUXER)                 += gif.o
OBJS-$(CONFIG_GIF_DEMUXER)               += gifdec.o
OBJS-$(CONFIG_GXF_DEMUXER)               += gxf.o
OBJS-$(CONFIG_GXF_MUXER)                 += gxfenc.o
OBJS-$(CONFIG_H261_DEMUXER)              += raw.o
OBJS-$(CONFIG_H261_MUXER)                += raw.o
OBJS-$(CONFIG_H263_DEMUXER)              += raw.o
OBJS-$(CONFIG_H263_MUXER)                += raw.o
OBJS-$(CONFIG_H264_DEMUXER)              += raw.o
OBJS-$(CONFIG_H264_MUXER)                += raw.o
OBJS-$(CONFIG_IDCIN_DEMUXER)             += idcin.o
OBJS-$(CONFIG_IMAGE2_DEMUXER)            += img2.o
OBJS-$(CONFIG_IMAGE2_MUXER)              += img2.o
OBJS-$(CONFIG_IMAGE2PIPE_DEMUXER)        += img2.o
OBJS-$(CONFIG_IMAGE2PIPE_MUXER)          += img2.o
OBJS-$(CONFIG_INGENIENT_DEMUXER)         += raw.o
OBJS-$(CONFIG_IPMOVIE_DEMUXER)           += ipmovie.o
OBJS-$(CONFIG_LMLM4_DEMUXER)             += lmlm4.o
OBJS-$(CONFIG_M4V_DEMUXER)               += raw.o
OBJS-$(CONFIG_M4V_MUXER)                 += raw.o
OBJS-$(CONFIG_MATROSKA_AUDIO_MUXER)      += matroskaenc.o matroska.o riff.o avc.o
OBJS-$(CONFIG_MATROSKA_DEMUXER)          += matroskadec.o matroska.o riff.o
OBJS-$(CONFIG_MATROSKA_MUXER)            += matroskaenc.o matroska.o riff.o avc.o
OBJS-$(CONFIG_MJPEG_DEMUXER)             += raw.o
OBJS-$(CONFIG_MJPEG_MUXER)               += raw.o
OBJS-$(CONFIG_MM_DEMUXER)                += mm.o
OBJS-$(CONFIG_MMF_DEMUXER)               += mmf.o raw.o
OBJS-$(CONFIG_MMF_MUXER)                 += mmf.o riff.o
OBJS-$(CONFIG_MOV_DEMUXER)               += mov.o riff.o isom.o
OBJS-$(CONFIG_MOV_MUXER)                 += movenc.o riff.o isom.o avc.o
OBJS-$(CONFIG_MP2_MUXER)                 += mp3.o
OBJS-$(CONFIG_MP3_DEMUXER)               += mp3.o
OBJS-$(CONFIG_MP3_MUXER)                 += mp3.o
OBJS-$(CONFIG_MP4_MUXER)                 += movenc.o riff.o isom.o avc.o
OBJS-$(CONFIG_MPC_DEMUXER)               += mpc.o
OBJS-$(CONFIG_MPC8_DEMUXER)              += mpc8.o
OBJS-$(CONFIG_MPEG1SYSTEM_MUXER)         += mpegenc.o
OBJS-$(CONFIG_MPEG1VCD_MUXER)            += mpegenc.o
OBJS-$(CONFIG_MPEG2DVD_MUXER)            += mpegenc.o
OBJS-$(CONFIG_MPEG2VOB_MUXER)            += mpegenc.o
OBJS-$(CONFIG_MPEG2SVCD_MUXER)           += mpegenc.o
OBJS-$(CONFIG_MPEG1VIDEO_MUXER)          += raw.o
OBJS-$(CONFIG_MPEG2VIDEO_MUXER)          += raw.o
OBJS-$(CONFIG_MPEGPS_DEMUXER)            += mpeg.o
OBJS-$(CONFIG_MPEGTS_DEMUXER)            += mpegts.o
OBJS-$(CONFIG_MPEGTSRAW_DEMUXER)         += mpegts.o
OBJS-$(CONFIG_MPEGTS_MUXER)              += mpegtsenc.o
OBJS-$(CONFIG_MPEGVIDEO_DEMUXER)         += raw.o
OBJS-$(CONFIG_MPJPEG_MUXER)              += mpjpeg.o
OBJS-$(CONFIG_MTV_DEMUXER)               += mtv.o
OBJS-$(CONFIG_MXF_DEMUXER)               += mxf.o
OBJS-$(CONFIG_NSV_DEMUXER)               += nsvdec.o
OBJS-$(CONFIG_NULL_MUXER)                += raw.o
OBJS-$(CONFIG_NUT_DEMUXER)               += nutdec.o nut.o riff.o
OBJS-$(CONFIG_NUT_MUXER)                 += nutenc.o nut.o riff.o
OBJS-$(CONFIG_NUV_DEMUXER)               += nuv.o riff.o
OBJS-$(CONFIG_OGG_DEMUXER)               += oggdec.o         \
                                            oggparseflac.o   \
                                            oggparseogm.o    \
                                            oggparsespeex.o  \
                                            oggparsetheora.o \
                                            oggparsevorbis.o \
                                            riff.o
OBJS-$(CONFIG_OGG_MUXER)                 += oggenc.o
OBJS-$(CONFIG_PCM_ALAW_DEMUXER)          += raw.o
OBJS-$(CONFIG_PCM_ALAW_MUXER)            += raw.o
OBJS-$(CONFIG_PCM_MULAW_DEMUXER)         += raw.o
OBJS-$(CONFIG_PCM_MULAW_MUXER)           += raw.o
OBJS-$(CONFIG_PCM_S16BE_DEMUXER)         += raw.o
OBJS-$(CONFIG_PCM_S16BE_MUXER)           += raw.o
OBJS-$(CONFIG_PCM_S16LE_DEMUXER)         += raw.o
OBJS-$(CONFIG_PCM_S16LE_MUXER)           += raw.o
OBJS-$(CONFIG_PCM_S8_DEMUXER)            += raw.o
OBJS-$(CONFIG_PCM_S8_MUXER)              += raw.o
OBJS-$(CONFIG_PCM_U16BE_DEMUXER)         += raw.o
OBJS-$(CONFIG_PCM_U16BE_MUXER)           += raw.o
OBJS-$(CONFIG_PCM_U16LE_DEMUXER)         += raw.o
OBJS-$(CONFIG_PCM_U16LE_MUXER)           += raw.o
OBJS-$(CONFIG_PCM_U8_DEMUXER)            += raw.o
OBJS-$(CONFIG_PCM_U8_MUXER)              += raw.o
OBJS-$(CONFIG_PSP_MUXER)                 += movenc.o riff.o isom.o avc.o
OBJS-$(CONFIG_PVA_DEMUXER)               += pva.o
OBJS-$(CONFIG_RAWVIDEO_DEMUXER)          += raw.o
OBJS-$(CONFIG_RAWVIDEO_MUXER)            += raw.o
OBJS-$(CONFIG_REDIR_DEMUXER)             += rtsp.o
OBJS-$(CONFIG_RM_DEMUXER)                += rmdec.o
OBJS-$(CONFIG_RM_MUXER)                  += rmenc.o
OBJS-$(CONFIG_ROQ_DEMUXER)               += idroq.o
OBJS-$(CONFIG_ROQ_MUXER)                 += raw.o
OBJS-$(CONFIG_RTP_MUXER)                 += rtp.o         \
                                            rtpenc.o      \
                                            rtp_mpv.o     \
                                            rtp_aac.o     \
                                            rtpenc_h264.o \
                                            avc.o
OBJS-$(CONFIG_RTSP_DEMUXER)              += rtsp.o
OBJS-$(CONFIG_SDP_DEMUXER)               += rtsp.o rtp.o rtpdec.o rtp_h264.o
OBJS-$(CONFIG_SEGAFILM_DEMUXER)          += segafilm.o
OBJS-$(CONFIG_SHORTEN_DEMUXER)           += raw.o
OBJS-$(CONFIG_SIFF_DEMUXER)              += siff.o
OBJS-$(CONFIG_SMACKER_DEMUXER)           += smacker.o
OBJS-$(CONFIG_SOL_DEMUXER)               += sol.o raw.o
OBJS-$(CONFIG_STR_DEMUXER)               += psxstr.o
OBJS-$(CONFIG_SWF_DEMUXER)               += swf.o
OBJS-$(CONFIG_SWF_MUXER)                 += swf.o
OBJS-$(CONFIG_TG2_MUXER)                 += movenc.o riff.o isom.o avc.o
OBJS-$(CONFIG_TGP_MUXER)                 += movenc.o riff.o isom.o avc.o
OBJS-$(CONFIG_THP_DEMUXER)               += thp.o
OBJS-$(CONFIG_TIERTEXSEQ_DEMUXER)        += tiertexseq.o
OBJS-$(CONFIG_TTA_DEMUXER)               += tta.o
OBJS-$(CONFIG_TXD_DEMUXER)               += txd.o
OBJS-$(CONFIG_VC1_DEMUXER)               += raw.o
OBJS-$(CONFIG_VC1T_DEMUXER)              += vc1test.o
OBJS-$(CONFIG_VMD_DEMUXER)               += sierravmd.o
OBJS-$(CONFIG_VOC_DEMUXER)               += vocdec.o voc.o
OBJS-$(CONFIG_VOC_MUXER)                 += vocenc.o voc.o
OBJS-$(CONFIG_WAV_DEMUXER)               += wav.o riff.o raw.o
OBJS-$(CONFIG_WAV_MUXER)                 += wav.o riff.o
OBJS-$(CONFIG_WC3_DEMUXER)               += wc3movie.o
OBJS-$(CONFIG_WSAUD_DEMUXER)             += westwood.o
OBJS-$(CONFIG_WSVQA_DEMUXER)             += westwood.o
OBJS-$(CONFIG_WV_DEMUXER)                += wv.o
OBJS-$(CONFIG_YUV4MPEGPIPE_MUXER)        += yuv4mpeg.o
OBJS-$(CONFIG_YUV4MPEGPIPE_DEMUXER)      += yuv4mpeg.o

# external libraries
OBJS-$(CONFIG_LIBNUT_DEMUXER)            += libnut.o riff.o
OBJS-$(CONFIG_LIBNUT_MUXER)              += libnut.o riff.o

OBJS-$(CONFIG_VHOOK)                     += framehook.o

# protocols I/O
OBJS+= avio.o aviobuf.o

OBJS-$(CONFIG_FILE_PROTOCOL)             += file.o
OBJS-$(CONFIG_HTTP_PROTOCOL)             += http.o
OBJS-$(CONFIG_PIPE_PROTOCOL)             += file.o
OBJS-$(CONFIG_RTP_PROTOCOL)              += rtpproto.o
OBJS-$(CONFIG_TCP_PROTOCOL)              += tcp.o
OBJS-$(CONFIG_UDP_PROTOCOL)              += udp.o

include ../common.mak
