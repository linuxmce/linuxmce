/**
 * FamicomEmulatorController - Support for the Nintendo Family Computer
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "FamicomEmulatorController.h"
#include "PlutoUtils/StringUtils.h"

namespace DCE
{

  FamicomEmulatorController::FamicomEmulatorController(Game_Player *pGame_Player, FamicomEmulatorModel *pEmulatorModel)
    : NESEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  FamicomEmulatorController::~FamicomEmulatorController()
  {
  
  }

  void FamicomEmulatorController::insertMediaNamed(string sMediaFile, string sSlot)
  {
    if (StringUtils::ToLower(sMediaFile).find(".fds") != string::npos)
      {
	m_pEmulatorModel->m_sSystemName="fds"; // grrr, #(%@#(% MESS!
	sSlot = "flop";
      }
    else if (StringUtils::ToLower(sMediaFile).find(".nes") != string::npos)
      {
	m_pEmulatorModel->m_sSystemName="famicom"; // grrrr...
	sSlot = "cart";
      }
    
    EmulatorController::insertMediaNamed(sMediaFile,sSlot);

  }

  string FamicomEmulatorController::getRomPathFromSlot()
  {
    string sMedia;
    if (!this->getMediaInSlot(sMedia))
      {
	// No media in cart slot, try floppy.
	if (!this->getMediaInSlot(sMedia,"flop"))
	  {
	    return "";
	  }
      }
    return FileUtils::BasePath(sMedia);
  }

}
