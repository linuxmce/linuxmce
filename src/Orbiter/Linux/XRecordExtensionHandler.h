/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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

#ifndef XRECORDEXTENSIONHANDLER_H
#define XRECORDEXTENSIONHANDLER_H

#include "../Orbiter.h"

#include <X11/Xlib.h>
#include <X11/extensions/record.h>

#include <pthread.h>

#include <string>

class XRecordExtensionHandler
{
	std::string 		m_strDisplayName;

	bool 				m_isRecordingEnabled;
	bool 				m_shouldRecord, m_shouldQuit;

	pthread_t			recordingThread;

	pthread_mutex_t 	mutexEnableRecordCondition;
	pthread_mutex_t 	mutexStoppedRecordingCondition;

    pthread_cond_t  	enableRecordCondition;
	pthread_cond_t  	recordingStateChangedCondition;

	XRecordContext		m_recordingContext;

	Orbiter 			*m_pOrbiter;
	Orbiter::Event 		m_OrbiterEvent;

	Display 			*m_pDisplay;

	int 				m_iMouseX, m_iMouseY;

private:
	static void *recordingThreadMainFunction(void *arguments);

	static void XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData);

	void processXRecordToOrbiterEvent(XRecordInterceptData *pRecordedData, Orbiter::Event *orbiterEvent, Display *pDisplay);

public:
	XRecordExtensionHandler(Orbiter *pRecordingOrbiter, std::string strDisplayName);
	virtual ~XRecordExtensionHandler();

	virtual bool enableRecording(bool bEnable = true);

	pthread_t	getRecordingThread();
};

#endif // XRECORDEXTENSIONHANDLER_H
