/**
 * ColecoVisionEmulatorModel - Based on SecondGenEmulatorModel
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "ColecoVisionEmulatorModel.h"
#include <X11/keysym.h>
#include "DCE/Logger.h"

namespace DCE
{

  ColecoVisionEmulatorModel::ColecoVisionEmulatorModel() : SecondGenEmulatorModel()
  {
    m_sSystemName = "coleco";
    initializeActionstoKeysyms();
  }

  ColecoVisionEmulatorModel::~ColecoVisionEmulatorModel()
  {
  
  }

  void ColecoVisionEmulatorModel::initializeActionstoKeysyms()
  {

    MESSEmulatorModel::initializeActionstoKeysyms();
    m_mapActionsToKeysyms["*"] = make_pair(XK_minus,0);
    m_mapActionsToKeysyms["#"] = make_pair(XK_equal,0);

  }

}
