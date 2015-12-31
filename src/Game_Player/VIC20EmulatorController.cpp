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
    string sFilepath = FileUtils::BasePath(sMediaFile);
    string sFilename = FileUtils::FilenameWithoutPath(sMediaFile);
    string sBaseName = FileUtils::FileWithoutExtension(sFilename);

    EmulatorController::insertMediaNamed(sMediaFile,"cartA");

    if (FileUtils::FileExists(sFilepath+"/"+sBaseName+".20"))
      {
	EmulatorController::insertMediaNamed(sFilepath+"/"+sBaseName+".20","cart2");
      }

    if (FileUtils::FileExists(sFilepath+"/"+sBaseName+".60"))
      {
	EmulatorController::insertMediaNamed(sFilepath+"/"+sBaseName+".60","cart6");
      }

  }

}
