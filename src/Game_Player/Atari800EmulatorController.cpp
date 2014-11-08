/**
 * Atari800EmulatorController - Support to control the Atari800 emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "Atari800EmulatorController.h"
#include "PlutoUtils/FileUtils.h"

namespace DCE
{
  Atari800EmulatorController::Atari800EmulatorController(Game_Player *pGame_Player, Atari800EmulatorModel *pEmulatorModel)
    : X11EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  Atari800EmulatorController::~Atari800EmulatorController()
  {
   
  }

  bool Atari800EmulatorController::init()
  {
    return X11EmulatorController::init();
  }

  void Atari800EmulatorController::insertMediaNamed(string sMediaFile, string sSlot)
  {
    string sMediaUpper = StringUtils::ToUpper(sMediaFile);
    if ((sMediaUpper.find(".ATR") != string::npos) ||
	(sMediaUpper.find(".XFD") != string::npos) ||
	(sMediaUpper.find(".DCM") != string::npos))
      {
	sSlot=""; // Atari800 doesn't use a switch for this one.
      }
    else if ((sMediaUpper.find(".CAR") != string::npos) ||
	     (sMediaUpper.find(".ROM") != string::npos))
      {
	sSlot="/cart";
      }
    else if ((sMediaUpper.find(".XEX") != string::npos))
      {
	sSlot="/run";
      }

    EmulatorController::insertMediaNamed(sMediaFile, sSlot);
  }

  string Atari800EmulatorController::getSlotsAndRoms()
  {
    string sRet = "";
    
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::getSlotsAndRoms() - no m_pEmulatorModel, bailing!");
	return "";
      }
    
    for (map<string,string>::iterator it=m_pEmulatorModel->m_mapMedia.begin(); it!=m_pEmulatorModel->m_mapMedia.end(); ++it)
      {
	string sSlot = it->first;
	string sMediaFile = it->second;
		
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Inserting %s into slot %s",sSlot.c_str(),sMediaFile.c_str());
	
	sRet += "-" + sSlot + "\t" +
	  sMediaFile;
      }

    return sRet;
  }

  bool Atari800EmulatorController::run()
  {
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800EmulatorController::run() - pEmulatorModel is null. Bailing.");
	return false;
      }

    m_pEmulatorModel->m_sArgs = "/opengl\t/f\t/portable\t";
    m_pEmulatorModel->m_sArgs = getSlotsAndRoms();

    if (!m_pEmulatorModel->m_sMediaPosition.empty() && !m_pEmulatorModel->m_bIsStreamingSource)
      {
	// Implement State loading if possible.
      }

    if (m_pEmulatorModel->m_bIsStreaming)
      {
	// Implement Streaming if possible.
	if (m_pEmulatorModel->m_bIsStreamingSource)
	  {
	    // This is the server, set args as needed.
	  }
	else
	  {
	    // This is the client, set appropriately.
	  }
      }
    else
      {
	// Regular Non-streaming instance
      }

    if (X11EmulatorController::run())
      {
        usleep(2000);
        m_pGame_Player->EVENT_Menu_Onscreen(m_pEmulatorModel->m_iStreamID,false);
        usleep(2000);
      	return true;
      }
    
    return false;
  }

  bool Atari800EmulatorController::stop()
  {
      X11EmulatorController::stop();

      // TODO: remember why the hell I removed this from the base class!
      m_pEmulatorModel->m_mapMedia.clear();
      return true;
  }

  bool Atari800EmulatorController::setSpeed(int iSpeed)
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
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Atari800EmulatorController:setSpeed(%d) not implemented. Ignoring.");
	break;
      }
    return EmulatorController::setSpeed(iSpeed);
  }

  bool Atari800EmulatorController::gotoMenu(int iMenu)
  {
    switch(iMenu)
      {
      case 0:
	if (m_pEmulatorModel->m_iActiveMenu > 1) // 2 and above is a UI of some sort.
	  doAction("UI_EXIT");
	break;
      case 1:
	doAction("MONITOR_MODE");
	break;
      case 2:
	doAction("UI_ENTER");
      }
    
    return X11EmulatorController::gotoMenu(iMenu); // and up the chain...
  }

  bool Atari800EmulatorController::saveState(string& sPosition, string& sText, bool bAutoSave, string sAutoSaveName)
  {
    return false;
  }

  void Atari800EmulatorController::setMediaPosition(string sMediaPosition)
  {
    EmulatorController::setMediaPosition(sMediaPosition);
    return;
  }
  
  bool Atari800EmulatorController::loadState(string sPosition)
  {
    return false;
  }
  
  bool Atari800EmulatorController::record()
  {
    // implement record.
    return false;
  }

}
