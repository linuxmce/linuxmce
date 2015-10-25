/**
 * Apple2EmulatorController - Support for the Apple //e
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "Apple2EmulatorController.h"

namespace DCE
{

  Apple2EmulatorController::Apple2EmulatorController(Game_Player *pGame_Player, Apple2EmulatorModel *pEmulatorModel)
    : MAMEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
    m_pEmulatorModel->m_sSystemName="apple2e";
  }

  Apple2EmulatorController::~Apple2EmulatorController()
  {
  
  }

/*  string Apple2EmulatorController::getRomPathFromSlot()
  {
    string sMedia;
    if (!this->getMediaInSlot(sMedia,"flop1"))
      {
	return "";
      }
    else
      {
	return FileUtils::BasePath(sMedia);
      }
  } */

  void Apple2EmulatorController::insertMediaNamed(string sMediaFile, string sSlot)
  {
    EmulatorController::insertMediaNamed(sMediaFile,"flop1");
  }

}
