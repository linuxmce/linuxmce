/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

// If using the thread logger, these generated classes create lots of activity
#ifdef NO_SQL_THREAD_LOG
#undef THREAD_LOG
#endif

#ifdef WIN32
#include <WinSock2.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
#include "PlutoUtils/StringUtils.h"
#include "Table_PnpQueue.h"
#include "Table_CommMethod.h"
#include "Table_PnpProtocol.h"
#include "Table_DeviceTemplate.h"
#include "Table_Device.h"
#include "Table_Device.h"



void Database_pluto_main::CreateTable_PnpQueue()
{
	tblPnpQueue = new Table_PnpQueue(this);
}

void Database_pluto_main::DeleteTable_PnpQueue()
{
	if( tblPnpQueue )
		delete tblPnpQueue;
}

bool Database_pluto_main::Commit_PnpQueue(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblPnpQueue->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_PnpQueue::~Table_PnpQueue()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_PnpQueue *pRow = (Row_PnpQueue *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_PnpQueue *pRow = (Row_PnpQueue *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_PnpQueue::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_PnpQueue *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_PnpQueue *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_PnpQueue);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_PnpQueue::Reload()
{
	Row_PnpQueue *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_PnpQueue);
		Row_PnpQueue *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_PnpQueue::Row_PnpQueue(Table_PnpQueue *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_PnpQueue::SetDefaultValues()
{
	m_PK_PnpQueue = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
m_FK_CommMethod = 0;
is_null[9] = true;
m_FK_PnpProtocol = 0;
is_null[10] = true;
m_FK_DeviceTemplate = 0;
is_null[11] = true;
m_FK_Device_Created = 0;
is_null[12] = true;
m_FK_Device_Reported = 0;
is_null[13] = true;
is_null[14] = true;
m_Removed = 0;
is_null[15] = true;
m_Stage = 0;
is_null[16] = true;
m_Processed = 0;
is_null[17] = true;
is_null[18] = true;
is_null[19] = true;
m_psc_id = 0;
is_null[20] = true;
m_psc_batch = 0;
is_null[21] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[22] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[23] = false;
is_null[24] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_PnpQueue::PK_PnpQueue_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_PnpQueue;}
string Row_PnpQueue::DetectedDate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DetectedDate;}
string Row_PnpQueue::VendorModelId_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_VendorModelId;}
string Row_PnpQueue::Path_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Path;}
string Row_PnpQueue::IPaddress_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IPaddress;}
string Row_PnpQueue::MACaddress_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MACaddress;}
string Row_PnpQueue::Category_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Category;}
string Row_PnpQueue::SerialNumber_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SerialNumber;}
long int Row_PnpQueue::FK_CommMethod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommMethod;}
long int Row_PnpQueue::FK_PnpProtocol_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_PnpProtocol;}
long int Row_PnpQueue::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DeviceTemplate;}
long int Row_PnpQueue::FK_Device_Created_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Device_Created;}
long int Row_PnpQueue::FK_Device_Reported_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Device_Reported;}
string Row_PnpQueue::Signature_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Signature;}
long int Row_PnpQueue::Removed_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Removed;}
short int Row_PnpQueue::Stage_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Stage;}
short int Row_PnpQueue::Processed_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Processed;}
string Row_PnpQueue::Parms_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Parms;}
string Row_PnpQueue::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
long int Row_PnpQueue::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_PnpQueue::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_PnpQueue::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_PnpQueue::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_PnpQueue::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_PnpQueue::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_PnpQueue::PK_PnpQueue_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_PnpQueue = val; is_modified=true; is_null[0]=false;}
void Row_PnpQueue::DetectedDate_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DetectedDate = val; is_modified=true; is_null[1]=false;}
void Row_PnpQueue::VendorModelId_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_VendorModelId = val; is_modified=true; is_null[2]=false;}
void Row_PnpQueue::Path_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Path = val; is_modified=true; is_null[3]=false;}
void Row_PnpQueue::IPaddress_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IPaddress = val; is_modified=true; is_null[4]=false;}
void Row_PnpQueue::MACaddress_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MACaddress = val; is_modified=true; is_null[5]=false;}
void Row_PnpQueue::Category_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Category = val; is_modified=true; is_null[6]=false;}
void Row_PnpQueue::SerialNumber_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SerialNumber = val; is_modified=true; is_null[7]=false;}
void Row_PnpQueue::FK_CommMethod_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommMethod = val; is_modified=true; is_null[8]=false;}
void Row_PnpQueue::FK_PnpProtocol_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_PnpProtocol = val; is_modified=true; is_null[9]=false;}
void Row_PnpQueue::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[10]=false;}
void Row_PnpQueue::FK_Device_Created_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Device_Created = val; is_modified=true; is_null[11]=false;}
void Row_PnpQueue::FK_Device_Reported_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Device_Reported = val; is_modified=true; is_null[12]=false;}
void Row_PnpQueue::Signature_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Signature = val; is_modified=true; is_null[13]=false;}
void Row_PnpQueue::Removed_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Removed = val; is_modified=true; is_null[14]=false;}
void Row_PnpQueue::Stage_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Stage = val; is_modified=true; is_null[15]=false;}
void Row_PnpQueue::Processed_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Processed = val; is_modified=true; is_null[16]=false;}
void Row_PnpQueue::Parms_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Parms = val; is_modified=true; is_null[17]=false;}
void Row_PnpQueue::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[18]=false;}
void Row_PnpQueue::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[19]=false;}
void Row_PnpQueue::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[20]=false;}
void Row_PnpQueue::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[21]=false;}
void Row_PnpQueue::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[22]=false;}
void Row_PnpQueue::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[23]=false;}
void Row_PnpQueue::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[24]=false;}

		
bool Row_PnpQueue::DetectedDate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_PnpQueue::VendorModelId_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_PnpQueue::Path_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_PnpQueue::IPaddress_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_PnpQueue::MACaddress_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_PnpQueue::Category_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_PnpQueue::SerialNumber_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_PnpQueue::FK_CommMethod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_PnpQueue::FK_PnpProtocol_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_PnpQueue::FK_DeviceTemplate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_PnpQueue::FK_Device_Created_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_PnpQueue::FK_Device_Reported_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_PnpQueue::Signature_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_PnpQueue::Removed_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_PnpQueue::Stage_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_PnpQueue::Processed_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_PnpQueue::Parms_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_PnpQueue::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_PnpQueue::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_PnpQueue::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_PnpQueue::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_PnpQueue::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}
bool Row_PnpQueue::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[24];}

			
void Row_PnpQueue::DetectedDate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_PnpQueue::VendorModelId_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_PnpQueue::Path_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_PnpQueue::IPaddress_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_PnpQueue::MACaddress_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_PnpQueue::Category_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_PnpQueue::SerialNumber_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_PnpQueue::FK_CommMethod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_PnpQueue::FK_PnpProtocol_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_PnpQueue::FK_DeviceTemplate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_PnpQueue::FK_Device_Created_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_PnpQueue::FK_Device_Reported_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_PnpQueue::Signature_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_PnpQueue::Removed_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_PnpQueue::Stage_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_PnpQueue::Processed_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_PnpQueue::Parms_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_PnpQueue::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_PnpQueue::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_PnpQueue::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_PnpQueue::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_PnpQueue::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
void Row_PnpQueue::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[24]=val;
is_modified=true;
}
	

string Row_PnpQueue::PK_PnpQueue_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_PnpQueue);

return buf;
}

string Row_PnpQueue::DetectedDate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_DetectedDate.c_str(), (unsigned long) min((size_t)19,m_DetectedDate.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::VendorModelId_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[121];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_VendorModelId.c_str(), (unsigned long) min((size_t)60,m_VendorModelId.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::Path_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[121];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Path.c_str(), (unsigned long) min((size_t)60,m_Path.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::IPaddress_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[31];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_IPaddress.c_str(), (unsigned long) min((size_t)15,m_IPaddress.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::MACaddress_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[35];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_MACaddress.c_str(), (unsigned long) min((size_t)17,m_MACaddress.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::Category_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Category.c_str(), (unsigned long) min((size_t)100,m_Category.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::SerialNumber_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_SerialNumber.c_str(), (unsigned long) min((size_t)255,m_SerialNumber.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::FK_CommMethod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CommMethod);

return buf;
}

string Row_PnpQueue::FK_PnpProtocol_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_PnpProtocol);

return buf;
}

string Row_PnpQueue::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_PnpQueue::FK_Device_Created_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_Created);

return buf;
}

string Row_PnpQueue::FK_Device_Reported_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_Reported);

return buf;
}

string Row_PnpQueue::Signature_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Signature.c_str(), (unsigned long) min((size_t)20,m_Signature.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::Removed_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Removed);

return buf;
}

string Row_PnpQueue::Stage_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Stage);

return buf;
}

string Row_PnpQueue::Processed_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Processed);

return buf;
}

string Row_PnpQueue::Parms_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Parms.c_str(), (unsigned long) min((size_t)255,m_Parms.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)255,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_PnpQueue::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_PnpQueue::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_PnpQueue::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_PnpQueue::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PnpQueue::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_PnpQueue::Key::Key(long int in_PK_PnpQueue)
{
			pk_PK_PnpQueue = in_PK_PnpQueue;
	
}

Table_PnpQueue::Key::Key(Row_PnpQueue *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_PnpQueue = pRow->m_PK_PnpQueue;
	
}		

bool Table_PnpQueue::Key_Less::operator()(const Table_PnpQueue::Key &key1, const Table_PnpQueue::Key &key2) const
{
			if (key1.pk_PK_PnpQueue!=key2.pk_PK_PnpQueue)
return key1.pk_PK_PnpQueue<key2.pk_PK_PnpQueue;
else
return false;	
}	

bool Table_PnpQueue::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_PnpQueue *pRow = (Row_PnpQueue *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_PnpQueue_asSQL()+", "+pRow->DetectedDate_asSQL()+", "+pRow->VendorModelId_asSQL()+", "+pRow->Path_asSQL()+", "+pRow->IPaddress_asSQL()+", "+pRow->MACaddress_asSQL()+", "+pRow->Category_asSQL()+", "+pRow->SerialNumber_asSQL()+", "+pRow->FK_CommMethod_asSQL()+", "+pRow->FK_PnpProtocol_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->FK_Device_Created_asSQL()+", "+pRow->FK_Device_Reported_asSQL()+", "+pRow->Signature_asSQL()+", "+pRow->Removed_asSQL()+", "+pRow->Stage_asSQL()+", "+pRow->Processed_asSQL()+", "+pRow->Parms_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into PnpQueue (`PK_PnpQueue`, `DetectedDate`, `VendorModelId`, `Path`, `IPaddress`, `MACaddress`, `Category`, `SerialNumber`, `FK_CommMethod`, `FK_PnpProtocol`, `FK_DeviceTemplate`, `FK_Device_Created`, `FK_Device_Reported`, `Signature`, `Removed`, `Stage`, `Processed`, `Parms`, `Description`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PnpQueue::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
			{
				if( bDeleteFailedInsertRow )
				{
					addedRows.erase(i);
					delete pRow;
				}
				break;   // Go ahead and continue to do the updates
			}
		}
	
		if (db_wrapper_affected_rows(database->m_pDB)!=0)
		{
			
			
			long int id = (long int) db_wrapper_insert_id(database->m_pDB);
		
			if (id!=0)
		pRow->m_PK_PnpQueue=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_PnpQueue is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_PnpQueue);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_PnpQueue* pRow = (Row_PnpQueue*) (*i).second;	
		SingleLongKey key(pRow->m_PK_PnpQueue);

		char tmp_PK_PnpQueue[32];
sprintf(tmp_PK_PnpQueue, "%li", key.pk);


string condition;
condition = condition + "`PK_PnpQueue`=" + tmp_PK_PnpQueue;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_PnpQueue`="+pRow->PK_PnpQueue_asSQL()+", `DetectedDate`="+pRow->DetectedDate_asSQL()+", `VendorModelId`="+pRow->VendorModelId_asSQL()+", `Path`="+pRow->Path_asSQL()+", `IPaddress`="+pRow->IPaddress_asSQL()+", `MACaddress`="+pRow->MACaddress_asSQL()+", `Category`="+pRow->Category_asSQL()+", `SerialNumber`="+pRow->SerialNumber_asSQL()+", `FK_CommMethod`="+pRow->FK_CommMethod_asSQL()+", `FK_PnpProtocol`="+pRow->FK_PnpProtocol_asSQL()+", `FK_DeviceTemplate`="+pRow->FK_DeviceTemplate_asSQL()+", `FK_Device_Created`="+pRow->FK_Device_Created_asSQL()+", `FK_Device_Reported`="+pRow->FK_Device_Reported_asSQL()+", `Signature`="+pRow->Signature_asSQL()+", `Removed`="+pRow->Removed_asSQL()+", `Stage`="+pRow->Stage_asSQL()+", `Processed`="+pRow->Processed_asSQL()+", `Parms`="+pRow->Parms_asSQL()+", `Description`="+pRow->Description_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update PnpQueue set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PnpQueue::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
			{
				if( bDeleteFailedModifiedRow )
				{
					cachedRows.erase(i);
					delete pRow;
				}
				break;  // Go ahead and do the deletes
			}
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_PnpQueue* pRow = (Row_PnpQueue*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_PnpQueue* pRow = (Row_PnpQueue*) (*i).second;	

		char tmp_PK_PnpQueue[32];
sprintf(tmp_PK_PnpQueue, "%li", key.pk);


string condition;
condition = condition + "`PK_PnpQueue`=" + tmp_PK_PnpQueue;

	
		string query = "delete from PnpQueue where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PnpQueue::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_PnpQueue*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_PnpQueue::GetRows(string where_statement,vector<class Row_PnpQueue*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `PnpQueue`.* from PnpQueue " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `PnpQueue`.* from PnpQueue where " + where_statement;
	else
		query = "select `PnpQueue`.* from PnpQueue";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PnpQueue::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PnpQueue::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_PnpQueue *pRow = new Row_PnpQueue(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_PnpQueue = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_PnpQueue));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_DetectedDate = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_DetectedDate = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_VendorModelId = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_VendorModelId = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Path = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IPaddress = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_IPaddress = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_MACaddress = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_MACaddress = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Category = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Category = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_SerialNumber = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_SerialNumber = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_CommMethod = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_CommMethod));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_PnpProtocol = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_PnpProtocol));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Device_Created = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Device_Created));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Device_Reported = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Device_Reported));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Signature = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_Signature = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Removed = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_Removed));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Stage = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_Stage));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Processed = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_Processed));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Parms = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Parms = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[18]=false;
pRow->m_Description = string(row[18],lengths[18]);
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_id));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_psc_batch));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%li", &(pRow->m_psc_user));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%hi", &(pRow->m_psc_frozen));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[23]=false;
pRow->m_psc_mod = string(row[23],lengths[23]);
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_PnpQueue);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_PnpQueue *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_PnpQueue* Table_PnpQueue::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_PnpQueue *pRow = new Row_PnpQueue(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_PnpQueue* Table_PnpQueue::GetRow(long int in_PK_PnpQueue)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_PnpQueue);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_PnpQueue*) (*i).second;
	//we have to fetch row
	Row_PnpQueue* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_PnpQueue* Table_PnpQueue::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_PnpQueue[32];
sprintf(tmp_PK_PnpQueue, "%li", key.pk);


string condition;
condition = condition + "`PK_PnpQueue`=" + tmp_PK_PnpQueue;


	string query = "select * from PnpQueue where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PnpQueue::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PnpQueue::FetchRow db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return NULL;
	}	
	
	DB_ROW row = db_wrapper_fetch_row(res);
	
	if (!row)
	{
		//dataset is empty
		db_wrapper_free_result(res);			
		return NULL;		
	}	
						
	unsigned long *lengths = db_wrapper_fetch_lengths(res);

	Row_PnpQueue *pRow = new Row_PnpQueue(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_PnpQueue = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_PnpQueue));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_DetectedDate = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_DetectedDate = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_VendorModelId = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_VendorModelId = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Path = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IPaddress = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_IPaddress = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_MACaddress = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_MACaddress = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Category = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Category = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_SerialNumber = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_SerialNumber = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_CommMethod = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_CommMethod));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_PnpProtocol = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_PnpProtocol));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Device_Created = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Device_Created));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Device_Reported = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Device_Reported));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Signature = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_Signature = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Removed = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_Removed));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Stage = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_Stage));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Processed = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_Processed));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Parms = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Parms = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[18]=false;
pRow->m_Description = string(row[18],lengths[18]);
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_id));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_psc_batch));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%li", &(pRow->m_psc_user));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%hi", &(pRow->m_psc_frozen));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[23]=false;
pRow->m_psc_mod = string(row[23],lengths[23]);
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_CommMethod* Row_PnpQueue::FK_CommMethod_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommMethod *pTable = table->database->CommMethod_get();
return pTable->GetRow(m_FK_CommMethod);
}
class Row_PnpProtocol* Row_PnpQueue::FK_PnpProtocol_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PnpProtocol *pTable = table->database->PnpProtocol_get();
return pTable->GetRow(m_FK_PnpProtocol);
}
class Row_DeviceTemplate* Row_PnpQueue::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}
class Row_Device* Row_PnpQueue::FK_Device_Created_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_Created);
}
class Row_Device* Row_PnpQueue::FK_Device_Reported_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_Reported);
}





