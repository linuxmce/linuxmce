/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Interface: VideoLan_PlugIn
//
// Description:
//
//
// Author: Toader Mihai Claudiu mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCEVIDEOLANMEDIASTREAM_H
#define DCEVIDEOLANMEDIASTREAM_H

#include "../Media_Plugin/MediaHandlerInfo.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaDevice.h"

#include <map>
#include <list>
#include <string>

namespace DCE
{
	using namespace std;

	class VideoLanMediaStream : public MediaStream
	{
		private:
			bool					m_bIsStreaming;

		public:
			VideoLanMediaStream(class VideoLan_PlugIn *pVideoLanPlugin, class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, MediaDevice *pMediaDevice, int PK_Users,enum SourceType sourceType,int iStreamID);

			virtual ~VideoLanMediaStream();
			virtual int GetType();

			bool ShouldUseStreaming();
			bool isStreaming();
			void setIsStreaming(bool isStreaming = true);

			bool CanPlayMore();
	};

};

#endif
