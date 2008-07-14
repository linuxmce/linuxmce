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
#ifndef TimedEvent_H
#define TimedEvent_H

#define	INTERVAL_EVENT	1
#define DAY_OF_WEEK		2
#define DAY_OF_MONTH	3
#define ABSOLUTE_TIME	4


#include "DCERouter/DataLayer_JSON.h"
//----------------------------------------------------------------------------------------------
extern "C"
{
	#include "json.h" 
	#include "bits.h"
	#include "debug.h"
	#include "linkhash.h"
	#include "arraylist.h"
	#include "json_util.h"
	#include "json_object.h"
	#include "json_object_private.h"
	#include "json_tokener.h" 
}

namespace DCE
{
	class Event_Plugin;
};

using namespace DCE;

class TimedEvent
{
public:
	int m_ID;
	string m_sDescription;
	map<int, Command_Data> m_mapCommands;
	DataLayer_JSON *m_pDataLayer_JSON;

	time_t m_tTime;
	int m_iTimedEventType;
	
	string m_sDaysOfWeek,m_sTimes,m_sDaysOfMonth;

	TimedEvent(int ID, struct json_object *json_obj,Event_Plugin *pEvent_Plugin);
	void ParseCriteria(struct json_object *json_obj);
	void CalcNextTime();

    /**
		Finds the next time in sTimes that is after tmAfter, or the first time in sTimes if tmAfter is NULL,
		and stores it in tmOutput
        */
	bool SetNextTime(tm *tmAfter,tm *tmOutput,string sTimes);
};

typedef map<int,TimedEvent *> MapTimedEvent;

#endif

