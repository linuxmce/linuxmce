/** 
 * X11EmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * an emulator controlled via X11 keys and events
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "X11EmulatorModel.h"
#include <X11/Xlib.h>

namespace DCE 
{
  X11EmulatorModel::X11EmulatorModel()
    : EmulatorModel()
  {
  }

  X11EmulatorModel::~X11EmulatorModel()
  {
  }

}
