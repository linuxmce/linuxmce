/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "ServerManagement.h"

#include "CommonFunctions.h"
//#include "Database.h"
//#include "MapManagement.h"
#include <iostream>
#include <sstream>
#include <map>
#include <string>

using namespace std;
using namespace Tribune;

ServerManagement* ServerManagement::instance = NULL;

ServerManagement::ServerManagement()
	: TribuneMutex("TribuneMapMutex")
{
	pthread_mutexattr_init( &m_MutexAttr );
	pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	TribuneMutex.Init( &m_MutexAttr );
}

ServerManagement::~ServerManagement()
{
}


ServerManagement* ServerManagement::getInstance(){
	
	if (instance==NULL){
		instance = new ServerManagement();
	}
	return instance;
}

void* ServerManagement::ServerManagement_Thread(void *p){

	ServerManagement *pServerManagement = (ServerManagement *) p;
	if( pServerManagement != NULL )
	{
		pServerManagement->Run();
	}
	
	return NULL;
}

void ServerManagement::Run(){
	
	/** Populate the server maps*/
	/** First the infos are stored in some temp maps because the pluto lock will last just 30 seconds
	and importing the maps takes much longer*/
	while (true){
		
		if(g_GlobalConfig.m_bImportTable){

			cout << "Fill the maps" << endl;

			if (! MapManagement::GetProgramRecordMap(maptmpProgramRecord) ){
		
				cerr << "Cannot fill the program record map " << endl;
			}
			cout << "ProgramRecord " << maptmpProgramRecord.size() <<endl;

			if (! MapManagement::GetStationMap(maptmpStation) ){

				cerr << "Cannot fill the station map " << endl;
			}

			cout << "Station " << maptmpStation.size() <<endl;
		
			/** this must not be commented after the serialization bug is fixed*/

			if (! MapManagement::GetScheduleMap(maptmpSchedule) ){
			
				cerr << "Cannot fill the schedule map: " << endl;
			}

			cout << "Schedule " << maptmpSchedule.size() <<endl;

			if (! MapManagement::GetActorMap(maptmpActor) ){
		
				cerr << "Cannot fill the actor map " << endl;
			}
			cout << "Actor " << maptmpActor.size() <<endl;

			if (! MapManagement::GetGenreMap(maptmpGenre) ){

				cerr << "Cannot fill the genre map " << endl;
			}

			cout << "Genre " << maptmpGenre.size() <<endl;

			if (! MapManagement::GetRoleMap(maptmpRole) ){
			
				cerr << "Cannot fill the role map: " << endl;
			}

			cout << "Role " << maptmpRole.size() <<endl;

			if (! MapManagement::GetActorRoleMap(maptmpActorRole) ){
		
				cerr << "Cannot fill the actor_role map " << endl;
			}
			cout << "Actor_Role " << maptmpActorRole.size() <<endl;


			PLUTO_SAFETY_LOCK(tb,TribuneMutex);

			(g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord).clear(); 
			(g_GlobalConfig.mapPrimaryKey_Timestam_Station).clear(); 
			/** this must not be commented after the serialization bug is fixed*/
			(g_GlobalConfig.mapPrimaryKey_Timestam_Schedule).clear();
			(g_GlobalConfig.mapPrimaryKey_Actor).clear(); 
			(g_GlobalConfig.mapPrimaryKey_Genre).clear(); 
			(g_GlobalConfig.mapPrimaryKey_Role).clear();
			(g_GlobalConfig.mapPrimaryKey_ActorRole).clear();

			g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord=maptmpProgramRecord;
			g_GlobalConfig.mapPrimaryKey_Timestam_Station=maptmpStation;
			/** this must not be commented after the serialization bug is fixed*/
			g_GlobalConfig.mapPrimaryKey_Timestam_Schedule=maptmpSchedule;
			g_GlobalConfig.mapPrimaryKey_Actor=maptmpActor;
			g_GlobalConfig.mapPrimaryKey_Genre=maptmpGenre;
			g_GlobalConfig.mapPrimaryKey_Role=maptmpRole;
			g_GlobalConfig.mapPrimaryKey_ActorRole=maptmpActorRole;

			maptmpProgramRecord.clear(); maptmpStation.clear(); maptmpSchedule.clear();
			maptmpActor.clear(); maptmpGenre.clear(); maptmpRole.clear(); maptmpActorRole.clear();

			
	cout << ">>>>>>>>In server management : " << endl;
	cout << "Program record size " << (g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord).size()<<endl; 
        cout << "Station size "<< (g_GlobalConfig.mapPrimaryKey_Timestam_Station).size()<<endl;
	cout << "Schedule size "<<(g_GlobalConfig.mapPrimaryKey_Timestam_Schedule).size()<<endl;
	cout << "Actor size " <<(g_GlobalConfig.mapPrimaryKey_Actor).size()<<endl;
	cout << "Genre sieze " <<(g_GlobalConfig.mapPrimaryKey_Genre).size()<<endl;
	cout << "Role size " <<(g_GlobalConfig.mapPrimaryKey_Role).size()<<endl;
	cout << "Actor_Role size " <<(g_GlobalConfig.mapPrimaryKey_ActorRole).size()<<endl;

			g_GlobalConfig.m_bImportTable=false;
		}
		
		Sleep(10);
	}
}
