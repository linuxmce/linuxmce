#ifdef WIN32
	#include <WinSock2.h>
#endif
#include <iostream>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_pluto_telecom.h"


#include "DCEConfig.h"


using namespace DCE;
Database_pluto_telecom::Database_pluto_telecom(Logger *pLogger)
{
	LoggerWrapper::SetInstance(pLogger);
tblCallersForMe=NULL;
tblContact=NULL;
tblContact_Users_Sync=NULL;
tblIncomingLog=NULL;
tblLine_HouseMode=NULL;
tblOutgoingLog=NULL;
tblPhoneNumber=NULL;
tblPhoneNumber_Users_Sync=NULL;
tblPhoneType=NULL;
tblPriorityCallers=NULL;
tblSpeedDial=NULL;
tblUserRouting=NULL;
tblUsers=NULL;
tblpsc_telecom_batdet=NULL;
tblpsc_telecom_bathdr=NULL;
tblpsc_telecom_batuser=NULL;
tblpsc_telecom_repset=NULL;
tblpsc_telecom_schema=NULL;
tblpsc_telecom_tables=NULL;
}

Database_pluto_telecom::~Database_pluto_telecom()
{
	DeleteAllTables();
}

bool Database_pluto_telecom::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblCallersForMe!=NULL )
	if( !Commit_CallersForMe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblContact!=NULL )
	if( !Commit_Contact(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblContact_Users_Sync!=NULL )
	if( !Commit_Contact_Users_Sync(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblIncomingLog!=NULL )
	if( !Commit_IncomingLog(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLine_HouseMode!=NULL )
	if( !Commit_Line_HouseMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOutgoingLog!=NULL )
	if( !Commit_OutgoingLog(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPhoneNumber!=NULL )
	if( !Commit_PhoneNumber(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPhoneNumber_Users_Sync!=NULL )
	if( !Commit_PhoneNumber_Users_Sync(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPhoneType!=NULL )
	if( !Commit_PhoneType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPriorityCallers!=NULL )
	if( !Commit_PriorityCallers(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSpeedDial!=NULL )
	if( !Commit_SpeedDial(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUserRouting!=NULL )
	if( !Commit_UserRouting(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUsers!=NULL )
	if( !Commit_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_telecom_batdet!=NULL )
	if( !Commit_psc_telecom_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_telecom_bathdr!=NULL )
	if( !Commit_psc_telecom_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_telecom_batuser!=NULL )
	if( !Commit_psc_telecom_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_telecom_repset!=NULL )
	if( !Commit_psc_telecom_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_telecom_schema!=NULL )
	if( !Commit_psc_telecom_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_telecom_tables!=NULL )
	if( !Commit_psc_telecom_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_pluto_telecom::DeleteAllTables()
{
DeleteTable_CallersForMe();
DeleteTable_Contact();
DeleteTable_Contact_Users_Sync();
DeleteTable_IncomingLog();
DeleteTable_Line_HouseMode();
DeleteTable_OutgoingLog();
DeleteTable_PhoneNumber();
DeleteTable_PhoneNumber_Users_Sync();
DeleteTable_PhoneType();
DeleteTable_PriorityCallers();
DeleteTable_SpeedDial();
DeleteTable_UserRouting();
DeleteTable_Users();
DeleteTable_psc_telecom_batdet();
DeleteTable_psc_telecom_bathdr();
DeleteTable_psc_telecom_batuser();
DeleteTable_psc_telecom_repset();
DeleteTable_psc_telecom_schema();
DeleteTable_psc_telecom_tables();
}

bool Database_pluto_telecom::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_telecom::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
