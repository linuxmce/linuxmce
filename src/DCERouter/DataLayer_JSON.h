#ifndef __DATA_LAYER_JSON_H__
#define __DATA_LAYER_JSON_H__

#include "IDataLayer.h"
#include "DeviceTemplate_Data.h"
#include "DeviceCategory_Data.h"
#include "Scene_Data.h"
#include "Room_Data.h"
#include "AlarmManager.h"
#include <map>

#define DELAYED_SAVE	1
//----------------------------------------------------------------------------------------------
class DataLayer_JSON : public IDataLayer, public AlarmEvent
{
	AlarmManager* m_pAlarmManager;
	int m_dwPK_Device_Largest;

	std::map<int, DeviceData_Router *> m_mapDeviceData_Router;
	std::map<int, DeviceTemplate_Data> m_mapDeviceTemplate_Data;
	std::map<int, DeviceCategory_Data> m_mapDeviceCategory_Data;
	std::map<int, Scene_Data> m_mapScene_Data;
	std::map<int, Room_Data> m_mapRoom_Data;
	
	struct json_object *m_root_json_obj_Devices;
	struct json_object *m_root_json_obj_NonDevices;
	struct json_object *m_root_json_obj_PM;

	pluto_pthread_mutex_t m_DataMutex;
	pthread_mutexattr_t m_MutexAttr;
	bool m_bNeedToSave;  // Set to true when we have something we need to save.  Do the saving offline in a separate thread for efficiency

public:
	DataLayer_JSON(void);
	~DataLayer_JSON(void);

	//load/save data
	bool Load();
	bool Save();
	bool DoSave();
	virtual void PluginsLoaded(); // Free up the json objects stored in memory so the plugin could load

	pluto_pthread_mutex_t& Mutex() { return m_DataMutex; }

	//get devices
	std::map<int, DeviceData_Router *>& Devices();
	struct json_object *m_root_json_obj_NonDevices_get() { return m_root_json_obj_NonDevices; }
	struct json_object *m_root_json_obj_Devices_get() { return m_root_json_obj_Devices; }
	struct json_object *m_root_json_obj_PM_get() { return m_root_json_obj_PM; }

	//queries for scenes and device templates
	Scene_Data* Scene(int nSceneID);
	DeviceTemplate_Data* DeviceTemplate(int nPK_DeviceTemplate);

	//get scenes
	const std::map<int, Scene_Data>& Scenes();

	//queries for devices
	int LargestDeviceNumber();
	DeviceData_Router *ChildMatchingDeviceData(int nPK_Device_Parent, int nFK_DeviceData, string sValue);
	DeviceData_Router *Device(int nPK_Device);
	void ChildrenDevices(int nPK_Device_Parent, std::vector<DeviceData_Router *>& vectDevice_Children, bool bRecursive = false);
	void DevicesByTemplate(int nPK_DeviceTemplate, std::list<DeviceData_Router *>& listDevices);

	//operations on devices
	DeviceData_Router *CreateDevice(int iPK_DeviceTemplate, string sDescription, string sIP_Address, string sMac_address, int iPK_Device_ControlledVia);
	void SetDeviceData(int nPK_Device, int nFK_DeviceData, string sValue);
	void SetDeviceData(DeviceData_Router *pDeviceData_Router, int nFK_DeviceData, string sValue);


private:

	bool LoadDynamicConfiguration(int iUseBackup=0);
	bool LoadDevicesConfiguration(int iUseBackup=0);
	bool LoadStaticConfiguration();
	void UpdateDevicesTree();

	void ParseDevicesList(struct json_object *json_obj);
	void ParseDevices(struct json_object *json_obj);
	void ParseDeviceDataList(int PK_Device,std::map<int, string>& mapDeviceData, struct json_object *json_obj);
	void ParseDeviceParameters(std::map<string, string>& mapDeviceParams, struct json_object *json_obj);
	void ParseScenes(struct json_object *json_obj);
	void ParseRooms(struct json_object *json_obj);
	void ParseCommands(map<int, Command_Data>& mapCommands, struct json_object *json_obj);
	void ParseDeviceTemplates(struct json_object *json_obj);
	void ParseDeviceCategories(struct json_object *json_obj);
	void ParseCommandParameters(std::map<int, string>& mapParams, struct json_object *json_obj);

	void SaveDevicesList();
	void SaveDevices();
	void SaveDevicesFile();

	void AssignParametersToDevice(DeviceData_Router *pDevice, const std::map<string, string>& mapDeviceParams);

	//utils
	char *GetUncompressedDataFromFile(string sFileName);

	void AlarmCallback(int id, void* param);
};
//----------------------------------------------------------------------------------------------
#endif //__DATA_LAYER_JSON_H__

