/**
 * lmce_game_mame_prune - A little utility to get rid of unwanted roms
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#include <string>
#include <map>
#include <iostream>

/* sql2cpp classes */
#include "lmce_game/Database_lmce_game.h"  
#include "lmce_game/Define_RomAttributeType.h"
#include "lmce_game/Define_Rom_RomAttribute.h"
#include "lmce_game/TableRow.h"
#include "lmce_game/Table_RomAttributeType.h"  
#include "lmce_game/Table_Rom_RomAttribute.h"
#include "lmce_game/Define_RomAttribute.h"   
#include "lmce_game/Define_Rom.h" 
#include "lmce_game/Define_GameSystem.h"      
#include "lmce_game/Table_RomAttribute.h"  
#include "lmce_game/Table_Rom.h"           
#include "lmce_game/Table_GameSystem.h"

#include "DCE/Logger.h"
#include "PlutoUtils/DBHelper.h"
#include "GameDatabase.h"
#include "MAMEParser.h"
#include "MAMECategory.h"

class LMCE_Game_Mame_Prune
{
 public:
  LMCE_Game_Mame_Prune(bool bDryRun, string sMamePath, string sRomPath, string sCategoryPath);
  virtual ~LMCE_Game_Mame_Prune();
  int Run();

 private:
  GameDatabase *m_pGameDatabase;
  MAMECategory *m_pMAMECategory;
  string m_sMamePath;
  string m_sRomPath;
  string m_sCategoryPath;
  int m_iDeletedRoms;
  int m_iNotDeletedRoms;
  bool m_bDryRun;
  bool IsUnwantedCategory(string sCategory);
  bool GetMAMEOutput(string &sMameOutput);
  void ProcessEntry(string sRomName, MAMERom *pCurrentRom);
};
