/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com


 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the GNU Public License, available at:
 http://www.fsf.org/licensing/licenses/gpl.html

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Public License for more details.

 */

 /** @file CriteriaInfo.h
For ???
 */
#ifndef CRITERIAINFO_H
#define CRITERIAINFO_H

/** @class CriteriaInfo
For ???
*/
class CriteriaInfo
{
public:
	class Message *pMessage;

	class EventHandler *pEventHandler;
	class DeviceData_Router *pDevice;
	class Event_Impl *pEvent_Impl;
	int PK_C_Mode_Room,PK_C_Mode_House;

	CriteriaInfo(class Event_Impl *ocDeviceEvent,class Message *message,class DeviceData_Router *device,int C_Mode_Room,int C_Mode_House) :
	pEvent_Impl(ocDeviceEvent), pMessage(message), pLogger(logger), pEventHandler(NULL), pDevice(device),PK_C_Mode_Room(C_Mode_Room),PK_C_Mode_House(C_Mode_House) {};
};

#endif
