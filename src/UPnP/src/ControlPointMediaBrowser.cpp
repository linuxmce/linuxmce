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
#include "ControlPointMediaBrowser.h"
#include "main.h"

using namespace DCE;

ControlPointMediaBrowser::ControlPointMediaBrowser()
{
}

ControlPointMediaBrowser::~ControlPointMediaBrowser()
{
}

int ControlPointMediaBrowser::MediaBrowserCallbackEventHandler(Upnp_EventType eventType, 
		void* event, 
		void *cookie)
{
	switch(eventType)
	{
		case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
		case UPNP_DISCOVERY_SEARCH_RESULT:
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "some discovery is on the way");
			Upnp_Discovery *d_event = (Upnp_Discovery *)event;
			IXML_Document *descDoc = NULL;
			int ret = UpnpDownloadXmlDoc(d_event->Location, &descDoc);
			if(ret != UPNP_E_SUCCESS)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "unable to get the xml file from device");
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "got the XML document from device");
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "friendly name %s", GetFirstDocumentItem(descDoc, "friendlyName"));
				
			}
			if(NULL != descDoc)
			{
				ixmlDocument_free(descDoc);
			}
			break;
		}

		case UPNP_DISCOVERY_SEARCH_TIMEOUT:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "search timed out");
			break;
		
		case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "discovery buh-bye");
			break;
		
		default:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "invalid event type");
			break;
	}
	return 0;
}
