/**
 * SMSEmulatorController - Support for the SEGA Master System
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "SMSEmulatorController.h"

namespace DCE
{

  SMSEmulatorController::SMSEmulatorController(Game_Player *pGame_Player, SMSEmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  SMSEmulatorController::~SMSEmulatorController()
  {
  
  }

}
