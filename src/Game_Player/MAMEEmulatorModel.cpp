/** 
 * X11EmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * an emulator controlled via X11 keys and events
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "MAMEEmulatorModel.h"
#include <X11/keysym.h>

namespace DCE
{

  MAMEEmulatorModel::MAMEEmulatorModel()
    : X11EmulatorModel()
  {
    
  }

  MAMEEmulatorModel::~MAMEEmulatorModel()
  {
  
  }

  void MAMEEmulatorModel::initializeActionstoKeysyms()
  {
  
    // overridden by subclass.
    m_mapActionsToKeysyms["START1"] = make_pair(XK_1,0);
    m_mapActionsToKeysyms["START2"] = make_pair(XK_2,0);
    m_mapActionsToKeysyms["START3"] = make_pair(XK_3,0);
    m_mapActionsToKeysyms["START4"] = make_pair(XK_4,0);
    m_mapActionsToKeysyms["COIN1"] = make_pair(XK_5,0);
    m_mapActionsToKeysyms["COIN2"] = make_pair(XK_6,0);
    m_mapActionsToKeysyms["PAUSE"] = make_pair(XK_p,0);
    m_mapActionsToKeysyms["UI_UP"] = make_pair(XK_Up,0);
    m_mapActionsToKeysyms["UI_DOWN"] = make_pair(XK_Down,0);
    m_mapActionsToKeysyms["UI_LEFT"] = make_pair(XK_Left,0);
    m_mapActionsToKeysyms["UI_RIGHT"] = make_pair(XK_Right,0);
    m_mapActionsToKeysyms["UI_SELECT"] = make_pair(XK_Return,0);
    m_mapActionsToKeysyms["0"] = make_pair(XK_KP_0,0);
    m_mapActionsToKeysyms["1"] = make_pair(XK_KP_1,0);
    m_mapActionsToKeysyms["2"] = make_pair(XK_KP_2,0);
    m_mapActionsToKeysyms["3"] = make_pair(XK_KP_3,0);
    m_mapActionsToKeysyms["4"] = make_pair(XK_KP_4,0);
    m_mapActionsToKeysyms["5"] = make_pair(XK_KP_5,0);
    m_mapActionsToKeysyms["6"] = make_pair(XK_KP_6,0);
    m_mapActionsToKeysyms["7"] = make_pair(XK_KP_7,0);
    m_mapActionsToKeysyms["8"] = make_pair(XK_KP_8,0);
    m_mapActionsToKeysyms["9"] = make_pair(XK_KP_9,0);
    m_mapActionsToKeysyms["UI_CANCEL"] = make_pair(XK_Escape,0);
    m_mapActionsToKeysyms["SERVICE1"] = make_pair(XK_9,0);
    m_mapActionsToKeysyms["SERVICE2"] = make_pair(XK_0,0);
    m_mapActionsToKeysyms["SERVICE"] = make_pair(XK_F2,0);
    m_mapActionsToKeysyms["RESET"] = make_pair(XK_F3,XK_Shift_L);
    m_mapActionsToKeysyms["RECORD"] = make_pair(XK_F12,XK_Shift_L);
    m_mapActionsToKeysyms["UI_LOAD_STATE"] = make_pair(XK_F7,0);
    m_mapActionsToKeysyms["UI_SAVE_STATE"] = make_pair(XK_F7,XK_Shift_L);
    m_mapActionsToKeysyms["UI_PAGE_UP"] = make_pair(XK_Page_Up,0);
    m_mapActionsToKeysyms["UI_PAGE_DOWN"] = make_pair(XK_Page_Down,0);
  
  }

}
