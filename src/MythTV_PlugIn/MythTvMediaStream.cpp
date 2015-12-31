//
// C++ Implementation: MythTV_Plugin (the Media stream )
//
// Description:
//
//
// Author: Toader Mihai Claudiu, mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "MythTV_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include <map>

#include <iostream>
using namespace std;
using namespace DCE;
#include "MythTvMediaStream.h"

namespace DCE
{
 	MythTvMediaStream::MythTvMediaStream(
								MediaHandlerInfo *pMediaHandlerInfo,
								int iPK_MediaProvider,
								MediaDevice *pMediaDevice,
								int PK_Users,
								enum SourceType sourceType,int iStreamID)
            : MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users, sourceType, iStreamID)
	{
		// m_pMythTV_PlugIn=pMythTV_PlugIn;
		m_iNextProgramChannelID = m_iCurrentProgramChannelID = 0;
	}

	MythTvMediaStream::~MythTvMediaStream()
	{
		// No op. Is here to keep the compiler happy.
	}

	int MythTvMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_MYTHTV;
	}

	bool MythTvMediaStream::ProcessJumpPosition(string sJumpSpecification)
	{
		vector<string> vectNumbers;
		StringUtils::Tokenize(sJumpSpecification, "|", vectNumbers);

		if ( vectNumbers.size() != 6 )
			return false;

		m_iNextProgramChannelID = atoi(vectNumbers[0].c_str());
		m_iNextProgramTimeYear = atoi(vectNumbers[1].c_str());
		m_iNextProgramTimeMonth = atoi(vectNumbers[2].c_str());
		m_iNextProgramTimeDay = atoi(vectNumbers[3].c_str());
		m_iNextProgramTimeHour = atoi(vectNumbers[4].c_str());
		m_iNextProgramTimeMinute = atoi(vectNumbers[5].c_str());

		if ( m_iNextProgramChannelID == 0 || m_iNextProgramTimeYear == 0 || m_iNextProgramTimeMonth == 0 || m_iNextProgramTimeDay == 0 )
			return false;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting next show to see to be on channel ID %d at %d/%d/%d %d:%d.",
			m_iNextProgramChannelID,
			m_iNextProgramTimeYear,
			m_iNextProgramTimeMonth,
			m_iNextProgramTimeDay,
			m_iNextProgramTimeHour,
			m_iNextProgramTimeMinute);

		return true;
	}

	bool MythTvMediaStream::ShouldTuneToNewChannel()
	{
		return m_iNextProgramChannelID != m_iCurrentProgramChannelID;
	}
};
