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

#ifndef IdentifyJob_h
#define IdentifyJob_h

#include "JobHandler/Job.h"
class Disk_Drive_Functions;

namespace DCE
{
	class PendingTaskList;
}
namespace nsJukeBox
{
	class JukeBox;
	class Slot;
}

using namespace DCE;
using namespace nsJukeBox;

namespace nsJobHandler
{
	class IdentifyJob : public Job
	{
		friend class IdentifyTask;

	public:
		Disk_Drive_Functions *m_pDisk_Drive_Functions;  // If non-null, identify the disc in a particular drive
		Slot *m_pSlot; // If specified, this is the slot to identify, and m_pDisk_Drive_Functions will be filled in as a drive becomes available

		IdentifyJob(class JobHandler *pJobHandler,
			Disk_Drive_Functions *pDisk_Drive_Functions,
			Slot *pSlot,
			int PK_Orbiter,
			Command_Impl *pCommand_Impl);
		virtual ~IdentifyJob();

		virtual bool ReadyToRun();
		void AddIdentifyTasks();

		virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList);
		virtual int PercentComplete();
		virtual int SecondsRemaining();
		virtual string ToString();

		virtual string GetType() { return "IdentifyJob"; }
	};
};

#endif
