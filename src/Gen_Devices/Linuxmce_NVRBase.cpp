#include "Linuxmce_NVRBase.h"
#include "DeviceData_Impl.h"
#include "Logger.h"

using namespace DCE;
#include "Linuxmce_NVRBase.h"
extern Linuxmce_NVR_Command *Create_Linuxmce_NVR(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
DeviceData_Impl *Linuxmce_NVR_Data::CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
{
	// Peek ahead in the stream.  We're going to pass in the above pointers anyway so it won't affect the position
	SerializeClass b;
	b.ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);
	int iPK_Device = b.Read_unsigned_long();
	int iPK_Installation = b.Read_unsigned_long();
	int iPK_DeviceTemplate = b.Read_unsigned_long();
	switch(iPK_DeviceTemplate) {
		case 2330:
			return new Linuxmce_NVR_Data();
	};
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateData for unknown type %d.", iPK_DeviceTemplate);
	return NULL;
}

Event_Impl *Linuxmce_NVR_Event::CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice )
{
	switch(dwPK_DeviceTemplate) {
		case 2330:
			return (Event_Impl *) new Linuxmce_NVR_Event(pOCClientSocket, dwDevice);
	};
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateEvent for unknown type %d.", dwPK_DeviceTemplate);
	return NULL;
}
Command_Impl  *Linuxmce_NVR_Command::CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateCommand for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
