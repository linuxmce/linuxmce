#ifndef __Database_lmce_datalog_H_
#define __Database_lmce_datalog_H_
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

class DECLSPECIFIER Database_lmce_datalog: public DBHelper
{
public:
Database_lmce_datalog(Logger *pLogger=NULL);
~Database_lmce_datalog();
void DeleteAllTables();
private:
class Table_Datapoints* tblDatapoints;
bool Commit_Datapoints(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Unit* tblUnit;
bool Commit_Unit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_Datapoints* Datapoints_get() { if( !tblDatapoints ) CreateTable_Datapoints(); return tblDatapoints; }
class Table_Unit* Unit_get() { if( !tblUnit ) CreateTable_Unit(); return tblUnit; }
string m_sLastDBError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_Datapoints();
void CreateTable_Unit();
void DeleteTable_Datapoints();
void DeleteTable_Unit();
};
#endif
