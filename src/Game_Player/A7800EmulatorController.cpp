/**
 * A7800EmulatorController - Support for the Atari 7800
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "A7800EmulatorController.h"

namespace DCE
{

  A7800EmulatorController::A7800EmulatorController(Game_Player *pGame_Player, A7800EmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  A7800EmulatorController::~A7800EmulatorController()
  {
  
  }

}
