/**
 * SecondGenEmulatorController - Support for Second Gen Game consoles (1979-1984)
 * 
 * These consoles all share some common characteristics, but namely, each of them had
 * a repurposed numeric keypad attached to each joystick to allow for more control
 * options. The idea was eventually dispensed with, but three consoles supported have
 * this characteristic:
 *
 * Intellivision (1979)
 * ColecoVision (1982)
 * Atari 5200 (1982)
 *
 * This model/controller pair is to help provide the numeric keypad characteristics
 * and to more easily compartmentalise those changes.
 *
 */
#include "SecondGenEmulatorController.h"

namespace DCE
{

  SecondGenEmulatorController::SecondGenEmulatorController(Game_Player *pGame_Player, SecondGenEmulatorModel *pEmulatorModel)
    : MESSEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  SecondGenEmulatorController::~SecondGenEmulatorController()
  {
  
  }

  bool SecondGenEmulatorController::pressButton(int iPK_Button, Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    int iPK_Button_Offset = 0;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	iPK_Button_Offset=SECONDGEN_P1;
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	iPK_Button_Offset=SECONDGEN_P2;
      }
    return X11EmulatorController::pressButton(iPK_Button+iPK_Button_Offset, pMessage);
  }

  bool SecondGenEmulatorController::press0(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("0");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_0");
      }
    return true;
  }

  bool SecondGenEmulatorController::press1(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("1");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_1");
      }
    return true;
  }

  bool SecondGenEmulatorController::press2(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("2");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_2");
      }
    return true;
  }

  bool SecondGenEmulatorController::press3(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("3");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_3");
      }
    return true;
  }

  bool SecondGenEmulatorController::press4(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("4");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_4");
      }
    return true;
  }

  bool SecondGenEmulatorController::press5(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("5");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_5");
      }
    return true;
  }

  bool SecondGenEmulatorController::press6(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("6");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_6");
      }
    return true;
  }

  bool SecondGenEmulatorController::press7(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("7");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_7");
      }
    return true;
  }

  bool SecondGenEmulatorController::press8(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("8");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_8");
      }
    return true;
  }

  bool SecondGenEmulatorController::press9(Message *pMessage)
  {
    long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
    if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P1)
      {
	return doAction("9");
      }
    else if (m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]==SECONDGEN_P2)
      {
	return doAction("P2_9");
      }
    return true;
  }

  void SecondGenEmulatorController::getOption(string sPath, string &sValue_To_Assign, Message *pMessage)
  {
    return EmulatorController::getOption(sPath, sValue_To_Assign,pMessage);
  }

  void SecondGenEmulatorController::setOption(string sPath, string sValue_To_Assign, Message *pMessage)
  {
    if (sPath=="KP")
      {
	long dwPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
	if (sValue_To_Assign=="P1")
	  {
	    m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]=SECONDGEN_P1;
	  }
	else if (sValue_To_Assign=="P2")
	  {
	    m_pEmulatorModel->m_mapOrbiterToKeypadPlayer[dwPK_Device_Orbiter]=SECONDGEN_P2;
	  }
      }
    return EmulatorController::setOption(sPath, sValue_To_Assign,pMessage);
  }

}
