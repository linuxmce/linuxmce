/**
 * NESEmulatorController - Support for the NEC PC-Engine
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "PCEngineEmulatorController.h"

namespace DCE
{

  PCEngineEmulatorController::PCEngineEmulatorController(Game_Player *pGame_Player, PCEngineEmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  PCEngineEmulatorController::~PCEngineEmulatorController()
  {
  
  }

}
