/**
 * FSUAEEmulatorModel - Store information needed by Commodore-Amiga emulations
 * emulated by FS-UAE.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "FSUAEEmulatorModel.h"
#include <X11/keysym.h>
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "pluto_main/Define_Button.h"

namespace DCE
{
  FSUAEEmulatorModel::FSUAEEmulatorModel() : X11EmulatorModel()
  {
    m_sConfigFile = FSUAE_CONFIG_FILE;
    m_sConfigFileTemplate = FSUAE_CONFIG_FILE_TEMPLATE;
    m_sWindowName = "fs-uae.fs-uae";
    initializeActionstoKeysyms();
    initializeButtontoKeysyms();
    m_sEmulatorBinary = "/usr/bin/fs-uae";
    m_sProcessName = ""; // set by the subclass.
    m_bChangeRequiresRestart=true;
    m_bRunning=false;
    m_bHasArgs=true;
    m_sArgs="";
  }

  FSUAEEmulatorModel::~FSUAEEmulatorModel()
  {
  }

  string FSUAEEmulatorModel::getVideoAccelleration()
  {
    return "opengl";   // not used yet.
  }

  /**
   * Update FSUAE configuration file
   */
  bool FSUAEEmulatorModel::updateConfig()
  {
    ConfigurationWriter config(FSUAE_CONFIG_FILE_TEMPLATE, FSUAE_CONFIG_FILE, m_sSystemConfiguration);
    return config.Write();
  }

  void FSUAEEmulatorModel::initializeActionstoKeysyms()
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
    m_mapActionsToKeysyms["LOAD_STATE"] = make_pair(XK_l,XK_Alt_L);
    m_mapActionsToKeysyms["SAVE_STATE"] = make_pair(XK_s,XK_Alt_L);
    m_mapActionsToKeysyms["PAUSE"] = make_pair(XK_p,XK_Alt_L);
    m_mapActionsToKeysyms["RESET"] = make_pair(XK_r,XK_Alt_L);
    m_mapActionsToKeysyms["UI_ENTER"] = make_pair(XK_F12,0);
    m_mapActionsToKeysyms["UI_EXIT"] = make_pair(XK_F12,0);
  }

  void FSUAEEmulatorModel::initializeButtontoKeysyms()
  {
    m_mapButtonToKeysyms[BUTTON_commoLeft_CONST] = XK_grave;
    m_mapButtonToKeysyms[BUTTON_sterling_CONST] = XK_Insert;
    m_mapButtonToKeysyms[BUTTON_plus_CONST] = XK_minus;
    m_mapButtonToKeysyms[BUTTON_dash_CONST] = XK_equal;
    m_mapButtonToKeysyms[BUTTON_commoInsDel_CONST] = XK_BackSpace;
    m_mapButtonToKeysyms[BUTTON_at_CONST] = XK_bracketleft;
    m_mapButtonToKeysyms[BUTTON_asterisk_CONST] = XK_bracketright;
    m_mapButtonToKeysyms[BUTTON_commoUp_CONST] = XK_Delete;
    m_mapButtonToKeysyms[BUTTON_Run_Stop_CONST] = XK_Escape;
    m_mapButtonToKeysyms[BUTTON_colon_CONST] = XK_colon;
    m_mapButtonToKeysyms[BUTTON_semicolumn_CONST] = XK_apostrophe;
    m_mapButtonToKeysyms[BUTTON_equals_sign_CONST] = XK_backslash;
    m_mapButtonToKeysyms[BUTTON_commoClrHome_CONST] = XK_Home;
    m_mapButtonToKeysyms[BUTTON_commoCRSRUD_CONST] = XK_Down;
    m_mapButtonToKeysyms[BUTTON_commoCRSRLR_CONST] = XK_Right;
  }

}
