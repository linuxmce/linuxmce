/**
 * NESEmulatorController - Support for the Nintendo Entertainment System
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "NESEmulatorController.h"

namespace DCE
{

  NESEmulatorController::NESEmulatorController(Game_Player *pGame_Player, NESEmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  NESEmulatorController::~NESEmulatorController()
  {
  
  }

}
