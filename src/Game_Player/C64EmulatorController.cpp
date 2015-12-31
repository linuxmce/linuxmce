/**
 * 
 * C64EmulatorController - C-64 Emulation Support
 * 
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "C64EmulatorController.h"

namespace DCE
{
  C64EmulatorController::C64EmulatorController(Game_Player *pGame_Player, C64EmulatorModel *pEmulatorModel)
    : VICEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  C64EmulatorController::~C64EmulatorController()
  {
    
  }

  void C64EmulatorController::insertMediaNamed(string sMediaFile, string sSlot)
  {

    if (StringUtils::ToLower(sMediaFile).find(".crt") != string::npos)
      {
	sSlot = "cartcrt";
      }

    if (StringUtils::ToLower(sMediaFile).find(".d64") != string::npos)
      {
	sSlot = "autostart";
      }

    if (StringUtils::ToLower(sMediaFile).find(".g64") != string::npos)
      {
	sSlot = "autostart";
      }

    if (StringUtils::ToLower(sMediaFile).find(".t64") != string::npos)
      {
        sSlot = "autostart";	
      }

    EmulatorController::insertMediaNamed(sMediaFile,sSlot);

  }

}
