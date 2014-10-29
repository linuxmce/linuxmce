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
#ifndef VDRMediaStream_H
#define VDRMediaStream_H

#include "../Media_Plugin/MediaStream.h"

namespace DCE
{
	/**
	 * A media stream that incapsulates TV and channel informations.
	 */
	class VDRMediaStream : public MediaStream
	{
	public:
		class VDRPlugin *m_pVDRPlugin;
		class VDRChannel *m_pVDRChannel; // The current channel
		class VDRProgramInstance *m_pVDRProgramInstance;  // The current program instance
		map<int,bool> m_mapOrbiter_HasInitialPosition;

		VDRMediaStream(VDRPlugin *pVDRPlugin,
						MediaHandlerInfo *pMediaHandlerInfo,
						  int iPK_MediaProvider,
		 				  MediaDevice *pMediaDevice,
						  int PK_Users,
						  enum SourceType sourceType,
						  int iStreamID);

		virtual ~VDRMediaStream();
        virtual void UpdateDescriptions(bool bAllFiles=false,MediaFile *pMediaFile_In=NULL);
        virtual int GetType() { return MEDIASTREAM_TYPE_VDR; }
	virtual bool StreamingRequired();
	};
};

#endif
