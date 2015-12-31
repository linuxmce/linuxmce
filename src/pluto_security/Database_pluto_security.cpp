#ifdef WIN32
	#include <WinSock2.h>
#endif
#include <iostream>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_pluto_security.h"


#include "DCEConfig.h"


using namespace DCE;
Database_pluto_security::Database_pluto_security(Logger *pLogger)
{
	LoggerWrapper::SetInstance(pLogger);
tblAlert=NULL;
tblAlertType=NULL;
tblAlert_Device=NULL;
tblModeChange=NULL;
tblNotification=NULL;
tblpsc_security_batdet=NULL;
tblpsc_security_bathdr=NULL;
tblpsc_security_batuser=NULL;
tblpsc_security_repset=NULL;
tblpsc_security_schema=NULL;
tblpsc_security_tables=NULL;
}

Database_pluto_security::~Database_pluto_security()
{
	DeleteAllTables();
}

bool Database_pluto_security::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblAlert!=NULL )
	if( !Commit_Alert(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblAlertType!=NULL )
	if( !Commit_AlertType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblAlert_Device!=NULL )
	if( !Commit_Alert_Device(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblModeChange!=NULL )
	if( !Commit_ModeChange(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblNotification!=NULL )
	if( !Commit_Notification(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_security_batdet!=NULL )
	if( !Commit_psc_security_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_security_bathdr!=NULL )
	if( !Commit_psc_security_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_security_batuser!=NULL )
	if( !Commit_psc_security_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_security_repset!=NULL )
	if( !Commit_psc_security_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_security_schema!=NULL )
	if( !Commit_psc_security_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_security_tables!=NULL )
	if( !Commit_psc_security_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_pluto_security::DeleteAllTables()
{
DeleteTable_Alert();
DeleteTable_AlertType();
DeleteTable_Alert_Device();
DeleteTable_ModeChange();
DeleteTable_Notification();
DeleteTable_psc_security_batdet();
DeleteTable_psc_security_bathdr();
DeleteTable_psc_security_batuser();
DeleteTable_psc_security_repset();
DeleteTable_psc_security_schema();
DeleteTable_psc_security_tables();
}

bool Database_pluto_security::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_security::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
