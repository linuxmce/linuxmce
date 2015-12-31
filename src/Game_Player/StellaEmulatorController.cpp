/**
 * StellaEmulatorController - Support to control the Stella emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "StellaEmulatorController.h"
#include "PlutoUtils/FileUtils.h"

namespace DCE
{
  StellaEmulatorController::StellaEmulatorController(Game_Player *pGame_Player, StellaEmulatorModel *pEmulatorModel)
    : INotifyEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
    m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/stella";
    m_pEmulatorModel->m_sProcessName="stella";
    m_pEmulatorModel->m_bChangeRequiresRestart=true;
    m_pEmulatorModel->m_bRunning=false;
    m_pEmulatorModel->m_bHasArgs=true;
    m_pEmulatorModel->m_sArgs="";
  }

  StellaEmulatorController::~StellaEmulatorController()
  {
   
  }

  string StellaEmulatorController::getRomFromSlot()
  {
    string sMedia;
    if (getMediaInSlot(sMedia))
      {
	if (!FileUtils::FileExists(sMedia))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::run() - could not find media in default slot: %s",sMedia.c_str());
	    return "";
	  }
      }
    else
      {
	return "";
      }
    return sMedia;
  }

  bool StellaEmulatorController::init()
  {
    return INotifyEmulatorController::init();
  }

  bool StellaEmulatorController::run()
  {
    m_pEmulatorModel->m_sArgs = getRomFromSlot();
    return INotifyEmulatorController::run();
  }


  string StellaEmulatorController::getSaveStateFromSlot()
  {
    return "";
  }

  // Since we have discrete pause, undo the default toggle behavior.
  bool StellaEmulatorController::pause()
  {
    return doAction("PAUSE");
  }

  bool StellaEmulatorController::unpause()
  {
    return doAction("UNPAUSE");
  }

  bool StellaEmulatorController::setSpeed(int iSpeed)
  {
    switch (iSpeed)
      {
      case 0:
	pause();
	break;
      case 1000:
	unpause();
	break;
      default:
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"StellaEmulatorController:setSpeed(%d) not implemented. Ignoring.");
	break;
      }
    return EmulatorController::setSpeed(iSpeed);
  }

  bool StellaEmulatorController::pressButton(int iPK_Button, Message *pMessage)
  {
    switch (iPK_Button)
      {
      case 32:
	doKeypadAction("*",pMessage);;
	break;
      case 33:
	doKeypadAction("#",pMessage);
	break;
      }
    return true;
  }

  bool StellaEmulatorController::doKeypadAction(string sKey, Message *pMessage)
  {
    string kp, sAction;
    getOption("KP",kp,pMessage);

    LoggerWrapper::GetInstance()->Write(LV_WARNING,"StellaEmulatorController::doKeypadAction() - kp is %s",kp.c_str());
    if (kp=="")
      kp="1";

    sAction = "KEYPAD"+kp+"_"+sKey;

    LoggerWrapper::GetInstance()->Write(LV_WARNING,"StellaEmulatorController::doKeypadAction() - Sending Action: %s",sAction.c_str());
    return doAction(sAction);
  }

  bool StellaEmulatorController::press0(Message *pMessage)
  {
    return doKeypadAction("0",pMessage);
  }

  bool StellaEmulatorController::press1(Message *pMessage)
  {
    return doKeypadAction("1",pMessage);
  }
  bool StellaEmulatorController::press2(Message *pMessage)
  {
    return doKeypadAction("2",pMessage);
  }
  bool StellaEmulatorController::press3(Message *pMessage)
  {
    return doKeypadAction("3",pMessage);
  }
  bool StellaEmulatorController::press4(Message *pMessage)
  {
    return doKeypadAction("4",pMessage);
  }
  bool StellaEmulatorController::press5(Message *pMessage)
  {
    return doKeypadAction("5",pMessage);
  }
  bool StellaEmulatorController::press6(Message *pMessage)
  {
    return doKeypadAction("6",pMessage);
  }
  bool StellaEmulatorController::press7(Message *pMessage)
  {
    return doKeypadAction("7",pMessage);
  }
  bool StellaEmulatorController::press8(Message *pMessage)
  {
    return doKeypadAction("8",pMessage);
  }
  bool StellaEmulatorController::press9(Message *pMessage)
  {
    return doKeypadAction("9",pMessage);
  }


}
