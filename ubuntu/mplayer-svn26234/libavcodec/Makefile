#
# libavcodec Makefile
# (c) 2000-2005 Fabrice Bellard
#
include ../config.mak

CFLAGS += -fomit-frame-pointer

NAME = avcodec
FFLIBS = avutil

OBJS = allcodecs.o \
       audioconvert.o \
       bitstream.o \
       bitstream_filter.o \
       dsputil.o \
       eval.o \
       faanidct.o \
       imgconvert.o \
       jrevdct.o \
       opt.o \
       parser.o \
       raw.o \
       resample.o \
       resample2.o \
       simple_idct.o \
       utils.o \


HEADERS = avcodec.h opt.h

OBJS-$(CONFIG_ENCODERS)                += faandct.o jfdctfst.o jfdctint.o

OBJS-$(CONFIG_AASC_DECODER)            += aasc.o
OBJS-$(CONFIG_AC3_DECODER)             += ac3dec.o ac3tab.o ac3.o mdct.o fft.o
OBJS-$(CONFIG_AC3_ENCODER)             += ac3enc.o ac3tab.o ac3.o
OBJS-$(CONFIG_ALAC_DECODER)            += alac.o
OBJS-$(CONFIG_AMV_DECODER)             += sp5xdec.o mjpegdec.o mjpeg.o
OBJS-$(CONFIG_APE_DECODER)             += apedec.o
OBJS-$(CONFIG_ASV1_DECODER)            += asv1.o mpeg12data.o
OBJS-$(CONFIG_ASV1_ENCODER)            += asv1.o mpeg12data.o
OBJS-$(CONFIG_ASV2_DECODER)            += asv1.o mpeg12data.o
OBJS-$(CONFIG_ASV2_ENCODER)            += asv1.o mpeg12data.o
OBJS-$(CONFIG_ATRAC3_DECODER)          += atrac3.o mdct.o fft.o
OBJS-$(CONFIG_AVS_DECODER)             += avs.o
OBJS-$(CONFIG_BETHSOFTVID_DECODER)     += bethsoftvideo.o
OBJS-$(CONFIG_BMP_DECODER)             += bmp.o
OBJS-$(CONFIG_BMP_ENCODER)             += bmpenc.o
OBJS-$(CONFIG_C93_DECODER)             += c93.o
OBJS-$(CONFIG_CAVS_DECODER)            += cavs.o cavsdec.o cavsdsp.o golomb.o mpeg12data.o mpegvideo.o
OBJS-$(CONFIG_CINEPAK_DECODER)         += cinepak.o
OBJS-$(CONFIG_CLJR_DECODER)            += cljr.o
OBJS-$(CONFIG_CLJR_ENCODER)            += cljr.o
OBJS-$(CONFIG_COOK_DECODER)            += cook.o mdct.o fft.o
OBJS-$(CONFIG_CSCD_DECODER)            += cscd.o
OBJS-$(CONFIG_CYUV_DECODER)            += cyuv.o
OBJS-$(CONFIG_DCA_DECODER)             += dca.o
OBJS-$(CONFIG_DNXHD_DECODER)           += dnxhddec.o dnxhddata.o
OBJS-$(CONFIG_DNXHD_ENCODER)           += dnxhdenc.o dnxhddata.o mpegvideo_enc.o motion_est.o ratecontrol.o mpeg12data.o mpegvideo.o
OBJS-$(CONFIG_DSICINAUDIO_DECODER)     += dsicinav.o
OBJS-$(CONFIG_DSICINVIDEO_DECODER)     += dsicinav.o
OBJS-$(CONFIG_DVBSUB_DECODER)          += dvbsubdec.o
OBJS-$(CONFIG_DVBSUB_ENCODER)          += dvbsub.o
OBJS-$(CONFIG_DVDSUB_DECODER)          += dvdsubdec.o
OBJS-$(CONFIG_DVDSUB_ENCODER)          += dvdsubenc.o
OBJS-$(CONFIG_DVVIDEO_DECODER)         += dv.o
OBJS-$(CONFIG_DVVIDEO_ENCODER)         += dv.o
OBJS-$(CONFIG_DXA_DECODER)             += dxa.o
OBJS-$(CONFIG_EIGHTBPS_DECODER)        += 8bps.o
OBJS-$(CONFIG_FFV1_DECODER)            += ffv1.o rangecoder.o golomb.o
OBJS-$(CONFIG_FFV1_ENCODER)            += ffv1.o rangecoder.o
OBJS-$(CONFIG_FFVHUFF_DECODER)         += huffyuv.o
OBJS-$(CONFIG_FFVHUFF_ENCODER)         += huffyuv.o
OBJS-$(CONFIG_FLAC_DECODER)            += flac.o golomb.o
OBJS-$(CONFIG_FLAC_ENCODER)            += flacenc.o golomb.o
OBJS-$(CONFIG_FLASHSV_DECODER)         += flashsv.o
OBJS-$(CONFIG_FLASHSV_ENCODER)         += flashsvenc.o
OBJS-$(CONFIG_FLIC_DECODER)            += flicvideo.o
OBJS-$(CONFIG_FLV_DECODER)             += h263dec.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_FLV_ENCODER)             += mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_FOURXM_DECODER)          += 4xm.o
OBJS-$(CONFIG_FRAPS_DECODER)           += fraps.o huffman.o
OBJS-$(CONFIG_GIF_DECODER)             += gifdec.o lzw.o
OBJS-$(CONFIG_GIF_ENCODER)             += gif.o
OBJS-$(CONFIG_H261_DECODER)            += h261dec.o h261.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_H261_ENCODER)            += h261enc.o h261.o mpegvideo_enc.o motion_est.o ratecontrol.o mpeg12data.o mpegvideo.o
OBJS-$(CONFIG_H263_DECODER)            += h263dec.o h263.o h263_parser.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_H263I_DECODER)           += h263dec.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_H263_ENCODER)            += mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_H263P_ENCODER)           += mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_H264_DECODER)            += h264.o h264idct.o h264pred.o h264_parser.o cabac.o golomb.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_H264_ENCODER)            += h264enc.o h264dspenc.o
OBJS-$(CONFIG_HUFFYUV_DECODER)         += huffyuv.o
OBJS-$(CONFIG_HUFFYUV_ENCODER)         += huffyuv.o
OBJS-$(CONFIG_IDCIN_DECODER)           += idcinvideo.o
OBJS-$(CONFIG_IMC_DECODER)             += imc.o mdct.o fft.o
OBJS-$(CONFIG_INDEO2_DECODER)          += indeo2.o
OBJS-$(CONFIG_INDEO3_DECODER)          += indeo3.o
OBJS-$(CONFIG_INTERPLAY_DPCM_DECODER)  += dpcm.o
OBJS-$(CONFIG_INTERPLAY_VIDEO_DECODER) += interplayvideo.o
OBJS-$(CONFIG_JPEGLS_DECODER)          += jpeglsdec.o jpegls.o mjpegdec.o mjpeg.o golomb.o
OBJS-$(CONFIG_JPEGLS_ENCODER)          += jpeglsenc.o jpegls.o golomb.o
OBJS-$(CONFIG_KMVC_DECODER)            += kmvc.o
OBJS-$(CONFIG_LJPEG_ENCODER)           += ljpegenc.o mjpegenc.o mjpeg.o mpegvideo_enc.o motion_est.o ratecontrol.o mpeg12data.o mpegvideo.o
OBJS-$(CONFIG_LOCO_DECODER)            += loco.o golomb.o
OBJS-$(CONFIG_MACE3_DECODER)           += mace.o
OBJS-$(CONFIG_MACE6_DECODER)           += mace.o
OBJS-$(CONFIG_MJPEG_DECODER)           += mjpegdec.o mjpeg.o
OBJS-$(CONFIG_MJPEG_ENCODER)           += mjpegenc.o mjpeg.o mpegvideo_enc.o motion_est.o ratecontrol.o mpeg12data.o mpegvideo.o
OBJS-$(CONFIG_MJPEGB_DECODER)          += mjpegbdec.o mjpegdec.o mjpeg.o
OBJS-$(CONFIG_MMVIDEO_DECODER)         += mmvideo.o
OBJS-$(CONFIG_MP2_DECODER)             += mpegaudiodec.o mpegaudiodecheader.o mpegaudio.o mpegaudiodata.o
OBJS-$(CONFIG_MP2_ENCODER)             += mpegaudioenc.o mpegaudio.o mpegaudiodata.o
OBJS-$(CONFIG_MP3_DECODER)             += mpegaudiodec.o mpegaudiodecheader.o mpegaudio.o mpegaudiodata.o
OBJS-$(CONFIG_MP3ADU_DECODER)          += mpegaudiodec.o mpegaudiodecheader.o mpegaudio.o mpegaudiodata.o
OBJS-$(CONFIG_MP3ON4_DECODER)          += mpegaudiodec.o mpegaudiodecheader.o mpegaudio.o mpegaudiodata.o
OBJS-$(CONFIG_MPC7_DECODER)            += mpc7.o mpc.o mpegaudiodec.o mpegaudiodecheader.o mpegaudio.o mpegaudiodata.o
OBJS-$(CONFIG_MPC8_DECODER)            += mpc8.o mpc.o mpegaudiodec.o mpegaudiodecheader.o mpegaudio.o mpegaudiodata.o
OBJS-$(CONFIG_MDEC_DECODER)            += mpeg12.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MPEGVIDEO_DECODER)       += mpeg12.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MPEG1VIDEO_DECODER)      += mpeg12.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MPEG1VIDEO_ENCODER)      += mpeg12enc.o mpeg12data.o mpegvideo_enc.o motion_est.o ratecontrol.o mpeg12.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MPEG2VIDEO_DECODER)      += mpeg12.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MPEG2VIDEO_ENCODER)      += mpeg12enc.o mpeg12data.o mpegvideo_enc.o motion_est.o ratecontrol.o mpeg12.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MPEG4_DECODER)           += h263dec.o h263.o mpeg4video_parser.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MPEG4_ENCODER)           += mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MSMPEG4V1_DECODER)       += msmpeg4.o msmpeg4data.o h263dec.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MSMPEG4V1_ENCODER)       += msmpeg4.o msmpeg4data.o mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MSMPEG4V2_DECODER)       += msmpeg4.o msmpeg4data.o h263dec.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MSMPEG4V2_ENCODER)       += msmpeg4.o msmpeg4data.o mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MSMPEG4V3_DECODER)       += msmpeg4.o msmpeg4data.o h263dec.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MSMPEG4V3_ENCODER)       += msmpeg4.o msmpeg4data.o mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MSRLE_DECODER)           += msrle.o
OBJS-$(CONFIG_MSVIDEO1_DECODER)        += msvideo1.o
OBJS-$(CONFIG_MSZH_DECODER)            += lcldec.o
OBJS-$(CONFIG_NELLYMOSER_DECODER)      += nellymoserdec.o mdct.o fft.o
OBJS-$(CONFIG_NUV_DECODER)             += nuv.o rtjpeg.o
OBJS-$(CONFIG_PAM_ENCODER)             += pnmenc.o pnm.o
OBJS-$(CONFIG_PBM_ENCODER)             += pnmenc.o pnm.o
OBJS-$(CONFIG_PCX_DECODER)             += pcx.o
OBJS-$(CONFIG_PGM_ENCODER)             += pnmenc.o pnm.o
OBJS-$(CONFIG_PGMYUV_ENCODER)          += pnmenc.o pnm.o
OBJS-$(CONFIG_PNG_DECODER)             += png.o pngdec.o
OBJS-$(CONFIG_PNG_ENCODER)             += png.o pngenc.o
OBJS-$(CONFIG_PPM_ENCODER)             += pnmenc.o pnm.o
OBJS-$(CONFIG_PTX_DECODER)             += ptx.o
OBJS-$(CONFIG_QDM2_DECODER)            += qdm2.o mdct.o fft.o mpegaudiodec.o mpegaudiodecheader.o mpegaudio.o mpegaudiodata.o
OBJS-$(CONFIG_QDRAW_DECODER)           += qdrw.o
OBJS-$(CONFIG_QPEG_DECODER)            += qpeg.o
OBJS-$(CONFIG_QTRLE_DECODER)           += qtrle.o
OBJS-$(CONFIG_QTRLE_ENCODER)           += qtrleenc.o
OBJS-$(CONFIG_RA_144_DECODER)          += ra144.o
OBJS-$(CONFIG_RA_288_DECODER)          += ra288.o
OBJS-$(CONFIG_RAWVIDEO_DECODER)        += rawdec.o
OBJS-$(CONFIG_RAWVIDEO_ENCODER)        += rawenc.o
OBJS-$(CONFIG_ROQ_DECODER)             += roqvideodec.o roqvideo.o
OBJS-$(CONFIG_ROQ_ENCODER)             += roqvideoenc.o roqvideo.o elbg.o
OBJS-$(CONFIG_ROQ_DPCM_DECODER)        += dpcm.o
OBJS-$(CONFIG_ROQ_DPCM_ENCODER)        += roqaudioenc.o
OBJS-$(CONFIG_RPZA_DECODER)            += rpza.o
OBJS-$(CONFIG_RV10_DECODER)            += rv10.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_RV10_ENCODER)            += rv10.o mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_RV20_DECODER)            += rv10.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_RV20_ENCODER)            += rv10.o mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_SGI_DECODER)             += sgidec.o
OBJS-$(CONFIG_SGI_ENCODER)             += sgienc.o rle.o
OBJS-$(CONFIG_SHORTEN_DECODER)         += shorten.o golomb.o
OBJS-$(CONFIG_SMACKAUD_DECODER)        += smacker.o
OBJS-$(CONFIG_SMACKER_DECODER)         += smacker.o
OBJS-$(CONFIG_SMC_DECODER)             += smc.o
OBJS-$(CONFIG_SNOW_DECODER)            += snow.o rangecoder.o
OBJS-$(CONFIG_SNOW_ENCODER)            += snow.o rangecoder.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_SOL_DPCM_DECODER)        += dpcm.o
OBJS-$(CONFIG_SONIC_DECODER)           += sonic.o golomb.o
OBJS-$(CONFIG_SONIC_ENCODER)           += sonic.o golomb.o
OBJS-$(CONFIG_SONIC_LS_ENCODER)        += sonic.o golomb.o
OBJS-$(CONFIG_SP5X_DECODER)            += sp5xdec.o mjpegdec.o mjpeg.o
OBJS-$(CONFIG_SUNRAST_DECODER)         += sunrast.o
OBJS-$(CONFIG_SVQ1_DECODER)            += svq1dec.o svq1.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_SVQ1_ENCODER)            += svq1enc.o svq1.o motion_est.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_SVQ3_DECODER)            += h264.o h264idct.o h264pred.o h264_parser.o cabac.o golomb.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_TARGA_DECODER)           += targa.o
OBJS-$(CONFIG_TARGA_ENCODER)           += targaenc.o rle.o
OBJS-$(CONFIG_THEORA_DECODER)          += vp3.o xiph.o vp3dsp.o
OBJS-$(CONFIG_THP_DECODER)             += mjpegdec.o mjpeg.o
OBJS-$(CONFIG_TIERTEXSEQVIDEO_DECODER) += tiertexseqv.o
OBJS-$(CONFIG_TIFF_DECODER)            += tiff.o lzw.o
OBJS-$(CONFIG_TIFF_ENCODER)            += tiffenc.o rle.o lzwenc.o
OBJS-$(CONFIG_TRUEMOTION1_DECODER)     += truemotion1.o
OBJS-$(CONFIG_TRUEMOTION2_DECODER)     += truemotion2.o
OBJS-$(CONFIG_TRUESPEECH_DECODER)      += truespeech.o
OBJS-$(CONFIG_TSCC_DECODER)            += tscc.o
OBJS-$(CONFIG_TTA_DECODER)             += tta.o
OBJS-$(CONFIG_TXD_DECODER)             += txd.o s3tc.o
OBJS-$(CONFIG_ULTI_DECODER)            += ulti.o
OBJS-$(CONFIG_VB_DECODER)              += vb.o
OBJS-$(CONFIG_VC1_DECODER)             += vc1.o vc1data.o vc1dsp.o msmpeg4data.o h263dec.o h263.o intrax8.o intrax8dsp.o error_resilience.o
OBJS-$(CONFIG_VCR1_DECODER)            += vcr1.o
OBJS-$(CONFIG_VCR1_ENCODER)            += vcr1.o
OBJS-$(CONFIG_VMDAUDIO_DECODER)        += vmdav.o
OBJS-$(CONFIG_VMDVIDEO_DECODER)        += vmdav.o
OBJS-$(CONFIG_VMNC_DECODER)            += vmnc.o
OBJS-$(CONFIG_VORBIS_DECODER)          += vorbis_dec.o vorbis.o vorbis_data.o xiph.o mdct.o fft.o
OBJS-$(CONFIG_VORBIS_ENCODER)          += vorbis_enc.o vorbis.o vorbis_data.o mdct.o fft.o
OBJS-$(CONFIG_VP3_DECODER)             += vp3.o vp3dsp.o
OBJS-$(CONFIG_VP5_DECODER)             += vp5.o vp56.o vp56data.o vp3dsp.o
OBJS-$(CONFIG_VP6_DECODER)             += vp6.o vp56.o vp56data.o vp3dsp.o huffman.o
OBJS-$(CONFIG_VP6A_DECODER)            += vp6.o vp56.o vp56data.o vp3dsp.o huffman.o
OBJS-$(CONFIG_VP6F_DECODER)            += vp6.o vp56.o vp56data.o vp3dsp.o huffman.o
OBJS-$(CONFIG_VQA_DECODER)             += vqavideo.o
OBJS-$(CONFIG_WAVPACK_DECODER)         += wavpack.o
OBJS-$(CONFIG_WMAV1_DECODER)           += wmadec.o wma.o mdct.o fft.o
OBJS-$(CONFIG_WMAV1_ENCODER)           += wmaenc.o wma.o mdct.o fft.o
OBJS-$(CONFIG_WMAV2_DECODER)           += wmadec.o wma.o mdct.o fft.o
OBJS-$(CONFIG_WMAV2_ENCODER)           += wmaenc.o wma.o mdct.o fft.o
OBJS-$(CONFIG_WMV1_DECODER)            += h263dec.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_WMV1_ENCODER)            += mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_WMV2_DECODER)            += wmv2dec.o wmv2.o msmpeg4.o msmpeg4data.o h263dec.o h263.o intrax8.o intrax8dsp.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_WMV2_ENCODER)            += wmv2enc.o wmv2.o msmpeg4.o msmpeg4data.o mpegvideo_enc.o motion_est.o ratecontrol.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_WMV3_DECODER)            += vc1.o vc1data.o vc1dsp.o msmpeg4data.o h263dec.o h263.o intrax8.o intrax8dsp.o error_resilience.o
OBJS-$(CONFIG_WNV1_DECODER)            += wnv1.o
OBJS-$(CONFIG_WS_SND1_DECODER)         += ws-snd1.o
OBJS-$(CONFIG_XAN_DPCM_DECODER)        += dpcm.o
OBJS-$(CONFIG_XAN_WC3_DECODER)         += xan.o
OBJS-$(CONFIG_XAN_WC4_DECODER)         += xan.o
OBJS-$(CONFIG_XL_DECODER)              += xl.o
OBJS-$(CONFIG_XSUB_DECODER)            += xsubdec.o
OBJS-$(CONFIG_ZLIB_DECODER)            += lcldec.o
OBJS-$(CONFIG_ZLIB_ENCODER)            += lclenc.o
OBJS-$(CONFIG_ZMBV_DECODER)            += zmbv.o
OBJS-$(CONFIG_ZMBV_ENCODER)            += zmbvenc.o

OBJS-$(CONFIG_PCM_S32BE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_S32BE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_S32LE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_S32LE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_U32BE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_U32BE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_U32LE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_U32LE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_S24BE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_S24BE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_S24LE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_S24LE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_U24BE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_U24BE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_U24LE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_U24LE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_S24DAUD_DECODER)     += pcm.o
OBJS-$(CONFIG_PCM_S24DAUD_ENCODER)     += pcm.o
OBJS-$(CONFIG_PCM_S16LE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_S16LE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_S16LE_PLANAR_DECODER)+= pcm.o
OBJS-$(CONFIG_PCM_S16BE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_S16BE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_U16LE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_U16LE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_U16BE_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_U16BE_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_S8_DECODER)          += pcm.o
OBJS-$(CONFIG_PCM_S8_ENCODER)          += pcm.o
OBJS-$(CONFIG_PCM_U8_DECODER)          += pcm.o
OBJS-$(CONFIG_PCM_U8_ENCODER)          += pcm.o
OBJS-$(CONFIG_PCM_ALAW_DECODER)        += pcm.o
OBJS-$(CONFIG_PCM_ALAW_ENCODER)        += pcm.o
OBJS-$(CONFIG_PCM_MULAW_DECODER)       += pcm.o
OBJS-$(CONFIG_PCM_MULAW_ENCODER)       += pcm.o
OBJS-$(CONFIG_PCM_ZORK_DECODER)        += pcm.o
OBJS-$(CONFIG_PCM_ZORK_ENCODER)        += pcm.o

OBJS-$(CONFIG_ADPCM_4XM_DECODER)       += adpcm.o
OBJS-$(CONFIG_ADPCM_ADX_DECODER)       += adxdec.o
OBJS-$(CONFIG_ADPCM_ADX_ENCODER)       += adxenc.o
OBJS-$(CONFIG_ADPCM_CT_DECODER)        += adpcm.o
OBJS-$(CONFIG_ADPCM_EA_DECODER)        += adpcm.o
OBJS-$(CONFIG_ADPCM_EA_R1_DECODER)     += adpcm.o
OBJS-$(CONFIG_ADPCM_EA_R2_DECODER)     += adpcm.o
OBJS-$(CONFIG_ADPCM_EA_R3_DECODER)     += adpcm.o
OBJS-$(CONFIG_ADPCM_EA_XAS_DECODER)    += adpcm.o
OBJS-$(CONFIG_ADPCM_G726_DECODER)      += g726.o
OBJS-$(CONFIG_ADPCM_G726_ENCODER)      += g726.o
OBJS-$(CONFIG_ADPCM_IMA_AMV_DECODER)   += adpcm.o
OBJS-$(CONFIG_ADPCM_IMA_DK3_DECODER)   += adpcm.o
OBJS-$(CONFIG_ADPCM_IMA_DK4_DECODER)   += adpcm.o
OBJS-$(CONFIG_ADPCM_IMA_EA_EACS_DECODER) += adpcm.o
OBJS-$(CONFIG_ADPCM_IMA_EA_SEAD_DECODER) += adpcm.o
OBJS-$(CONFIG_ADPCM_IMA_QT_DECODER)    += adpcm.o
OBJS-$(CONFIG_ADPCM_IMA_SMJPEG_DECODER) += adpcm.o
OBJS-$(CONFIG_ADPCM_IMA_WAV_DECODER)   += adpcm.o
OBJS-$(CONFIG_ADPCM_IMA_WAV_ENCODER)   += adpcm.o
OBJS-$(CONFIG_ADPCM_IMA_WS_DECODER)    += adpcm.o
OBJS-$(CONFIG_ADPCM_MS_DECODER)        += adpcm.o
OBJS-$(CONFIG_ADPCM_MS_ENCODER)        += adpcm.o
OBJS-$(CONFIG_ADPCM_SBPRO_2_DECODER)   += adpcm.o
OBJS-$(CONFIG_ADPCM_SBPRO_3_DECODER)   += adpcm.o
OBJS-$(CONFIG_ADPCM_SBPRO_4_DECODER)   += adpcm.o
OBJS-$(CONFIG_ADPCM_SWF_DECODER)       += adpcm.o
OBJS-$(CONFIG_ADPCM_SWF_ENCODER)       += adpcm.o
OBJS-$(CONFIG_ADPCM_THP_DECODER)       += adpcm.o
OBJS-$(CONFIG_ADPCM_XA_DECODER)        += adpcm.o
OBJS-$(CONFIG_ADPCM_YAMAHA_DECODER)    += adpcm.o
OBJS-$(CONFIG_ADPCM_YAMAHA_ENCODER)    += adpcm.o

# libavformat dependencies
OBJS-$(CONFIG_GXF_DEMUXER)             += mpeg12data.o
OBJS-$(CONFIG_MATROSKA_AUDIO_MUXER)    += xiph.o
OBJS-$(CONFIG_MATROSKA_MUXER)          += xiph.o
OBJS-$(CONFIG_NUT_MUXER)               += mpegaudiodata.o
OBJS-$(CONFIG_OGG_MUXER)               += xiph.o
OBJS-$(CONFIG_RTP_MUXER)               += mpegvideo.o

# external codec libraries
OBJS-$(CONFIG_LIBA52)                  += liba52.o
OBJS-$(CONFIG_LIBAMR_NB)               += libamr.o
OBJS-$(CONFIG_LIBAMR_WB)               += libamr.o
OBJS-$(CONFIG_LIBFAAC)                 += libfaac.o
OBJS-$(CONFIG_LIBFAAD)                 += libfaad.o
OBJS-$(CONFIG_LIBGSM)                  += libgsm.o
OBJS-$(CONFIG_LIBMP3LAME)              += libmp3lame.o
OBJS-$(CONFIG_LIBTHEORA)               += libtheoraenc.o
OBJS-$(CONFIG_LIBVORBIS)               += libvorbis.o
OBJS-$(CONFIG_LIBX264)                 += libx264.o
OBJS-$(CONFIG_LIBXVID)                 += libxvidff.o libxvid_rc.o


OBJS-$(CONFIG_AAC_PARSER)              += aac_parser.o aac_ac3_parser.o
OBJS-$(CONFIG_AC3_PARSER)              += ac3_parser.o ac3tab.o aac_ac3_parser.o
OBJS-$(CONFIG_CAVSVIDEO_PARSER)        += cavs_parser.o
OBJS-$(CONFIG_DCA_PARSER)              += dca_parser.o
OBJS-$(CONFIG_DVBSUB_PARSER)           += dvbsub_parser.o
OBJS-$(CONFIG_DVDSUB_PARSER)           += dvdsub_parser.o
OBJS-$(CONFIG_H261_PARSER)             += h261_parser.o
OBJS-$(CONFIG_H263_PARSER)             += h263_parser.o
OBJS-$(CONFIG_H264_PARSER)             += h264_parser.o
OBJS-$(CONFIG_MJPEG_PARSER)            += mjpeg_parser.o
OBJS-$(CONFIG_MLP_PARSER)              += mlp_parser.o
OBJS-$(CONFIG_MPEG4VIDEO_PARSER)       += mpeg4video_parser.o h263.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_MPEGAUDIO_PARSER)        += mpegaudio_parser.o mpegaudiodecheader.o mpegaudiodata.o
OBJS-$(CONFIG_MPEGVIDEO_PARSER)        += mpegvideo_parser.o mpeg12.o mpeg12data.o mpegvideo.o error_resilience.o
OBJS-$(CONFIG_PNM_PARSER)              += pnm_parser.o pnm.o
OBJS-$(CONFIG_VC1_PARSER)              += vc1_parser.o

OBJS-$(CONFIG_DUMP_EXTRADATA_BSF)      += dump_extradata_bsf.o
OBJS-$(CONFIG_H264_MP4TOANNEXB_BSF)    += h264_mp4toannexb_bsf.o
OBJS-$(CONFIG_IMX_DUMP_HEADER_BSF)     += imx_dump_header_bsf.o
OBJS-$(CONFIG_MJPEGA_DUMP_HEADER_BSF)  += mjpega_dump_header_bsf.o
OBJS-$(CONFIG_MOV2TEXTSUB_BSF)         += movsub_bsf.o
OBJS-$(CONFIG_MP3_HEADER_COMPRESS_BSF) += mp3_header_compress_bsf.o
OBJS-$(CONFIG_MP3_HEADER_DECOMPRESS_BSF) += mp3_header_decompress_bsf.o mpegaudiodata.o
OBJS-$(CONFIG_NOISE_BSF)               += noise_bsf.o
OBJS-$(CONFIG_REMOVE_EXTRADATA_BSF)    += remove_extradata_bsf.o
OBJS-$(CONFIG_TEXT2MOVSUB_BSF)         += movsub_bsf.o

OBJS-$(HAVE_BEOSTHREADS)               += beosthread.o
OBJS-$(HAVE_OS2THREADS)                += os2thread.o
OBJS-$(HAVE_PTHREADS)                  += pthread.o
OBJS-$(HAVE_W32THREADS)                += w32thread.o

OBJS-$(HAVE_XVMC_ACCEL)                += xvmcvideo.o

ifneq ($(CONFIG_SWSCALE),yes)
OBJS += imgresample.o
endif

# processor-specific code
ifeq ($(HAVE_MMX),yes)
OBJS += i386/fdct_mmx.o \
        i386/cputest.o \
        i386/dsputil_mmx.o \
        i386/mpegvideo_mmx.o \
        i386/motion_est_mmx.o \
        i386/simple_idct_mmx.o \
        i386/idct_mmx_xvid.o \
        i386/fft_sse.o \
        i386/fft_3dn.o \
        i386/fft_3dn2.o \

OBJS-$(CONFIG_GPL)                     += i386/idct_mmx.o

OBJS-$(CONFIG_ENCODERS)                += i386/dsputilenc_mmx.o

OBJS-$(CONFIG_CAVS_DECODER)            += i386/cavsdsp_mmx.o
OBJS-$(CONFIG_FLAC_ENCODER)            += i386/flacdsp_mmx.o
OBJS-$(CONFIG_SNOW_DECODER)            += i386/snowdsp_mmx.o
OBJS-$(CONFIG_VC1_DECODER)             += i386/vc1dsp_mmx.o
OBJS-$(CONFIG_VP3_DECODER)             += i386/vp3dsp_mmx.o i386/vp3dsp_sse2.o
OBJS-$(CONFIG_VP5_DECODER)             += i386/vp3dsp_mmx.o i386/vp3dsp_sse2.o
OBJS-$(CONFIG_VP6_DECODER)             += i386/vp3dsp_mmx.o i386/vp3dsp_sse2.o
OBJS-$(CONFIG_VP6A_DECODER)            += i386/vp3dsp_mmx.o i386/vp3dsp_sse2.o
OBJS-$(CONFIG_VP6F_DECODER)            += i386/vp3dsp_mmx.o i386/vp3dsp_sse2.o
OBJS-$(CONFIG_WMV3_DECODER)            += i386/vc1dsp_mmx.o
endif

ASM_OBJS-$(ARCH_ARMV4L)                += armv4l/jrevdct_arm.o     \
                                          armv4l/simple_idct_arm.o \
                                          armv4l/dsputil_arm_s.o   \

OBJS-$(ARCH_ARMV4L)                    += armv4l/dsputil_arm.o   \
                                          armv4l/mpegvideo_arm.o \

OBJS-$(HAVE_IWMMXT)                    += armv4l/dsputil_iwmmxt.o   \
                                          armv4l/mpegvideo_iwmmxt.o \

ASM_OBJS-$(HAVE_ARMV5TE)               += armv4l/simple_idct_armv5te.o \
                                          armv4l/mpegvideo_armv5te.o \

ASM_OBJS-$(HAVE_ARMV6)                 += armv4l/simple_idct_armv6.o \

OBJS-$(HAVE_VIS)                       += sparc/dsputil_vis.o \
                                          sparc/simple_idct_vis.o \

OBJS-$(HAVE_MLIB)                      += mlib/dsputil_mlib.o \

OBJS-$(ARCH_ALPHA)                     += alpha/dsputil_alpha.o     \
                                          alpha/motion_est_alpha.o  \
                                          alpha/mpegvideo_alpha.o   \
                                          alpha/simple_idct_alpha.o \

ASM_OBJS-$(ARCH_ALPHA)                 += alpha/dsputil_alpha_asm.o  \
                                          alpha/motion_est_mvi_asm.o \

OBJS-$(ARCH_POWERPC)                   += ppc/dsputil_ppc.o   \

OBJS-$(HAVE_MMI)                       += ps2/dsputil_mmi.o   \
                                          ps2/idct_mmi.o      \
                                          ps2/mpegvideo_mmi.o \

OBJS-$(ARCH_SH4)                       += sh4/idct_sh4.o      \
                                          sh4/dsputil_align.o \
                                          sh4/dsputil_sh4.o   \

ALTIVEC-OBJS-yes                       += ppc/dsputil_altivec.o      \
                                          ppc/fdct_altivec.o         \
                                          ppc/fft_altivec.o          \
                                          ppc/float_altivec.o        \
                                          ppc/gmc_altivec.o          \
                                          ppc/idct_altivec.o         \
                                          ppc/int_altivec.o          \
                                          ppc/mpegvideo_altivec.o    \

ALTIVEC-OBJS-$(CONFIG_H264_DECODER)    += ppc/h264_altivec.o
ALTIVEC-OBJS-$(CONFIG_SNOW_DECODER)    += ppc/snow_altivec.o
ALTIVEC-OBJS-$(CONFIG_VC1_DECODER)     += ppc/vc1dsp_altivec.o
ALTIVEC-OBJS-$(CONFIG_WMV3_DECODER)    += ppc/vc1dsp_altivec.o

# -maltivec is needed in order to build AltiVec code.
$(ALTIVEC-OBJS-yes): CFLAGS += -maltivec -mabi=altivec

# check_altivec must be built without -maltivec
OBJS-$(HAVE_ALTIVEC)                   += $(ALTIVEC-OBJS-yes)       \
                                          ppc/check_altivec.o

OBJS-$(ARCH_BFIN)                      += bfin/dsputil_bfin.o \
                                          bfin/mpegvideo_bfin.o \
                                          bfin/vp3_bfin.o \

ASM_OBJS-$(ARCH_BFIN)                  += bfin/pixels_bfin.o \
                                          bfin/fdct_bfin.o   \
                                          bfin/idct_bfin.o   \
                                          bfin/vp3_idct_bfin.o   \

TESTS = $(addsuffix -test$(EXESUF), cabac dct eval fft h264 imgresample rangecoder snow)
ifeq ($(ARCH_X86),yes)
TESTS += cpuid-test$(EXESUF) motion-test$(EXESUF)
endif

include ../common.mak

clean::
	rm -f \
	   alpha/*.o alpha/*~ \
	   armv4l/*.o armv4l/*~ \
	   bfin/*.o bfin/*~ \
	   i386/*.o i386/*~ \
	   mlib/*.o mlib/*~ \
	   ppc/*.o ppc/*~ \
	   ps2/*.o ps2/*~ \
	   sh4/*.o sh4/*~ \
	   sparc/*.o sparc/*~ \
	   apiexample$(EXESUF)

cpuid-test$(EXESUF): i386/cputest.c
apiexample$(EXESUF): apiexample.o $(LIBNAME)
dct-test$(EXESUF): dct-test.o fdctref.o $(LIBNAME)
fft-test$(EXESUF): fft-test.o $(LIBNAME)
motion-test$(EXESUF): motion-test.o $(LIBNAME)
