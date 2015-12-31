/**
 * SGXEmulatorController - Support for the NEC SuperGrafx
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "SGXEmulatorController.h"

namespace DCE
{

  SGXEmulatorController::SGXEmulatorController(Game_Player *pGame_Player, SGXEmulatorModel *pEmulatorModel)
    : PCEngineEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  SGXEmulatorController::~SGXEmulatorController()
  {
  
  }

}
