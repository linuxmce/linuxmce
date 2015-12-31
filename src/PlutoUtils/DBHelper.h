/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file DBHelper.h
 Header file
 */
#ifndef DBHelper_h
#define DBHelper_h

#include "db_wrapper/db_wrapper.h"
#include "DCE/Logger.h"
#include "PlutoUtils/MultiThreadIncludes.h"

#ifdef __APPLE_CC__
#define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
#endif

using namespace DCE;


/** @class PlutoSqlResult
Use this class to be sure the results are automatically freed.
*/
class PlutoSqlResult
{
public:
	DB_RES *r;
	PlutoSqlResult() : r(NULL) {};
	~PlutoSqlResult()
	{
		ClearResults();
	}
	void ClearResults()
	{
		if(NULL != r)
		{
			db_wrapper_free_result(r);
			r = NULL;
		}
	}
};

/** @class DBHelper
You can add this to a base class to get some helper db functions.
*/
//
class DBHelper
{
private:
	bool m_bConnectFromConstructor;

public:
	pluto_pthread_mutex_t m_DBMutex;
    pthread_mutexattr_t m_MutexAttr;
	DB_LINK *m_pDB;
	bool m_bConnected;
	string m_sDBHost,m_sDBUser,m_sDBPass,m_sDBDBName;
	int m_iDBPort;
	string m_sCharset;

    /** Default constructor.
    */
	DBHelper()
		: m_DBMutex("db_wrapper")
	{
		m_pDB=NULL;
		m_bConnected=false;
		m_sCharset="utf8";

		pthread_mutexattr_init( &m_MutexAttr );
		pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
		m_DBMutex.Init(&m_MutexAttr);
		m_bConnectFromConstructor = false;
	}

    /** Constructor.
    @param host is the host name for the database server.
    @param user is the username for the database.
    @param pass is the password for the database.
    @param db_name is the database name.
    @param port is the database port (default 3306).
    */
	DBHelper(string host, string user, string pass, string db_name, int port=3306, string charset="utf8")
		: m_DBMutex("db_wrapper")
	{
		pthread_mutexattr_init( &m_MutexAttr );
		pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
		m_DBMutex.Init(&m_MutexAttr);

		m_pDB=NULL;
		m_sDBHost=host;
		m_sDBUser=user;
		m_sDBPass=pass;
		m_sDBDBName=db_name;
		m_iDBPort=port;
		m_sCharset = charset;
		m_bConnectFromConstructor = true;
		DBConnect();
	}

    /** Destructor.
    */
	virtual ~DBHelper()
	{
		if( m_pDB )
			db_wrapper_close(m_pDB);
		m_pDB=NULL;

		pthread_mutexattr_destroy(&m_MutexAttr);
		pthread_mutex_destroy(&m_DBMutex.mutex);
	}

	bool DBConnect(string host, string user, string pass, string db_name, int port=3306,bool bReset=false)
	{
		m_sDBHost=host;
		m_sDBUser=user;
		m_sDBPass=pass;
		m_sDBDBName=db_name;
		m_iDBPort=port;
		m_bConnectFromConstructor = true;
		return DBConnect(bReset);
	}

	bool DBConnect(bool bReset=false)
	{
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_DBMutex);
		if( bReset && m_pDB )
		{

				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Resetting db_wrapper connection");
			db_wrapper_close(m_pDB);
			m_pDB=NULL;
		}
		if( !m_pDB )
		{
			m_pDB = db_wrapper_init(NULL);
		}

// commented since it can taint the Confirm dependencies output.
// 		if  ( m_bConnectFromConstructor )
// 			LoggerWrapper::GetInstance()->Write(LV_STATUS, "DBHelper connecting to: %s, Port: %d, Database: %s", m_sDBHost.c_str(), m_iDBPort, m_sDBDBName.c_str());
// 		else
// 			LoggerWrapper::GetInstance()->Write(LV_WARNING, "DBHelper reconnecting to failed Host: %s, Port: %d, Database: %s", m_sDBHost.c_str(), m_iDBPort, m_sDBDBName.c_str());

		if (db_wrapper_real_connect(m_pDB, m_sDBHost.c_str(), m_sDBUser.c_str(), m_sDBPass.c_str(), m_sDBDBName.c_str(), m_iDBPort, NULL, 0) == NULL)
		{

				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Connect failed %s",db_wrapper_error(m_pDB));
			m_bConnected=false;
		}
		else
			m_bConnected=true;
		if (db_wrapper_set_character_set(m_pDB, m_sCharset.c_str())) 
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to set character set to '%s': %s",m_sCharset.c_str(), db_wrapper_error(m_pDB));			
		}
		m_bConnectFromConstructor = false;
		return m_bConnected;
	}

	void SetConnection(DB_LINK *pDB)
	{
		m_pDB = pDB;
		m_bConnected = true;
	}

	string md5(string Input)
	{
		PlutoSqlResult result_set;
		DB_ROW row=NULL;
		if( (result_set.r=db_wrapper_query_result("SELECT md5('" + Input +"')"))==NULL || (row = db_wrapper_fetch_row(result_set.r))==NULL )
			return "";

		return row[0];
	}

	DB_RES *db_wrapper_query_result(string query,bool Retry=false)
	{
	#ifdef DEBUG
		clock_t cStart = clock();
	#endif
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_DBMutex);
		int iresult;
		if( (iresult=db_wrapper_query(m_pDB,query.c_str()))!=0 )
		{

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s (%d)",db_wrapper_error(m_pDB),query.c_str(),iresult);
			if( DBConnect(true) && Retry==false )
			{
				DB_RES *p = db_wrapper_query_result(query,true);  // Try a second time if we were able to reconnect
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s retry: %p",db_wrapper_error(m_pDB),query.c_str(),p);
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

		DB_RES *pDB_RES = db_wrapper_store_result(m_pDB);
#ifdef LOG_ALL_QUERIES

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Query returned %d rows: %s",(int) (pDB_RES ? pDB_RES->row_count : 0), query.c_str());
#endif
		return pDB_RES;
	}

	int threaded_db_wrapper_query(string query,bool bIgnoreErrors=false,bool Retry=false)
	{
	#ifdef DEBUG
		clock_t cStart = clock();
	#endif
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_DBMutex);
		int iresult;
		if( (iresult=db_wrapper_query(m_pDB,query.c_str()))!=0 )
		{
			if( bIgnoreErrors )
				return -1;

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s (%d)",db_wrapper_error(m_pDB),query.c_str(),iresult);
			if( DBConnect(true) && Retry==false )
			{
				int i = threaded_db_wrapper_query(query,bIgnoreErrors,true);  // Try a second time if we were able to reconnect
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s retry: %d",db_wrapper_error(m_pDB),query.c_str(),i);
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

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Executed query %s (%d rows)", query.c_str(), (int) db_wrapper_affected_rows(m_pDB));
#endif
		return (int) db_wrapper_affected_rows(m_pDB);
	}

	int threaded_db_wrapper_query_withID(string query,bool Retry=false)
	{
	#ifdef DEBUG
		clock_t cStart = clock();
	#endif
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_DBMutex);
		int iresult;
		if( (iresult=db_wrapper_query(m_pDB,query.c_str()))!=0 )
		{

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s (%d)",db_wrapper_error(m_pDB),query.c_str(),iresult);
			if( DBConnect(true) && Retry==false )
			{
				int i = threaded_db_wrapper_query_withID(query,true);  // Try a second time if we were able to reconnect
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Query failed (%s): %s retry: %d",db_wrapper_error(m_pDB),query.c_str(),i);
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

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Query has ID %d: %s", (int) db_wrapper_insert_id(m_pDB), query.c_str());
#endif
		return (int) db_wrapper_insert_id(m_pDB);
	}
};
#endif
