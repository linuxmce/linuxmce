#include "HueControllerBase.h"
#include "DeviceData_Impl.h"
#include "Logger.h"

using namespace DCE;
#include "HueControllerBase.h"
extern HueController_Command *Create_HueController(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
DeviceData_Impl *HueController_Data::CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
{
	// Peek ahead in the stream.  We're going to pass in the above pointers anyway so it won't affect the position
	SerializeClass b;
	b.ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);
	int iPK_Device = b.Read_unsigned_long();
	int iPK_Installation = b.Read_unsigned_long();
	int iPK_DeviceTemplate = b.Read_unsigned_long();
	switch(iPK_DeviceTemplate) {
		case 2256:
			return new HueController_Data();
	};
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateData for unknown type %d.", iPK_DeviceTemplate);
	return NULL;
}

Event_Impl *HueController_Event::CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice )
{
	switch(dwPK_DeviceTemplate) {
		case 2256:
			return (Event_Impl *) new HueController_Event(pOCClientSocket, dwDevice);
	};
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateEvent for unknown type %d.", dwPK_DeviceTemplate);
	return NULL;
}
Command_Impl  *HueController_Command::CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateCommand for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
