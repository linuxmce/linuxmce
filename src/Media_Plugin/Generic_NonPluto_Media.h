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
/**
 *
 * @file Generic_NonPluto_Media.h
 * @brief header file for the Generic_NonPluto_Media class
 * @author
 *
 */


#ifndef Generic_NonPluto_Media_h
#define Generic_NonPluto_Media_h


#include "Media_Plugin.h"
#include "MediaStream.h"
#include "MediaHandlerBase.h"

namespace DCE
{

	class Generic_NonPluto_Media : public MediaHandlerBase
	{
public:
		// Constructors/Destructor
		Generic_NonPluto_Media(Media_Plugin *pMedia_Plugin);
		virtual ~Generic_NonPluto_Media();

	protected:

	/** Mandatory implementations */

		/**
		* @brief
		*/
		virtual class MediaStream *CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

		/**
		* @brief Start media playback
		*/
		virtual bool StartMedia( class MediaStream *pMediaStream,string &sError );

		/**
		* @brief Stop media playback
		*/
		virtual bool StopMedia( class MediaStream *pMediaStream );

	};
}
#endif
