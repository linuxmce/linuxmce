/**
 * X11EmulatorController - A class to define common elements for 
 * emulators controlled by synthesizing X11 events. 
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef X11EMULATORCONTROLLER_H
#define X11EMULATORCONTROLLER_H

#include "EmulatorController.h"
#include <X11/Xlib.h>

/* STL includes */
#include <map>

using namespace std;

namespace DCE
{
  class X11EmulatorController : public EmulatorController
  {
  public:

    X11EmulatorController(Game_Player *pGame_Player, EmulatorModel *pEmulatorModel);  // ctor
    ~X11EmulatorController(); // dtor

    virtual bool run();
    virtual bool stop();

  protected:
    Display *m_pDisplay;
  private:
  };
}


#endif
