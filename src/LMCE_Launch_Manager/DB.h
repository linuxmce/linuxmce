#ifndef DB_h
#define DB_h
//Simple  MySQL DB accessor class. Uses standard mysql library.
//jondecker76
#include "LMCE_Launch_Manager.h"
#include "Gen_Devices/LMCE_Launch_ManagerBase.h"
#include <mysql.h>
#include <vector>


class DB {
private:
	//vector m_vResults;
	bool m_bConnected;
	string m_sHost;
	string m_sUser;
	string m_sPass;
	string m_sPort;
	string m_sDatabase;

	MYSQL *m_pConnection;
	MYSQL m_mysqlInit;
	MYSQL_RES *m_pResult;
	MYSQL_ROW m_mysqlRow;	

public:
	//constructor
	DB();
	//destructor
	~DB();
	//connect to databse
	bool connect(string sHost, string sUser, string sPass, string sDatabase);
	void close();
	string query(string sQuery);
	bool next();
	bool connected();
	string value(int i);

};
#endif
