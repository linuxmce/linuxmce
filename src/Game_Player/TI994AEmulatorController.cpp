/**
 * TI994AEmulatorController - Support for the Apple //e
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "TI994AEmulatorController.h"

namespace DCE
{

  TI994AEmulatorController::TI994AEmulatorController(Game_Player *pGame_Player, TI994AEmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  TI994AEmulatorController::~TI994AEmulatorController()
  {
  
  }

  string TI994AEmulatorController::getAdditionalOptions()
  {
    return "\t-autoboot_command\t\"\\r2\"";
  }

}
