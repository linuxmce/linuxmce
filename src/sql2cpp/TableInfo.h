/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/**
 *
 * @file TableInfo.h
 * @brief header file for the TableInfo class
 *
 */


#ifndef TABLEINFO_H
#define TABLEINFO_H

#include "mysql.h"
#include "FieldInfo.h"

#include <string>
#include <vector>
using namespace std;

/**
 * @brief class storing table parameters and column parameters
 */
class TableInfo
{

protected:

	string m_sTableName; /** < table name */
	vector<FieldInfo*> m_Fields; /** < vector of column parameters */
	MYSQL *m_pDB; /** < pointer to database */
	vector<FieldInfo*> m_vectFieldInfoDependencies; /** < vector of dependencie fields */

public:

	/**
	 * @brief constructor, initializes the database pointer member
	 */
	TableInfo( MYSQL * pDB ) : m_pDB( pDB ) {}

	/** @todo add destructor */

	/**
	 * @brief load table definition from table
	 */
	virtual bool loadFromDB( string sTable );
	
	/**
	 * @brief returns true if table has primary keys, false otherwise
	 */
	bool HasPrimaryKeys();

	/**
        * @brief returns true if table has the specified field 
	*/ 
	bool HasField(string sFieldName); 

	/**  
	 * @brief returns the table name
	 */
	string get_table_name();

        /* 
	*      @brief return a pointer to db 
	*/ 
	MYSQL *db() { return m_pDB; } 


	virtual ~TableInfo() {}
};

#endif
