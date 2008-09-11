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
class Table_GameSystem* tblGameSystem;
bool Commit_GameSystem(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Rom* tblRom;
bool Commit_Rom(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RomAttribute* tblRomAttribute;
bool Commit_RomAttribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RomAttributeType* tblRomAttributeType;
bool Commit_RomAttributeType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Rom_RomAttribute* tblRom_RomAttribute;
bool Commit_Rom_RomAttribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_GameSystem* GameSystem_get() { if( !tblGameSystem ) CreateTable_GameSystem(); return tblGameSystem; }
class Table_Rom* Rom_get() { if( !tblRom ) CreateTable_Rom(); return tblRom; }
class Table_RomAttribute* RomAttribute_get() { if( !tblRomAttribute ) CreateTable_RomAttribute(); return tblRomAttribute; }
class Table_RomAttributeType* RomAttributeType_get() { if( !tblRomAttributeType ) CreateTable_RomAttributeType(); return tblRomAttributeType; }
class Table_Rom_RomAttribute* Rom_RomAttribute_get() { if( !tblRom_RomAttribute ) CreateTable_Rom_RomAttribute(); return tblRom_RomAttribute; }
string m_sLastDBError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_GameSystem();
void CreateTable_Rom();
void CreateTable_RomAttribute();
void CreateTable_RomAttributeType();
void CreateTable_Rom_RomAttribute();
void DeleteTable_GameSystem();
void DeleteTable_Rom();
void DeleteTable_RomAttribute();
void DeleteTable_RomAttributeType();
void DeleteTable_Rom_RomAttribute();
};
#endif
