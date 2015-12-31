/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#if !defined(__START_ORBITER_SDL_H__)
#define __START_ORBITER_SDL_H__

#include "../Orbiter.h"
#include "../Linux/OrbiterLinux.h"
#include "../Linux/OrbiterRenderer_Linux.h"

#include <SDL.h>

struct keyboardState
{
  keyboardState()
    : bShiftDown(false)
      , bControlDown(false)
      , bAltDown(false)
      , bRepeat(false)
      , bCapsLock(false)
      , cKeyDown(0)
    {
    }
  bool bShiftDown;
  bool bControlDown;
  bool bAltDown;
  bool bRepeat;
  bool bCapsLock;
  clock_t cKeyDown;
};

// initialize OS stuff
// application-specific code
// exit if this fails
bool Init_System();

struct SDL_Event_Loop_Data
{
  Orbiter* pOrbiter;
  SDL_Event event;
  Orbiter::Event orbiterEvent;
  keyboardState kbdState;
};

OrbiterLinux *CreateOrbiter(int PK_Device,int PK_DeviceTemplate,string sRouter_IP,string sLocalDirectory,bool bLocalMode, int Width, int Height, bool bFullScreen,bool &bMustQuit, bool &bNeedReload);

bool SDL_Event_Process(SDL_Event_Loop_Data &sdl_event_loop_data);
bool SDL_Event_Loop_End(SDL_Event_Loop_Data &sdl_event_loop_data);

class SDL_App_Object
{
public:
    SDL_App_Object(int argc, char *argv[]);
    ~SDL_App_Object();

    bool Run(); // load-config, create, event-loop, destroy

    bool LoadConfig();

    bool Create(bool &bMustQuit, bool &bNeedReload);
    bool EventProcess();
    void Destroy();

    int GetExitCode() const;
    void SetExitCode(int nExitCode);

    SDL_Event_Loop_Data *m_pSDL_Event_Loop_Data;
protected:
    int argc;
    char **argv;
    int m_nExitCode;
};

int main(int argc, char *argv[]);

#endif
// __START_ORBITER_SDL_H__
