/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __FEMONAAC_H
#define __FEMONAAC_H

#include "femonaudio.h"

class cFemonAAC {
private:
  cFemonAudioIf *m_AudioHandler;

  static unsigned int s_Samplerates[16];

public:
  cFemonAAC(cFemonAudioIf *audiohandler);
  virtual ~cFemonAAC();

  bool processAudio(const uint8_t *buf, int len);
  };

#endif //__FEMONAAC_H
