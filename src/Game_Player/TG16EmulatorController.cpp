/**
 * TG16EmulatorController - Support for the NEC TurboGrafx-16
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "TG16EmulatorController.h"

namespace DCE
{

  TG16EmulatorController::TG16EmulatorController(Game_Player *pGame_Player, TG16EmulatorModel *pEmulatorModel)
    : PCEngineEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  TG16EmulatorController::~TG16EmulatorController()
  {
  
  }

}
