/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/


#ifndef INSTEONJOB_H_
#define INSTEONJOB_H_



#include <stddef.h>
#include <time.h>

class LMCEInsteonAPI;


/**Virtual class that stands for an atomic job*/
class InsteonJob
{
	public:
	
		enum JobState { IDLE=0, RUNNING, STOPPED };
		
		enum INSTEON_JOB { NOTHING=0, GET_VERSION, GET_ID, GET_INIT_DATA, INITIALIZE, RECEIVE, RESET, ADD_NODE, REMOVE_NODE,
							SET_SWITCH_LEVEL, GET_SWITCH_BINARY, LIGHT_LEVEL, SET_VALVE, SEND_DATA
		};
		
		InsteonJob(LMCEInsteonAPI*);
		
		virtual ~InsteonJob();
		
		/** It is called to perform the job.*/
		virtual bool run() = 0;
		
		/** Try again the last command.*/
		virtual bool runAgain();
		
				
		/** Returns the job state.*/
		virtual JobState state() const;
		
		/** Sets the job state.*/
		virtual void setState(JobState);
		
		/** Returns the job type.*/
		virtual INSTEON_JOB type() const;
		
		/** Sets the job type.*/
		virtual void setType(INSTEON_JOB);
		
		/** Callback to handle receive timeout.*/
//		virtual void timeoutHandler();
		
		
	protected:
	
		/** Returns the job handler.*/
		virtual LMCEInsteonAPI* handler() const;
		
		/** Return true if the 'again' flag is set.*/
		virtual bool again() const;
		
		/** Set the start time.*/
//		virtual void setStartTime(time_t);
		
		/** Set the answer time.
		  * The time when the job got the last answer.
		  */
//		virtual void setAnswerTime(time_t);
		
	private:
	
		// disable the default constructor
		InsteonJob();
		
		class Private;
		Private * d;
};
		
#endif /*INSTEONJOB_H_*/

