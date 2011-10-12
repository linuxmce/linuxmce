/** 
 * EmulatorController - a base class providing a standardized way to 
 * control an emulator engine from the Game Player. 
 * 
 * Uses an EmulatorEngine model to provide state.
 * 
 */

#include "EmulatorController.h"

namespace DCE
{

  /** ctor */
  EmulatorController::EmulatorController(Game_Player *pGame_Player)
  {
    m_pGame_Player = pGame_Player;
    m_pEngine = NULL;
  }

  /** dtor */
  EmulatorController::~EmulatorController()
  {
    m_pGame_Player = NULL;
    m_pEngine = NULL;
  }

}
