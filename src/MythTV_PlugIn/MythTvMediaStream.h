//
// C++ Interface: Xine_Plugin
//
// Description:
//
//
// Author: Toader Mihai Claudiu, mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCEMYTHTVMEDIASTREAM_H
#define DCEMYTHTVMEDIASTREAM_H

#include "../Media_Plugin/MediaStream.h"

namespace DCE
{
	/**
	 * A media stream that incapsulates TV and channel informations.
	 */
	class MythTvMediaStream : public MediaStream
	{
	public:
		int m_iCurrentProgramChannelID;

		int m_iNextProgramChannelID;
		int m_iNextProgramTimeYear;
		int m_iNextProgramTimeMonth;
		int m_iNextProgramTimeDay;
		int m_iNextProgramTimeHour;
		int m_iNextProgramTimeMinute;
		//int m_iCurrentShow;

		// class MythTV_PlugIn *m_pMythTV_PlugIn;

		MythTvMediaStream(MediaHandlerInfo *pMediaHandlerInfo,
						  int iPK_MediaProvider,
		 				  MediaDevice *pMediaDevice,
						  int PK_Users,
						  enum SourceType sourceType,
						  int iStreamID);

		virtual ~MythTvMediaStream();

		virtual int GetType();

		virtual bool ProcessJumpPosition(string sJumpSpecification);

		virtual bool ShouldTuneToNewChannel();

		virtual void UpdateDescriptions(bool bAllFiles=false,MediaFile *pMediaFile_In=NULL) { }  // Handled by playback started
	};
};

#endif
