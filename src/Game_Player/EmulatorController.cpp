/**
 * EmulatorController - A base class to implement a controller
 * to control emulation engines.
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "EmulatorController.h"
#include "PlutoUtils/FileUtils.h"

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

  /**
   * Basic sanity checks run before run() or stop().
   */
  bool EmulatorController::sanityCheck(string sFuncName)
  {
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::%s() - m_pEmulatorModel is NULL. Bailing.",sFuncName.c_str());
	return false;
      }
    
    if (!m_pGame_Player)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::%s() - m_pGame_Player is NULL. Bailing.",sFuncName.c_str());
	return false;
      }
    return true; // sanity checks passed.
  }

  /** This just sets state to running. */
  bool EmulatorController::run()
  {
    if (!sanityCheck("run"))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::sanityCheck() - failed. Bailing.");
	return false;
      }
    if (m_pEmulatorModel->m_sEmulatorBinary.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::run() - m_sEmulatorBinary is empty. Bailing.");
	return false;
      }

    if (FileUtils::FileExists(m_pEmulatorModel->m_sEmulatorBinary))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::run() - Could not find emulator binary. Bailing.");
	return false;
      }

    if (m_pEmulatorModel->m_bHasArgs && m_pEmulatorModel->m_sArgs.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::run() - Emulator Specifies that args are required and arguments are empty. Bailing.");
	return false;
      }

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorController::run() - Launching %s with args %s",m_pEmulatorModel->m_sEmulatorBinary.c_str(),m_pEmulatorModel->m_sArgs.c_str());

    if (!m_pGame_Player->m_pDevice_App_Server)
      {
	// This should never happen, but I put this here just in case.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::run() - Unable to find a pointer to the nearest app server. Bailing.");
	return false;
      }

    if (m_pEmulatorModel->m_bChangeRequiresRestart)
      {
	EmulatorController::stop();
      }
    
    string sMessage = 
      StringUtils::itos(m_pGame_Player->m_dwPK_Device) + " " +
      StringUtils::itos(m_pGame_Player->m_dwPK_Device) +
      " 1 " TOSTRING (COMMAND_Application_Exited_CONST) " "
      TOSTRING (COMMANDPARAMETER_Exit_Code_CONST) " ";

    // Send the command back to the Game Player to launch the app ///////////////////////////////////////

    DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pGame_Player->m_dwPK_Device,
						     m_pGame_Player->m_pDevice_App_Server->m_dwPK_Device,
						     "emulator",
						     m_pEmulatorModel->m_sEmulatorBinary,
						     m_pEmulatorModel->m_sArgs,
						     sMessage + "1",
						     sMessage + "0",
						     false,
						     false,
						     true,
						     false);

    m_pGame_Player->SendCommand(CMD_Spawn_Application);

    LoggerWrapper::GetInstance()
      ->Write(LV_STATUS,"EmulatorController::run() - sent command back to game player to spawn application");

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // Any further processing should be taken care of by subclasses.

    m_pEmulatorModel->m_bRunning_set(true);
    return true;
  }

  /** Just sets the state to not running. */
  bool EmulatorController::stop()
  {
    if (!sanityCheck("stop"))
      {
	LoggerWrapper::GetInstance()
	  ->Write(LV_CRITICAL,"EmulatorController::stop() - Sanity checks failed.");
      }
    
    DCE::CMD_Kill_Application CMD_Kill_Application(m_pGame_Player->m_dwPK_Device,
						   m_pGame_Player->m_pDevice_App_Server->m_dwPK_Device,
						   "emulator",
						   false);
    
    m_pGame_Player->SendCommand(CMD_Kill_Application);

    m_pEmulatorModel->m_bRunning_set(false);

    // any further deinitialization should be handled in the subclasses.

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
