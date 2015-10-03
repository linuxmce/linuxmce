/**
 * A2600EmulatorController - Support for the Atari 2600
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "A2600EmulatorController.h"

namespace DCE
{

  A2600EmulatorController::A2600EmulatorController(Game_Player *pGame_Player, A2600EmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  A2600EmulatorController::~A2600EmulatorController()
  {
  
  }

}
