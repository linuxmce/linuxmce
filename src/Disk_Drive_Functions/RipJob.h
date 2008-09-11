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

#ifndef RipJob_h
#define RipJob_h

#include "JobHandler/Job.h"
class Disk_Drive_Functions;

class Row_DiscLocation;

namespace DCE
{
	class PendingTaskList;
}
namespace nsJukeBox
{
	class JukeBox;
	class Slot;
	class Drive;
}

using namespace DCE;
using namespace nsJukeBox;
class Database_pluto_media;

namespace nsJobHandler
{
	class RipJob : public Job
	{
		friend class RipTask;
		friend class FixupRippingInfoTask;
		friend class JukeBox;

	public:  // Should be private.  Works under windows, but under linux, it doesn't respect the friend class JukeBox and complains about JukeBox accessing the private members!
		Disk_Drive_Functions *m_pDisk_Drive_Functions;
		Slot *m_pSlot;
		int m_iPK_Users, m_iEK_Disc, m_iPK_MediaType;
		string m_sFormat, m_sFileName, m_sDirectory, m_sTracks;
		Row_DiscLocation *m_pRow_DiscLocation;
		Database_pluto_media *m_pDatabase_pluto_media;
		bool m_bReportResult;
		int m_nTracksFailedToRip;
		bool m_bHasErrors;

	public:
		RipJob(Database_pluto_media *pDatabase_pluto_media,
			class JobHandler *pJobHandler,
			Disk_Drive_Functions *pDisk_Drive_Functions,
			Slot *pSlot,
			int iPK_Users, int iEK_Disc,
			int iPK_Orbiter,
			string sFormat, string sFileName, string sDirectory, string sTracks,
			bool bReportResult,
			Command_Impl *pCommand_Impl);
		virtual ~RipJob();

		virtual bool ReadyToRun();
		void AddRippingTasks(Drive *pDrive=NULL, TasklistPosition position=position_TasklistEnd);
		virtual void JobDone();

		virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList);
		virtual int PercentComplete();
		virtual int SecondsRemaining();
		virtual string ToString();

		virtual string GetType() { return "RipJob"; }

		int Get_PK_Orbiter();
	};
};

#endif
