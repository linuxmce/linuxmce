#include "StdAfx.h"
#include ".\Database.h"
#include "Log.h"

DatabaseWrapper::DatabaseWrapper()
{
	HRESULT hr;

	try
	{
		hr = m_pConnect.CreateInstance(__uuidof(Connection));
		hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	}
	catch(_com_error& ce)
	{
		Log::m_pLog->writeLine( "Database constructor catch");
		errDesc = (char *) ce.Description();
	}
}

DatabaseWrapper::~DatabaseWrapper(void)
{
	close();
}

void DatabaseWrapper::close()
{
	HRESULT hr;

	try
	{
		hr = m_pConnect->Close();
	}
	catch( _com_error &ce )
	{
		Log::m_pLog->writeLine( "Database close catch");
	}
}

bool DatabaseWrapper::connect(string dns)
{	
	HRESULT hr;

	try
	{
		hr = m_pConnect->Open( dns.c_str(), "" , "", 0 );
		if( FAILED(hr) )
			return false;
	}
	catch(_com_error& ce)
    {
		Log::m_pLog->writeLine( "Database connect catch");
		errDesc = (char *) ce.Description();
		return false;
    }
	return true;
}

bool DatabaseWrapper::executeSql(string szCommand,int &afectedRows,bool bReturn)
{
	variant_t var;
	CComBSTR sqlCommand(szCommand.size(),(LPCSTR) szCommand.c_str() );
	HRESULT hr;

	afectedRows = 0;
	try
	{
		if( bReturn )
			m_pRecordset = m_pConnect->Execute( sqlCommand.operator BSTR(), &var, adOptionUnspecified );
		else
		{
			m_pConnect->Execute( sqlCommand.operator BSTR(), &var, adExecuteNoRecords );
			m_pRecordset = NULL;
		}

		if(  var.iVal > 0 && bReturn )
			hr = m_pRecordset->MoveFirst();
		else
			m_pRecordset = NULL;
	}
	catch(_com_error& ce)
    {
		Log::m_pLog->writeLine( "Database executeSql catch");
		errDesc = (char *) ce.Description();
		return false;
    }

	afectedRows = var.iVal;
	return true;
}

string DatabaseWrapper::getFieldValue(string fieldName)
{
	int nCount = 0;
	_variant_t name = fieldName.c_str();
	
	_variant_t value;
	_bstr_t   retVal;
	char aux[20];

	if( m_pRecordset == NULL )
		return "";

	try
	{
		m_pFields = m_pRecordset->GetFields();
		nCount = m_pFields->GetCount();
		m_pField = m_pFields->GetItem( name );
		value = m_pField->GetValue();
	}
	catch(_com_error& ce)
    {
		Log::m_pLog->writeLine( "Database getFieldValue catch");
		errDesc = (char *) ce.Description();
		return false;
    }

	switch( value.vt )
	{
		case VT_I4:
		return string( itoa(value.iVal,aux,10) );

		case VT_BSTR:
		//rezVal.Attach( value.bstrVal );
		retVal = value.bstrVal;
		_stprintf(aux, _T("%s"), retVal.operator const char *() );
		return string( aux );
	}
}



///////////////////////Util functions
////////////////////////////////////////////////////////

bool DatabaseWrapper::constructInsert(string databaseName,vector<string> params,vector<string> values,
									vector<bool> isString,string &resSql)
{
	vector<string>::const_iterator it;
	vector<bool>::const_iterator it2;

	if( (params.size() != values.size()) || (params.size() != isString.size()) )
		return false;
	if( (params.size() == 0) && (values.size() == 0) ) 
		return false;

	resSql = "INSERT INTO " + databaseName + "(";
	for(it =params.begin();it!=params.end();it++)
		resSql += *it + ",";
	resSql[resSql.size()-1] = ')';

	resSql += " VALUES (";
	for(it=values.begin(),it2=isString.begin();it!=values.end();it++,it2++)
	{
		if( *it2 )
			resSql += "'" + *it + "',";
		else
			resSql += *it + ",";
	}
	resSql[resSql.size()-1] = ')';
	
	return true;
}

bool DatabaseWrapper::constructSelect(string databaseName,string condition,string &resSql)
{
	resSql = string("SELECT * FROM ") + databaseName.c_str() + " ";
	resSql += string("WHERE ") + condition.c_str();
	return true;
}

bool DatabaseWrapper::constructUpdate(string databaseName,string condition,
									  string update,string &resSql)
{
	resSql = string("UPDATE ") + databaseName.c_str() + " ";
	resSql += string("Set ") + update.c_str() + " ";
	resSql += string("WHERE ") + condition.c_str();

	return true;
}
