/**
 * lmce_game_update - Update the lmce_game database with the output from a running sdlmame.
 * 
 * This will skip any existing ROM entries, only putting in ROM entries that do not already
 * exist.
 *
 * Author:     Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version:    1.0
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

class LMCE_Game_Update
{
  /* ctor and dtor */
 public:
  LMCE_Game_Update(string sMamePath);
  virtual ~LMCE_Game_Update();
  int Run();

 private:
  Database_lmce_game *m_pMyDatabase;
  string m_sMamePath;
  vector<string> m_vRomNames;
  vector<string> m_vRomDescriptions;

  bool InitDatabase();
  bool RomExists(string sRomName);
  int AddRomm(string sRomName);  // Returns PK_Rom #
  bool AddRommAttribute(int iPK_Rom, int iRomAttributeType,string sName);
  bool ParseGameListFromMAME();
  bool GetMetadataForRom(string sRomName, string& sMetaData);
  
};
