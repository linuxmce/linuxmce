/**
 * MAMEEmulatorController - Support to control the MAME emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "MAMEEmulatorController.h"

namespace DCE
{
  MAMEEmulatorController::MAMEEmulatorController(Game_Player *pGame_Player, MAMEEmulatorModel *pEmulatorModel)
    : X11EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  MAMEEmulatorController::~MAMEEmulatorController()
  {
   
  }

}
