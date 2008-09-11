#ifndef __DATA_LAYER_H__
#define __DATA_LAYER_H__
//----------------------------------------------------------------------------------------------
#include <map>
#include <vector>
#include <list>
//----------------------------------------------------------------------------------------------
#include "DeviceData_Router.h"
#include "Scene_Data.h"
#include "DeviceTemplate_Data.h"
//----------------------------------------------------------------------------------------------
using namespace DCE;
//----------------------------------------------------------------------------------------------
class IDataLayer
{
public:

	IDataLayer(void)
	{
	}

	virtual ~IDataLayer(void)
	{
	}

	//load/save data
	virtual bool Load() = 0;
	virtual bool Save() = 0;

	//the mutex
	virtual pluto_pthread_mutex_t& Mutex() = 0;

	//get devices
	virtual std::map<int, DeviceData_Router *>& Devices() = 0;

	//queries for scenes and device templates
	virtual Scene_Data* Scene(int nSceneID) = 0;
	virtual DeviceTemplate_Data* DeviceTemplate(int nPK_DeviceTemplate) = 0;

	//queries for devices
	virtual int LargestDeviceNumber() = 0;
	virtual DeviceData_Router *ChildMatchingDeviceData(int nPK_Device_Parent, int nFK_DeviceData, string sValue) = 0;
	virtual DeviceData_Router *Device(int nPK_Device) = 0;
	virtual void ChildrenDevices(int nPK_Device_Parent, std::vector<DeviceData_Router *>& vectDevice_Children, bool bRecursive = false) = 0;
	virtual void DevicesByTemplate(int nPK_DeviceTemplate, std::list<DeviceData_Router *>& listDevices) = 0;

	//operations on devices
	virtual DeviceData_Router *CreateDevice(int iPK_DeviceTemplate, string sDescription, string sIP_Address, string sMac_address, int iPK_Device_ControlledVia) = 0;
	virtual void SetDeviceData(int nPK_Device, int nFK_DeviceData, string sValue) = 0;
	virtual void SetDeviceData(DeviceData_Router *pDeviceData_Router, int nFK_DeviceData, string sValue) = 0;
};
//----------------------------------------------------------------------------------------------
#endif //__DATA_LAYER_H__
