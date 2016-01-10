/**
 * VectrexEmulatorController - Support for the GCE Vectrex
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "VectrexEmulatorController.h"

namespace DCE
{

  VectrexEmulatorController::VectrexEmulatorController(Game_Player *pGame_Player, VectrexEmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  VectrexEmulatorController::~VectrexEmulatorController()
  {
  
  }

}
