/**
 * GameDatabase - A simple class to abstract sql2cpp calls to update the
 * game database.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
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
#include "lmce_game/Table_RomTitles.h"

#include "DCE/Logger.h"
#include "PlutoUtils/DBHelper.h"

class GameDatabase
{
 public:
  GameDatabase();
  virtual ~GameDatabase();
  bool Init();
  bool RomExists(string sRomName);
  long int AddRom(string sRomName);
  long int GetPK_Rom(string sRomName);
  bool AddRomAttribute(long int iPK_Rom, int iRomAttributeType, string sName);
  bool UpdateRomAttribute(long int iPK_Rom, int iRomAttributeType, string sName);
  bool AddTitleHash(string sTitle, string sTitleHash);
  bool GetTitleForHash(string sTitleHash, string &sTitle);

 private:
  Database_lmce_game *m_pDatabase;
  
};
