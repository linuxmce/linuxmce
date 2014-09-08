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
    string m_sMediaURL = "";
    int m_iStreamIDNew = 0;
    string m_sMediaURLNew = "";
    bool bEvent = true;
    int64_t m_xDuration = -1;


    int m_iStreamID = 0;
    int m_iChapter = -1;
    int m_iTitle = -1;

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

    bool Play(int iStreamID, string sMediaURL, string sMediaPosition);
    void Stop(int iStreamID);
    int Get_iStreamID();
    string Get_MediaType();
    int Get_MediaID();

    string GetPosition();
    void ReportTimecode();
    int CalculatePosition(string &sMediaPosition,string *sMRL,int *Subtitle,int *Angle,int *AudioTrack);

};

} // DCE namespace
