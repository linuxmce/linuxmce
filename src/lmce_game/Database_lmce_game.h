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
class Table_GameSystem* tblGameSystem;
bool Commit_GameSystem(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Rom* tblRom;
bool Commit_Rom(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RomAttribute* tblRomAttribute;
bool Commit_RomAttribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RomAttributeType* tblRomAttributeType;
bool Commit_RomAttributeType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RomTitles* tblRomTitles;
bool Commit_RomTitles(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Rom_Configuration* tblRom_Configuration;
bool Commit_Rom_Configuration(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Rom_RomAttribute* tblRom_RomAttribute;
bool Commit_Rom_RomAttribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_game_batdet* tblpsc_game_batdet;
bool Commit_psc_game_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_game_bathdr* tblpsc_game_bathdr;
bool Commit_psc_game_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_game_batuser* tblpsc_game_batuser;
bool Commit_psc_game_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_game_repset* tblpsc_game_repset;
bool Commit_psc_game_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_game_schema* tblpsc_game_schema;
bool Commit_psc_game_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_game_tables* tblpsc_game_tables;
bool Commit_psc_game_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_Configuration* Configuration_get() { if( !tblConfiguration ) CreateTable_Configuration(); return tblConfiguration; }
class Table_GameSystem* GameSystem_get() { if( !tblGameSystem ) CreateTable_GameSystem(); return tblGameSystem; }
class Table_Rom* Rom_get() { if( !tblRom ) CreateTable_Rom(); return tblRom; }
class Table_RomAttribute* RomAttribute_get() { if( !tblRomAttribute ) CreateTable_RomAttribute(); return tblRomAttribute; }
class Table_RomAttributeType* RomAttributeType_get() { if( !tblRomAttributeType ) CreateTable_RomAttributeType(); return tblRomAttributeType; }
class Table_RomTitles* RomTitles_get() { if( !tblRomTitles ) CreateTable_RomTitles(); return tblRomTitles; }
class Table_Rom_Configuration* Rom_Configuration_get() { if( !tblRom_Configuration ) CreateTable_Rom_Configuration(); return tblRom_Configuration; }
class Table_Rom_RomAttribute* Rom_RomAttribute_get() { if( !tblRom_RomAttribute ) CreateTable_Rom_RomAttribute(); return tblRom_RomAttribute; }
class Table_psc_game_batdet* psc_game_batdet_get() { if( !tblpsc_game_batdet ) CreateTable_psc_game_batdet(); return tblpsc_game_batdet; }
class Table_psc_game_bathdr* psc_game_bathdr_get() { if( !tblpsc_game_bathdr ) CreateTable_psc_game_bathdr(); return tblpsc_game_bathdr; }
class Table_psc_game_batuser* psc_game_batuser_get() { if( !tblpsc_game_batuser ) CreateTable_psc_game_batuser(); return tblpsc_game_batuser; }
class Table_psc_game_repset* psc_game_repset_get() { if( !tblpsc_game_repset ) CreateTable_psc_game_repset(); return tblpsc_game_repset; }
class Table_psc_game_schema* psc_game_schema_get() { if( !tblpsc_game_schema ) CreateTable_psc_game_schema(); return tblpsc_game_schema; }
class Table_psc_game_tables* psc_game_tables_get() { if( !tblpsc_game_tables ) CreateTable_psc_game_tables(); return tblpsc_game_tables; }
string m_sLastDBError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_Configuration();
void CreateTable_GameSystem();
void CreateTable_Rom();
void CreateTable_RomAttribute();
void CreateTable_RomAttributeType();
void CreateTable_RomTitles();
void CreateTable_Rom_Configuration();
void CreateTable_Rom_RomAttribute();
void CreateTable_psc_game_batdet();
void CreateTable_psc_game_bathdr();
void CreateTable_psc_game_batuser();
void CreateTable_psc_game_repset();
void CreateTable_psc_game_schema();
void CreateTable_psc_game_tables();
void DeleteTable_Configuration();
void DeleteTable_GameSystem();
void DeleteTable_Rom();
void DeleteTable_RomAttribute();
void DeleteTable_RomAttributeType();
void DeleteTable_RomTitles();
void DeleteTable_Rom_Configuration();
void DeleteTable_Rom_RomAttribute();
void DeleteTable_psc_game_batdet();
void DeleteTable_psc_game_bathdr();
void DeleteTable_psc_game_batuser();
void DeleteTable_psc_game_repset();
void DeleteTable_psc_game_schema();
void DeleteTable_psc_game_tables();
};
#endif
