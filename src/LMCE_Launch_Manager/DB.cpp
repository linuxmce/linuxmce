#include "DB.h"

DB::DB()
{
	m_bConnected = false;
	m_pConnection = NULL;
}
DB::~DB()
{

}
bool DB::connect(string sHost, string sUser, string sPass, string sDatabase)
{
	mysql_init(&m_mysqlInit);
	my_bool reconnect = true;
	mysql_options(&m_mysqlInit, MYSQL_OPT_RECONNECT, &reconnect);
	if ( m_pConnection = mysql_real_connect(&m_mysqlInit,sHost.c_str(),sUser.c_str(),sPass.c_str(),sDatabase.c_str(),0,0,0) ) {
		m_bConnected = true;
		
		// Save parameters for possible reconnect.
                m_sHost=sHost;
	        m_sUser=sUser;
		m_sPass=sPass;
		m_sDatabase=sDatabase;
		
		return true;
	} else {
		m_bConnected = false;
		return false;
	}
	
}

void DB::reconnect()
{
  connect(m_sHost,m_sUser,m_sPass,m_sDatabase);
}

void DB::close()
{
	if( m_pConnection )
		mysql_close(m_pConnection);
	//mysql_free_result(m_pResult);
	m_pConnection=NULL;
	m_bConnected = false;
}
bool DB::connected()
{
	if( m_pConnection && (mysql_ping(m_pConnection) != 0) )
		close();
	return m_bConnected;
}
DBResult DB::query(string sQuery)
{
	DBResult result;

	if (sQuery.empty())
	  {
	    result.m_pResult=NULL;
	    return result;
	  }

	if (!m_pConnection)
	  {
	    reconnect();
	    if (m_pConnection)
	      {
		mysql_query(m_pConnection,sQuery.c_str());
		result.m_pResult = mysql_store_result(m_pConnection);
	      }
	    else
	      {
		result.m_pResult=NULL;
	      }
	  }
	else
	  {
	    mysql_query(m_pConnection,sQuery.c_str());
	    result.m_pResult = mysql_store_result(m_pConnection);
	  }

	return result;
}
string DB::quickQuery(string sQuery) 
{
	string sResult="";
	DBResult result;
	result=query(sQuery);
	if ( result.next() ) {
		sResult = result.value(0);
	}

	return sResult;	
}
DBResult::DBResult()
{

}
DBResult::~DBResult()
{

}
bool DBResult::next()
{
	if (m_mysqlRow = mysql_fetch_row(m_pResult)) {
		return true;
	} else {
		return false;
	}

}
string DBResult::value(int i)
{
	return m_mysqlRow[i];
}
