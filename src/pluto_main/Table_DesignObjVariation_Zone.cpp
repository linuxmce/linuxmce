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
#include "Table_DesignObjVariation_Zone.h"
#include "Table_DesignObjVariation.h"
#include "Table_Button.h"
#include "Table_CommandGroup_D.h"
#include "Table_DesignObj.h"



void Database_pluto_main::CreateTable_DesignObjVariation_Zone()
{
	tblDesignObjVariation_Zone = new Table_DesignObjVariation_Zone(this);
}

void Database_pluto_main::DeleteTable_DesignObjVariation_Zone()
{
	if( tblDesignObjVariation_Zone )
		delete tblDesignObjVariation_Zone;
}

bool Database_pluto_main::Commit_DesignObjVariation_Zone(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDesignObjVariation_Zone->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DesignObjVariation_Zone::~Table_DesignObjVariation_Zone()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DesignObjVariation_Zone *pRow = (Row_DesignObjVariation_Zone *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DesignObjVariation_Zone *pRow = (Row_DesignObjVariation_Zone *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DesignObjVariation_Zone::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DesignObjVariation_Zone *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DesignObjVariation_Zone *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_DesignObjVariation_Zone);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_DesignObjVariation_Zone::Reload()
{
	Row_DesignObjVariation_Zone *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_DesignObjVariation_Zone);
		Row_DesignObjVariation_Zone *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DesignObjVariation_Zone::Row_DesignObjVariation_Zone(Table_DesignObjVariation_Zone *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DesignObjVariation_Zone::SetDefaultValues()
{
	m_PK_DesignObjVariation_Zone = 0;
is_null[0] = false;
is_null[1] = true;
m_FK_DesignObjVariation = 0;
is_null[2] = true;
m_FK_Button = 0;
m_FK_CommandGroup_D = 0;
is_null[3] = false;
is_null[4] = true;
m_FK_DesignObj_Goto = 0;
m_Description = "";
is_null[5] = false;
m_X = 0;
is_null[6] = false;
m_Y = 0;
is_null[7] = false;
m_Width = 0;
is_null[8] = false;
m_Height = 0;
is_null[9] = false;
is_null[10] = true;
m_psc_id = 0;
is_null[11] = true;
m_psc_batch = 0;
is_null[12] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[13] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[14] = false;
is_null[15] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DesignObjVariation_Zone::PK_DesignObjVariation_Zone_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_DesignObjVariation_Zone;}
long int Row_DesignObjVariation_Zone::FK_DesignObjVariation_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObjVariation;}
long int Row_DesignObjVariation_Zone::FK_Button_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Button;}
long int Row_DesignObjVariation_Zone::FK_CommandGroup_D_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommandGroup_D;}
long int Row_DesignObjVariation_Zone::FK_DesignObj_Goto_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_Goto;}
string Row_DesignObjVariation_Zone::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
long int Row_DesignObjVariation_Zone::X_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_X;}
long int Row_DesignObjVariation_Zone::Y_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Y;}
long int Row_DesignObjVariation_Zone::Width_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Width;}
long int Row_DesignObjVariation_Zone::Height_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Height;}
long int Row_DesignObjVariation_Zone::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DesignObjVariation_Zone::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_DesignObjVariation_Zone::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DesignObjVariation_Zone::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DesignObjVariation_Zone::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_DesignObjVariation_Zone::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_DesignObjVariation_Zone::PK_DesignObjVariation_Zone_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_DesignObjVariation_Zone = val; is_modified=true; is_null[0]=false;}
void Row_DesignObjVariation_Zone::FK_DesignObjVariation_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObjVariation = val; is_modified=true; is_null[1]=false;}
void Row_DesignObjVariation_Zone::FK_Button_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Button = val; is_modified=true; is_null[2]=false;}
void Row_DesignObjVariation_Zone::FK_CommandGroup_D_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommandGroup_D = val; is_modified=true; is_null[3]=false;}
void Row_DesignObjVariation_Zone::FK_DesignObj_Goto_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_Goto = val; is_modified=true; is_null[4]=false;}
void Row_DesignObjVariation_Zone::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[5]=false;}
void Row_DesignObjVariation_Zone::X_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_X = val; is_modified=true; is_null[6]=false;}
void Row_DesignObjVariation_Zone::Y_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Y = val; is_modified=true; is_null[7]=false;}
void Row_DesignObjVariation_Zone::Width_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Width = val; is_modified=true; is_null[8]=false;}
void Row_DesignObjVariation_Zone::Height_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Height = val; is_modified=true; is_null[9]=false;}
void Row_DesignObjVariation_Zone::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[10]=false;}
void Row_DesignObjVariation_Zone::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[11]=false;}
void Row_DesignObjVariation_Zone::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[12]=false;}
void Row_DesignObjVariation_Zone::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[13]=false;}
void Row_DesignObjVariation_Zone::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[14]=false;}
void Row_DesignObjVariation_Zone::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[15]=false;}

		
bool Row_DesignObjVariation_Zone::FK_DesignObjVariation_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_DesignObjVariation_Zone::FK_Button_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DesignObjVariation_Zone::FK_DesignObj_Goto_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_DesignObjVariation_Zone::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_DesignObjVariation_Zone::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_DesignObjVariation_Zone::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_DesignObjVariation_Zone::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_DesignObjVariation_Zone::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}

			
void Row_DesignObjVariation_Zone::FK_DesignObjVariation_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_DesignObjVariation_Zone::FK_Button_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DesignObjVariation_Zone::FK_DesignObj_Goto_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_DesignObjVariation_Zone::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_DesignObjVariation_Zone::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_DesignObjVariation_Zone::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_DesignObjVariation_Zone::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_DesignObjVariation_Zone::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
	

string Row_DesignObjVariation_Zone::PK_DesignObjVariation_Zone_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_DesignObjVariation_Zone);

return buf;
}

string Row_DesignObjVariation_Zone::FK_DesignObjVariation_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObjVariation);

return buf;
}

string Row_DesignObjVariation_Zone::FK_Button_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Button);

return buf;
}

string Row_DesignObjVariation_Zone::FK_CommandGroup_D_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CommandGroup_D);

return buf;
}

string Row_DesignObjVariation_Zone::FK_DesignObj_Goto_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_Goto);

return buf;
}

string Row_DesignObjVariation_Zone::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)20,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DesignObjVariation_Zone::X_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_X);

return buf;
}

string Row_DesignObjVariation_Zone::Y_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Y);

return buf;
}

string Row_DesignObjVariation_Zone::Width_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Width);

return buf;
}

string Row_DesignObjVariation_Zone::Height_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Height);

return buf;
}

string Row_DesignObjVariation_Zone::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DesignObjVariation_Zone::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DesignObjVariation_Zone::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DesignObjVariation_Zone::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DesignObjVariation_Zone::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DesignObjVariation_Zone::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_DesignObjVariation_Zone::Key::Key(long int in_PK_DesignObjVariation_Zone)
{
			pk_PK_DesignObjVariation_Zone = in_PK_DesignObjVariation_Zone;
	
}

Table_DesignObjVariation_Zone::Key::Key(Row_DesignObjVariation_Zone *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_DesignObjVariation_Zone = pRow->m_PK_DesignObjVariation_Zone;
	
}		

bool Table_DesignObjVariation_Zone::Key_Less::operator()(const Table_DesignObjVariation_Zone::Key &key1, const Table_DesignObjVariation_Zone::Key &key2) const
{
			if (key1.pk_PK_DesignObjVariation_Zone!=key2.pk_PK_DesignObjVariation_Zone)
return key1.pk_PK_DesignObjVariation_Zone<key2.pk_PK_DesignObjVariation_Zone;
else
return false;	
}	

bool Table_DesignObjVariation_Zone::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DesignObjVariation_Zone *pRow = (Row_DesignObjVariation_Zone *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DesignObjVariation_Zone_asSQL()+", "+pRow->FK_DesignObjVariation_asSQL()+", "+pRow->FK_Button_asSQL()+", "+pRow->FK_CommandGroup_D_asSQL()+", "+pRow->FK_DesignObj_Goto_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->X_asSQL()+", "+pRow->Y_asSQL()+", "+pRow->Width_asSQL()+", "+pRow->Height_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into DesignObjVariation_Zone (`PK_DesignObjVariation_Zone`, `FK_DesignObjVariation`, `FK_Button`, `FK_CommandGroup_D`, `FK_DesignObj_Goto`, `Description`, `X`, `Y`, `Width`, `Height`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Zone::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_DesignObjVariation_Zone=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_DesignObjVariation_Zone is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_DesignObjVariation_Zone);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_DesignObjVariation_Zone* pRow = (Row_DesignObjVariation_Zone*) (*i).second;	
		SingleLongKey key(pRow->m_PK_DesignObjVariation_Zone);

		char tmp_PK_DesignObjVariation_Zone[32];
sprintf(tmp_PK_DesignObjVariation_Zone, "%li", key.pk);


string condition;
condition = condition + "`PK_DesignObjVariation_Zone`=" + tmp_PK_DesignObjVariation_Zone;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DesignObjVariation_Zone`="+pRow->PK_DesignObjVariation_Zone_asSQL()+", `FK_DesignObjVariation`="+pRow->FK_DesignObjVariation_asSQL()+", `FK_Button`="+pRow->FK_Button_asSQL()+", `FK_CommandGroup_D`="+pRow->FK_CommandGroup_D_asSQL()+", `FK_DesignObj_Goto`="+pRow->FK_DesignObj_Goto_asSQL()+", `Description`="+pRow->Description_asSQL()+", `X`="+pRow->X_asSQL()+", `Y`="+pRow->Y_asSQL()+", `Width`="+pRow->Width_asSQL()+", `Height`="+pRow->Height_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update DesignObjVariation_Zone set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Zone::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DesignObjVariation_Zone* pRow = (Row_DesignObjVariation_Zone*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_DesignObjVariation_Zone* pRow = (Row_DesignObjVariation_Zone*) (*i).second;	

		char tmp_PK_DesignObjVariation_Zone[32];
sprintf(tmp_PK_DesignObjVariation_Zone, "%li", key.pk);


string condition;
condition = condition + "`PK_DesignObjVariation_Zone`=" + tmp_PK_DesignObjVariation_Zone;

	
		string query = "delete from DesignObjVariation_Zone where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Zone::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DesignObjVariation_Zone*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DesignObjVariation_Zone::GetRows(string where_statement,vector<class Row_DesignObjVariation_Zone*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DesignObjVariation_Zone`.* from DesignObjVariation_Zone " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DesignObjVariation_Zone`.* from DesignObjVariation_Zone where " + where_statement;
	else
		query = "select `DesignObjVariation_Zone`.* from DesignObjVariation_Zone";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Zone::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Zone::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DesignObjVariation_Zone *pRow = new Row_DesignObjVariation_Zone(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObjVariation_Zone = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DesignObjVariation_Zone));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObjVariation = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DesignObjVariation));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Button = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Button));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_CommandGroup_D = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_CommandGroup_D));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_DesignObj_Goto = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_DesignObj_Goto));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Description = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_X = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_X));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Y = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_Y));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_Width));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_Height));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_id));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_batch));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_user));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_psc_frozen));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_psc_mod = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_DesignObjVariation_Zone);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DesignObjVariation_Zone *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DesignObjVariation_Zone* Table_DesignObjVariation_Zone::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DesignObjVariation_Zone *pRow = new Row_DesignObjVariation_Zone(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DesignObjVariation_Zone* Table_DesignObjVariation_Zone::GetRow(long int in_PK_DesignObjVariation_Zone)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_DesignObjVariation_Zone);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_DesignObjVariation_Zone*) (*i).second;
	//we have to fetch row
	Row_DesignObjVariation_Zone* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DesignObjVariation_Zone* Table_DesignObjVariation_Zone::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_DesignObjVariation_Zone[32];
sprintf(tmp_PK_DesignObjVariation_Zone, "%li", key.pk);


string condition;
condition = condition + "`PK_DesignObjVariation_Zone`=" + tmp_PK_DesignObjVariation_Zone;


	string query = "select * from DesignObjVariation_Zone where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Zone::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Zone::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DesignObjVariation_Zone *pRow = new Row_DesignObjVariation_Zone(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObjVariation_Zone = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DesignObjVariation_Zone));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObjVariation = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DesignObjVariation));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Button = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Button));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_CommandGroup_D = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_CommandGroup_D));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_DesignObj_Goto = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_DesignObj_Goto));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Description = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_X = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_X));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Y = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_Y));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_Width));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_Height));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_id));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_batch));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_user));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_psc_frozen));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_psc_mod = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_DesignObjVariation* Row_DesignObjVariation_Zone::FK_DesignObjVariation_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation *pTable = table->database->DesignObjVariation_get();
return pTable->GetRow(m_FK_DesignObjVariation);
}
class Row_Button* Row_DesignObjVariation_Zone::FK_Button_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Button *pTable = table->database->Button_get();
return pTable->GetRow(m_FK_Button);
}
class Row_CommandGroup_D* Row_DesignObjVariation_Zone::FK_CommandGroup_D_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
return pTable->GetRow(m_FK_CommandGroup_D);
}
class Row_DesignObj* Row_DesignObjVariation_Zone::FK_DesignObj_Goto_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Goto);
}





