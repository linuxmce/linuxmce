/**
 * GenesisEmulatorController - Support for the SEGA Genesis
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "GenesisEmulatorController.h"

namespace DCE
{

  GenesisEmulatorController::GenesisEmulatorController(Game_Player *pGame_Player, GenesisEmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  GenesisEmulatorController::~GenesisEmulatorController()
  {
  
  }

}
