/**
 * AirPlay_Videolan - A class that talks to libVLC and provides the media player
 * for the AirPlay part of the protocol
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef AIRPLAYVIDEOLAN_H
#define AIRPLAYVIDEOLAN_H

#include <vlc/vlc.h>
#include <X11/Xlib.h>

namespace DCE
{

  class AirPlay_Videolan
  {
  private:
    libvlc_instance_t *m_pInst;
    libvlc_media_player_t *m_pMp;
    Display *m_pDisplay;
    Window m_Window;
    Cursor m_Cursor;
    Pixmap m_NoPixmap;
    float m_fDuration, m_fPosition;
    bool m_bIsPlaying;

  protected:
  public:
    
    string m_sWindowTitle;
    
    AirPlay_Videolan();
    ~AirPlay_Videolan();
    bool init();
    bool CreateWindow();
    bool Minimize();
    void PlayURL(string sMediaURL);
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
  };

}

#endif
