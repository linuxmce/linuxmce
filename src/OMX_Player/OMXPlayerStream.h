#pragma once

//#include "OMX_Player.h"
#include "OMXPlayerInterface.h"

using namespace std;

class OMXPlayerStream : public OMXPlayerInterface
{
  private:
    // notifier callback from OMXPlayerInterface
    void PlayerStateNotifier(OMXPlayerInterface::State PlayerState);

  public:
  OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, const void *pInstance);
//  OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt);
  ~OMXPlayerStream(void);
};
