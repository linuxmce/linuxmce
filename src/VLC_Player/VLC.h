/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef VLC_H
#define VLC_H

#include <vlc/vlc.h>
#include <X11/Xlib.h>

namespace DCE
{

  class VLC_Player;

  class VLC
  {
  private:
    libvlc_instance_t* m_pInst;
    libvlc_media_player_t* m_pMp;
    libvlc_event_manager_t* m_pMediaEventManager;
    libvlc_event_manager_t* m_pMediaPlayerEventManager;
    Display* m_pDisplay;
    Window m_Window;
    Cursor m_Cursor;
    Pixmap m_NoPixmap;
    float m_fDuration, m_fPosition;
    bool m_bIsPlaying;
    int m_iSerialNum;
    int m_iStreamID;
    DCE::VLC_Player* m_pVLC_Player;

  public:

    class Config
    {
    public:
      string m_sSoundCard;
      string m_sAudioSettings;
      string m_sHardwareAcceleration;
      string m_sDeinterlacingMode;
    };

    Config *m_pConfig;
    string m_sAudioInfo; // TBD: Compatible with Xine_Player
    string m_sVideoInfo; // TBD: Compatible with Xine_Player

    string m_sWindowTitle;
    VLC(Config* pConfig, DCE::VLC_Player* pVLC_Player);
    ~VLC();
    bool init();
    bool CreateWindow();
    bool Minimize();
    bool PlayURL(string sMediaURL, string sMediaPosition, string& sMediaInfo);
    void Stop();
    float GetDuration();
    void SetDuration(libvlc_time_t newDuration);
    float GetPosition();
    void SetPosition(float fPosition);
    int64_t SetTime(int64_t iTime);
    int64_t GetTime();
    bool IsPlaying();
    void SetPlaying(bool bIsPlaying);
    void UpdateStatus();
    void Pause();
    void Restart();
    void SetRate(float fMediaPlayBackSpeed);
    void JumpFwd(int iMult);
    void JumpBack(int iMult);
    bool IsWindowValid() { return m_Window != 0; }
    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();
    void EnterGo();
    void SetAudioVideoInfo();
    void NextChapter();
    void PreviousChapter();
    static void Media_Callbacks(const libvlc_event_t* event, void* ptr);
    void UpdateNav();
    void SetStreamID(int iStreamID);
    int GetStreamID();
    bool GetScreenShot(int iWidth, int iHeight, char** pData, int& iDataSize, string& sFormat, string& sCMD_Result);
    void GotoMediaMenu(int iMenu);
    bool hasChapters();
    int GetCurrentChapter();
    void SetAudioTrack(int iAudioTrack);
    int GetAudioTrack();
  };

}

#endif
