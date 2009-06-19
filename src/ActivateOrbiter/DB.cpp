#include "DB.h"

DB::DB()
{
	m_bConnected = false;
}
DB::~DB()
{

}
bool DB::connect(string sHost, string sUser, string sPass, string sDatabase)
{
	mysql_init(&m_mysqlInit);
	if ( m_pConnection = mysql_real_connect(&m_mysqlInit,sHost.c_str(),sUser.c_str(),sPass.c_str(),sDatabase.c_str(),0,0,0) ) {
		m_bConnected = true;
		return true;
	} else {
		m_bConnected = false;
		return false;
	}
	
}
void DB::close()
{
	mysql_close(m_pConnection);
	//mysql_free_result(m_pResult);
	m_pConnection=NULL;
	m_bConnected = false;
}
bool DB::connected()
{
	//TODO: Actually test connection to make sure it is still there	
	return m_bConnected;
}
DBResult DB::query(string sQuery)
{
	DBResult result;
	
	mysql_query(m_pConnection,sQuery.c_str());
	result.m_pResult = mysql_store_result(m_pConnection);
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
