/**
 * IntellivisionEmulatorModel - Based on SecondGenEmulatorModel
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "IntellivisionEmulatorModel.h"
#include <X11/keysym.h>
#include "DCE/Logger.h"

namespace DCE
{

  IntellivisionEmulatorModel::IntellivisionEmulatorModel() : SecondGenEmulatorModel()
  {
    m_sSystemName = "intv";
    initializeActionstoKeysyms();
  }

  IntellivisionEmulatorModel::~IntellivisionEmulatorModel()
  {
  
  }

  bool IntellivisionEmulatorModel::updateConfig()
  {
    MAMEEmulatorModel::updateConfig();
    // ConfigurationWriter keypadConfig(INTV_CONFIG_FILE_TEMPLATE, INTV_CONFIG_FILE, "");
    // return keypadConfig.Write();
  }

}
