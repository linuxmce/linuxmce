/**
 * A5200EmulatorModel - Based on SecondGenEmulatorModel
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "A5200EmulatorModel.h"
#include <X11/keysym.h>
#include "DCE/Logger.h"

namespace DCE
{

  A5200EmulatorModel::A5200EmulatorModel() : SecondGenEmulatorModel()
  {
    m_sSystemName = "a5200";
    initializeActionstoKeysyms();
  }

  A5200EmulatorModel::~A5200EmulatorModel()
  {
  
  }

  void A5200EmulatorModel::initializeActionstoKeysyms()
  {
    MESSEmulatorModel::initializeActionstoKeysyms();
    m_mapActionsToKeysyms["*"] = make_pair(XK_KP_Add,0);
    m_mapActionsToKeysyms["#"] = make_pair(XK_KP_Enter,0);
  }

}
