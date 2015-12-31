/**
 * A5200EmulatorController - Support for the A5200
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "A5200EmulatorController.h"

namespace DCE
{

  A5200EmulatorController::A5200EmulatorController(Game_Player *pGame_Player, A5200EmulatorModel *pEmulatorModel)
    : SecondGenEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  A5200EmulatorController::~A5200EmulatorController()
  {
  
  }

}
