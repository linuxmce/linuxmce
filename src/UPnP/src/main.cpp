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
* The entry point in the application
*/

#include "main.h"


#include "ControlPointMediaBrowser.h"
#include "DeviceMediaServer.h"

#include <string>

using namespace DCE;

namespace DCE
{
	Logger *
}

int main()
{
	char *ipAddress = NULL;
	int port = 0;
	LoggerWrapper::SetType(LT_LOGGER_FILE,"upnp.log");
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "========start==========");
	UpnpClient_Handle handle = -1;
			
//starting the UPNP lib
	int err = UpnpInit(ipAddress, port);
	if(err != UPNP_E_SUCCESS)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "init failed %d", err);
		goto exit_point;
	}
	
	if(NULL == ipAddress)
	{
		ipAddress = UpnpGetServerIpAddress();
	}
	if(0 == port)
	{
		port = UpnpGetServerPort();
	}
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Upnp initialized with ip %s:%d", ipAddress, port);

	//register control point (aka client)
	err = UpnpRegisterClient(ControlPointMediaBrowser::MediaBrowserCallbackEventHandler,
							&handle, &handle);
	if(UPNP_E_SUCCESS != err)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "register failed %d", err);
		goto exit_point;
	}
		
	//try to find the media servers on the net
	err = UpnpSearchAsync(handle, 15, DeviceMediaServer::MediaServerDeviceType, NULL);
	if(UPNP_E_SUCCESS != err)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "search failed %d", err);
		goto exit_point;
	}
	
	//wait a bit
	usleep(50*1000*1000);
	
	UpnpUnRegisterClient(handle);
			
exit_point:
	UpnpFinish();
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Upnp closed");
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "=========end===========");
	LoggerWrapper::GetInstance()->Flush();
	delete(LoggerWrapper::GetInstance());
	return 0;
}
