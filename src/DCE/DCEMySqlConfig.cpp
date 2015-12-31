/* 
	DCEMySqlConfig
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU Lesser General Public License for more details.
*/

#include "DCE/DCEMySqlConfig.h"
//------------------------------------------------------------------------------------------------------
#include <iostream>
using namespace std;
//------------------------------------------------------------------------------------------------------
DCEMySqlConfig::DCEMySqlConfig(string sFilename) : 
	DCEConfig(sFilename), m_MySqlMutex("MYSQL"), m_iTransNumber(0)
{
	m_MySqlMutex.Init(NULL);

	MysqlConnect(false);
}
//------------------------------------------------------------------------------------------------------
/*virtual*/ DCEMySqlConfig::~DCEMySqlConfig()
{
	db_wrapper_close(&m_MySQL);
}
//------------------------------------------------------------------------------------------------------
bool DCEMySqlConfig::MysqlConnect(bool bReset)
{
	PLUTO_SAFETY_LOCK(ms, m_MySqlMutex);

	if( bReset )
		db_wrapper_close(&m_MySQL);

	db_wrapper_init(&m_MySQL);
	if(! db_wrapper_real_connect(&m_MySQL, m_sDBHost.c_str(), m_sDBUser.c_str(), m_sDBPassword.c_str(), 
		m_sDBName.c_str(), 0, NULL,0)
	)
	{
		cout << "Failed to connect to MySQL:" << db_wrapper_error(&m_MySQL) << endl; 
		return false;
	} 

	return true;
}
//------------------------------------------------------------------------------------------------------
DB_RES *DCEMySqlConfig::MySqlQueryResult(string sQuery)
{
	PLUTO_SAFETY_LOCK(sl, m_MySqlMutex);

	long iResult;
	if( (iResult = db_wrapper_query(&m_MySQL, sQuery.c_str())) != 0 )
	{
		sl.Release();
		cout << "Query failed" << db_wrapper_error(&m_MySQL) << sQuery.c_str() << iResult << endl;
		MysqlConnect(true);
		return NULL;
	}

	return db_wrapper_store_result(&m_MySQL);
}
//------------------------------------------------------------------------------------------------------
long DCEMySqlConfig::ThreadedMysqlQuery(string sQuery, bool bIgnoreErrors/*= false*/)
{
	PLUTO_SAFETY_LOCK(sl,m_MySqlMutex);

	long iResult;
	if( (iResult = db_wrapper_query(&m_MySQL, sQuery.c_str())) != 0 )
	{
		sl.Release();
		if( bIgnoreErrors )
			return -1;

		cout << "Query failed " << db_wrapper_error(&m_MySQL) << sQuery << iResult << endl;
		MysqlConnect(true);
	}

	return iResult;
}
//------------------------------------------------------------------------------------------------------
long DCEMySqlConfig::ThreadedMysqlQueryWithID(string sQuery)
{
	PLUTO_SAFETY_LOCK(sl,m_MySqlMutex);

	long iResult;
	if( (iResult = db_wrapper_query(&m_MySQL, sQuery.c_str())) != 0 )
	{
		sl.Release();

		cout << "Query failed " << db_wrapper_error(&m_MySQL) << sQuery << iResult << endl; 
		MysqlConnect(true);
		return 0;
	}

	return (long) db_wrapper_insert_id(&m_MySQL);
}
//------------------------------------------------------------------------------------------------------
