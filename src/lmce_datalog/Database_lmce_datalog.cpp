#ifdef WIN32
	#include <WinSock2.h>
#endif
#include <iostream>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_lmce_datalog.h"


#include "DCEConfig.h"


using namespace DCE;
Database_lmce_datalog::Database_lmce_datalog(Logger *pLogger)
{
	LoggerWrapper::SetInstance(pLogger);
tblDatapoints=NULL;
tblUnit=NULL;
}

Database_lmce_datalog::~Database_lmce_datalog()
{
	DeleteAllTables();
}

bool Database_lmce_datalog::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblDatapoints!=NULL )
	if( !Commit_Datapoints(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUnit!=NULL )
	if( !Commit_Unit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_lmce_datalog::DeleteAllTables()
{
DeleteTable_Datapoints();
DeleteTable_Unit();
}

bool Database_lmce_datalog::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_lmce_datalog::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
