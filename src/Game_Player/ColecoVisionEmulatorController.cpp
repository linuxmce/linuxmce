/**
 * ColecoVisionEmulatorController - Support for the ColecoVision
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "ColecoVisionEmulatorController.h"

namespace DCE
{

  ColecoVisionEmulatorController::ColecoVisionEmulatorController(Game_Player *pGame_Player, ColecoVisionEmulatorModel *pEmulatorModel)
    : SecondGenEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  ColecoVisionEmulatorController::~ColecoVisionEmulatorController()
  {
  
  }

}
