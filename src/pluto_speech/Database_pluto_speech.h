#ifndef __Database_pluto_speech_H_
#define __Database_pluto_speech_H_
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

class DECLSPECIFIER Database_pluto_speech: public DBHelper
{
public:
Database_pluto_speech(Logger *pLogger=NULL);
~Database_pluto_speech();
void DeleteAllTables();
private:
class Table_Alias* tblAlias;
bool Commit_Alias(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Keyword* tblKeyword;
bool Commit_Keyword(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_Alias* Alias_get() { if( !tblAlias ) CreateTable_Alias(); return tblAlias; }
class Table_Keyword* Keyword_get() { if( !tblKeyword ) CreateTable_Keyword(); return tblKeyword; }
string m_sLastDBError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_Alias();
void CreateTable_Keyword();
void DeleteTable_Alias();
void DeleteTable_Keyword();
};
#endif
