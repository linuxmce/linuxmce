//
// C++ Interface: TelecomTask
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TELECOMTASK_H
#define TELECOMTASK_H

#include <string>

using namespace std;

/**
It takes care of complex commands for telecom.

	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/
namespace DCE
{
	class Telecom_Plugin;
	
	class TelecomTask
	{
		public:
			
			enum TaskState { Ready=0, Processing, Completed, Failed };
			
			enum JobState { Stop=0, Running, Done };
			
			enum TaskType { Nothing=0, Assisted_Transfer };
			
			/***/
			static void SetTelecom(const Telecom_Plugin * tp);
			
			/***/
			TelecomTask();
			/***/
			virtual ~TelecomTask();
			
			/***/
			virtual bool ProcessJob(const string &) = 0;
			/***/
			virtual bool ProcessEvent(class Message *) = 0;
			/***/
			virtual TaskType RTTI() const = 0;
			
			/***/
			const string& GetID() const { return sID; }
			/***/
			void SetState(TaskState st) { state = st; }
			/***/
			TaskState GetState() const { return state; }
			/***/
			const string& GetJob() const { return sJobID; }
			/***/
			void SetJobState(JobState js) { jobState = js; }
			/***/
			JobState GetJobState() const { return jobState; }
			
		protected:
			
			static Telecom_Plugin * telecom;
			static unsigned uID;
			
			string sID;
			TaskState state;
			string sJobID;
			JobState jobState;
	};
};
#endif
