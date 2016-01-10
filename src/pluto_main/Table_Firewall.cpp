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
#include "Table_Firewall.h"



void Database_pluto_main::CreateTable_Firewall()
{
	tblFirewall = new Table_Firewall(this);
}

void Database_pluto_main::DeleteTable_Firewall()
{
	if( tblFirewall )
		delete tblFirewall;
}

bool Database_pluto_main::Commit_Firewall(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblFirewall->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Firewall::~Table_Firewall()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Firewall *pRow = (Row_Firewall *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Firewall *pRow = (Row_Firewall *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Firewall::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Firewall *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Firewall *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Firewall);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Firewall::Reload()
{
	Row_Firewall *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Firewall);
		Row_Firewall *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Firewall::Row_Firewall(Table_Firewall *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Firewall::SetDefaultValues()
{
	m_PK_Firewall = 0;
is_null[0] = false;
m_Place = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
m_Protocol = "tcp";
is_null[5] = false;
is_null[6] = true;
m_SourcePortEnd = 0;
is_null[7] = false;
m_DestinationPort = "0";
is_null[8] = false;
m_SourceIP = "";
is_null[9] = false;
m_DestinationIP = "";
is_null[10] = false;
m_RuleType = "";
is_null[11] = false;
m_Disabled = 0;
is_null[12] = false;
m_Offline = 0;
is_null[13] = false;
is_null[14] = true;
is_null[15] = true;
m_Bantime = 0;
is_null[16] = false;
is_null[17] = true;
m_psc_id = 0;
is_null[18] = true;
m_psc_batch = 0;
is_null[19] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[20] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[21] = false;
is_null[22] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Firewall::PK_Firewall_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Firewall;}
long int Row_Firewall::Place_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Place;}
string Row_Firewall::Matchname_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Matchname;}
string Row_Firewall::IntIF_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IntIF;}
string Row_Firewall::ExtIF_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ExtIF;}
string Row_Firewall::Protocol_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Protocol;}
string Row_Firewall::SourcePort_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SourcePort;}
long int Row_Firewall::SourcePortEnd_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SourcePortEnd;}
string Row_Firewall::DestinationPort_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DestinationPort;}
string Row_Firewall::SourceIP_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SourceIP;}
string Row_Firewall::DestinationIP_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DestinationIP;}
string Row_Firewall::RuleType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RuleType;}
long int Row_Firewall::Disabled_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Disabled;}
long int Row_Firewall::Offline_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Offline;}
string Row_Firewall::RPolicy_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RPolicy;}
string Row_Firewall::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
long int Row_Firewall::Bantime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Bantime;}
long int Row_Firewall::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Firewall::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Firewall::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Firewall::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Firewall::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Firewall::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Firewall::PK_Firewall_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Firewall = val; is_modified=true; is_null[0]=false;}
void Row_Firewall::Place_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Place = val; is_modified=true; is_null[1]=false;}
void Row_Firewall::Matchname_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Matchname = val; is_modified=true; is_null[2]=false;}
void Row_Firewall::IntIF_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IntIF = val; is_modified=true; is_null[3]=false;}
void Row_Firewall::ExtIF_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ExtIF = val; is_modified=true; is_null[4]=false;}
void Row_Firewall::Protocol_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Protocol = val; is_modified=true; is_null[5]=false;}
void Row_Firewall::SourcePort_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SourcePort = val; is_modified=true; is_null[6]=false;}
void Row_Firewall::SourcePortEnd_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SourcePortEnd = val; is_modified=true; is_null[7]=false;}
void Row_Firewall::DestinationPort_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DestinationPort = val; is_modified=true; is_null[8]=false;}
void Row_Firewall::SourceIP_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SourceIP = val; is_modified=true; is_null[9]=false;}
void Row_Firewall::DestinationIP_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DestinationIP = val; is_modified=true; is_null[10]=false;}
void Row_Firewall::RuleType_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RuleType = val; is_modified=true; is_null[11]=false;}
void Row_Firewall::Disabled_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Disabled = val; is_modified=true; is_null[12]=false;}
void Row_Firewall::Offline_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Offline = val; is_modified=true; is_null[13]=false;}
void Row_Firewall::RPolicy_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RPolicy = val; is_modified=true; is_null[14]=false;}
void Row_Firewall::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[15]=false;}
void Row_Firewall::Bantime_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Bantime = val; is_modified=true; is_null[16]=false;}
void Row_Firewall::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[17]=false;}
void Row_Firewall::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[18]=false;}
void Row_Firewall::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[19]=false;}
void Row_Firewall::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[20]=false;}
void Row_Firewall::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[21]=false;}
void Row_Firewall::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[22]=false;}

		
bool Row_Firewall::Matchname_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_Firewall::IntIF_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_Firewall::ExtIF_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Firewall::SourcePort_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Firewall::Disabled_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_Firewall::Offline_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_Firewall::RPolicy_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_Firewall::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_Firewall::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_Firewall::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_Firewall::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_Firewall::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_Firewall::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}

			
void Row_Firewall::Matchname_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Firewall::IntIF_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Firewall::ExtIF_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Firewall::SourcePort_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Firewall::Disabled_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Firewall::Offline_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_Firewall::RPolicy_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_Firewall::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_Firewall::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_Firewall::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_Firewall::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_Firewall::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_Firewall::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
	

string Row_Firewall::PK_Firewall_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Firewall);

return buf;
}

string Row_Firewall::Place_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Place);

return buf;
}

string Row_Firewall::Matchname_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Matchname.c_str(), (unsigned long) min((size_t)65535,m_Matchname.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::IntIF_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[17];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_IntIF.c_str(), (unsigned long) min((size_t)8,m_IntIF.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::ExtIF_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[17];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ExtIF.c_str(), (unsigned long) min((size_t)8,m_ExtIF.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::Protocol_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Protocol.c_str(), (unsigned long) min((size_t)10,m_Protocol.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::SourcePort_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_SourcePort.c_str(), (unsigned long) min((size_t)10,m_SourcePort.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::SourcePortEnd_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_SourcePortEnd);

return buf;
}

string Row_Firewall::DestinationPort_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[51];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_DestinationPort.c_str(), (unsigned long) min((size_t)25,m_DestinationPort.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::SourceIP_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_SourceIP.c_str(), (unsigned long) min((size_t)50,m_SourceIP.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::DestinationIP_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_DestinationIP.c_str(), (unsigned long) min((size_t)50,m_DestinationIP.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::RuleType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_RuleType.c_str(), (unsigned long) min((size_t)50,m_RuleType.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::Disabled_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Disabled);

return buf;
}

string Row_Firewall::Offline_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Offline);

return buf;
}

string Row_Firewall::RPolicy_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_RPolicy.c_str(), (unsigned long) min((size_t)65535,m_RPolicy.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)65535,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::Bantime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Bantime);

return buf;
}

string Row_Firewall::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Firewall::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Firewall::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Firewall::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Firewall::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Firewall::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Firewall::Key::Key(long int in_PK_Firewall)
{
			pk_PK_Firewall = in_PK_Firewall;
	
}

Table_Firewall::Key::Key(Row_Firewall *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Firewall = pRow->m_PK_Firewall;
	
}		

bool Table_Firewall::Key_Less::operator()(const Table_Firewall::Key &key1, const Table_Firewall::Key &key2) const
{
			if (key1.pk_PK_Firewall!=key2.pk_PK_Firewall)
return key1.pk_PK_Firewall<key2.pk_PK_Firewall;
else
return false;	
}	

bool Table_Firewall::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Firewall *pRow = (Row_Firewall *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Firewall_asSQL()+", "+pRow->Place_asSQL()+", "+pRow->Matchname_asSQL()+", "+pRow->IntIF_asSQL()+", "+pRow->ExtIF_asSQL()+", "+pRow->Protocol_asSQL()+", "+pRow->SourcePort_asSQL()+", "+pRow->SourcePortEnd_asSQL()+", "+pRow->DestinationPort_asSQL()+", "+pRow->SourceIP_asSQL()+", "+pRow->DestinationIP_asSQL()+", "+pRow->RuleType_asSQL()+", "+pRow->Disabled_asSQL()+", "+pRow->Offline_asSQL()+", "+pRow->RPolicy_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Bantime_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Firewall (`PK_Firewall`, `Place`, `Matchname`, `IntIF`, `ExtIF`, `Protocol`, `SourcePort`, `SourcePortEnd`, `DestinationPort`, `SourceIP`, `DestinationIP`, `RuleType`, `Disabled`, `Offline`, `RPolicy`, `Description`, `Bantime`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Firewall::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Firewall=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Firewall is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Firewall);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Firewall* pRow = (Row_Firewall*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Firewall);

		char tmp_PK_Firewall[32];
sprintf(tmp_PK_Firewall, "%li", key.pk);


string condition;
condition = condition + "`PK_Firewall`=" + tmp_PK_Firewall;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Firewall`="+pRow->PK_Firewall_asSQL()+", `Place`="+pRow->Place_asSQL()+", `Matchname`="+pRow->Matchname_asSQL()+", `IntIF`="+pRow->IntIF_asSQL()+", `ExtIF`="+pRow->ExtIF_asSQL()+", `Protocol`="+pRow->Protocol_asSQL()+", `SourcePort`="+pRow->SourcePort_asSQL()+", `SourcePortEnd`="+pRow->SourcePortEnd_asSQL()+", `DestinationPort`="+pRow->DestinationPort_asSQL()+", `SourceIP`="+pRow->SourceIP_asSQL()+", `DestinationIP`="+pRow->DestinationIP_asSQL()+", `RuleType`="+pRow->RuleType_asSQL()+", `Disabled`="+pRow->Disabled_asSQL()+", `Offline`="+pRow->Offline_asSQL()+", `RPolicy`="+pRow->RPolicy_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Bantime`="+pRow->Bantime_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Firewall set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Firewall::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Firewall* pRow = (Row_Firewall*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Firewall* pRow = (Row_Firewall*) (*i).second;	

		char tmp_PK_Firewall[32];
sprintf(tmp_PK_Firewall, "%li", key.pk);


string condition;
condition = condition + "`PK_Firewall`=" + tmp_PK_Firewall;

	
		string query = "delete from Firewall where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Firewall::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Firewall*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Firewall::GetRows(string where_statement,vector<class Row_Firewall*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Firewall`.* from Firewall " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Firewall`.* from Firewall where " + where_statement;
	else
		query = "select `Firewall`.* from Firewall";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Firewall::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Firewall::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Firewall *pRow = new Row_Firewall(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Firewall = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Firewall));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Place = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_Place));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Matchname = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Matchname = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IntIF = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_IntIF = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ExtIF = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ExtIF = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Protocol = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Protocol = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_SourcePort = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_SourcePort = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_SourcePortEnd = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_SourcePortEnd));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_DestinationPort = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_DestinationPort = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_SourceIP = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_SourceIP = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_DestinationIP = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_DestinationIP = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_RuleType = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_RuleType = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Disabled = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_Disabled));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Offline = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_Offline));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_RPolicy = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_RPolicy = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_Description = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Bantime = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_Bantime));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_id));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_batch));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_user));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_psc_frozen));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_psc_mod = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Firewall);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Firewall *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Firewall* Table_Firewall::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Firewall *pRow = new Row_Firewall(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Firewall* Table_Firewall::GetRow(long int in_PK_Firewall)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Firewall);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Firewall*) (*i).second;
	//we have to fetch row
	Row_Firewall* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Firewall* Table_Firewall::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Firewall[32];
sprintf(tmp_PK_Firewall, "%li", key.pk);


string condition;
condition = condition + "`PK_Firewall`=" + tmp_PK_Firewall;


	string query = "select * from Firewall where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Firewall::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Firewall::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Firewall *pRow = new Row_Firewall(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Firewall = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Firewall));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Place = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_Place));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Matchname = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Matchname = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IntIF = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_IntIF = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ExtIF = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ExtIF = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Protocol = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Protocol = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_SourcePort = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_SourcePort = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_SourcePortEnd = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_SourcePortEnd));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_DestinationPort = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_DestinationPort = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_SourceIP = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_SourceIP = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_DestinationIP = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_DestinationIP = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_RuleType = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_RuleType = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Disabled = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_Disabled));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Offline = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_Offline));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_RPolicy = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_RPolicy = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_Description = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Bantime = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_Bantime));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_id));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_batch));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_user));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_psc_frozen));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_psc_mod = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







