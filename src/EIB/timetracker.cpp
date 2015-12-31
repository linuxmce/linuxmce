/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Implementation: timetracker
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "timetracker.h"

namespace EIBBUS {

TimeTracker::TimeTracker(bool start)
{
	starttm_.tv_sec = starttm_.tv_nsec = 0;
	stoptm_.tv_sec = stoptm_.tv_nsec = 0;
	if(start) {
		Start();
	}
}

TimeTracker::~TimeTracker(){
}

void 
TimeTracker::Start() {
	clock_gettime(CLOCK_REALTIME, &starttm_);
}

void 
TimeTracker::Stop() {
	clock_gettime(CLOCK_REALTIME, &stoptm_);
}

double 
TimeTracker::getCurrentPeriodInMiliSecs() {
	struct timespec currtm;
	clock_gettime(CLOCK_REALTIME, &currtm);
	return (currtm.tv_sec - starttm_.tv_sec)* 1000 + 1.0 * (currtm.tv_nsec - starttm_.tv_nsec) / 1000000;
}

double 
TimeTracker::getCurrentPeriodInSecs() {
	struct timespec currtm;
	clock_gettime(CLOCK_REALTIME, &currtm);
	return currtm.tv_sec - starttm_.tv_sec + 1.0 * (currtm.tv_nsec - starttm_.tv_nsec) / 1000000000;
}

double 
TimeTracker::getTotalPeriodInSecs() {
	return stoptm_.tv_sec - starttm_.tv_sec + 1.0 * (stoptm_.tv_nsec - starttm_.tv_nsec) / 1000000000;
}

};
