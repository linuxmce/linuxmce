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
#include <winsock.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>

#include <mysql.h>

using namespace std;
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "Table_Listing.h"



void Database_FakeEPG::CreateTable_Listing()
{
	tblListing = new Table_Listing(this);
}

void Database_FakeEPG::DeleteTable_Listing()
{
	delete tblListing;
}

Table_Listing::~Table_Listing()
{
	map<Table_Listing::Key, class Row_Listing*, Table_Listing::Key_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		delete (*it).second;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		delete (*it).second;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Listing::Delete()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<Row_Listing*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && (*i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_Listing::Key key(this);					
			map<Table_Listing::Key, Row_Listing*, Table_Listing::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Listing::Reload()
{
	PLUTO_SAFETY_LOCK(M, table->m_Mutex);
	
	
	if (!is_added)
	{
		Table_Listing::Key key(this);		
		Row_Listing *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Listing::Row_Listing(Table_Listing *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Listing::SetDefaultValues()
{
	m_PK_Listing = 0;
is_null[0] = false;
m_ShowName = "";
is_null[1] = false;
m_Time = "";
is_null[2] = false;
m_ChannelNum = 0;
is_null[3] = false;
m_ChannelName = "";
is_null[4] = false;
m_Synopsis = "";
is_null[5] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Listing::PK_Listing_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_PK_Listing;}
string Row_Listing::ShowName_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ShowName;}
string Row_Listing::Time_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Time;}
long int Row_Listing::ChannelNum_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ChannelNum;}
string Row_Listing::ChannelName_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_ChannelName;}
string Row_Listing::Synopsis_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

return m_Synopsis;}

		
void Row_Listing::PK_Listing_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_PK_Listing = val; is_modified=true; is_null[0]=false;}
void Row_Listing::ShowName_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ShowName = val; is_modified=true; is_null[1]=false;}
void Row_Listing::Time_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Time = val; is_modified=true; is_null[2]=false;}
void Row_Listing::ChannelNum_set(long int val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ChannelNum = val; is_modified=true; is_null[3]=false;}
void Row_Listing::ChannelName_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_ChannelName = val; is_modified=true; is_null[4]=false;}
void Row_Listing::Synopsis_set(string val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);

m_Synopsis = val; is_modified=true; is_null[5]=false;}

		

			
	

string Row_Listing::PK_Listing_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Listing);

return buf;
}

string Row_Listing::ShowName_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[1])
return "NULL";

char buf[121];
mysql_real_escape_string(table->database->db_handle, buf, m_ShowName.c_str(), (unsigned long) m_ShowName.size());
return string()+"\""+buf+"\"";
}

string Row_Listing::Time_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[2])
return "NULL";

char buf[31];
mysql_real_escape_string(table->database->db_handle, buf, m_Time.c_str(), (unsigned long) m_Time.size());
return string()+"\""+buf+"\"";
}

string Row_Listing::ChannelNum_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ChannelNum);

return buf;
}

string Row_Listing::ChannelName_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[4])
return "NULL";

char buf[21];
mysql_real_escape_string(table->database->db_handle, buf, m_ChannelName.c_str(), (unsigned long) m_ChannelName.size());
return string()+"\""+buf+"\"";
}

string Row_Listing::Synopsis_asSQL()
{
PLUTO_SAFETY_LOCK(M, table->m_Mutex);

if (is_null[5])
return "NULL";

char buf[131071];
mysql_real_escape_string(table->database->db_handle, buf, m_Synopsis.c_str(), (unsigned long) m_Synopsis.size());
return string()+"\""+buf+"\"";
}




Table_Listing::Key::Key(long int in_PK_Listing)
{
			pk_PK_Listing = in_PK_Listing;
	
}

Table_Listing::Key::Key(Row_Listing *pRow)
{
			PLUTO_SAFETY_LOCK(M, pRow->table->m_Mutex);

			pk_PK_Listing = pRow->m_PK_Listing;
	
}		

bool Table_Listing::Key_Less::operator()(const Table_Listing::Key &key1, const Table_Listing::Key &key2) const
{
			if (key1.pk_PK_Listing!=key2.pk_PK_Listing)
return key1.pk_PK_Listing<key2.pk_PK_Listing;
else
return false;	
}	

void Table_Listing::Commit()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

//insert added
	while (!addedRows.empty())
	{
		vector<Row_Listing*>::iterator i = addedRows.begin();
	
		Row_Listing *pRow = *i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Listing_asSQL()+", "+pRow->ShowName_asSQL()+", "+pRow->Time_asSQL()+", "+pRow->ChannelNum_asSQL()+", "+pRow->ChannelName_asSQL()+", "+pRow->Synopsis_asSQL();

	
		string query = "insert into Listing (PK_Listing, ShowName, Time, ChannelNum, ChannelName, Synopsis) values ("+
			values_list_comma_separated+")";
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		if (mysql_affected_rows(database->db_handle)!=0)
		{
			
			
			long int id	= (long int) mysql_insert_id(database->db_handle);
		
			if (id!=0)
pRow->m_PK_Listing=id;
	
			
			addedRows.erase(i);
			Key key(pRow);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Key, Row_Listing*, Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified)
	{
		Row_Listing* pRow = (*i).second;	
		Key key(pRow);	

		char tmp_PK_Listing[32];
sprintf(tmp_PK_Listing, "%li", key.pk_PK_Listing);


string condition;
condition = condition + "PK_Listing=" + tmp_PK_Listing;
	
			
		
string update_values_list;
update_values_list = update_values_list + "PK_Listing="+pRow->PK_Listing_asSQL()+", ShowName="+pRow->ShowName_asSQL()+", Time="+pRow->Time_asSQL()+", ChannelNum="+pRow->ChannelNum_asSQL()+", ChannelName="+pRow->ChannelName_asSQL()+", Synopsis="+pRow->Synopsis_asSQL();

	
		string query = "update Listing set " + update_values_list + " where " + condition;
			
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<Row_Listing*>::iterator i = deleted_addedRows.begin();
		delete (*i);
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Key, Row_Listing*, Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Key key = (*i).first;
	
		char tmp_PK_Listing[32];
sprintf(tmp_PK_Listing, "%li", key.pk_PK_Listing);


string condition;
condition = condition + "PK_Listing=" + tmp_PK_Listing;

	
		string query = "delete from Listing where " + condition;
		
		if (mysql_query(database->db_handle, query.c_str()))
		{	
			cerr << "Cannot perform query: [" << query << "]" << endl;
		}	
		
		delete (*i).second;
		deleted_cachedRows.erase(key);
	}
	
}

bool Table_Listing::GetRows(string where_statement,vector<class Row_Listing*> *rows)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	string query = "select * from Listing where " + where_statement;
		
	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query" << endl;
		return false;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		return false;
	}	
	
	MYSQL_ROW row;
						
		
	while ((row = mysql_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = mysql_fetch_lengths(res);

		Row_Listing *pRow = new Row_Listing(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Listing = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Listing));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_ShowName = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_ShowName = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Time = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Time = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_ChannelNum = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_ChannelNum));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ChannelName = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ChannelName = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Synopsis = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Synopsis = string(row[5],lengths[5]);
}



		//checking for duplicates

		Key key(pRow);
		
                map<Table_Listing::Key, Row_Listing*, Table_Listing::Key_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	mysql_free_result(res);			
		
	return true;					
}

Row_Listing* Table_Listing::AddRow()
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Row_Listing *pRow = new Row_Listing(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Listing* Table_Listing::GetRow(long int in_PK_Listing)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	Key row_key(in_PK_Listing);

	map<Key, Row_Listing*, Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (*i).second;
	//we have to fetch row
	Row_Listing* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Listing* Table_Listing::FetchRow(Table_Listing::Key &key)
{
	PLUTO_SAFETY_LOCK(M, m_Mutex);

	//defines the string query for the value of key
	char tmp_PK_Listing[32];
sprintf(tmp_PK_Listing, "%li", key.pk_PK_Listing);


string condition;
condition = condition + "PK_Listing=" + tmp_PK_Listing;


	string query = "select * from Listing where " + condition;		

	if (mysql_query(database->db_handle, query.c_str()))
	{	
		cerr << "Cannot perform query" << endl;
		return NULL;
	}	

	MYSQL_RES *res = mysql_store_result(database->db_handle);
	
	if (!res)
	{
		cerr << "mysql_store_result returned NULL handler" << endl;
		return NULL;
	}	
	
	MYSQL_ROW row = mysql_fetch_row(res);
	
	if (!row)
	{
		//dataset is empty
		mysql_free_result(res);			
		return NULL;		
	}	
						
	unsigned long *lengths = mysql_fetch_lengths(res);

	Row_Listing *pRow = new Row_Listing(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Listing = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Listing));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_ShowName = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_ShowName = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Time = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Time = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_ChannelNum = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_ChannelNum));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ChannelName = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ChannelName = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Synopsis = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Synopsis = string(row[5],lengths[5]);
}



	mysql_free_result(res);			
	
	return pRow;						
}







