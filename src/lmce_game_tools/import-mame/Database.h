/**
 * Database - the class to talk to the sql2cpp stuff.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "lmce_game/Database_lmce_game.h"
#include "lmce_game/Define_Configuration.h"
#include "lmce_game/Define_Game_GameSystem.h"
#include "lmce_game/Define_Game_GameSystem_Picture.h"
#include "lmce_game/Define_Game_GameSystem_Rom_Configuration.h"
#include "lmce_game/Define_Game_GameSystem_Rom.h"
#include "lmce_game/Define_Game.h"
#include "lmce_game/Define_GameSystem.h"
#include "lmce_game/Define_GameSystem_Picture.h"
#include "lmce_game/Define_Genre.h"
#include "lmce_game/Define_Genre_NameHash.h"
#include "lmce_game/Define_NameHash.h"
#include "lmce_game/Define_Picture.h"
#include "lmce_game/Define_Rom.h"
#include "lmce_game/Table_Configuration.h"
#include "lmce_game/Table_Game_GameSystem_Configuration.h"
#include "lmce_game/Table_Game_GameSystem.h"
#include "lmce_game/Table_Game_GameSystem_Picture.h"
#include "lmce_game/Table_Game_GameSystem_Rom_Configuration.h"
#include "lmce_game/Table_Game_GameSystem_Rom.h"
#include "lmce_game/Table_Game.h"
#include "lmce_game/Table_GameSystem.h"
#include "lmce_game/Table_GameSystem_Picture.h"
#include "lmce_game/Table_Genre.h"
#include "lmce_game/Table_Genre_NameHash.h"
#include "lmce_game/Table_Manufacturer.h"
#include "lmce_game/Table_Manufacturer_NameHash.h"
#include "lmce_game/Table_NameHash.h"
#include "lmce_game/Table_Picture.h"
#include "lmce_game/Table_Rom.h"
#include "lmce_game/TableRow.h"

#include "DCE/Logger.h"
#include "PlutoUtils/DBHelper.h"

#include "MAMEMachine.h"

class Database
{
 public:
  Database();
  virtual ~Database();
  bool m_bInitialized;
  bool Init();
  bool ProcessMachine(MAMEMachine* m);

 private:
  Database_lmce_game *m_pDatabase;
  bool NameHashExists(MAMEMachine* m);
  long int AddNameHash(MAMEMachine* m);
};

#endif
