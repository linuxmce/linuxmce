#pragma once


#include "Database.h"
#include "Contacts.h"

class PlutoDatabase
{
private:
	DatabaseWrapper *m_pContactsTable, *m_pPhoneTable;

	string m_ServerName, m_DatabaseName;
	string m_User,m_Password;
	string m_ConnStr;

	int m_nUpdateRows;

	// ContactTable
	vector<string> m_ContactParam, m_ContactValues;
	vector<bool> m_ContactType;

	// PhoneTable
	vector<string> m_PhoneParam, m_PhoneValues;
	vector<bool> m_PhoneType;

	bool writeContact(Contact *pContact);
	bool updatesPhoneNumbers(Contact *pContact,int nUserId);
	bool writePhoneNumber(string phoneNumber,int nType,int nUserID);

	// if exist return the id of Contact
	int existContact(Contact *pContact);

public:
	PlutoDatabase(void);
	~PlutoDatabase(void);

	string getServerName(){ return m_ServerName; }
	string getDatabaseName(){ return m_DatabaseName; }
	string getUser(){ return m_User; }
	string getPassword(){ return m_Password; }
	string getConnectionString(){ return m_ConnStr; }

	int getUpdateRowsNo(){ return m_nUpdateRows; }

	void setConnectionString(string serverName,string database,
				string user,string password);
	void setConnectionString();

	bool connect();
	int writeContacts(ContactsList &list);
};
