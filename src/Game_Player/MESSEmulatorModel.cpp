/** 
 * MESSEmulatorModel - base configuration state for MESS based emulators
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "MESSEmulatorModel.h"
#include <X11/keysym.h>
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

namespace DCE
{

  MESSEmulatorModel::MESSEmulatorModel()
    : X11EmulatorModel()
  {
    m_sWindowName = "mess.mess";
    m_sSystemName = "";
    m_sRomPath = "";
    m_bCanSaveState=true;
    initializeActionstoKeysyms();
    m_bIsComputer=false;
  }

  MESSEmulatorModel::~MESSEmulatorModel()
  {
  }

  string MESSEmulatorModel::getVideoAccelleration()
  {
    // come back here later and actually flesh this out.
    return "opengl";
  }

  string MESSEmulatorModel::getResolution()
  {
    string sRet = StringUtils::itos(m_iScreenWidth) + "x" +
      StringUtils::itos(m_iScreenHeight) + "@" +
      StringUtils::itos(m_iRefreshRate);
    return sRet;
  }

  /**
   * Update the /root/.mame/mame.ini file as needed.
   */
  bool MESSEmulatorModel::updateConfig()
  {
    ConfigurationWriter config(MESS_CONFIG_FILE_TEMPLATE, MESS_CONFIG_FILE, m_sSystemConfiguration);
    config.Add("###VIDEO###",getVideoAccelleration());
    config.Add("###AVIWRITE###",(m_bIsRecording ? "recorded.avi" : ""));
    config.Add("###STATE###",m_sState);
    config.Add("###ROMPATH###",m_sRomPath);
    config.Add("###RESOLUTION###",getResolution());
    return config.Write();
  }

  void MESSEmulatorModel::initializeActionstoKeysyms()
  {
  
    // overridden by subclass.
    m_mapActionsToKeysyms["PARTIALKBD"] = make_pair(XK_Scroll_Lock,0);
    m_mapActionsToKeysyms["P1_START"] = make_pair(XK_1,0);
    m_mapActionsToKeysyms["P2_START"] = make_pair(XK_2,0);
    m_mapActionsToKeysyms["P3_START"] = make_pair(XK_3,0);
    m_mapActionsToKeysyms["P4_START"] = make_pair(XK_4,0);
    m_mapActionsToKeysyms["COIN1"] = make_pair(XK_5,0);
    m_mapActionsToKeysyms["COIN2"] = make_pair(XK_6,0);
    m_mapActionsToKeysyms["PAUSE"] = make_pair(XK_p,0);
    m_mapActionsToKeysyms["UNPAUSE"] = make_pair(XK_p,0);
    m_mapActionsToKeysyms["SPEED_1000"] = make_pair(0,0); // null key
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
    m_mapActionsToKeysyms["SERVICE1"] = make_pair(XK_F2,0);
    m_mapActionsToKeysyms["SERVICE2"] = make_pair(XK_0,0);
    m_mapActionsToKeysyms["SERVICE"] = make_pair(XK_F2,0);
    m_mapActionsToKeysyms["RESET"] = make_pair(XK_F3,0);
    m_mapActionsToKeysyms["RECORD"] = make_pair(XK_F12,XK_Shift_L);
    m_mapActionsToKeysyms["LOAD_STATE"] = make_pair(XK_F7,0);
    m_mapActionsToKeysyms["SAVE_STATE"] = make_pair(XK_F7,XK_Shift_L);
    m_mapActionsToKeysyms["UI_PAGE_UP"] = make_pair(XK_Page_Up,0);
    m_mapActionsToKeysyms["UI_PAGE_DOWN"] = make_pair(XK_Page_Down,0);  
    m_mapActionsToKeysyms["UI_ENTER"] = make_pair(XK_Tab,0);
    m_mapActionsToKeysyms["UI_EXIT"] = make_pair(XK_Tab,0);
    m_mapActionsToKeysyms["EXIT"] = make_pair(XK_Escape,0);
  }

}
