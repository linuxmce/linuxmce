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
 * @file DatabaseInfo.h
 * @brief header file for the request processor class
 *
 */


#ifndef DATABASEINFO_h
#define DATABASEINFO_h

#include "mysql.h"

#include <iostream>
using namespace std;

#ifndef GENERATOR
#define GENERATOR
#endif

#ifdef GENERATOR
	#include "TableInfo_Generator.h"
	#define TABLEINFO_TYPE TableInfo_Generator
#else
	#include "TableInfo_SCVS.h"
	#define TABLEINFO_TYPE TableInfo_SCVS
#endif

class DatabaseInfo
{
	vector<class TABLEINFO_TYPE *> m_listTableInfo; /** < table info list */
	MYSQL *m_pDB; /** < the database */
	map<string,TableInfo_Generator *> map_tables_info; /** < for faster lookups */

public:
	bool m_bConnected;

	/** @brief constructor */
	DatabaseInfo( string db_host, string db_user, string db_pass, string db_name, int db_port )
	{
		// Establishing database connection			  	
		bool reconnect = true;
		m_pDB = mysql_init(NULL);
		mysql_options(m_pDB, MYSQL_OPT_RECONNECT, &reconnect);
		if ( mysql_real_connect( m_pDB, db_host.c_str(), db_user.c_str(), db_pass.c_str(), db_name.c_str(), db_port, NULL, 0 ) == NULL )
		{
			cout << "MySQL connection failed" << endl;
			m_bConnected=false;
			return;
		}
		cout << "Connection established!" << endl;
		m_bConnected=true;
	}

	/** @brief destructor */
	~DatabaseInfo() { mysql_close(m_pDB); }

	void getDatabaseTablesList();

	/** @brief for acces to private member */
	vector<class TABLEINFO_TYPE *> *listTableInfo_get() { return &m_listTableInfo; }

	/** @brief puts the table names in the parameter vector */
	void ConvertTablesToStrings(vector<string> *vect_string);
};

#endif
