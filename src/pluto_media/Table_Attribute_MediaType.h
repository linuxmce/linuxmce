/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Attribute_MediaType_H__
#define __Table_Attribute_MediaType_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Attribute_MediaType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Attribute_MediaType : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_Attribute_MediaType(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_Attribute_MediaType();

private:		
	friend class Row_Attribute_MediaType;
	struct Key
	{
		friend class Row_Attribute_MediaType;
		long int pk_FK_Attribute;
long int pk_EK_MediaType;

		
		Key(long int in_FK_Attribute, long int in_EK_MediaType);
	
		Key(class Row_Attribute_MediaType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Attribute_MediaType::Key &key1, const Table_Attribute_MediaType::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Attribute_MediaType*> *rows);
	class Row_Attribute_MediaType* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Attribute_MediaType* GetRow(long int in_FK_Attribute, long int in_EK_MediaType);
	

private:	
	
		
	class Row_Attribute_MediaType* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Attribute_MediaType : public TableRow, public SerializeClass
	{
		friend struct Table_Attribute_MediaType::Key;
		friend class Table_Attribute_MediaType;
	private:
		Table_Attribute_MediaType *table;
		
		long int m_FK_Attribute;
long int m_EK_MediaType;
long int m_NumFiles;

		bool is_null[3];
	
	public:
		long int FK_Attribute_get();
long int EK_MediaType_get();
long int NumFiles_get();

		
		void FK_Attribute_set(long int val);
void EK_MediaType_set(long int val);
void NumFiles_set(long int val);

		
		bool NumFiles_isNull();

			
		void NumFiles_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Attribute_MediaType(Table_Attribute_MediaType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Attribute_MediaType *Table_Attribute_MediaType_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Attribute* FK_Attribute_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Attribute+ m_EK_MediaType+ m_NumFiles;
		}
	private:
		void SetDefaultValues();
		
		string FK_Attribute_asSQL();
string EK_MediaType_asSQL();
string NumFiles_asSQL();

	};

#endif

