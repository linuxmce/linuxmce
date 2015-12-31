#pragma once

#include <windows.h>

#include "string"
#include "vector"
using namespace std;

#include "Contacts.h"

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "EndOfFile")

class DatabaseWrapper
{
private:
	_ConnectionPtr m_pConnect;
	_RecordsetPtr  m_pRecordset;
	FieldsPtr m_pFields;
	FieldPtr m_pField;

	string errDesc;

public:
	DatabaseWrapper();
	~DatabaseWrapper(void);

	bool connect(string dns);

	bool constructInsert(string databaseName,vector<string> param,vector<string> values,
					vector<bool> isString,string &resSql);
	bool constructSelect(string databaseName,string condition,string &resSql);
	bool constructUpdate(string databaseName,string condition,string update,string &resSql);

	bool executeSql(string szCommand,int &afectedRows,bool bReturn=true);
	string getFieldValue(string fielName);

	void close();

	bool writeContacts(ContactsList &list);
};
