#ifdef WIN32
	#include <winsock.h>
#endif
#include <iostream>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_lmce_game.h"


#include "DCEConfig.h"


using namespace DCE;
Database_lmce_game::Database_lmce_game(Logger *pLogger)
{
	LoggerWrapper::SetInstance(pLogger);
tblGameSystem=NULL;
tblRom=NULL;
tblRomAttribute=NULL;
tblRomAttributeType=NULL;
tblRom_RomAttribute=NULL;
}

Database_lmce_game::~Database_lmce_game()
{
	DeleteAllTables();
}

bool Database_lmce_game::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblGameSystem!=NULL )
	if( !Commit_GameSystem(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRom!=NULL )
	if( !Commit_Rom(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRomAttribute!=NULL )
	if( !Commit_RomAttribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRomAttributeType!=NULL )
	if( !Commit_RomAttributeType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRom_RomAttribute!=NULL )
	if( !Commit_Rom_RomAttribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_lmce_game::DeleteAllTables()
{
DeleteTable_GameSystem();
DeleteTable_Rom();
DeleteTable_RomAttribute();
DeleteTable_RomAttributeType();
DeleteTable_Rom_RomAttribute();
}

bool Database_lmce_game::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_lmce_game::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
