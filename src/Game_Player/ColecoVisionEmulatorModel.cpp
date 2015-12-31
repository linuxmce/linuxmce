/**
 * ColecoVisionEmulatorModel - Based on SecondGenEmulatorModel
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "ColecoVisionEmulatorModel.h"
#include <X11/keysym.h>
#include "DCE/Logger.h"
#include "pluto_main/Define_Button.h"

namespace DCE
{

  ColecoVisionEmulatorModel::ColecoVisionEmulatorModel() : SecondGenEmulatorModel()
  {
    m_sSystemName = "coleco";
    initializeActionstoKeysyms();
    initializeButtontoKeysyms();
  }

  ColecoVisionEmulatorModel::~ColecoVisionEmulatorModel()
  {
  
  }

  void ColecoVisionEmulatorModel::initializeActionstoKeysyms()
  {
    /* MESSEmulatorModel::initializeActionstoKeysyms();
    m_mapActionsToKeysyms["*"] = make_pair(XK_KP_Add,0);
    m_mapActionsToKeysyms["#"] = make_pair(XK_KP_Subtract,0); */
  }

  void ColecoVisionEmulatorModel::initializeButtontoKeysyms()
  {
    /* m_mapButtonToKeysyms[BUTTON_asterisk_CONST] = XK_KP_Add;
       m_mapButtonToKeysyms[BUTTON_Pound_CONST] = XK_KP_Subtract; */
  }

  bool ColecoVisionEmulatorModel::updateConfig()
  {
    MAMEEmulatorModel::updateConfig();
    /*
    ConfigurationWriter keypadConfig(COLECO_CONFIG_FILE_TEMPLATE, COLECO_CONFIG_FILE, "");
    return keypadConfig.Write(); */
  }

}
