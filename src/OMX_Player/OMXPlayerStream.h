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
    OMX_Player *m_pPlayer = NULL;
    int m_iStreamID = 0;
    string m_sMediaURL = "";
    int m_iStreamIDNew = 0;
    string m_sMediaURLNew = "";
    bool bEvent = true;
    int64_t xDuration;

    mutex m_mtxLog;

    // methods
    void Log(string txt);

    // notifier callback from OMXPlayerInterface
    void PlayerStateNotifier(OMXPlayerStream::STATE playerState);

  public:
    // members

    // methods
    OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, OMX_Player *pOwner);
    ~OMXPlayerStream(void);

    bool Play(int iStreamID, string sMediaURL);
    void Stop(int iStreamID);

};

} // DCE namespace
