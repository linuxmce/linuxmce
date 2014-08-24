#pragma once

#include "OMXPlayerInterface.h"
#include "OMX_Player.h"

namespace DCE
{
class OMX_Player;

class OMXPlayerStream : public OMXPlayerInterface
{
  private:
    // members
    OMX_Player *m_pOwner;

    // methods
    void Log(string txt);

    // notifier callback from OMXPlayerInterface
    void PlayerStateNotifier(OMXPlayerInterface::State PlayerState);

  public:
    // members

    // methods
    OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, OMX_Player *pOwner);
    ~OMXPlayerStream(void);
};

} // DCE namespace
