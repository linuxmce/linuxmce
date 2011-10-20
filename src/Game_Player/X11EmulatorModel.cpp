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
    m_pDisplay = NULL;
    m_iWindowId = 0;
    initializeButtontoKeysyms();
  }

  X11EmulatorModel::~X11EmulatorModel()
  {
    m_pDisplay = NULL;
    m_iWindowId = 0;
  }

  bool X11EmulatorModel::updateConfig()
  {
    return EmulatorModel::updateConfig();
  }

  /** 
   * Initialize the map of PK_Buttons to XKeySyms
   *
   * This is ultimately used by functions requiring a PK_Button
   * value, such as CMD_Simulate_Keypress()
   *
   * This can be overriden by subclasses to subtly change mappings
   * as needed.
   */
  void X11EmulatorModel::initializeButtontoKeysyms()
  {
    // Come back here and fill out this mapping 
    /*    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Down_Arrow_CONST] = XK_Down;
    m_mapButtonToKeysyms[BUTTON_Left_Arrow_CONST] = XK_Left;
    m_mapButtonToKeysyms[BUTTON_Right_Arrow_CONST] = XK_Right;
    m_mapButtonToKeysyms[BUTTON_Enter_CONST] = XK_Enter;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    */
  }
 
}
