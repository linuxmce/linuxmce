/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "Generic_Input_OuputBase.h"
#include "DeviceData_Impl.h"
#include "Logger.h"

using namespace DCE;
#include "Generic_Input_OuputBase.h"
DeviceData_Impl *Generic_Input_Ouput_Data::CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
{
	// Peek ahead in the stream.  We're going to pass in the above pointers anyway so it won't affect the position
	SerializeClass b;
	b.ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);
	int iPK_Device = b.Read_unsigned_long();
	int iPK_Installation = b.Read_unsigned_long();
	int iPK_DeviceTemplate = b.Read_unsigned_long();
	switch(iPK_DeviceTemplate) {
		case 43:
			return new Generic_Input_Ouput_Data();
	};
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got CreateData for unknown type %d.", iPK_DeviceTemplate);
	return NULL;
}

Event_Impl *Generic_Input_Ouput_Event::CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID)
{
	switch(PK_DeviceTemplate) {
		case 43:
			return (Event_Impl *) new Generic_Input_Ouput_Event(pOCClientSocket, DeviceID);
	};
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got CreateEvent for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
Command_Impl  *Generic_Input_Ouput_Command::CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent)
{
	switch(PK_DeviceTemplate)
	{
		case 43:
			return (Command_Impl *) new Generic_Input_Ouput_Command(pPrimaryDeviceCommand, pData, pEvent, m_pRouter);
	};
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got CreateCommand for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
