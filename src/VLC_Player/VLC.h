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

    string m_sWindowTitle;
    VLC(Config* pConfig);
    ~VLC();
    bool init();
    bool CreateWindow();
    bool Minimize();
  };

}

#endif
