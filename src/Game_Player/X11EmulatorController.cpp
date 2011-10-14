/**
 * X11EmulatorController - A class to define common elements for 
 * emulators controlled by synthesizing X11 events. 
 *
 * This is another abstract class. Do not instantiate.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "X11EmulatorController.h"

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace DCE
{
  X11EmulatorController::X11EmulatorController(Game_Player *pGame_Player, EmulatorModel *pEmulatorModel) 
    : EmulatorController(pGame_Player, pEmulatorModel)
  {
    
  }

  X11EmulatorController::~X11EmulatorController()
  {
  }

  bool X11EmulatorController::run()
  {
    EmulatorController::run(); // superclass, sets running flag.

    // grab display.
    m_pDisplay = XOpenDisplay(getenv("DISPLAY"));
    if (!m_pDisplay)
      {
	EmulatorController::stop();
	return false;
      }

    return true;

  }

  bool X11EmulatorController::stop()
  {
    EmulatorController::stop(); // superclass, unsets running flag.
    
    // Close display
    XCloseDisplay(m_pDisplay);
    return true;
  }

}
