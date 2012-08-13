/**
 * FamicomEmulatorController - Support for the Nintendo Family Computer
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "FamicomEmulatorController.h"
#include "PlutoUtils/StringUtils.h"

namespace DCE
{

  FamicomEmulatorController::FamicomEmulatorController(Game_Player *pGame_Player, FamicomEmulatorModel *pEmulatorModel)
    : NESEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  FamicomEmulatorController::~FamicomEmulatorController()
  {
  
  }

  void FamicomEmulatorController::insertMediaNamed(string sMediaFile, string sSlot)
  {
    if (StringUtils::ToLower(sMediaFile).find(".fds"))
      {
	sSlot = "flop";
      }
    
    EmulatorController::insertMediaNamed(sMediaFile,sSlot);

  }

}
