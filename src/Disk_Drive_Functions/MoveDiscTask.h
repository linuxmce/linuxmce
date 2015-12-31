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

#ifndef MoveDiscTask_h
#define MoveDiscTask_h

#include "JobHandler/Task.h"

namespace nsJukeBox
{
	class Drive;
	class Slot;
	class JukeBox;
};

using namespace nsJukeBox;

namespace nsJobHandler
{
	class MoveDiscTask : public Task
	{
	public:
		enum MoveDiscTaskType
		{
			mdt_SlotToDrive,
			mdt_DriveToSlot,
			mdt_DriveToEject, // Eject the disc in a drive
			mdt_SlotToEject,  // Eject the disc in a slot
			mdt_Load
		} m_MoveDiscTaskType;

		Drive *m_pDrive;
		Slot *m_pSlot;
		JukeBox *m_pJukeBox;

		MoveDiscTask(class Job *pJob,string sName,MoveDiscTaskType MoveDiscTaskType,JukeBox *pJukeBox,Drive *pDrive,Slot *pSlot);
		virtual ~MoveDiscTask() {}
		virtual string GetType() { return "MoveDiscTask"; }

		virtual int Run();  // Return 0 if the task is done, or a number of milliseconds if you want Run to be called again in that many ms

		virtual bool Abort();
		void UpdateProgress(string sStatus,int iPercent,int iTime,string sText,string sFilename); // Update the progress of this rip
		void ReportFailure();
		void ReportSuccess();
		void ReportProgress();
		virtual int SecondsRemaining();
	};
};

#endif
