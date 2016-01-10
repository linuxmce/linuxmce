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

#ifndef IdentifyTask_h
#define IdentifyTask_h

#include "JobHandler/Task.h"

class Disk_Drive_Functions;
namespace DCE { class Command_Impl; }
using namespace DCE;

namespace nsJobHandler
{
	class IdentifyTask : public Task
	{
		Disk_Drive_Functions *m_pDisk_Drive_Functions;
		Command_Impl *m_pCommand_Impl;
		time_t m_tSentIdCommand;

	public:
		IdentifyTask(Job *pJob,Disk_Drive_Functions *pDisk_Drive_Functions,Command_Impl *pCommand_Impl,string sName);
		virtual ~IdentifyTask() {}
		virtual string GetType() { return "IdentifyTask"; }

		virtual int Run();  // Return 0 if the task is done, or a number of milliseconds if you want Run to be called again in that many ms

		virtual bool Abort();
		int RunAlreadySpawned(); // Called by Run if we've already spawned the ripping process
		void UpdateProgress(string sStatus,int iPercent,int iTime,string sText,string sFilename); // Update the progress of this rip
		void ReportFailure();
		void ReportSuccess();
		void ReportProgress();
		virtual int SecondsRemaining();
	};
};

#endif
