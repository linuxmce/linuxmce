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

  void IntellivisionEmulatorModel::initializeActionstoKeysyms()
  {

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Setting new keysyms for intv...");
    MESSEmulatorModel::initializeActionstoKeysyms();
    m_mapActionsToKeysyms["*"] = make_pair(XK_KP_Delete,0);
    m_mapActionsToKeysyms["#"] = make_pair(XK_KP_Enter,0);

  }

}
