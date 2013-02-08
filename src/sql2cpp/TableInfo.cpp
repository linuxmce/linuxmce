/*
	TableInfo
	
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
 * @file TableInfo.cpp
 * @brief source file for the table info class
 *
 */


#include "PlutoUtils/CommonIncludes.h"
#include "TableInfo.h"

bool TableInfo::loadFromDB( string sTable )
{
	m_sTableName = sTable;
	
	MYSQL_RES *pRes = mysql_list_fields( m_pDB, m_sTableName.c_str(), NULL );
	if ( pRes != NULL )
	{		
		int iFieldsCount = mysql_num_fields( pRes );
			
		MYSQL_FIELD *pMysqlFields = mysql_fetch_fields( pRes );
			
		for ( int i=0; i < iFieldsCount; i++ )
			m_Fields.push_back( new FieldInfo( pMysqlFields + i ) );
		
		mysql_free_result( pRes );
		
		return true;
	}
	else
		return false;	
}

bool TableInfo::HasField(string sFieldName)
{
	for ( vector<FieldInfo*>::iterator i=m_Fields.begin(); i!=m_Fields.end(); i++ )
		if(sFieldName == (*i)->m_pcFieldName)
			return true;

	return false; 
}

bool TableInfo::HasPrimaryKeys()
{
	for ( vector<FieldInfo*>::iterator i=m_Fields.begin(); i!=m_Fields.end(); i++ )
		if ( (*i)->m_iFlags & PRI_KEY_FLAG )
			return true;
	return false;
}

string TableInfo::get_table_name()
{
	return m_sTableName;
}

