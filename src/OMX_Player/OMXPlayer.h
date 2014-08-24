#pragma once

//#include "OMX_Player.h"
#include "OMXPlayerInterface.h"

using namespace std;

class OMXPlayer : public OMXPlayerInterface
{
  private:
    // notifier callback from OMXPlayerInterface
    void PlayerStateNotifier(OMXPlayerInterface::State PlayerState);

  public:
  OMXPlayer(string sAudioDevice, bool bPassthrough, string sGpuDeInt, const void *pInstance);
  OMXPlayer(string sAudioDevice, bool bPassthrough, string sGpuDeInt);
//     OMXPlayer(const void *pInstance);
    ~OMXPlayer(void);
};
