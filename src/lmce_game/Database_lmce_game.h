#ifndef __Database_lmce_game_H_
#define __Database_lmce_game_H_
#include "PlutoUtils/DBHelper.h"
#include "DCE/Logger.h"
#ifdef WIN32
#ifdef EXPORT_DLL
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllexport)
	#endif
#else
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllimport)
	#endif
#endif
#else
#define DECLSPECIFIER
#endif
class DECLSPECIFIER DBHelper;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Database_lmce_game: public DBHelper
{
public:
Database_lmce_game(Logger *pLogger=NULL);
~Database_lmce_game();
void DeleteAllTables();
private:
class Table_Configuration* tblConfiguration;
bool Commit_Configuration(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Game* tblGame;
bool Commit_Game(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_GameSystem* tblGameSystem;
bool Commit_GameSystem(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_GameSystem_Picture* tblGameSystem_Picture;
bool Commit_GameSystem_Picture(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Game_GameSystem* tblGame_GameSystem;
bool Commit_Game_GameSystem(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Game_GameSystem_Configuration* tblGame_GameSystem_Configuration;
bool Commit_Game_GameSystem_Configuration(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Game_GameSystem_Picture* tblGame_GameSystem_Picture;
bool Commit_Game_GameSystem_Picture(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Game_GameSystem_Rom* tblGame_GameSystem_Rom;
bool Commit_Game_GameSystem_Rom(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Game_GameSystem_Rom_Configuration* tblGame_GameSystem_Rom_Configuration;
bool Commit_Game_GameSystem_Rom_Configuration(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Genre* tblGenre;
bool Commit_Genre(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Manufacturer* tblManufacturer;
bool Commit_Manufacturer(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_NameHash* tblNameHash;
bool Commit_NameHash(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Picture* tblPicture;
bool Commit_Picture(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Rom* tblRom;
bool Commit_Rom(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_Configuration* Configuration_get() { if( !tblConfiguration ) CreateTable_Configuration(); return tblConfiguration; }
class Table_Game* Game_get() { if( !tblGame ) CreateTable_Game(); return tblGame; }
class Table_GameSystem* GameSystem_get() { if( !tblGameSystem ) CreateTable_GameSystem(); return tblGameSystem; }
class Table_GameSystem_Picture* GameSystem_Picture_get() { if( !tblGameSystem_Picture ) CreateTable_GameSystem_Picture(); return tblGameSystem_Picture; }
class Table_Game_GameSystem* Game_GameSystem_get() { if( !tblGame_GameSystem ) CreateTable_Game_GameSystem(); return tblGame_GameSystem; }
class Table_Game_GameSystem_Configuration* Game_GameSystem_Configuration_get() { if( !tblGame_GameSystem_Configuration ) CreateTable_Game_GameSystem_Configuration(); return tblGame_GameSystem_Configuration; }
class Table_Game_GameSystem_Picture* Game_GameSystem_Picture_get() { if( !tblGame_GameSystem_Picture ) CreateTable_Game_GameSystem_Picture(); return tblGame_GameSystem_Picture; }
class Table_Game_GameSystem_Rom* Game_GameSystem_Rom_get() { if( !tblGame_GameSystem_Rom ) CreateTable_Game_GameSystem_Rom(); return tblGame_GameSystem_Rom; }
class Table_Game_GameSystem_Rom_Configuration* Game_GameSystem_Rom_Configuration_get() { if( !tblGame_GameSystem_Rom_Configuration ) CreateTable_Game_GameSystem_Rom_Configuration(); return tblGame_GameSystem_Rom_Configuration; }
class Table_Genre* Genre_get() { if( !tblGenre ) CreateTable_Genre(); return tblGenre; }
class Table_Manufacturer* Manufacturer_get() { if( !tblManufacturer ) CreateTable_Manufacturer(); return tblManufacturer; }
class Table_NameHash* NameHash_get() { if( !tblNameHash ) CreateTable_NameHash(); return tblNameHash; }
class Table_Picture* Picture_get() { if( !tblPicture ) CreateTable_Picture(); return tblPicture; }
class Table_Rom* Rom_get() { if( !tblRom ) CreateTable_Rom(); return tblRom; }
string m_sLastDBError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_Configuration();
void CreateTable_Game();
void CreateTable_GameSystem();
void CreateTable_GameSystem_Picture();
void CreateTable_Game_GameSystem();
void CreateTable_Game_GameSystem_Configuration();
void CreateTable_Game_GameSystem_Picture();
void CreateTable_Game_GameSystem_Rom();
void CreateTable_Game_GameSystem_Rom_Configuration();
void CreateTable_Genre();
void CreateTable_Manufacturer();
void CreateTable_NameHash();
void CreateTable_Picture();
void CreateTable_Rom();
void DeleteTable_Configuration();
void DeleteTable_Game();
void DeleteTable_GameSystem();
void DeleteTable_GameSystem_Picture();
void DeleteTable_Game_GameSystem();
void DeleteTable_Game_GameSystem_Configuration();
void DeleteTable_Game_GameSystem_Picture();
void DeleteTable_Game_GameSystem_Rom();
void DeleteTable_Game_GameSystem_Rom_Configuration();
void DeleteTable_Genre();
void DeleteTable_Manufacturer();
void DeleteTable_NameHash();
void DeleteTable_Picture();
void DeleteTable_Rom();
};
#endif
