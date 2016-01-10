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

#ifndef LoadUnloadJob_h
#define LoadUnloadJob_h

#include "JobHandler/Job.h"
class Disk_Drive_Functions;

namespace DCE
{
	class PendingTaskList;
}
namespace nsJukeBox
{
	class Drive;
	class JukeBox;
	class Slot;
}

using namespace DCE;
using namespace nsJukeBox;

namespace nsJobHandler
{
	class LoadUnloadJob : public Job
	{
	public:
		enum LoadUnloadJobType
		{
			eLoadOneDisc,
			eLoadMultipleDiscs,
			eEjectOneDisc,
			eEjectMultipleDiscs,
			eMoveFromSlotToDrive,
			eMoveFromDriveToSlot
		} m_eLoadUnloadJobType;

		Drive *m_pDrive;  // The drive involved in the move
		Slot *m_pSlot; // The slot involved
		JukeBox *m_pJukeBox;  // The jukebox

		LoadUnloadJob(JobHandler *pJobHandler,
			LoadUnloadJobType loadUnloadJobType,
			JukeBox *pJukeBox,
			Drive *pDrive,
			Slot *pSlot,
			int PK_Orbiter,
			Command_Impl *pCommand_Impl);
		virtual ~LoadUnloadJob();

		virtual bool ReadyToRun();
		void AddTasks();

		virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList);
		virtual int PercentComplete();
		virtual int SecondsRemaining();
		virtual string ToString();

		virtual string GetType() { return "LoadUnloadJob"; }
	};
};

#endif
