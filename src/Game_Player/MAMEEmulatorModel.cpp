/** 
 * MAMEEmulatorModel - Store information needed by MAME Emulation
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 2.0
 *     rewritten for the inotify control.
 */

#include "MAMEEmulatorModel.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

namespace DCE
{

  MAMEEmulatorModel::MAMEEmulatorModel()
    : INotifyEmulatorModel()
  {
    m_bCanSaveState=true;
    m_sState="";
    m_sSystemName="arcade";
    
    m_mapActionsToCommandBytes["PAUSE"] = make_pair(0x01,false);
    m_mapActionsToCommandBytes["UNPAUSE"] = make_pair(0x02,false);
    m_mapActionsToCommandBytes["P1_START"] = make_pair(0x03,true);
    m_mapActionsToCommandBytes["P2_START"] = make_pair(0x04,true);
    m_mapActionsToCommandBytes["P3_START"] = make_pair(0x05,true);
    m_mapActionsToCommandBytes["P4_START"] = make_pair(0x06,true);
    m_mapActionsToCommandBytes["COIN1"] = make_pair(0x07,true);
    m_mapActionsToCommandBytes["COIN2"] = make_pair(0x08,true);
    m_mapActionsToCommandBytes["SNAP"] = make_pair(0x0C,true);
    m_mapActionsToCommandBytes["MENU"] = make_pair(0x0D,true);
    m_mapActionsToCommandBytes["UI_UP"] = make_pair(0x0E,true);
    m_mapActionsToCommandBytes["UI_DOWN"] = make_pair(0x0F,true);
    m_mapActionsToCommandBytes["UI_LEFT"] = make_pair(0x10,true);
    m_mapActionsToCommandBytes["UI_RIGHT"] = make_pair(0x11,true);
    m_mapActionsToCommandBytes["KEYPAD1_0"] = make_pair(0x13,true);
    m_mapActionsToCommandBytes["KEYPAD1_1"] = make_pair(0x14,true);
    m_mapActionsToCommandBytes["KEYPAD1_2"] = make_pair(0x15,true);
    m_mapActionsToCommandBytes["KEYPAD1_3"] = make_pair(0x16,true);
    m_mapActionsToCommandBytes["KEYPAD1_4"] = make_pair(0x17,true);
    m_mapActionsToCommandBytes["KEYPAD1_5"] = make_pair(0x18,true);
    m_mapActionsToCommandBytes["KEYPAD1_6"] = make_pair(0x19,true);
    m_mapActionsToCommandBytes["KEYPAD1_7"] = make_pair(0x1A,true);
    m_mapActionsToCommandBytes["KEYPAD1_8"] = make_pair(0x1B,true);
    m_mapActionsToCommandBytes["KEYPAD1_9"] = make_pair(0x1C,true);
    m_mapActionsToCommandBytes["KEYPAD1_*"] = make_pair(0x1D,true);
    m_mapActionsToCommandBytes["KEYPAD1_#"] = make_pair(0x1E,true);
    m_mapActionsToCommandBytes["SAVE_STATE"] = make_pair(0x20,false);
    m_mapActionsToCommandBytes["LOAD_STATE"] = make_pair(0x21,false);
    m_mapActionsToCommandBytes["SERVICE"] = make_pair(0x22,true);
    m_mapActionsToCommandBytes["SELECT"] = make_pair(0x24,true);
    m_mapActionsToCommandBytes["OPTION"] = make_pair(0x25,true);
    m_mapActionsToCommandBytes["RESET"] = make_pair(0x26,false);
    m_mapActionsToCommandBytes["KEYPAD2_0"] = make_pair(0x2B,true);
    m_mapActionsToCommandBytes["KEYPAD2_1"] = make_pair(0x2C,true);
    m_mapActionsToCommandBytes["KEYPAD2_2"] = make_pair(0x2D,true);
    m_mapActionsToCommandBytes["KEYPAD2_3"] = make_pair(0x2E,true);
    m_mapActionsToCommandBytes["KEYPAD2_4"] = make_pair(0x2F,true);
    m_mapActionsToCommandBytes["KEYPAD2_5"] = make_pair(0x30,true);
    m_mapActionsToCommandBytes["KEYPAD2_6"] = make_pair(0x31,true);
    m_mapActionsToCommandBytes["KEYPAD2_7"] = make_pair(0x32,true);
    m_mapActionsToCommandBytes["KEYPAD2_8"] = make_pair(0x33,true);
    m_mapActionsToCommandBytes["KEYPAD2_9"] = make_pair(0x34,true);
    m_mapActionsToCommandBytes["KEYPAD2_*"] = make_pair(0x35,true);
    m_mapActionsToCommandBytes["KEYPAD2_#"] = make_pair(0x36,true);
  }

  MAMEEmulatorModel::~MAMEEmulatorModel()
  {
    m_mapActionsToCommandBytes.clear();
  }

  string MAMEEmulatorModel::getVideoAccelleration()
  {
    // come back here later and actually flesh this out.
    return "opengl";
  }

  string MAMEEmulatorModel::getResolution()
  {
    string sRet = StringUtils::itos(m_iScreenWidth) + "x" +
      StringUtils::itos(m_iScreenHeight) + "@" +
      StringUtils::itos(m_iRefreshRate);
    return sRet;
  }

  /**
   * Update the /root/.mame/mame.ini file as needed.
   */
  bool MAMEEmulatorModel::updateConfig()
  {
    ConfigurationWriter config(MAME_CONFIG_FILE_TEMPLATE, MAME_CONFIG_FILE, m_sSystemConfiguration);

    if (m_sSystemName == "arcade")
      {
	config.Add("###ROMPATH###",FileUtils::BasePath(m_mapMedia_Find("default")));
      }
    else
      {
	config.Add("###ROMPATH###","/home/mamedata/roms");
      }

    config.Add("###VIDEO###",getVideoAccelleration());
    config.Add("###AVIWRITE###",(m_bIsRecording ? "recorded.avi" : ""));
    config.Add("###STATE###",m_sState);
    config.Add("###RESOLUTION###",getResolution());
    return config.Write();
  }

  void MAMEEmulatorModel::initializeActionstoKeysyms()
  {
  
    // overridden by subclass.
    /* m_mapActionsToKeysyms["P1_START"] = make_pair(XK_1,0);
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
    m_mapActionsToKeysyms["EXIT"] = make_pair(XK_Escape,0); */
  }

}
