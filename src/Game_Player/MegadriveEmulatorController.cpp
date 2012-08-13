/**
 * MegadriveEmulatorController - Support for the SEGA Megadrive
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "MegadriveEmulatorController.h"

namespace DCE
{

  MegadriveEmulatorController::MegadriveEmulatorController(Game_Player *pGame_Player, MegadriveEmulatorModel *pEmulatorModel)
    : GenesisEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  MegadriveEmulatorController::~MegadriveEmulatorController()
  {
  
  }

}
