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

#ifndef RipTask_h
#define RipTask_h

#include "JobHandler/Task.h"

class Row_RipStatus;

namespace nsJukeBox
{
	class Drive;
	class Slot;
	class JukeBox;
};

using namespace nsJukeBox;

namespace nsJobHandler
{
	class RipTask : public Task
	{
		int m_iTime;
		string m_sTrack;
		bool m_bSpawnedRip;
		string m_sText,m_sSpawnName;
		class RipJob *m_pRipJob;
		Row_RipStatus *m_pRow_RipStatus;
		bool m_bReportResult;
		Drive *m_pDrive;

	public:
		RipTask(class RipJob *pRipJob,string sName,bool bReportResult,string sTrack,Drive *pDrive);
		virtual ~RipTask() {}
		virtual string GetType() { return "RipTask"; }

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
