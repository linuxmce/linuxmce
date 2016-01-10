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
#ifndef __ServerManagement_h_
#define __ServerManagement_h_

#include "DCE/PlainClientSocket.h"
#include "MapManagement.h"
#include <map>

/** Singelton class that populate the maps for the server*/
class ServerManagement{
	
public:
	static ServerManagement* getInstance();
	static void* ServerManagement_Thread(void *p);
	
	~ServerManagement();

private:

	ServerManagement();
	void Run();
	
	static ServerManagement* instance;
	pthread_mutexattr_t m_MutexAttr;
	pluto_pthread_mutex_t TribuneMutex;
	map<string,string> maptmpProgramRecord;
	map<int,string> maptmpStation;
	map<u_int64_t,string> maptmpSchedule;
	map<string,string> maptmpActor;
	map<string,string> maptmpGenre;
	map<string,string> maptmpRole;
	map<string,string> maptmpActorRole;

};

#endif
