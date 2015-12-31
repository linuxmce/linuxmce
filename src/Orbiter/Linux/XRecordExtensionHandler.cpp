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
#include "DCE/Logger.h"
#include "XRecordExtensionHandler.h"

#include "pluto_main/Define_Button.h"

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>


using namespace DCE;
using namespace std;

XRecordExtensionHandler::XRecordExtensionHandler(Orbiter *pRecordingOrbiter, string displayName)
	: m_strDisplayName(displayName)
    , m_isRecordingEnabled(false)
    , m_shouldRecord(false)
    , m_shouldQuit(false)
    , m_pOrbiter(pRecordingOrbiter)
{
 	pthread_mutex_init(&mutexEnableRecordCondition, NULL);
	pthread_cond_init(&enableRecordCondition, NULL);

	pthread_mutex_init(&mutexStoppedRecordingCondition, NULL);
	pthread_cond_init(&recordingStateChangedCondition, NULL);

    pthread_create(&recordingThread, NULL, recordingThreadMainFunction, this);
}

XRecordExtensionHandler::~XRecordExtensionHandler()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Marking as quit");
	m_shouldQuit = true;

	if ( m_isRecordingEnabled )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): Disabling recording");
		enableRecording(false); // stop any recording taking place. This will not put the thread of the cond_wait if it is there.
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): Signaling the condition");
	pthread_cond_signal(&enableRecordCondition); // if it is there it will be put out.

	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): cancelling the thread");
	//pthread_cancel(recordingThread);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): Joining the thread");
	pthread_join(recordingThread, NULL);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::~XRecordExtensionHandler(): Done");
}

void *XRecordExtensionHandler::recordingThreadMainFunction(void *arguments)
{
	int iMinorVersion = 0;
    int iMajorVersion = 0;
	XRecordExtensionHandler *pXRecordObject = NULL;
	XRecordRange        *recordRange = NULL;
    XRecordClientSpec   recordClient = 0;
	Display *pDisplay_ControlConnection = NULL;
    Display *pDisplay_DataConnection = NULL;

	pXRecordObject = (XRecordExtensionHandler*)arguments;

	pDisplay_ControlConnection = XOpenDisplay(getenv(pXRecordObject->m_strDisplayName.c_str()));
	pDisplay_DataConnection = XOpenDisplay(getenv(pXRecordObject->m_strDisplayName.c_str()));
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): pDisplay_ControlConnection=%d, pDisplay_DataConnection=%d ->Opened", pDisplay_ControlConnection, pDisplay_DataConnection);

	if ( ! XRecordQueryVersion(pDisplay_ControlConnection, &iMinorVersion, &iMajorVersion) )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "XRecordExtensionHandler::recordingThreadMainFunction(): XRecord extension not available.");
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "XRecordExtensionHandler::recordingThreadMainFunction(): pDisplay_ControlConnection=%d, pDisplay_DataConnection=%d ->Closing", pDisplay_ControlConnection, pDisplay_DataConnection);
        XCloseDisplay(pDisplay_ControlConnection);
        XCloseDisplay(pDisplay_DataConnection);
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "XRecordExtensionHandler::recordingThreadMainFunction(): pDisplay_ControlConnection=%d, pDisplay_DataConnection=%d ->Closed", pDisplay_ControlConnection, pDisplay_DataConnection);
		return NULL;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Available XRecord extension with version %d.%d.", iMajorVersion, iMinorVersion);

	//XSync(pDisplay_ControlConnection, True);
	//XSync(pDisplay_DataConnection, True);
	recordRange = XRecordAllocRange();

	recordClient = XRecordAllClients;
	recordRange->device_events.first = KeyPress;
	recordRange->device_events.last = MotionNotify;

	recordRange->core_requests.first = X_NoOperation;
	recordRange->core_requests.last = X_NoOperation;

	recordRange->core_replies.first = X_NoOperation;
	recordRange->core_replies.last = X_NoOperation;

	recordRange->errors.first = Success;
	recordRange->errors.last = Success;

	recordRange->ext_requests.ext_major.first = 0;
	recordRange->ext_requests.ext_major.last = 0;
	recordRange->ext_requests.ext_minor.first = 0;
	recordRange->ext_requests.ext_minor.last = 0;

	recordRange->ext_replies.ext_major.first = 0;
	recordRange->ext_replies.ext_major.last = 0;
	recordRange->ext_replies.ext_minor.first = 0;
	recordRange->ext_replies.ext_minor.last = 0;

	// pXRecordObject->m_recordingContext = XRecordCreateContext(pDisplay_DataConnection, XRecordFromServerTime | XRecordFromClientTime | XRecordFromClientSequence, &recordClient, 1, &recordRange, 1);
	//pXRecordObject->m_recordingContext = XRecordCreateContext(pDisplay_DataConnection, 0, &recordClient, 1, &recordRange, 1);
	pXRecordObject->m_recordingContext = XRecordCreateContext(pDisplay_ControlConnection, 0, &recordClient, 1, &recordRange, 1);

	pXRecordObject->m_isRecordingEnabled = false;
	pXRecordObject->m_pDisplay = pDisplay_ControlConnection;

	XSync(pXRecordObject->m_pDisplay, True);
	while ( true )
	{
		if ( pXRecordObject->m_shouldRecord && ! pXRecordObject->m_isRecordingEnabled )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Enabling recording!!!");
			if ( ! XRecordEnableContext(
					pDisplay_DataConnection,
					pXRecordObject->m_recordingContext,
					(XRecordInterceptProc)&XRecordExtensionHandler::XRecordingDataCallback, (char*)pXRecordObject))
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "XRecordExtensionHandler::recordingThreadMainFunction(): Could not enable recording context!");

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Recording completed!!!");
		}

        if ( pXRecordObject->m_shouldQuit )
            break;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Recording thread going to sleep!!!");
        pthread_mutex_lock(&pXRecordObject->mutexEnableRecordCondition);
        while ( ! pXRecordObject->m_shouldQuit && ! pXRecordObject->m_shouldRecord )
			pthread_cond_wait(&pXRecordObject->enableRecordCondition, &pXRecordObject->mutexEnableRecordCondition);
        pthread_mutex_unlock(&pXRecordObject->mutexEnableRecordCondition);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Recording thread is awake!!!");

        if ( pXRecordObject->m_shouldQuit ) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::recordingThreadMainFunction(): Quitting now ...");
            break;
	}
	}

	XRecordFreeContext( pDisplay_ControlConnection, pXRecordObject->m_recordingContext);
	XFree(recordRange);

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "XRecordExtensionHandler::recordingThreadMainFunction(): pDisplay_ControlConnection=%d, pDisplay_DataConnection=%d ->Closing", pDisplay_ControlConnection, pDisplay_DataConnection);
	XCloseDisplay(pDisplay_ControlConnection);
	XCloseDisplay(pDisplay_DataConnection);
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "XRecordExtensionHandler::recordingThreadMainFunction(): pDisplay_ControlConnection=%d, pDisplay_DataConnection=%d ->Closed", pDisplay_ControlConnection, pDisplay_DataConnection);

	return NULL;
}

bool XRecordExtensionHandler::enableRecording(bool bEnable)
{
	m_shouldRecord = bEnable;

	if (m_shouldRecord == m_isRecordingEnabled )
		return m_isRecordingEnabled;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording(): Trying to change recording state: CurrentState[:ShouldRecord: %d, :IsRecording: %d]", m_shouldRecord, m_isRecordingEnabled);
	if ( m_isRecordingEnabled  && ! m_shouldRecord )
    {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording(): Calling XRecord Disable: ShouldRecord: %d, IsRecording: %d", m_shouldRecord, m_isRecordingEnabled);
		XRecordProcessReplies(m_pDisplay);
		XRecordDisableContext(m_pDisplay, m_recordingContext);
		XFlush(m_pDisplay);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording():  XRecordDisableContext called: ShouldRecord: %d, IsRecording: %d", m_shouldRecord, m_isRecordingEnabled);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording(): Waiting for state change. for condition: %p", &recordingStateChangedCondition);

		pthread_cond_signal(&enableRecordCondition);

		pthread_mutex_lock(&mutexStoppedRecordingCondition);
		while ( ! m_isRecordingEnabled )
			pthread_cond_wait(&recordingStateChangedCondition, &mutexStoppedRecordingCondition);
    	pthread_mutex_unlock(&mutexStoppedRecordingCondition);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::enableRecording(): State changed: CurrentState[:ShouldRecord: %d, :IsRecording: %d].", m_shouldRecord, m_isRecordingEnabled);
	}

	return m_isRecordingEnabled;
}

void XRecordExtensionHandler::XRecordingDataCallback(XPointer pData, XRecordInterceptData *pRecordedData)
{
	XRecordExtensionHandler *pRecordingHandler = (XRecordExtensionHandler*)pData;
#ifdef DEBUG
//	LoggerWrapper::GetInstance()->Write(LV_STATUS,"XRecordExtensionHandler::XRecordingDataCallback cat %d rrr",(int)pRecordedData->category);
#endif
	switch ( pRecordedData->category )
	{
		case XRecordStartOfData:
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::XRecordingDataCallback(): Recording context enabled.");
#endif
			pRecordingHandler->m_iMouseX = pRecordingHandler->m_iMouseY = -1;

			pRecordingHandler->m_isRecordingEnabled = true;
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::XRecordingDataCallback(): Signalling.");
#endif
			pthread_cond_signal(&pRecordingHandler->recordingStateChangedCondition);
			break;

		case XRecordEndOfData:
			pRecordingHandler->m_isRecordingEnabled = false;
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::XRecordingDataCallback(): Recording context got end of data.");
#endif
			pthread_cond_signal(&pRecordingHandler->recordingStateChangedCondition);
			break;

		default:
			pRecordingHandler->processXRecordToOrbiterEvent(pRecordedData, &pRecordingHandler->m_OrbiterEvent, pRecordingHandler->m_pDisplay);
			if ( pRecordingHandler->m_pOrbiter )
			{
				Orbiter::Event *pEvent = new Orbiter::Event;
				*pEvent = pRecordingHandler->m_OrbiterEvent;

				if( pEvent->type==Orbiter::Event::MOUSE_MOVE )
				{
					PLUTO_SAFETY_LOCK_ERRORSONLY( cm, pRecordingHandler->m_pOrbiter->m_MaintThreadMutex );
					for(map<int,PendingCallBackInfo *>::iterator it=pRecordingHandler->m_pOrbiter->m_mapPendingCallbacks.begin();it!=pRecordingHandler->m_pOrbiter->m_mapPendingCallbacks.end();++it)
					{
						PendingCallBackInfo *pCallBackInfo = (*it).second;
						Orbiter::Event *pEvent_cb = (Orbiter::Event *) pCallBackInfo->m_pData;
						if( pCallBackInfo->m_fnCallBack==&Orbiter::QueueEventForProcessing &&
							pEvent_cb && pEvent_cb->type==Orbiter::Event::MOUSE_MOVE )
								pCallBackInfo->m_bStop=true;
					}
#ifdef DEBUG
//LoggerWrapper::GetInstance()->Write(LV_STATUS,"XRecordExtensionHandler::XRecordingDataCallback queueing to orbiter mouse x %d y %d",
//					  pEvent->data.region.m_iX,pEvent->data.region.m_iY);
#endif
				}
				else
				{
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"XRecordExtensionHandler::XRecordingDataCallback queueing to orbiter button type %d key %d keycode %d",
					  pEvent->type,pEvent->data.button.m_iPK_Button,pEvent->data.button.m_iKeycode);
#endif
				}

				if(!pRecordingHandler->m_pOrbiter->m_bIgnoreMouse)
					pRecordingHandler->m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false );
			}
	}

	XRecordFreeData(pRecordedData);
}

void XRecordExtensionHandler::processXRecordToOrbiterEvent(XRecordInterceptData *pRecordedData, Orbiter::Event *orbiterEvent, Display *pDisplay)
{
#ifdef DEBUG
//LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::processXRecordToOrbiterEvent cat %d",(int) pRecordedData->category);
#endif
	switch (pRecordedData->category )
	{
		case XRecordFromServer:
		case XRecordFromClient:
		{
			xEvent *pxEvent = (xEvent*)pRecordedData->data;

			switch ( pxEvent->u.u.type )
			{
				case KeyPress: case KeyRelease: // key related events types
	                    orbiterEvent->type = pxEvent->u.u.type == KeyPress ? Orbiter::Event::BUTTON_DOWN : Orbiter::Event::BUTTON_UP;
#ifdef DEBUG
    	                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Key %s with keycode %d", pxEvent->u.u.type == KeyPress ? "down" : "up", pxEvent->u.u.detail);
#endif
						orbiterEvent->data.button.m_bSimulated = false;
        	            orbiterEvent->data.button.m_iKeycode = pxEvent->u.u.detail;
        	            orbiterEvent->data.button.m_iPK_Button = 0;

					break;

                case MotionNotify:
					m_iMouseX = pxEvent->u.keyButtonPointer.rootX;
					m_iMouseY = pxEvent->u.keyButtonPointer.rootY;

		            orbiterEvent->type = Orbiter::Event::MOUSE_MOVE;
		            orbiterEvent->data.region.m_iX = m_iMouseX;
		            orbiterEvent->data.region.m_iY = m_iMouseY;

					break;

				case ButtonPress:  case ButtonRelease: // mouse button related event types
                    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Mouse Button with id: %d is %s", pxEvent->u.u.detail, pxEvent->u.u.type == ButtonPress ? "down" : "up"); //TODO: comment this
					orbiterEvent->type = pxEvent->u.u.type == ButtonPress ? Orbiter::Event::REGION_DOWN : Orbiter::Event::REGION_UP;
					orbiterEvent->data.region.m_iButton = pxEvent->u.u.detail;
					orbiterEvent->data.region.m_iX = m_iMouseX;
					orbiterEvent->data.region.m_iY = m_iMouseY;

					if ( m_iMouseX == -1 && m_iMouseY == -1 )
					 	orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;

					break;

				default:
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "XRecordExtensionHandler::processXRecordToOrbiterEvent(): Don't know how to processs recorded event: %d", pxEvent->u.u.type );
			}
		}
	}
}

pthread_t XRecordExtensionHandler::getRecordingThread()
{
	return recordingThread;
}
