#ifndef __Database_pluto_security_H_
#define __Database_pluto_security_H_
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

class DECLSPECIFIER Database_pluto_security: public DBHelper
{
public:
Database_pluto_security(Logger *pLogger=NULL);
~Database_pluto_security();
void DeleteAllTables();
private:
class Table_Alert* tblAlert;
bool Commit_Alert(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_AlertType* tblAlertType;
bool Commit_AlertType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Alert_Device* tblAlert_Device;
bool Commit_Alert_Device(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_ModeChange* tblModeChange;
bool Commit_ModeChange(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Notification* tblNotification;
bool Commit_Notification(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_security_batdet* tblpsc_security_batdet;
bool Commit_psc_security_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_security_bathdr* tblpsc_security_bathdr;
bool Commit_psc_security_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_security_batuser* tblpsc_security_batuser;
bool Commit_psc_security_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_security_repset* tblpsc_security_repset;
bool Commit_psc_security_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_security_schema* tblpsc_security_schema;
bool Commit_psc_security_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_security_tables* tblpsc_security_tables;
bool Commit_psc_security_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_Alert* Alert_get() { if( !tblAlert ) CreateTable_Alert(); return tblAlert; }
class Table_AlertType* AlertType_get() { if( !tblAlertType ) CreateTable_AlertType(); return tblAlertType; }
class Table_Alert_Device* Alert_Device_get() { if( !tblAlert_Device ) CreateTable_Alert_Device(); return tblAlert_Device; }
class Table_ModeChange* ModeChange_get() { if( !tblModeChange ) CreateTable_ModeChange(); return tblModeChange; }
class Table_Notification* Notification_get() { if( !tblNotification ) CreateTable_Notification(); return tblNotification; }
class Table_psc_security_batdet* psc_security_batdet_get() { if( !tblpsc_security_batdet ) CreateTable_psc_security_batdet(); return tblpsc_security_batdet; }
class Table_psc_security_bathdr* psc_security_bathdr_get() { if( !tblpsc_security_bathdr ) CreateTable_psc_security_bathdr(); return tblpsc_security_bathdr; }
class Table_psc_security_batuser* psc_security_batuser_get() { if( !tblpsc_security_batuser ) CreateTable_psc_security_batuser(); return tblpsc_security_batuser; }
class Table_psc_security_repset* psc_security_repset_get() { if( !tblpsc_security_repset ) CreateTable_psc_security_repset(); return tblpsc_security_repset; }
class Table_psc_security_schema* psc_security_schema_get() { if( !tblpsc_security_schema ) CreateTable_psc_security_schema(); return tblpsc_security_schema; }
class Table_psc_security_tables* psc_security_tables_get() { if( !tblpsc_security_tables ) CreateTable_psc_security_tables(); return tblpsc_security_tables; }
string m_sLastDBError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_Alert();
void CreateTable_AlertType();
void CreateTable_Alert_Device();
void CreateTable_ModeChange();
void CreateTable_Notification();
void CreateTable_psc_security_batdet();
void CreateTable_psc_security_bathdr();
void CreateTable_psc_security_batuser();
void CreateTable_psc_security_repset();
void CreateTable_psc_security_schema();
void CreateTable_psc_security_tables();
void DeleteTable_Alert();
void DeleteTable_AlertType();
void DeleteTable_Alert_Device();
void DeleteTable_ModeChange();
void DeleteTable_Notification();
void DeleteTable_psc_security_batdet();
void DeleteTable_psc_security_bathdr();
void DeleteTable_psc_security_batuser();
void DeleteTable_psc_security_repset();
void DeleteTable_psc_security_schema();
void DeleteTable_psc_security_tables();
};
#endif
