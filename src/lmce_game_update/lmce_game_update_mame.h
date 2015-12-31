/**
 * lmce_game_update_mame - Update the lmce_game database with the output from a running sdlmame.
 * 
 * This will skip any existing ROM entries, only putting in ROM entries that do not already
 * exist.
 *
 * Author:     Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version:    2.0
 *
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
#include "MAMECategory.h"
#include "GameDatabase.h"
#include "MAMEParser.h"

class LMCE_Game_Update_MAME
{
  /* ctor and dtor */
 public:
  LMCE_Game_Update_MAME(string sMamePath, string sCategoryPath);
  virtual ~LMCE_Game_Update_MAME();
  int Run();

 private:
  GameDatabase *m_pGameDatabase;
  string m_sMamePath;
  string m_sCategoryPath;
  MAMECategory *m_pMAMECategory;

  bool GetMAMEOutput(string &sMameOutput);
  void ProcessEntry(string sRomName, MAMERom *pCurrentRom);
  bool AddRom(MAMERom *pCurrentRom, string sRomFile, string sRomName);
  bool UpdateRom(MAMERom *pCurrentRom, string sRomFile, string sRomName);

};
