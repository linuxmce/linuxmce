/**
 * EmulatorController - A base class to implement a controller
 * to control emulation engines.
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "EmulatorController.h"

namespace DCE
{

  EmulatorController::EmulatorController(Game_Player *pGame_Player)
  {
    m_pGame_Player = pGame_Player;
  }

  EmulatorController::~EmulatorController()
  {
    delete m_pGame_Player;
    m_pGame_Player = NULL;
  }

  void EmulatorController::setEmulatorModel(EmulatorModel *pEmulatorModel)
  {
    m_pEmulatorModel = pEmulatorModel;
  }
  
  /** This just sets state to running. */
  bool EmulatorController::run()
  {
    if (m_pEmulatorModel)
      m_pEmulatorModel->m_bRunning_set(true);
    return true;
  }

  /** Just sets the state to not running. */
  bool EmulatorController::stop()
  {
    if (m_pEmulatorModel)
      m_pEmulatorModel->m_bRunning_set(false);
    return true;
  }

}
