/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __FEMONAC3_H
#define __FEMONAC3_H

#include "femonaudio.h"

class cFemonAC3 {
private:
  cFemonAC3If *m_AudioHandler;

  static unsigned int s_Bitrates[32];
  static unsigned int s_Frequencies[4];
  static unsigned int s_Frames[3][32];

public:
  cFemonAC3(cFemonAC3If *audiohandler);
  virtual ~cFemonAC3();

  bool processAudio(const uint8_t *buf, int len);
  };

#endif //__FEMONAC3_H
