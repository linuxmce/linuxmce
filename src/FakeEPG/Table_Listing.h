/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __Table_Listing_H__
#define __Table_Listing_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_FakeEPG.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Listing.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Listing
{
private:
	Database_FakeEPG *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Listing(Database_FakeEPG *pDatabase):database(pDatabase), m_Mutex("Listing")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Listing();

private:		
	friend class Row_Listing;
	struct Key
	{
		friend class Row_Listing;
		long int pk_PK_Listing;

		
		Key(long int in_PK_Listing);
	
		Key(class Row_Listing *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Listing::Key &key1, const Table_Listing::Key &key2) const;
	};	

	map<Table_Listing::Key, class Row_Listing*, Table_Listing::Key_Less> cachedRows;
	map<Table_Listing::Key, class Row_Listing*, Table_Listing::Key_Less> deleted_cachedRows;
	vector<class Row_Listing*> addedRows;
	vector<class Row_Listing*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Listing*> *rows);
	class Row_Listing* AddRow();
	Database_FakeEPG *Database_FakeEPG_get() { return database; }
	
		
	class Row_Listing* GetRow(long int in_PK_Listing);
	

private:	
	
		
	class Row_Listing* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Listing : public TableRow, public SerializeClass
	{
		friend struct Table_Listing::Key;
		friend class Table_Listing;
	private:
		Table_Listing *table;
		
		long int m_PK_Listing;
string m_ShowName;
string m_Time;
long int m_ChannelNum;
string m_ChannelName;
string m_Synopsis;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Listing_get();
string ShowName_get();
string Time_get();
long int ChannelNum_get();
string ChannelName_get();
string Synopsis_get();

		
		void PK_Listing_set(long int val);
void ShowName_set(string val);
void Time_set(string val);
void ChannelNum_set(long int val);
void ChannelName_set(string val);
void Synopsis_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Listing(Table_Listing *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Listing *Table_Listing_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Listing+ m_ShowName+ m_Time+ m_ChannelNum+ m_ChannelName+ m_Synopsis;
		}
	private:
		void SetDefaultValues();
		
		string PK_Listing_asSQL();
string ShowName_asSQL();
string Time_asSQL();
string ChannelNum_asSQL();
string ChannelName_asSQL();
string Synopsis_asSQL();

	};

#endif

