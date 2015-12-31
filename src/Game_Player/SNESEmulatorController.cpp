/**
 * SNESEmulatorController - Support for the Super Nintendo Entertainment System
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "SNESEmulatorController.h"

namespace DCE
{

  SNESEmulatorController::SNESEmulatorController(Game_Player *pGame_Player, SNESEmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  SNESEmulatorController::~SNESEmulatorController()
  {
  
  }

}
