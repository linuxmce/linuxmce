/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __FEMONMPEG_H
#define __FEMONMPEG_H

#include "femonvideo.h"
#include "femonaudio.h"

class cFemonMPEG {
private:
  cFemonVideoIf *m_VideoHandler;
  cFemonAudioIf *m_AudioHandler;

  static unsigned int s_Bitrates[2][3][16];
  static unsigned int s_Samplerates[2][4];
  static eAudioCodec  s_Formats[2][4];

public:
  cFemonMPEG(cFemonVideoIf *videohandler, cFemonAudioIf *audiohandler);
  virtual ~cFemonMPEG();

  bool processVideo(const uint8_t *buf, int len);
  bool processAudio(const uint8_t *buf, int len);
  };

#endif //__FEMONMPEG_H
