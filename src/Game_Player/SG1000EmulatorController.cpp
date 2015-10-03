/**
 * SG1000EmulatorController - Support for the Nintendo Entertainment System
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "SG1000EmulatorController.h"

namespace DCE
{

  SG1000EmulatorController::SG1000EmulatorController(Game_Player *pGame_Player, SG1000EmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  SG1000EmulatorController::~SG1000EmulatorController()
  {
  
  }

}
