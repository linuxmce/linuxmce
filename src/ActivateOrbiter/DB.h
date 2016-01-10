#ifndef DB_h
#define DB_h
//Simple  MySQL DB accessor class. Uses standard mysql library.
//jondecker76
#include <mysql.h>
#include <vector>
#include <string>

using namespace std;

class DBResult
{
private:
	
public:
	DBResult();
	~DBResult();
	bool next();
	string value(int i);

	MYSQL_RES *m_pResult; //TODO:these should be private with some accessor methods
	MYSQL_ROW m_mysqlRow;
	
};
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
		

public:
	//constructor
	DB();
	//destructor
	~DB();
	//connect to databse
	bool connect(string sHost, string sUser, string sPass, string sDatabase);
	void close();
	DBResult query(string sQuery);
	bool connected();
	string quickQuery(string sQuery);

};
#endif
