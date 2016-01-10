/*
	DatabaseInfo
	
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
 * @file DatabaseInfo.cpp
 * @brief source file for the DatabaseInfo class
 *
 */


#include "PlutoUtils/CommonIncludes.h"
#include "DatabaseInfo.h"

void DatabaseInfo::getDatabaseTablesList()
{
	MYSQL_RES *pRes = mysql_list_tables( m_pDB, NULL );
	
	if ( pRes != NULL )
	{
		MYSQL_ROW Row;
		
		while (( Row = mysql_fetch_row( pRes ) ))
		{
			// Each table needs a list of all the other tables so that it can find foreign keys pointing to itself
			TABLEINFO_TYPE *pInfo = new TABLEINFO_TYPE( m_pDB, &map_tables_info );

			if ( pInfo->loadFromDB( Row[0] ) )
			{
				m_listTableInfo.push_back( pInfo );
				map_tables_info[pInfo->get_table_name()] = pInfo;
			}
			else
			{
				delete pInfo;			
				cout << "Cannot load info for table: " << Row[0] << endl;
			}
		}
	}

	mysql_free_result( pRes );
}

void DatabaseInfo::ConvertTablesToStrings(vector<string> *vect_string)
{
	vector<class TABLEINFO_TYPE *>::iterator it;

	for( it = m_listTableInfo.begin(); it != m_listTableInfo.end(); ++it )
	{
		TableInfo *pTableInfo = *it;
		vect_string->push_back( pTableInfo->get_table_name() );
	}
}

