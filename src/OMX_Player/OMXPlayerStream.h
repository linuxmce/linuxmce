#pragma once

#include "OMXPlayerInterface.h"
#include "OMX_Player.h"
#include <X11/Xlib.h>

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

    // display (X window) related
    Display* m_pXDisplay;
    Window m_Window;
    XEvent *m_Event;
    Cursor m_Cursor;
    int m_ixpos, m_iypos, m_iwidth, m_iheight;
    double m_dScreenPixelAspect;
    int m_iCurrentScreen;

    // methods
    void Log(string txt);

    // x window control
    int XServerEventProcessor(XEvent &event );

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

    string m_sWindowTitle;

    // methods
    OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, OMX_Player *pOwner, int iTimeCodeReportFrequency);
    ~OMXPlayerStream(void);

    // x window control
    bool Init();
    void GetXDisplayAspectRatio();
    bool CreateWindow();
    bool Minimize();
    bool DestroyWindow();

    // media control
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
    void SetDeviceData_SubtitleTracks();
    void SetDeviceData_AudioTracks();
    void SetDeviceData_VideoTracks();

    bool setVolumeUp(int iRepeat_Command, string &sCMD_Result);
    bool setVolumeDown(int iRepeat_Command, string &sCMD_Result);
    bool setMute(string &sCMD_Result);
    bool setUnMute(string &sCMD_Result);
    string GetPosition();
    void ReportTimecode();
    int CalculatePosition(string &sMediaPosition,string *sMRL,int *Subtitle,int *Angle,int *AudioTrack);

};

} // DCE namespace
