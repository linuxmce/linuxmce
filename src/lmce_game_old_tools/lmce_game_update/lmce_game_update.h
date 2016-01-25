/**
 * lmce_game_update - Manipulate lmce_game database.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 2.0
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

class LMCE_Game_Update
{
 public:
  LMCE_Game_Update(string sFilename,
		   string sTitle,
		   string sSubtitle,
		   string sManufacturer,
		   string sYear,
		   string sGenre,
		   int iSystem);
  virtual ~LMCE_Game_Update();
  int Run();

 private:
  GameDatabase *m_pGameDatabase;
  string m_sFilename;
  string m_sTitle;
  string m_sSubtitle;
  string m_sManufacturer;
  string m_sYear;
  string m_sGenre;
  int m_iSystem;
  GameDatabase *m_pGameDatabase;

  bool MergeAttributesFromFile(string sFilename);
  bool UpdateRom();
  bool DeleteRom();

}
