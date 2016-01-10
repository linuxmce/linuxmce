/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file timetracker.h
Header for TimeTracker class.
*/
//
// C++ Interface: timetracker
//
// Description:
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSTIMETRACKER_H
#define EIBBUSTIMETRACKER_H

#include <time.h>

/** @namespace EIBBUS
For ???
*/
namespace EIBBUS {

/** @class TimeTracker
For tracking time ?
*/
class TimeTracker
{
public:
    /** Constructor.
    */
    TimeTracker(bool start = false);

    /** Destructor.
    */
    ~TimeTracker();

public:
    /** Start the timer.
    */
	void Start();

    /** Stop the timer.
    */
	void Stop();

	double getCurrentPeriodInMiliSecs(); //!< diff between current time and stop time
	double getCurrentPeriodInSecs(); //!< diff between current time and stop time
	double getTotalPeriodInSecs(); //!< diff between stop time and start time

private:
	struct timespec starttm_; /**< Starting time. */
	struct timespec stoptm_; /**< Ending time. */
};

};

#endif
