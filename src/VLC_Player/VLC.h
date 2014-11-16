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

  class VLC
  {
  private:
    libvlc_instance_t* m_pInst;
    libvlc_media_player_t* m_pMp;
    Display* m_pDisplay;
    Window m_Window;
    Cursor m_Cursor;
    Pixmap m_NoPixmap;
    float m_fDuration, m_fPosition;
    bool m_bIsPlaying;
    int m_iSerialNum;

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
    VLC(Config* pConfig);
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
    void SetTime(float fTime);
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

  };

}

#endif
