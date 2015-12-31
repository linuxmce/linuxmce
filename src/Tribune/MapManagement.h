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
#ifndef __MapManagement_h_
#define __MapManagement_h_

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

/** Class that contains methods that populates the maps from the database (key,timestamp) */

class MapManagement{
	
public:

	/** get program records from the database.*/
	static bool GetProgramRecordMap(map <string,string> &);

	/** get stations from the database.*/
	static bool GetStationMap(map<int,string>&);
	
	/** get schedule from the database.*/
	static bool GetScheduleMap(map<u_int64_t,string>&);
	
	/** get actors from the database.*/
	static bool GetActorMap(map<string,string>&);
	
	/** get genres from the database.*/
	static bool GetGenreMap(map<string,string>&);
	
	/** get maps from the database.*/
	static bool GetRoleMap(map<string,string>&);

	static bool GetActorRoleMap(map<string,string>&);

};

#endif
