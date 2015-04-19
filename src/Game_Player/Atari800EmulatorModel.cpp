/** 
 * Atari800EmulatorModel - Hold configuration state for emulations based on Atari800
 * (http://atari800.sf.net/)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "Atari800EmulatorModel.h"
#include <X11/keysym.h>
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "pluto_main/Define_Button.h"

namespace DCE
{

  Atari800EmulatorModel::Atari800EmulatorModel()
    : X11EmulatorModel()
  {
    m_sConfigFile = ATARI800_CONFIG_FILE;
    m_sConfigFileTemplate = ATARI800_CONFIG_FILE_TEMPLATE;
    m_sWindowName = "atari800.atari800";
    initializeActionstoKeysyms();
    initializeButtontoKeysyms();
    m_sEmulatorBinary = "/usr/bin/atari800";
    m_sProcessName = "atari800";
    m_bChangeRequiresRestart=true;
    m_bRunning=false;
    m_bHasArgs=true;
    m_bCapsLockPressed=false;
    m_sArgs="";
    m_bCanSaveState=true;
  }

  Atari800EmulatorModel::~Atari800EmulatorModel()
  {
  }

  string Atari800EmulatorModel::getVideoAccelleration()
  {
    // come back here later and actually flesh this out.
    return "opengl";
  }

  /**
   * Update the /root/.mame/mame.ini file as needed.
   */
  bool Atari800EmulatorModel::updateConfig()
  {
    ConfigurationWriter config(ATARI800_CONFIG_FILE_TEMPLATE, ATARI800_CONFIG_FILE, m_sSystemConfiguration);
    config.Add("###WIDTH###",StringUtils::itos(m_iScreenWidth));
    config.Add("###HEIGHT###",StringUtils::itos(m_iScreenHeight));
    return config.Write();
  }

  void Atari800EmulatorModel::initializeActionstoKeysyms()
  {
    m_mapActionsToKeysyms["0"] = make_pair(XK_0,0);
    m_mapActionsToKeysyms["1"] = make_pair(XK_1,0);
    m_mapActionsToKeysyms["2"] = make_pair(XK_2,0);
    m_mapActionsToKeysyms["3"] = make_pair(XK_3,0);
    m_mapActionsToKeysyms["4"] = make_pair(XK_4,0);
    m_mapActionsToKeysyms["5"] = make_pair(XK_5,0);
    m_mapActionsToKeysyms["6"] = make_pair(XK_6,0);
    m_mapActionsToKeysyms["7"] = make_pair(XK_7,0);
    m_mapActionsToKeysyms["8"] = make_pair(XK_8,0);
    m_mapActionsToKeysyms["9"] = make_pair(XK_9,0);
    m_mapActionsToKeysyms["LOAD_STATE"] = make_pair(XK_F11,0);
    m_mapActionsToKeysyms["SAVE_STATE"] = make_pair(XK_F11,XK_Shift_L);
    m_mapActionsToKeysyms["UI_ENTER"] = make_pair(XK_F1,0);
    m_mapActionsToKeysyms["UI_EXIT"] = make_pair(XK_F1,0);
    m_mapActionsToKeysyms["GET_SNAPSHOT"] = make_pair(XK_F10,0);
    m_mapActionsToKeysyms["PAUSE"] = make_pair(XK_Pause,0);
    m_mapActionsToKeysyms["UNPAUSE"] = make_pair(XK_Scroll_Lock,0);
    m_mapActionsToKeysyms["RESET"] = make_pair(XK_F5,XK_Shift_L);
    m_mapActionsToKeysyms["OPTION"] = make_pair(XK_F2,0);
    m_mapActionsToKeysyms["SELECT"] = make_pair(XK_F3,0);
    m_mapActionsToKeysyms["START"] = make_pair(XK_F4,0);
    // overridden by subclass.
    // todo: come back here and fill this in.

  }

  void Atari800EmulatorModel::initializeButtontoKeysyms()
  {
    // Because Atari800 doesn't do "authentic" key mapping ala MAME/MESS/UME,
    // I have decided instead to override pressButton, and handle the crazy 
    // logic entirely there. I know I will regret it. -tschak

  }

}
