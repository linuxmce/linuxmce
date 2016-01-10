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

#ifndef ProcessMediaIdentifiedJob_h
#define ProcessMediaIdentifiedJob_h

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
	class ProcessMediaIdentifiedJob : public Job
	{
		int m_iPK_Device,m_iPK_MediaType,m_iEK_Disc;
		string m_sValue_To_Assign,m_sID,m_sFormat,m_sMediaURL,m_sURL;
		char *m_pData;
		int m_iData_Size;
		JukeBox *m_pJukeBox;
		Disk_Drive_Functions *m_pDisk_Drive_Functions;

	public:

		ProcessMediaIdentifiedJob(JobHandler *pJobHandler,int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc, JukeBox *pJukeBox, Disk_Drive_Functions *pDisk_Drive_Functions);
		virtual ~ProcessMediaIdentifiedJob()
		{
			delete m_pData;
		}

		virtual void Run();

		virtual string GetType() { return "ProcessMediaIdentifiedJob"; }
	};
};

#endif
