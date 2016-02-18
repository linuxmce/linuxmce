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
#include "lmce_game/Table_Manufacturer.h"
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
  long int GetPKNameHash(MAMEMachine *m);
  bool GameExists(MAMEMachine* m);
  long int AddGame(MAMEMachine *m);
  long int GetPKGame(MAMEMachine *m);
  bool ManufacturerNameHashExists(MAMEMachine* m);
  long int AddManufacturerNameHash(MAMEMachine *m);
  long int GetManufacturerPKNameHash(MAMEMachine *m);
  bool ManufacturerExists(MAMEMachine* m);
  long int AddManufacturer(MAMEMachine *m);
  long int GetPKManufacturer(MAMEMachine *m);
  bool GenreNameHashExists(MAMEMachine* m);
  long int AddGenreNameHash(MAMEMachine *m);
  long int GetGenrePKNameHash(MAMEMachine *m);
  bool GenreExists(MAMEMachine* m);
  long int AddGenre(MAMEMachine *m);
  long int GetPKGenre(MAMEMachine *m);
  bool GameGameSystemExists(MAMEMachine* m);
  long int AddGameGameSystem(MAMEMachine *m);
  long int GetPKGameGameSystem(MAMEMachine *m);


};

#endif
