/** 
 * VICEEmulatorController - Support for VICE based Emulators
 *
 * Author: Thomas Cherryhomes <thom.cccherryhomes@gmail.com>
 *
 */

#include "VICEEmulatorController.h"
#include "PlutoUtils/FileUtils.h"

namespace DCE
{
  VICEEmulatorController::VICEEmulatorController(Game_Player *pGame_Player, VICEEmulatorModel *pEmulatorModel)
    : X11EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  VICEEmulatorController::~VICEEmulatorController()
  {
  
  }

  bool VICEEmulatorController::init()
  {
    return X11EmulatorController::init();
  }

  void VICEEmulatorController::insertMediaNamed(string sMediaFile, string sSlot)
  {
    EmulatorController::insertMediaNamed(sMediaFile, sSlot);
  }

  string VICEEmulatorController::getSlotsAndRoms()
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

  bool VICEEmulatorController::run()
  {
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VICEEmulatorController::run() - pEmulatorModel is null. Bailing.");
	return false;
      }

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
      return true;
    
    return false;

  }

  bool VICEEmulatorController::setSpeed(int iSpeed)
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
	if (iSpeed > 1000)
	  {
	    // Implement Warp Speed here.
	  }
      }
    return EmulatorController::setSpeed(iSpeed);
  }
  
  bool VICEEmulatorController::gotoMenu(int iMenu)
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

  bool VICEEmulatorController::saveState(string& sPosition, string& sText, bool bAutoSave, string sAutoSaveName)
  {
    return false;
  }

  void VICEEmulatorController::setMediaPosition(string sMediaPosition)
  {
    EmulatorController::setMediaPosition(sMediaPosition);
    return;
  }

  bool VICEEmulatorController::loadState(string sPosition)
  {
    if (!m_pEmulatorModel->m_bIsStreaming)
      {
	// Implement State loading.
	return false;
      }
    return false;
  }

  bool VICEEmulatorController::record()
  {
    // Implement Record
    return false;
  }

}
