/** 
 * lmce_game_utils - CRUD operations for ROM entries in lmce_game
 *
 * Author:        Thomas Cherryhomes
 * Version:       1.0
 *
 */

#include <string>
#include <map>
#include <iostream>
#include <algorithm>

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

class LMCE_Game_Utils
{
  /* ctor and dtor */
 public:
  string m_sRomFilename;
  string m_sRomMD5;
  string m_sRomDescription;
  string m_sRomManufacturer;
  string m_sRomYear;
  string m_sRomGenre;
  string m_sRomSystem;
  bool m_bVerbose;

  enum eOperation { OPERATION_NONE=0, OPERATION_UPDATE=1, OPERATION_DELETE=2 } m_eOperation;
  
  LMCE_Game_Utils();
  virtual ~LMCE_Game_Utils();
  
  bool updateRom(); 
  bool deleteRom();
  bool findMatchingRom();
  bool Run(int &iRetValue);

 private:

  Database_lmce_game *m_pDatabase;

  bool sanityCheck();
  bool checkNoption();
  bool checkFoption();
  void ClearVars();
  void DumpInstanceVars();
  bool OpenDatabase();
  Row_Rom* RomExists();
  void parseCoweringTags();

  // Convenience Methods for grabbing lmce_game data.
  string GetRomAttribute(Row_Rom *pRow_Rom, int iFK_RomAttributeType);
  string GetRomDescription(Row_Rom *pRow_Rom);
  string GetRomManufacturer(Row_Rom *pRow_Rom);
  string GetRomGenre(Row_Rom *pRow_Rom);
  string GetRomYear(Row_Rom *pRow_Rom);
  string GetRomSystem(Row_Rom *pRow_Rom);

  // Return a FK_GameSystem value
  int GetPK_GameSystem(string sGameSystem);

  bool UpdateRomAttribute(Row_Rom *pRow_Rom, int iRomAttributeType, string sValue);

  // trim functions
  // trim from both ends
  static inline std::string &trim(std::string &s) 
  {
    return ltrim(rtrim(s));
  }
  
  // trim from start
  static inline std::string &ltrim(std::string &s) 
  {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
  }
  
  // trim from end
  static inline std::string &rtrim(std::string &s) 
  {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }

};
