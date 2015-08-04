#pragma once

#include "OMXPlayerInterface.h"
#include "OMX_Player.h"

namespace DCE
{
class OMX_Player;

void* EventProcessingLoop(void* pInstance);

// client class
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


    bool m_bExitThread = false;
    int m_iStreamID = 0;
    int m_iChapter = 0;
    int m_iTitle = 0;
    int m_iMediaType = 0;

    mutex m_mtxLog;

    // methods
    void Log(string txt);

    // events processing thread
    pthread_t threadEventLoop;
    friend void* EventProcessingLoop(void* pInstance);

    // notifier callback from OMXPlayerInterface
    void PlayerStateNotifier(OMXPlayerStream::STATE playerState);

  public:
    // members
    int m_iTimeCodeReportFrequency;
    int m_iMediaID = -1;
    string m_sMediaType = ""; // this is actually the MediaID, not to be confused with the LMCE MediaType above.

    // methods
    OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, OMX_Player *pOwner, int iTimeCodeReportFrequency);
    ~OMXPlayerStream(void);

    bool Play(int iStreamID, string sMediaURL, string sMediaPosition, int iMediaType);
    void Stop(int iStreamID);
    int Get_iStreamID();
    string Get_MediaType();
    int Get_MediaID();
    bool GetScreenShot(int iWidth, int iHeight, string& sCMD_Result);

//    int setVideo(int track);
    int setAudio(int track);
    int setSubtitle(int track);
    int getCurrentSubtitle();
    int getCurrentAudioTrack();

    bool setVolumeUp(int iRepeat_Command, string &sCMD_Result);
    bool setVolumeDown(int iRepeat_Command, string &sCMD_Result);
    bool setMute(string &sCMD_Result);
    bool setUnMute(string &sCMD_Result);
    string GetPosition();
    void ReportTimecode();
    int CalculatePosition(string &sMediaPosition,string *sMRL,int *Subtitle,int *Angle,int *AudioTrack);

};

} // DCE namespace
