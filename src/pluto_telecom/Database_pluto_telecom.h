#ifndef __Database_pluto_telecom_H_
#define __Database_pluto_telecom_H_
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

class DECLSPECIFIER Database_pluto_telecom: public DBHelper
{
public:
Database_pluto_telecom(Logger *pLogger=NULL);
~Database_pluto_telecom();
void DeleteAllTables();
private:
class Table_CallersForMe* tblCallersForMe;
bool Commit_CallersForMe(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Contact* tblContact;
bool Commit_Contact(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Contact_Users_Sync* tblContact_Users_Sync;
bool Commit_Contact_Users_Sync(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_IncomingLog* tblIncomingLog;
bool Commit_IncomingLog(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Line_HouseMode* tblLine_HouseMode;
bool Commit_Line_HouseMode(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_OutgoingLog* tblOutgoingLog;
bool Commit_OutgoingLog(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PhoneNumber* tblPhoneNumber;
bool Commit_PhoneNumber(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PhoneNumber_Users_Sync* tblPhoneNumber_Users_Sync;
bool Commit_PhoneNumber_Users_Sync(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PhoneType* tblPhoneType;
bool Commit_PhoneType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PriorityCallers* tblPriorityCallers;
bool Commit_PriorityCallers(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_SpeedDial* tblSpeedDial;
bool Commit_SpeedDial(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_UserRouting* tblUserRouting;
bool Commit_UserRouting(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Users* tblUsers;
bool Commit_Users(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_telecom_batdet* tblpsc_telecom_batdet;
bool Commit_psc_telecom_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_telecom_bathdr* tblpsc_telecom_bathdr;
bool Commit_psc_telecom_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_telecom_batuser* tblpsc_telecom_batuser;
bool Commit_psc_telecom_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_telecom_repset* tblpsc_telecom_repset;
bool Commit_psc_telecom_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_telecom_schema* tblpsc_telecom_schema;
bool Commit_psc_telecom_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_telecom_tables* tblpsc_telecom_tables;
bool Commit_psc_telecom_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_CallersForMe* CallersForMe_get() { if( !tblCallersForMe ) CreateTable_CallersForMe(); return tblCallersForMe; }
class Table_Contact* Contact_get() { if( !tblContact ) CreateTable_Contact(); return tblContact; }
class Table_Contact_Users_Sync* Contact_Users_Sync_get() { if( !tblContact_Users_Sync ) CreateTable_Contact_Users_Sync(); return tblContact_Users_Sync; }
class Table_IncomingLog* IncomingLog_get() { if( !tblIncomingLog ) CreateTable_IncomingLog(); return tblIncomingLog; }
class Table_Line_HouseMode* Line_HouseMode_get() { if( !tblLine_HouseMode ) CreateTable_Line_HouseMode(); return tblLine_HouseMode; }
class Table_OutgoingLog* OutgoingLog_get() { if( !tblOutgoingLog ) CreateTable_OutgoingLog(); return tblOutgoingLog; }
class Table_PhoneNumber* PhoneNumber_get() { if( !tblPhoneNumber ) CreateTable_PhoneNumber(); return tblPhoneNumber; }
class Table_PhoneNumber_Users_Sync* PhoneNumber_Users_Sync_get() { if( !tblPhoneNumber_Users_Sync ) CreateTable_PhoneNumber_Users_Sync(); return tblPhoneNumber_Users_Sync; }
class Table_PhoneType* PhoneType_get() { if( !tblPhoneType ) CreateTable_PhoneType(); return tblPhoneType; }
class Table_PriorityCallers* PriorityCallers_get() { if( !tblPriorityCallers ) CreateTable_PriorityCallers(); return tblPriorityCallers; }
class Table_SpeedDial* SpeedDial_get() { if( !tblSpeedDial ) CreateTable_SpeedDial(); return tblSpeedDial; }
class Table_UserRouting* UserRouting_get() { if( !tblUserRouting ) CreateTable_UserRouting(); return tblUserRouting; }
class Table_Users* Users_get() { if( !tblUsers ) CreateTable_Users(); return tblUsers; }
class Table_psc_telecom_batdet* psc_telecom_batdet_get() { if( !tblpsc_telecom_batdet ) CreateTable_psc_telecom_batdet(); return tblpsc_telecom_batdet; }
class Table_psc_telecom_bathdr* psc_telecom_bathdr_get() { if( !tblpsc_telecom_bathdr ) CreateTable_psc_telecom_bathdr(); return tblpsc_telecom_bathdr; }
class Table_psc_telecom_batuser* psc_telecom_batuser_get() { if( !tblpsc_telecom_batuser ) CreateTable_psc_telecom_batuser(); return tblpsc_telecom_batuser; }
class Table_psc_telecom_repset* psc_telecom_repset_get() { if( !tblpsc_telecom_repset ) CreateTable_psc_telecom_repset(); return tblpsc_telecom_repset; }
class Table_psc_telecom_schema* psc_telecom_schema_get() { if( !tblpsc_telecom_schema ) CreateTable_psc_telecom_schema(); return tblpsc_telecom_schema; }
class Table_psc_telecom_tables* psc_telecom_tables_get() { if( !tblpsc_telecom_tables ) CreateTable_psc_telecom_tables(); return tblpsc_telecom_tables; }
string m_sLastDBError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_CallersForMe();
void CreateTable_Contact();
void CreateTable_Contact_Users_Sync();
void CreateTable_IncomingLog();
void CreateTable_Line_HouseMode();
void CreateTable_OutgoingLog();
void CreateTable_PhoneNumber();
void CreateTable_PhoneNumber_Users_Sync();
void CreateTable_PhoneType();
void CreateTable_PriorityCallers();
void CreateTable_SpeedDial();
void CreateTable_UserRouting();
void CreateTable_Users();
void CreateTable_psc_telecom_batdet();
void CreateTable_psc_telecom_bathdr();
void CreateTable_psc_telecom_batuser();
void CreateTable_psc_telecom_repset();
void CreateTable_psc_telecom_schema();
void CreateTable_psc_telecom_tables();
void DeleteTable_CallersForMe();
void DeleteTable_Contact();
void DeleteTable_Contact_Users_Sync();
void DeleteTable_IncomingLog();
void DeleteTable_Line_HouseMode();
void DeleteTable_OutgoingLog();
void DeleteTable_PhoneNumber();
void DeleteTable_PhoneNumber_Users_Sync();
void DeleteTable_PhoneType();
void DeleteTable_PriorityCallers();
void DeleteTable_SpeedDial();
void DeleteTable_UserRouting();
void DeleteTable_Users();
void DeleteTable_psc_telecom_batdet();
void DeleteTable_psc_telecom_bathdr();
void DeleteTable_psc_telecom_batuser();
void DeleteTable_psc_telecom_repset();
void DeleteTable_psc_telecom_schema();
void DeleteTable_psc_telecom_tables();
};
#endif
