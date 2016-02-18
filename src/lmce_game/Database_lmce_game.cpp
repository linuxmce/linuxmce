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
tblGame=NULL;
tblGameSystem=NULL;
tblGameSystem_Picture=NULL;
tblGame_GameSystem=NULL;
tblGame_GameSystem_Configuration=NULL;
tblGame_GameSystem_Picture=NULL;
tblGame_GameSystem_Rom=NULL;
tblGame_GameSystem_Rom_Configuration=NULL;
tblGenre=NULL;
tblManufacturer=NULL;
tblNameHash=NULL;
tblPicture=NULL;
tblRom=NULL;
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
if( tblGame!=NULL )
	if( !Commit_Game(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblGameSystem!=NULL )
	if( !Commit_GameSystem(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblGameSystem_Picture!=NULL )
	if( !Commit_GameSystem_Picture(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblGame_GameSystem!=NULL )
	if( !Commit_Game_GameSystem(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblGame_GameSystem_Configuration!=NULL )
	if( !Commit_Game_GameSystem_Configuration(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblGame_GameSystem_Picture!=NULL )
	if( !Commit_Game_GameSystem_Picture(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblGame_GameSystem_Rom!=NULL )
	if( !Commit_Game_GameSystem_Rom(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblGame_GameSystem_Rom_Configuration!=NULL )
	if( !Commit_Game_GameSystem_Rom_Configuration(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblGenre!=NULL )
	if( !Commit_Genre(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblManufacturer!=NULL )
	if( !Commit_Manufacturer(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblNameHash!=NULL )
	if( !Commit_NameHash(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture!=NULL )
	if( !Commit_Picture(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRom!=NULL )
	if( !Commit_Rom(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_lmce_game::DeleteAllTables()
{
DeleteTable_Configuration();
DeleteTable_Game();
DeleteTable_GameSystem();
DeleteTable_GameSystem_Picture();
DeleteTable_Game_GameSystem();
DeleteTable_Game_GameSystem_Configuration();
DeleteTable_Game_GameSystem_Picture();
DeleteTable_Game_GameSystem_Rom();
DeleteTable_Game_GameSystem_Rom_Configuration();
DeleteTable_Genre();
DeleteTable_Manufacturer();
DeleteTable_NameHash();
DeleteTable_Picture();
DeleteTable_Rom();
}

bool Database_lmce_game::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_lmce_game::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
