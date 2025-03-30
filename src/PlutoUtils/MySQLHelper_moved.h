/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file MySQLHelper_moved.h
 Header file for ??? .
 */
#ifndef MySqlHelper_h
#define MySqlHelper_h

#include <mysql.h>
#include "DCE/Logger.h"
#include "PlutoUtils/MultiThreadIncludes.h"

using namespace DCE;


/** @class PlutoSqlResult
 Class for ???

Use this to be sure the results are automatically freed
*/
class PlutoSqlResult
{
public:
	MYSQL_RES *r;
	PlutoSqlResult() : r(NULL) {};
	~PlutoSqlResult()
	{
		ClearResults();
	}
	void ClearResults()
	{
		if(NULL != r)
		{
			mysql_free_result(r);
			r = NULL;
		}
	}
};

/** @class MySqlHelper

 You can add this to a base class to get some helper mysql functions.
 */
class MySqlHelper
{
private:
	bool m_bConnectFromConstructor;

public:
	pluto_pthread_mutex_t m_MySqlMutex;
    pthread_mutexattr_t m_MutexAttr;
	MYSQL *m_pMySQL;
	bool m_bConnected;
	string m_sMySQLHost,m_sMySQLUser,m_sMySQLPass,m_sMySQLDBName;
	int m_iMySQLPort;

	MySqlHelper()
		: m_MySqlMutex("mysql")
	{
		m_pMySQL=NULL;
		m_bConnected=false;

		pthread_mutexattr_init( &m_MutexAttr );
		pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
		m_MySqlMutex.Init(&m_MutexAttr);
		m_bConnectFromConstructor = false;
	}

	MySqlHelper(string host, string user, string pass, string db_name, int port=3306)
		: m_MySqlMutex("mysql")
	{
		pthread_mutexattr_init( &m_MutexAttr );
		pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
		m_MySqlMutex.Init(&m_MutexAttr);

		m_pMySQL=NULL;
		m_sMySQLHost=host;
		m_sMySQLUser=user;
		m_sMySQLPass=pass;
		m_sMySQLDBName=db_name;
		m_iMySQLPort=port;
		m_bConnectFromConstructor = true;
		MySQLConnect();
	}

	virtual ~MySqlHelper()
	{
		if( m_pMySQL )
			mysql_close(m_pMySQL);
		m_pMySQL=NULL;

		pthread_mutexattr_destroy(&m_MutexAttr);
		pthread_mutex_destroy(&m_MySqlMutex.mutex);
	}

	bool MySQLConnect(string host, string user, string pass, string db_name, int port=3306,bool bReset=false)
	{
		m_sMySQLHost=host;
		m_sMySQLUser=user;
		m_sMySQLPass=pass;
		m_sMySQLDBName=db_name;
		m_iMySQLPort=port;
		m_bConnectFromConstructor = true;
		return MySQLConnect(bReset);
	}

	bool MySQLConnect(bool bReset=false)
	{
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_MySqlMutex);
		if( bReset && m_pMySQL )
		{

				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Resetting mysql connection");
			mysql_close(m_pMySQL);
			m_pMySQL=NULL;
		}
		if( !m_pMySQL )
		{
			m_pMySQL = mysql_init(NULL);
		}

// commented since it can taint the Confirm dependencies output.
// 		if  ( m_bConnectFromConstructor )
// 			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MysqlHelper connecting to: %s, Port: %d, Database: %s", m_sMySQLHost.c_str(), m_iMySQLPort, m_sMySQLDBName.c_str());
// 		else
// 			LoggerWrapper::GetInstance()->Write(LV_WARNING, "MysqlHelper reconnecting to failed Host: %s, Port: %d, Database: %s", m_sMySQLHost.c_str(), m_iMySQLPort, m_sMySQLDBName.c_str());
		bool reconnect=1;
		mysql_options(m_pMySQL, MYSQL_OPT_RECONNECT,&reconnect);
		if (mysql_real_connect(m_pMySQL, m_sMySQLHost.c_str(), m_sMySQLUser.c_str(), m_sMySQLPass.c_str(), m_sMySQLDBName.c_str(), m_iMySQLPort, NULL, 0) == NULL)
		{

				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Connect failed %s",mysql_error(m_pMySQL));
			m_bConnected=false;
		}
		else
			m_bConnected=true;

		m_bConnectFromConstructor = false;
		return m_bConnected;
	}

	void SetConnection(MYSQL *pMySQL)
	{
		m_pMySQL = pMySQL;
		m_bConnected = true;
	}

	string md5(string Input)
	{
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( (result_set.r=mysql_query_result("SELECT md5('" + Input +"')"))==NULL || (row = mysql_fetch_row(result_set.r))==NULL )
			return "";

		return row[0];
	}

	MYSQL_RES *mysql_query_result(string query,bool Retry=false)
	{
	#ifdef DEBUG
		clock_t cStart = clock();
	#endif
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_MySqlMutex);
		int iresult;
		if( (iresult=mysql_query(m_pMySQL,query.c_str()))!=0 )
		{

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s (%d)",mysql_error(m_pMySQL),query.c_str(),iresult);
			if( MySQLConnect(true) && Retry==false )
			{
				MYSQL_RES *p = mysql_query_result(query,true);  // Try a second time if we were able to reconnect
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s retry: %p",mysql_error(m_pMySQL),query.c_str(),p);
				return p;
			}
			return NULL;
		}
	//	else
	//		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Query succeeded: %s (%d)",query.c_str(),iresult);

	//	LoggerWrapper::GetInstance()->Write(LV_STATUS,"store result: %s",query.c_str());

	#ifdef DEBUG
		clock_t cStop = clock();
		if( cStop-cStart>CLOCKS_PER_SEC/2 )  // Nothing should take 500 ms
		{

				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		} else if( cStop-cStart>CLOCKS_PER_SEC/10 )
		{

				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		}
	#endif

		MYSQL_RES *pMYSQL_RES = mysql_store_result(m_pMySQL);
#ifdef LOG_ALL_QUERIES

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Query returned %d rows: %s",(int) (pMYSQL_RES ? pMYSQL_RES->row_count : 0), query.c_str());
#endif
		return pMYSQL_RES;
	}

	int threaded_mysql_query(string query,bool bIgnoreErrors=false,bool Retry=false)
	{
	#ifdef DEBUG
		clock_t cStart = clock();
	#endif
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_MySqlMutex);
		int iresult;
		if( (iresult=mysql_query(m_pMySQL,query.c_str()))!=0 )
		{
			if( bIgnoreErrors )
				return -1;

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s (%d)",mysql_error(m_pMySQL),query.c_str(),iresult);
			if( MySQLConnect(true) && Retry==false )
			{
				int i = threaded_mysql_query(query,bIgnoreErrors,true);  // Try a second time if we were able to reconnect
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s retry: %d",mysql_error(m_pMySQL),query.c_str(),i);
				return i;
			}
			return -1;
		}

	#ifdef DEBUG
		clock_t cStop = clock();
		if( cStop-cStart>CLOCKS_PER_SEC/2 )  // Nothing should take 500 ms
		{

				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		} else if( cStop-cStart>CLOCKS_PER_SEC/10 )
		{

				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		}
	#endif
#ifdef LOG_ALL_QUERIES

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Executed query %s (%d rows)", query.c_str(), (int) mysql_affected_rows(m_pMySQL));
#endif
		return (int) mysql_affected_rows(m_pMySQL);
	}

	int threaded_mysql_query_withID(string query,bool Retry=false)
	{
	#ifdef DEBUG
		clock_t cStart = clock();
	#endif
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_MySqlMutex);
		int iresult;
		if( (iresult=mysql_query(m_pMySQL,query.c_str()))!=0 )
		{

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s (%d)",mysql_error(m_pMySQL),query.c_str(),iresult);
			if( MySQLConnect(true) && Retry==false )
			{
				int i = threaded_mysql_query_withID(query,true);  // Try a second time if we were able to reconnect
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s retry: %d",mysql_error(m_pMySQL),query.c_str(),i);
				return i;
			}
			return 0;
		}

	#ifdef DEBUG
		clock_t cStop = clock();
		if( cStop-cStart>CLOCKS_PER_SEC/2 )  // Nothing should take 500 ms
		{

				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		} else if( cStop-cStart>CLOCKS_PER_SEC/10 )
		{

				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		}
	#endif
#ifdef LOG_ALL_QUERIES

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Query has ID %d: %s", (int) mysql_insert_id(m_pMySQL), query.c_str());
#endif
		return (int) mysql_insert_id(m_pMySQL);
	}
};
#endif
