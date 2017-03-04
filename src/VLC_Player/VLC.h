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
#include <map>

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
    bool m_bEventsAttached;
    Display* m_pDisplay;
    Window m_Window;
    Cursor m_Cursor;
    Pixmap m_NoPixmap;
    float m_fDuration, m_fPosition;
    bool m_bIsPlaying;
    int m_iSerialNum;
    int m_iStreamID;
    int m_iPreviousAudioTrack;
    int m_iChapter;
    map<int, int> m_mapDgIndexToSubtitleId;
    map<int, int> m_mapDgIndexToAudioTrackId;
    DCE::VLC_Player* m_pVLC_Player;

  public:

    class Config
    {
    public:
      string m_sSoundCard;
      string m_sAudioSettings;
      string m_sHardwareAcceleration;
      string m_sDeinterlacingMode;
      string m_sVideoSettings;
      unsigned m_iWidth;
      unsigned m_iHeight;

      Config();
      ~Config();

      bool parseVideoSettings();

    };

    Config *m_pConfig;
    string m_sAudioInfo; // TBD: Compatible with Xine_Player
    string m_sVideoInfo; // TBD: Compatible with Xine_Player
    string m_sMediaInfo; // Section description info.

    string m_sWindowTitle;
    string m_sMediaType;
    string m_sMediaURL;
    int m_iMediaID;
    int m_iZoomPercent;

    VLC(Config* pConfig, DCE::VLC_Player* pVLC_Player);
    ~VLC();
    bool init();
    bool CreateWindow();
    bool Minimize();
    bool PlayURL(string sMediaURL, int iStreamID, string sMediaPosition, string& sMediaInfo);
    void Stop();
    float GetDuration();
    int64_t GetCurrentDuration();
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
    void UpdateMediaState(const libvlc_event_t* event);
    void SetStreamID(int iStreamID);
    int GetStreamID();
    void SetMediaURL(string sMediaURL);
    string GetMediaURL();
    bool GetScreenShot(int iWidth, int iHeight, string& sCMD_Result);
    void GotoMediaMenu(int iMenu);
    bool hasChapters();
    int numChapters();
    int GetCurrentChapter();
    int GetCurrentTitle();
    int GetCurrentAngle() { return 0; } 
    void SetAudioTrack(int iAudioTrack);
    int GetAudioTrack();
    int GetVLCAudioTrack();
    void SetPreviousAudioTrack(int iTrack) { m_iPreviousAudioTrack = iTrack; };
    void SetSubtitle(int iSubtitle);
    int GetSubtitle();
    string GetAllSubtitles();
    string GetAllAudioTracks();
    string GetAllAngles();
    void SetAspectRatio(string sAspectRatio);
    void SetZoomFactor(string sZoomPercent);
    void SetMediaType(string sMediaType, int iMediaID);
    string GetMediaType();
    int GetMediaID();
    void SetChapter(int iChapter);
    void SetTitle(int iTitle);
    bool GetSize(unsigned& iWidth, unsigned& iHeight);
    void UpdateTracks();
    string GetMediaInfo();
    void ReportPlaybackStarted();
    void EnumerateAudioDevices();
    string PreProcessMediaURL(string sMediaURL);
    void OSD_Status(string sStatusString);
    string OSD_Time(int64_t time);
  };

}

#endif
