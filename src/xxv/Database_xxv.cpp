#ifdef WIN32
	#include <winsock.h>
#endif
#include <iostream>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_xxv.h"


#include "DCEConfig.h"


using namespace DCE;
Database_xxv::Database_xxv(Logger *pLogger)
{
	LoggerWrapper::SetInstance(pLogger);
tblAUTOTIMER=NULL;
tblCHANNELGROUPS=NULL;
tblCHANNELS=NULL;
tblCHRONICLE=NULL;
tblEPG=NULL;
tblMUSIC=NULL;
tblOLDEPG=NULL;
tblRECORDS=NULL;
tblTIMERS=NULL;
tblUSER=NULL;
tblVERSION=NULL;
}

Database_xxv::~Database_xxv()
{
	DeleteAllTables();
}

bool Database_xxv::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblAUTOTIMER!=NULL )
	if( !Commit_AUTOTIMER(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCHANNELGROUPS!=NULL )
	if( !Commit_CHANNELGROUPS(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCHANNELS!=NULL )
	if( !Commit_CHANNELS(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCHRONICLE!=NULL )
	if( !Commit_CHRONICLE(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEPG!=NULL )
	if( !Commit_EPG(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMUSIC!=NULL )
	if( !Commit_MUSIC(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOLDEPG!=NULL )
	if( !Commit_OLDEPG(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRECORDS!=NULL )
	if( !Commit_RECORDS(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTIMERS!=NULL )
	if( !Commit_TIMERS(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUSER!=NULL )
	if( !Commit_USER(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVERSION!=NULL )
	if( !Commit_VERSION(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_xxv::DeleteAllTables()
{
DeleteTable_AUTOTIMER();
DeleteTable_CHANNELGROUPS();
DeleteTable_CHANNELS();
DeleteTable_CHRONICLE();
DeleteTable_EPG();
DeleteTable_MUSIC();
DeleteTable_OLDEPG();
DeleteTable_RECORDS();
DeleteTable_TIMERS();
DeleteTable_USER();
DeleteTable_VERSION();
}

bool Database_xxv::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_xxv::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
