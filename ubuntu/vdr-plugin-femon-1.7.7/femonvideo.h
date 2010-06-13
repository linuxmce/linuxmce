/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __FEMONVIDEO_H
#define __FEMONVIDEO_H

enum eVideoCodec {
  VIDEO_CODEC_INVALID = -1,
  VIDEO_CODEC_UNKNOWN,
  VIDEO_CODEC_MPEG2,
  VIDEO_CODEC_H264
  };

enum eVideoFormat {
  VIDEO_FORMAT_INVALID = -1,
  VIDEO_FORMAT_UNKNOWN,
  VIDEO_FORMAT_RESERVED,
  VIDEO_FORMAT_COMPONENT,
  VIDEO_FORMAT_PAL,
  VIDEO_FORMAT_NTSC,
  VIDEO_FORMAT_SECAM,
  VIDEO_FORMAT_MAC
  };

enum eVideoScan {
  VIDEO_SCAN_INVALID = -1,
  VIDEO_SCAN_UNKNOWN,
  VIDEO_SCAN_RESERVED,
  VIDEO_SCAN_INTERLACED,
  VIDEO_SCAN_PROGRESSIVE
  };

enum eVideoAspectRatio {
  VIDEO_ASPECT_RATIO_INVALID = -1,
  VIDEO_ASPECT_RATIO_RESERVED,
  VIDEO_ASPECT_RATIO_EXTENDED,
  VIDEO_ASPECT_RATIO_1_1,
  VIDEO_ASPECT_RATIO_4_3,
  VIDEO_ASPECT_RATIO_16_9,
  VIDEO_ASPECT_RATIO_2_21_1,
  VIDEO_ASPECT_RATIO_12_11,
  VIDEO_ASPECT_RATIO_10_11,
  VIDEO_ASPECT_RATIO_16_11,
  VIDEO_ASPECT_RATIO_40_33,
  VIDEO_ASPECT_RATIO_24_11,
  VIDEO_ASPECT_RATIO_20_11,
  VIDEO_ASPECT_RATIO_32_11,
  VIDEO_ASPECT_RATIO_80_33,
  VIDEO_ASPECT_RATIO_18_11,
  VIDEO_ASPECT_RATIO_15_11,
  VIDEO_ASPECT_RATIO_64_33,
  VIDEO_ASPECT_RATIO_160_99,
  VIDEO_ASPECT_RATIO_3_2,
  VIDEO_ASPECT_RATIO_2_1
  };

typedef struct video_info {
  eVideoCodec       codec;       // enum
  eVideoFormat      format;      // enum
  eVideoScan        scan;        // enum
  eVideoAspectRatio aspectRatio; // enum
  int               width;       // pixels
  int               height;      // pixels
  double            frameRate;   // Hz
  double            bitrate;     // bit/s
} video_info_t;

class cFemonVideoIf {
public:
  cFemonVideoIf() {}
  virtual ~cFemonVideoIf() {}

  // eVideoCodec
  virtual void SetVideoCodec(eVideoCodec codec) = 0;
  // eVideoFormat
  virtual void SetVideoFormat(eVideoFormat format) = 0;
  // eVideoScan
  virtual void SetVideoScan(eVideoScan scan) = 0;
  // eVideoAspectRatio
  virtual void SetVideoAspectRatio(eVideoAspectRatio aspectratio) = 0;
  // pixels
  virtual void SetVideoSize(int width, int height) = 0;
  // Hz
  virtual void SetVideoFramerate(double framerate) = 0;
  // Mbit/s
  virtual void SetVideoBitrate(double bitrate) = 0;
  };

#endif //__FEMONVIDEO_H
