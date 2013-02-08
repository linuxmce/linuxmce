/**
 * IntellivisionEmulatorController - Support for the Intellivision
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "IntellivisionEmulatorController.h"

namespace DCE
{

  IntellivisionEmulatorController::IntellivisionEmulatorController(Game_Player *pGame_Player, IntellivisionEmulatorModel *pEmulatorModel)
    : SecondGenEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  IntellivisionEmulatorController::~IntellivisionEmulatorController()
  {
  
  }

}
