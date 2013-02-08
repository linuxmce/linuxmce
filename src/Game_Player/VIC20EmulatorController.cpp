/**
 * 
 * VIC20EmulatorController - VIC-20 Emulation Support
 * 
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "VIC20EmulatorController.h"

namespace DCE
{
  VIC20EmulatorController::VIC20EmulatorController(Game_Player *pGame_Player, VIC20EmulatorModel *pEmulatorModel)
    : VICEEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  VIC20EmulatorController::~VIC20EmulatorController()
  {
    
  }

  void VIC20EmulatorController::insertMediaNamed(string sMediaFile, string sSlot)
  {
    if (sMediaFile.find(".20") != string::npos)
      {
	sSlot = "cart2";
      }

    if (sMediaFile.find(".40") != string::npos)
      {
	sSlot = "cart4";
      }
    
    if (sMediaFile.find(".60") != string::npos)
      {
	sSlot = "cart6";
      }
    
    if (StringUtils::ToLower(sMediaFile).find(".a0") != string::npos)
      {
	sSlot = "cartA";
      }

    if (StringUtils::ToLower(sMediaFile).find(".b0") != string::npos)
      {
	sSlot = "cartB";
      }

    EmulatorController::insertMediaNamed(sMediaFile,sSlot);

  }

}
