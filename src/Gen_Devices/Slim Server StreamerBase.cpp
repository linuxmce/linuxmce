#include "Slim Server StreamerBase.h"
#include "DeviceData_Impl.h"
#include "Logger.h"

using namespace DCE;
#include "Slim_Server_StreamerBase.h"
#include "SqueezeBox_PlayerBase.h"
DeviceData_Impl *Slim Server Streamer_Data::CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
{
	// Peek ahead in the stream.  We're going to pass in the above pointers anyway so it won't affect the position
	SerializeClass b;
	b.ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);
	int iPK_Device = b.Read_unsigned_long();
	int iPK_Installation = b.Read_unsigned_long();
	int iPK_DeviceTemplate = b.Read_unsigned_long();
	switch(iPK_DeviceTemplate) {
		case 53:
			return new Slim_Server_Streamer_Data();
		case 58:
			return new SqueezeBox_Player_Data();
	};
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateData for unknown type %d.", iPK_DeviceTemplate);
	return NULL;
}

Event_Impl *Slim Server Streamer_Event::CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID)
{
	switch(PK_DeviceTemplate) {
		case 53:
			return (Event_Impl *) new Slim_Server_Streamer_Event(pOCClientSocket, DeviceID);
		case 58:
			return (Event_Impl *) new SqueezeBox_Player_Event(pOCClientSocket, DeviceID);
	};
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateEvent for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
Command_Impl  *Slim Server Streamer_Command::CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent)
{
	switch(PK_DeviceTemplate)
	{
		case 53:
			return (Command_Impl *) new Slim_Server_Streamer_Command(pPrimaryDeviceCommand, pData, pEvent, m_pRouter);
		case 58:
			return (Command_Impl *) new SqueezeBox_Player_Command(pPrimaryDeviceCommand, pData, pEvent, m_pRouter);
	};
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateCommand for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
