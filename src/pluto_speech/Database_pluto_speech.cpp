#ifdef WIN32
	#include <winsock.h>
#endif
#include <iostream>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_pluto_speech.h"


#include "DCEConfig.h"


using namespace DCE;
Database_pluto_speech::Database_pluto_speech(Logger *pLogger)
{
	LoggerWrapper::SetInstance(pLogger);
tblAlias=NULL;
tblKeyword=NULL;
}

Database_pluto_speech::~Database_pluto_speech()
{
	DeleteAllTables();
}

bool Database_pluto_speech::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblAlias!=NULL )
	if( !Commit_Alias(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblKeyword!=NULL )
	if( !Commit_Keyword(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_pluto_speech::DeleteAllTables()
{
DeleteTable_Alias();
DeleteTable_Keyword();
}

bool Database_pluto_speech::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_speech::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
