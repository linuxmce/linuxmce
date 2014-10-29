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
//
// C++ Implementation: VDR_Plugin (the Media stream )
//
// Description:
//
//

//
#include "VDRPlugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include <map>

#include <iostream>
using namespace std;
using namespace DCE;
#include "VDRMediaStream.h"
#include "EPG.h"

namespace DCE
{
 	VDRMediaStream::VDRMediaStream(VDRPlugin *pVDRPlugin,
								MediaHandlerInfo *pMediaHandlerInfo,
								int iPK_MediaProvider,
								MediaDevice *pMediaDevice,
								int PK_Users,
								enum SourceType sourceType,int iStreamID)
            : MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users, sourceType, iStreamID)
	{
		m_pVDRPlugin=pVDRPlugin;
		m_pVDRChannel=NULL;
		m_pVDRProgramInstance=NULL;
	}

	VDRMediaStream::~VDRMediaStream()
	{
		// No op. Is here to keep the compiler happy.
	}

  bool VDRMediaStream::StreamingRequired()
  {
    // if we have more than one target device.
    if ( m_mapEntertainArea.size() > 1 )
      return true;
    
    if ( m_mapEntertainArea.size() == 0 )
      return false;
  }

	void VDRMediaStream::UpdateDescriptions(bool bAllFiles,MediaFile *pMediaFile_In)
	{
		/*
		PLUTO_SAFETY_LOCK(vm,m_pVDRPlugin->m_VDRMutex);
		VDREPG::EPG *pEPG = m_pVDRPlugin->m_mapEPG_Find(m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
		if( !pEPG )
			pEPG = m_pVDRPlugin->m_mapEPG.begin()->second;

		VDREPG::Event *pEvent;
		if( !pEPG || (pEvent=pEPG->m_mapEvent_Find(m_EventID))==NULL )
			m_sMediaDescription = "Unknown";
		else
		{
			m_sMediaDescription = StringUtils::itos(pEvent->m_pChannel->m_ChannelID) + " " + pEvent->m_pChannel->m_sChannelName;
			m_sSectionDescription = pEvent->m_pProgram->m_sTitle;
			if( pEvent->m_sDescription_Long.size() )
				m_sMediaSynopsis = pEvent->m_sDescription_Long;
			else
				m_sMediaSynopsis = pEvent->m_sDescription_Short;
			m_sTimecode = pEvent->GetShortShowtime();
		}
		*/
		
	}
};
