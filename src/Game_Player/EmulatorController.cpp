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

  EmulatorController::EmulatorController(Game_Player *pGame_Player, EmulatorModel *pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  EmulatorController::~EmulatorController()
  {
    delete m_pGame_Player;
    m_pGame_Player = NULL;
    delete m_pEmulatorModel;
    m_pEmulatorModel = NULL;
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

  bool EmulatorController::P1Start()
  {
    return doAction("P1_START");
  }

  bool EmulatorController::P2Start()
  {
    return doAction("P2_START");
  }

  bool EmulatorController::P3Start()
  {
    return doAction("P3_START");
  }

  bool EmulatorController::P4Start()
  {
    return doAction("P4_START");
  }

  bool EmulatorController::coin1()
  {
    return doAction("COIN1");
  }

  bool EmulatorController::coin2()
  {
    return doAction("COIN2");
  }

  bool EmulatorController::pause()
  {
    return doAction("PAUSE");
  }

  bool EmulatorController::unpause()
  {
    return doAction("UNPAUSE");
  }

  bool EmulatorController::uiUp()
  {
    return doAction("UI_UP");
  }

  bool EmulatorController::uiDown()
  {
    return doAction("UI_DOWN");
  }

  bool EmulatorController::uiLeft()
  {
    return doAction("UI_LEFT");
  }

  bool EmulatorController::uiRight()
  {
    return doAction("UI_RIGHT");
  }

  bool EmulatorController::uiOK()
  {
    return doAction("UI_OK");
  }

  bool EmulatorController::press0()
  {
    return doAction("0");
  }

  bool EmulatorController::press1()
  {
    return doAction("1");
  }

  bool EmulatorController::press2()
  {
    return doAction("2");
  }

  bool EmulatorController::press3()
  {
    return doAction("3");
  }

  bool EmulatorController::press4()
  {
    return doAction("4");
  }

  bool EmulatorController::press5()
  {
    return doAction("5");
  }

  bool EmulatorController::press6()
  {
    return doAction("6");
  }

  bool EmulatorController::press7()
  {
    return doAction("7");
  }

  bool EmulatorController::press8()
  {
    return doAction("8");
  }

  bool EmulatorController::press9()
  {
    return doAction("9");
  }

  bool EmulatorController::uiBack()
  {
    return doAction("UI_BACK");
  }

  bool EmulatorController::emulatorExited()
  {
    m_pEmulatorModel->m_bRunning_set(false);
    return true;
  }

  bool EmulatorController::service1()
  {
    return doAction("SERVICE1");
  }
  
  bool EmulatorController::service2()
  {
    return doAction("SERVICE2");
  }

  bool EmulatorController::start()
  {
    return doAction("SERVICE2");
  }
  
  bool EmulatorController::select()
  {
    return doAction("SELECT");
  }

  bool EmulatorController::option()
  {
    return doAction("OPTION");
  }

  bool EmulatorController::reset()
  {
    return doAction("RESET");
  }


}
