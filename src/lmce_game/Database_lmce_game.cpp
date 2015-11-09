#ifdef WIN32
	#include <WinSock2.h>
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
tblConfiguration=NULL;
tblGameSystem=NULL;
tblRom=NULL;
tblRomAttribute=NULL;
tblRomAttributeType=NULL;
tblRomTitles=NULL;
tblRom_Configuration=NULL;
tblRom_RomAttribute=NULL;
tblpsc_game_batdet=NULL;
tblpsc_game_bathdr=NULL;
tblpsc_game_batuser=NULL;
tblpsc_game_repset=NULL;
tblpsc_game_schema=NULL;
tblpsc_game_tables=NULL;
}

Database_lmce_game::~Database_lmce_game()
{
	DeleteAllTables();
}

bool Database_lmce_game::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblConfiguration!=NULL )
	if( !Commit_Configuration(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
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
if( tblRomTitles!=NULL )
	if( !Commit_RomTitles(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRom_Configuration!=NULL )
	if( !Commit_Rom_Configuration(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRom_RomAttribute!=NULL )
	if( !Commit_Rom_RomAttribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_game_batdet!=NULL )
	if( !Commit_psc_game_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_game_bathdr!=NULL )
	if( !Commit_psc_game_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_game_batuser!=NULL )
	if( !Commit_psc_game_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_game_repset!=NULL )
	if( !Commit_psc_game_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_game_schema!=NULL )
	if( !Commit_psc_game_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_game_tables!=NULL )
	if( !Commit_psc_game_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_lmce_game::DeleteAllTables()
{
DeleteTable_Configuration();
DeleteTable_GameSystem();
DeleteTable_Rom();
DeleteTable_RomAttribute();
DeleteTable_RomAttributeType();
DeleteTable_RomTitles();
DeleteTable_Rom_Configuration();
DeleteTable_Rom_RomAttribute();
DeleteTable_psc_game_batdet();
DeleteTable_psc_game_bathdr();
DeleteTable_psc_game_batuser();
DeleteTable_psc_game_repset();
DeleteTable_psc_game_schema();
DeleteTable_psc_game_tables();
}

bool Database_lmce_game::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_lmce_game::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
