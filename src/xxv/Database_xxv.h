#ifndef __Database_xxv_H_
#define __Database_xxv_H_
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

class DECLSPECIFIER Database_xxv: public DBHelper
{
public:
Database_xxv(Logger *pLogger=NULL);
~Database_xxv();
void DeleteAllTables();
private:
class Table_AUTOTIMER* tblAUTOTIMER;
bool Commit_AUTOTIMER(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CHANNELGROUPS* tblCHANNELGROUPS;
bool Commit_CHANNELGROUPS(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CHANNELS* tblCHANNELS;
bool Commit_CHANNELS(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CHRONICLE* tblCHRONICLE;
bool Commit_CHRONICLE(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_EPG* tblEPG;
bool Commit_EPG(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MUSIC* tblMUSIC;
bool Commit_MUSIC(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_OLDEPG* tblOLDEPG;
bool Commit_OLDEPG(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RECORDS* tblRECORDS;
bool Commit_RECORDS(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_TIMERS* tblTIMERS;
bool Commit_TIMERS(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_USER* tblUSER;
bool Commit_USER(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_VERSION* tblVERSION;
bool Commit_VERSION(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_AUTOTIMER* AUTOTIMER_get() { if( !tblAUTOTIMER ) CreateTable_AUTOTIMER(); return tblAUTOTIMER; }
class Table_CHANNELGROUPS* CHANNELGROUPS_get() { if( !tblCHANNELGROUPS ) CreateTable_CHANNELGROUPS(); return tblCHANNELGROUPS; }
class Table_CHANNELS* CHANNELS_get() { if( !tblCHANNELS ) CreateTable_CHANNELS(); return tblCHANNELS; }
class Table_CHRONICLE* CHRONICLE_get() { if( !tblCHRONICLE ) CreateTable_CHRONICLE(); return tblCHRONICLE; }
class Table_EPG* EPG_get() { if( !tblEPG ) CreateTable_EPG(); return tblEPG; }
class Table_MUSIC* MUSIC_get() { if( !tblMUSIC ) CreateTable_MUSIC(); return tblMUSIC; }
class Table_OLDEPG* OLDEPG_get() { if( !tblOLDEPG ) CreateTable_OLDEPG(); return tblOLDEPG; }
class Table_RECORDS* RECORDS_get() { if( !tblRECORDS ) CreateTable_RECORDS(); return tblRECORDS; }
class Table_TIMERS* TIMERS_get() { if( !tblTIMERS ) CreateTable_TIMERS(); return tblTIMERS; }
class Table_USER* USER_get() { if( !tblUSER ) CreateTable_USER(); return tblUSER; }
class Table_VERSION* VERSION_get() { if( !tblVERSION ) CreateTable_VERSION(); return tblVERSION; }
string m_sLastDBError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_AUTOTIMER();
void CreateTable_CHANNELGROUPS();
void CreateTable_CHANNELS();
void CreateTable_CHRONICLE();
void CreateTable_EPG();
void CreateTable_MUSIC();
void CreateTable_OLDEPG();
void CreateTable_RECORDS();
void CreateTable_TIMERS();
void CreateTable_USER();
void CreateTable_VERSION();
void DeleteTable_AUTOTIMER();
void DeleteTable_CHANNELGROUPS();
void DeleteTable_CHANNELS();
void DeleteTable_CHRONICLE();
void DeleteTable_EPG();
void DeleteTable_MUSIC();
void DeleteTable_OLDEPG();
void DeleteTable_RECORDS();
void DeleteTable_TIMERS();
void DeleteTable_USER();
void DeleteTable_VERSION();
};
#endif
