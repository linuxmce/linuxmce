#include "DataLayer_JSON.h"
//----------------------------------------------------------------------------------------------
extern "C"
{
	#include "json.h" 
	#include "bits.h"
	#include "debug.h"
	#include "linkhash.h"
	#include "arraylist.h"
	#include "json_util.h"
	#include "json_object.h"
	#include "json_object_private.h"
	#include "json_tokener.h" 
}

#include "PlutoUtils/LZO.h"
#include "PlutoUtils/PlutoDefs.h"
//----------------------------------------------------------------------------------------------
#if !defined(WIN32) && defined(EMBEDDED_LMCE)
	#define JSON_DYNAMIC_CONFIG_FILE "/etc/pluto/pluto.json.lzo"
	#define JSON_STATIC_CONFIG_FILE "/etc/pluto/pluto_main.json.lzo"
	#define JSON_DEVICES_CONFIG_FILE "/etc/pluto/devices.json.lzo"
	#define DEVICES_FILE "/etc/pluto/devices"
#else
	#define JSON_DYNAMIC_CONFIG_FILE "pluto.json.lzo"
	#define JSON_STATIC_CONFIG_FILE "pluto_main.json.lzo"
	#define JSON_DEVICES_CONFIG_FILE "devices.json.lzo"
	#define DEVICES_FILE "devices"
#endif
//----------------------------------------------------------------------------------------------
#define INSTALLATIONID 1 //HARDCODED VALUE
//----------------------------------------------------------------------------------------------
#define ADD_STRING_CHILD(parent, key, value) json_object_object_add(parent, key, json_object_new_string(const_cast<char *>(value.c_str())));
//----------------------------------------------------------------------------------------------
DataLayer_JSON::DataLayer_JSON(void) : m_root_json_obj_Devices(NULL), m_root_json_obj_NonDevices(NULL), m_root_json_obj_PM(NULL), m_DataMutex("data")
{
	m_dwPK_Device_Largest = 0;
	m_bNeedToSave=false;

	pthread_mutexattr_init(&m_MutexAttr);
	pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	m_DataMutex.Init(&m_MutexAttr);

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(1);      //1 = number of worker threads
}
//----------------------------------------------------------------------------------------------
DataLayer_JSON::~DataLayer_JSON(void)
{
	if( m_bNeedToSave )
		DoSave();

	if( m_root_json_obj_Devices )
		json_object_put(m_root_json_obj_Devices); 
	if( m_root_json_obj_NonDevices )
		json_object_put(m_root_json_obj_NonDevices); 
	if( m_root_json_obj_PM )
		json_object_put(m_root_json_obj_PM); 

	delete m_pAlarmManager;

	pthread_mutexattr_destroy(&m_MutexAttr);
}
//----------------------------------------------------------------------------------------------
bool DataLayer_JSON::Load()
{
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);

	if(LoadDynamicConfiguration() && LoadDevicesConfiguration() && LoadStaticConfiguration())
	{
		UpdateDevicesTree();
		SaveDevicesFile(); //update devices file
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------------
void DataLayer_JSON::PluginsLoaded()
{
	if( m_root_json_obj_Devices )
	{
		json_object_put(m_root_json_obj_Devices); 
		m_root_json_obj_Devices=NULL;
	}
	if( m_root_json_obj_NonDevices )
	{
		json_object_put(m_root_json_obj_NonDevices);
		m_root_json_obj_NonDevices=NULL;
	}
	if( m_root_json_obj_PM )
	{
		json_object_put(m_root_json_obj_PM);
		m_root_json_obj_PM=NULL;
	}
}
//----------------------------------------------------------------------------------------------
bool DataLayer_JSON::Save()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataLayer_JSON::Save starting");
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);
	m_bNeedToSave=true;
	m_pAlarmManager->CancelAlarmByType(DELAYED_SAVE);
	m_pAlarmManager->AddRelativeAlarm(2,this,DELAYED_SAVE,NULL);
	return true;
}

void DataLayer_JSON::AlarmCallback(int id, void* param)
{
	if( id==DELAYED_SAVE )
		DoSave();
}

//----------------------------------------------------------------------------------------------
bool DataLayer_JSON::DoSave()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataLayer_JSON::DoSave starting");
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);
	UpdateDevicesTree();

	SaveDevicesFile();
	SaveDevicesList();
	SaveDevices();

	//NOTE: json-c saves the json data into an unformatted string
	//use http://www.jsonlint.com/ to format the json file
	string sData(json_object_to_json_string(m_root_json_obj_Devices));

#ifdef WIN32
	//Also save the uncompressed version -- for debugging
	string sPlainFileName = JSON_DEVICES_CONFIG_FILE;
	StringUtils::Replace(&sPlainFileName, ".lzo", "");
	FileUtils::WriteTextFile(sPlainFileName, sData);
#endif

	//Save into compressed version
	long iSize = 0;
	char *pCompressedData = LZO::Compress((lzo_byte *)sData.c_str(), (lzo_uint)sData.size(), (lzo_uint *)&iSize);
	FileUtils::WriteBufferIntoFile(JSON_DEVICES_CONFIG_FILE, pCompressedData, iSize);
	delete [] pCompressedData;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataLayer_JSON::Save done");

	PluginsLoaded(); // This will delete the newly created objects so we're not holding up the memory

	m_bNeedToSave=false;
	return true;
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::SaveDevicesFile()
{
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);

	string sDevices;
	for(std::map<int, DeviceData_Router *>::iterator it = m_mapDeviceData_Router.begin(),
		end = m_mapDeviceData_Router.end(); it != end; ++it)
	{
		DeviceData_Router *pDeviceData_Router = it->second;

		if(
			pDeviceData_Router->m_dwPK_DeviceCategory != DEVICECATEGORY_Lighting_Device_CONST &&
			pDeviceData_Router->m_dwPK_DeviceCategory != DEVICECATEGORY_Surveillance_Cameras_CONST &&
			pDeviceData_Router->m_dwPK_DeviceCategory != DEVICECATEGORY_IP_Cameras_CONST &&
			pDeviceData_Router->m_dwPK_DeviceCategory != DEVICECATEGORY_Climate_Device_CONST
   	    )
			continue;

		if(pDeviceData_Router->m_sDescription.empty())
		{
			DeviceTemplate_Data* pDeviceTemplate_Data = DeviceTemplate(pDeviceData_Router->m_dwPK_DeviceTemplate);
			if(NULL != pDeviceTemplate_Data)
				pDeviceData_Router->m_sDescription = pDeviceTemplate_Data->Description();
			else
				pDeviceData_Router->m_sDescription = "Unknown device";
		}

		sDevices += 
			StringUtils::ltos(pDeviceData_Router->m_dwPK_Device) + "|" + 
			StringUtils::ltos(pDeviceData_Router->m_dwPK_DeviceTemplate) + "|" + 
			pDeviceData_Router->m_sIPAddress + "|" + 
			pDeviceData_Router->m_sDescription + "|" + 
			m_mapRoom_Data[pDeviceData_Router->m_dwPK_Room].Description() + "\n"; 
	}

	//save devices list
	FileUtils::WriteBufferIntoFile(DEVICES_FILE, sDevices.c_str(), sDevices.size());
}
//----------------------------------------------------------------------------------------------
std::map<int, DeviceData_Router *>& DataLayer_JSON::Devices()
{
	return m_mapDeviceData_Router;
}
//----------------------------------------------------------------------------------------------
bool DataLayer_JSON::LoadDevicesConfiguration(int iUseBackup)
{
	string sFilename = JSON_DEVICES_CONFIG_FILE;
	if( iUseBackup )
		sFilename += "." + StringUtils::itos(iUseBackup);

	char *pData = GetUncompressedDataFromFile(sFilename);

	bool bGotDevices=false;

	if(NULL != pData)
	{
		try
		{
			m_root_json_obj_Devices = json_tokener_parse(pData);
			PLUTO_SAFE_DELETE_ARRAY(pData);

			struct json_object_iter iter;
			json_object_object_foreachC(m_root_json_obj_Devices, iter) 
			{
				string sValue = iter.key;

				if(sValue == "Device_ids")
					ParseDevicesList(iter.val);
				else if(sValue == "Device")
				{
					bGotDevices=true;
					ParseDevices(iter.val);
				}
			}
		}
		catch(...)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DataLayer::LoadDevicesConfiguration exception on file %s", sFilename.c_str());
			bGotDevices=false;  // So we'll retry
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "DataLayer: Got %d devices", m_mapDeviceData_Router.size());
		if( bGotDevices )
			return true;
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DataLayer: LoadDevicesConfiguration %s doesn't exist", sFilename.c_str());

	if( bGotDevices == false )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DataLayer: LoadDevicesConfiguration failed to load %s retry %d", sFilename.c_str(), iUseBackup);
		if( iUseBackup<5 )
			return LoadDevicesConfiguration(iUseBackup+1);
	}

	return false;
}
//----------------------------------------------------------------------------------------------
bool DataLayer_JSON::LoadDynamicConfiguration(int iUseBackup)
{
	string sFilename = JSON_DYNAMIC_CONFIG_FILE;
	if( iUseBackup )
		sFilename += "." + StringUtils::itos(iUseBackup);

	char *pData = GetUncompressedDataFromFile(sFilename);

	bool bGotScenes=false,bGotRooms=false;
	if(NULL != pData)
	{
		try
		{
			m_root_json_obj_NonDevices = json_tokener_parse(pData);
			PLUTO_SAFE_DELETE_ARRAY(pData);

			struct json_object_iter iter;
			json_object_object_foreachC(m_root_json_obj_NonDevices, iter) 
			{
				string sValue = iter.key;

				if(sValue == "scenes")
				{
					bGotScenes = true;
					ParseScenes(iter.val);
				}
				if(sValue == "Room")
				{
					bGotRooms = true;
					ParseRooms(iter.val);
				}
			}
		}
		catch(...)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DataLayer::LoadDynamicConfiguration exception on file %s", sFilename.c_str());
			bGotScenes=bGotRooms=false;  // So we'll retry
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "DataLayer: Got %d scenes from %s", m_mapScene_Data.size(), sFilename.c_str());
		for( std::map<int, Scene_Data>::iterator it = m_mapScene_Data.begin(); it != m_mapScene_Data.end(); ++it )
		{
			Scene_Data *pScene_Data = &(it->second);
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataLayer: Scene %d=%s", it->first, pScene_Data->Description().c_str());
		}

		if( bGotScenes && bGotRooms )
			return true;
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DataLayer: LoadDynamicConfiguration %s doesn't exist", sFilename.c_str());

	if( bGotScenes == false || bGotRooms==false )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DataLayer: LoadDynamicConfiguration failed to load %s retry %d", sFilename.c_str(), iUseBackup);
		if( iUseBackup<5 )
			return LoadDynamicConfiguration(iUseBackup+1);
	}

	return false;
}
//----------------------------------------------------------------------------------------------
bool DataLayer_JSON::LoadStaticConfiguration()
{
	char *pData = GetUncompressedDataFromFile(JSON_STATIC_CONFIG_FILE);

	if(NULL != pData)
	{
		m_root_json_obj_PM = json_tokener_parse(pData);

		struct json_object_iter iter;
		json_object_object_foreachC(m_root_json_obj_PM, iter) 
		{
			string sValue = iter.key;

			if(sValue == "DeviceTemplate")
				ParseDeviceTemplates(iter.val);
			else if(sValue == "DeviceCategory")
				ParseDeviceCategories(iter.val);
		}

		PLUTO_SAFE_DELETE_ARRAY(pData);

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "DataLayer: Got %d device templates", m_mapDeviceTemplate_Data.size());
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "DataLayer: Got %d device categories", m_mapDeviceCategory_Data.size());

		UpdateDevicesTree();
		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::UpdateDevicesTree()
{
	for(map<int, DeviceData_Router *>::iterator it = m_mapDeviceData_Router.begin(); it != m_mapDeviceData_Router.end(); ++it)
	{
		DeviceData_Router *pDevice = it->second;

		std::map<int, DeviceTemplate_Data>::iterator it_devtemplate = m_mapDeviceTemplate_Data.find(pDevice->m_dwPK_DeviceTemplate);
		if(it_devtemplate != m_mapDeviceTemplate_Data.end())
		{
			pDevice->m_bImplementsDCE = it_devtemplate->second.ImplementsDCE();
			pDevice->m_bIsEmbedded = it_devtemplate->second.IsEmbedded();
			pDevice->m_dwPK_DeviceCategory = it_devtemplate->second.DeviceCategory();
			pDevice->m_sCommandLine = it_devtemplate->second.CommandLine();

			if(pDevice->m_sDescription.empty())
				pDevice->m_sDescription = it_devtemplate->second.Description();
		}
	}
}
//----------------------------------------------------------------------------------------------
int DataLayer_JSON::LargestDeviceNumber()
{
	return m_dwPK_Device_Largest;
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDevicesList(struct json_object *json_obj)
{
	if(json_obj != NULL && json_obj->o_type == json_type_array)
	{
		for(int i = 0; i < json_object_array_length(json_obj); i++) 
		{
			struct json_object *obj_device_id = json_object_array_get_idx(json_obj, i);

			if(obj_device_id->o_type == json_type_int)
			{
				int nDeviceID = json_object_get_int(obj_device_id);
				m_mapDeviceData_Router[nDeviceID] = NULL;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Device id from array must be an int!");
			}
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Not an array : devices_list!");
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDevices(struct json_object *json_obj)
{
	struct json_object *obj_devices = json_obj;
	struct json_object_iter iter_devices;
	json_object_object_foreachC(obj_devices, iter_devices) 
	{
		string sDeviceID = iter_devices.key;

		StringUtils::Replace(&sDeviceID, "PK_Device_", "");
		int nDeviceID = atoi(sDeviceID.c_str());

		if(m_mapDeviceData_Router.find(nDeviceID) != m_mapDeviceData_Router.end())
		{
			DeviceData_Router *pDevice = NULL;
			int PK_DeviceTemplate = 0;
			int PK_Installation = INSTALLATIONID; //Hardcoded!
			int PK_Device_ControlledVia = 0;
			int PK_Room = 0;
			string sDescription;
			map<int, string> mapDeviceData;
			map<string, string> mapDeviceParams;
	
			struct json_object *obj_device = iter_devices.val;
			struct json_object_iter iter_device;
			json_object_object_foreachC(obj_device, iter_device) 
			{
				string sValue = iter_device.key;

				if(sValue == "Description" && iter_device.val->o_type == json_type_string)
				{
					sDescription = json_object_get_string(iter_device.val);
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "\tDevice %d Description: %s", nDeviceID, sDescription.c_str());
				}
				else if(sValue == "FK_DeviceTemplate" && (iter_device.val->o_type == json_type_int || iter_device.val->o_type == json_type_string) )
					PK_DeviceTemplate = json_object_get_int(iter_device.val);
				else if(sValue == "FK_Room" && (iter_device.val->o_type == json_type_int || iter_device.val->o_type == json_type_string))
					PK_Room = json_object_get_int(iter_device.val);
				else if(sValue == "FK_Device_ControlledVia" && (iter_device.val->o_type == json_type_int || iter_device.val->o_type == json_type_string))
					PK_Device_ControlledVia = json_object_get_int(iter_device.val);
				else if(sValue == "params")
					ParseDeviceParameters(mapDeviceParams, iter_device.val);
				else if(sValue == "DeviceData" && iter_device.val->o_type == json_type_object)
					ParseDeviceDataList(nDeviceID,mapDeviceData, iter_device.val);
			}

			pDevice = new DeviceData_Router(nDeviceID, PK_DeviceTemplate, PK_Installation, PK_Device_ControlledVia);
			pDevice->m_sDescription = sDescription;
			pDevice->m_dwPK_Room = PK_Room;
			pDevice->AssignParameters(mapDeviceData);
			m_mapDeviceData_Router[pDevice->m_dwPK_Device] = pDevice;

			if(m_dwPK_Device_Largest < nDeviceID)
				m_dwPK_Device_Largest = nDeviceID;

			AssignParametersToDevice(pDevice, mapDeviceParams);

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Parsing device %d: template %d controlled via %d room %d state %s status %s IPaddress %s mac %s disabled %d",
				nDeviceID, PK_DeviceTemplate, PK_Device_ControlledVia, pDevice->m_dwPK_Room, pDevice->m_sState_get().c_str(), pDevice->m_sStatus_get().c_str(),
				pDevice->m_sIPAddress.c_str(), pDevice->m_sMacAddress.c_str(), (int) pDevice->m_bDisabled);
		}
 		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got a device, but it's not in the device id list: '%s'",
				iter_devices.key);
		}
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDeviceDataList(int PK_Device,std::map<int, string>& mapDeviceData, struct json_object *json_obj)
{
	struct json_object_iter iter_devicedata;
	json_object_object_foreachC(json_obj, iter_devicedata)
	{
		string sValue = iter_devicedata.key;
		int PK_DeviceData = atoi(StringUtils::Replace(&sValue, "FK_DeviceData_", "").c_str());

		if(iter_devicedata.val->o_type == json_type_int || iter_devicedata.val->o_type == json_type_string)
		{
			string sValue = json_object_get_string(iter_devicedata.val);
			mapDeviceData[PK_DeviceData] = sValue;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "\tDevice %d data %d = %s", PK_Device, PK_DeviceData, sValue.c_str());
		}
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDeviceParameters(std::map<string, string>& mapDeviceParams, struct json_object *json_obj)
{
	struct json_object_iter iter_devicedata;
	json_object_object_foreachC(json_obj, iter_devicedata)
	{
		string sKey = iter_devicedata.key;

		if(iter_devicedata.val->o_type == json_type_int || iter_devicedata.val->o_type == json_type_string)
			mapDeviceParams[sKey] = json_object_get_string(iter_devicedata.val);
	}	
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::AssignParametersToDevice(DeviceData_Router *pDevice, const std::map<string, string>& mapDeviceParams)
{
	std::map<string, string>::const_iterator it = mapDeviceParams.begin();
	
	for(; it != mapDeviceParams.end(); ++it)
	{
		if(it->first == "IPaddress")
			pDevice->m_sIPAddress = it->second;
		else if(it->first == "MACaddress")
			pDevice->m_sMacAddress = it->second;
		else if(it->first == "IgnoreOnOff")
			pDevice->m_bIgnoreOnOff = it->second == "1";
		else if(it->first == "State")
			pDevice->m_sState_set(it->second);
		else if(it->first == "State_NonTemporary")
			pDevice->m_sState_NonTemporary_set(it->second);
		else if(it->first == "Status")
			pDevice->m_sStatus_set(it->second);
		else if(it->first == "Disabled")
			pDevice->m_bDisabled = it->second == "1";
		else if(it->first == "FK_Device_RouteTo")
		{
			int FK_Device_RouterTo = atoi(it->second.c_str());

			std::map<int, DeviceData_Router *>::const_iterator it_device = m_mapDeviceData_Router.find(FK_Device_RouterTo);
			if(it_device != m_mapDeviceData_Router.end())
			{
				if(NULL != it_device->second)
				{
					pDevice->m_pDevice_RouteTo = it_device->second;
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Message to device %d routed to device %d",
						pDevice->m_dwPK_Device, FK_Device_RouterTo);
				}
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot find device %d to route messages from %d",
					FK_Device_RouterTo, pDevice->m_dwPK_Device);
			}
		}
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDeviceTemplates(struct json_object *json_obj)
{
	struct json_object *obj_devices = json_obj;
	struct json_object_iter iter_devices;
	json_object_object_foreachC(obj_devices, iter_devices) 
	{
		string sDeviceTemplateID = iter_devices.key;

		StringUtils::Replace(&sDeviceTemplateID, "PK_DeviceTemplate_", "");
		int nDeviceTemplateID = atoi(sDeviceTemplateID.c_str());

		DeviceTemplate_Data aDeviceTemplate_Data(nDeviceTemplateID);

		struct json_object_iter iter_deviceparams;
		json_object_object_foreachC(iter_devices.val, iter_deviceparams)
		{
			string sKey = iter_deviceparams.key;

			if(iter_deviceparams.val->o_type == json_type_int || iter_deviceparams.val->o_type == json_type_string)
			{
				string sValue = json_object_get_string(iter_deviceparams.val);

				if(sKey == "Description")
					aDeviceTemplate_Data.Description(sValue);
				else if(sKey == "FK_DeviceCategory")
					aDeviceTemplate_Data.DeviceCategory(atoi(sValue.c_str()));
				else if(sKey == "ImplementsDCE")
					aDeviceTemplate_Data.ImplementsDCE(sValue == "1");
				else if(sKey == "IsEmbedded")
					aDeviceTemplate_Data.IsEmbedded(sValue == "1");
				else if(sKey == "CommandLine")
					aDeviceTemplate_Data.CommandLine(sValue);
			}
		}	

		m_mapDeviceTemplate_Data[nDeviceTemplateID] = aDeviceTemplate_Data;
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDeviceCategories(struct json_object *json_obj)
{
	struct json_object *obj_devices = json_obj;
	struct json_object_iter iter_categories;
	json_object_object_foreachC(obj_devices, iter_categories) 
	{
		string sDeviceCategoryID = iter_categories.key;

		StringUtils::Replace(&sDeviceCategoryID, "PK_DeviceCategory_", "");
		int nDeviceCategoryID = atoi(sDeviceCategoryID.c_str());

		DeviceCategory_Data aDeviceCategory_Data(nDeviceCategoryID);

		struct json_object_iter iter_deviceparams;
		json_object_object_foreachC(iter_categories.val, iter_deviceparams)
		{
			string sKey = iter_deviceparams.key;

			if(iter_deviceparams.val->o_type == json_type_int || iter_deviceparams.val->o_type == json_type_string)
			{
				string sValue = json_object_get_string(iter_deviceparams.val);

				if(sKey == "Description")
					aDeviceCategory_Data.Description(sValue);
				else if(sKey == "FK_DeviceCategory_Parent")
					aDeviceCategory_Data.DeviceCategoryParent(atoi(sValue.c_str()));
			}
		}	

		m_mapDeviceCategory_Data[nDeviceCategoryID] = aDeviceCategory_Data;
	}	
}
//----------------------------------------------------------------------------------------------
char *DataLayer_JSON::GetUncompressedDataFromFile(string sFileName)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataLayer_JSON::GetUncompressedDataFromFile starting %s", sFileName.c_str());

#ifndef WIN32 //force lzo creating everytime for debugging
	if(!FileUtils::FileExists(sFileName))
#endif
	{
		string sPlainFileName = sFileName;
		StringUtils::Replace(&sPlainFileName, ".lzo", "");

		if(FileUtils::FileExists(sPlainFileName))
		{
			string sData;
			FileUtils::ReadTextFile(sPlainFileName, sData);

			long iSize = 0;
			char *pCompressedData = LZO::Compress((lzo_byte *)sData.c_str(), (lzo_uint)sData.size(), (lzo_uint *)&iSize);
			
			FileUtils::WriteBufferIntoFile(sFileName, pCompressedData, iSize);
			delete [] pCompressedData;
		}
	}	

	size_t nCompressedDataSize = 0;
	char *pCompressedData = FileUtils::ReadFileIntoBuffer(sFileName, nCompressedDataSize);

	if(NULL == pCompressedData || nCompressedDataSize < 5)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unabled to find/parse json file: %s", sFileName.c_str());
		return NULL;
	}

	long size;
	char *pUncompressedData = LZO::Decompress((lzo_byte *)pCompressedData, (lzo_uint)nCompressedDataSize, (lzo_uint *)&size);

	PLUTO_SAFE_DELETE_ARRAY(pCompressedData);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataLayer_JSON::GetUncompressedDataFromFile done %s", sFileName.c_str());
#ifdef WIN32
	FileUtils::WriteBufferIntoFile("/temp/" + sFileName + ".txt", pUncompressedData,size);
#endif

	return pUncompressedData;
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseScenes(struct json_object *json_obj)
{
	struct json_object *obj_scenes = json_obj;
	struct json_object_iter iter_scenes;
	json_object_object_foreachC(obj_scenes, iter_scenes) 
	{
		string sSceneID = iter_scenes.key;

		StringUtils::Replace(&sSceneID, "scene_", "");
		int nSceneID = atoi(sSceneID.c_str());
		
		Scene_Data scene_data(nSceneID);

		struct json_object_iter iter_sceneparams;
		json_object_object_foreachC(iter_scenes.val, iter_sceneparams)
		{
			string sKey = iter_sceneparams.key;

			if(iter_sceneparams.val->o_type == json_type_int || iter_sceneparams.val->o_type == json_type_string)
			{
				string sValue = json_object_get_string(iter_sceneparams.val);

				if(sKey == "Description")
				{
					scene_data.Description(sValue);
				}
				else if(sKey == "FK_Room")
				{
					scene_data.RoomID(atoi(sValue.c_str()));
				}
			}
			else if(sKey == "commands" && iter_sceneparams.val->o_type == json_type_object)
			{
				ParseCommands(scene_data.Commands(), iter_sceneparams.val);
			}
		}

		m_mapScene_Data[nSceneID] = scene_data;
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseRooms(struct json_object *json_obj)
{
	struct json_object *obj_rooms = json_obj;
	struct json_object_iter iter_rooms;
	json_object_object_foreachC(obj_rooms, iter_rooms) 
	{
		string sRoomID = iter_rooms.key;
		StringUtils::Replace(&sRoomID, "PK_Room_", "");

		int nRoomID = atoi(sRoomID.c_str());

		Room_Data room_data(nRoomID);

		struct json_object_iter iter_roomparams;
		json_object_object_foreachC(iter_rooms.val, iter_roomparams)
		{
			string sKey = iter_roomparams.key;

			if(iter_roomparams.val->o_type == json_type_string)
			{
				string sValue = json_object_get_string(iter_roomparams.val);

				if(sKey == "Description")
				{
					room_data.Description(sValue);
				}
				else if(sKey == "room_type")
				{
					room_data.RoomType(sValue);
				}
				
				//parse here FK_Section or FK_RoomType if needed
			}
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataLayer_JSON::ParseRooms %d=%s", nRoomID, room_data.Description().c_str());

		m_mapRoom_Data[nRoomID] = room_data;
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseCommands(map<int, Command_Data>& mapCommands, struct json_object *json_obj)
{
	struct json_object *obj_commands = json_obj;
	struct json_object_iter iter_commands;
	json_object_object_foreachC(obj_commands, iter_commands) 
	{
		string sCommand = iter_commands.key;
		StringUtils::Replace(&sCommand, "command_", "");
		int nCommandIndex = atoi(sCommand.c_str());
		Command_Data aCommand_data;

		if(iter_commands.val->o_type == json_type_object)
		{
			struct json_object *obj_command = iter_commands.val;
			struct json_object_iter iter_command;
			json_object_object_foreachC(obj_command, iter_command) 
			{
				string sKey = iter_command.key;

				if(iter_command.val->o_type == json_type_int || iter_command.val->o_type == json_type_string )
				{
					int nValue = iter_command.val->o_type == json_type_string ? atoi(json_object_get_string(iter_command.val)) : json_object_get_int(iter_command.val);

					if(sKey == "Device_From")
						aCommand_data.Device_From(nValue);
					else if(sKey == "Device_To")
						aCommand_data.Device_To(nValue);
					else if(sKey == "FK_Command")
						aCommand_data.PK_Command(nValue);
					else if(sKey == "IsTemporary")
						aCommand_data.IsTemporary(nValue);
					else if(sKey == "CancelIfOther")
						aCommand_data.CancelIfOther(nValue);
					else if(sKey == "Delay")
						aCommand_data.Delay(nValue);
				}
				else if(sKey == "params" && iter_command.val->o_type == json_type_object)
				{
					ParseCommandParameters(aCommand_data.Params(), iter_command.val);
				}
			}
		}

		mapCommands[nCommandIndex] = aCommand_data;
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseCommandParameters(std::map<int, string>& mapParams, struct json_object *json_obj)
{
	struct json_object *obj_params = json_obj;
	struct json_object_iter iter_params;
	json_object_object_foreachC(obj_params, iter_params) 
	{
		string sFK_CommandParameter = iter_params.key;
		StringUtils::Replace(&sFK_CommandParameter, "FK_CommandParameter_", "");
		int nFK_CommandParameter = atoi(sFK_CommandParameter.c_str());
		
		if(iter_params.val->o_type == json_type_int || iter_params.val->o_type == json_type_string)
		{
			string sValue = json_object_get_string(iter_params.val);
			mapParams[nFK_CommandParameter] = sValue;
		}
	}
}
//----------------------------------------------------------------------------------------------
Scene_Data* DataLayer_JSON::Scene(int nSceneID)
{
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);

	std::map<int, Scene_Data>::iterator it = m_mapScene_Data.find(nSceneID);
	if(it != m_mapScene_Data.end())
	{
		return &(it->second);
	}

	return NULL;
}
//----------------------------------------------------------------------------------------------
const std::map<int, Scene_Data>& DataLayer_JSON::Scenes()
{
	return m_mapScene_Data;
}
//----------------------------------------------------------------------------------------------
DeviceTemplate_Data* DataLayer_JSON::DeviceTemplate(int nPK_DeviceTemplate)
{
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);

	std::map<int, DeviceTemplate_Data>::iterator it = m_mapDeviceTemplate_Data.find(nPK_DeviceTemplate);

	if(it != m_mapDeviceTemplate_Data.end())
		return &(it->second);

	return NULL;
}
//----------------------------------------------------------------------------------------------
DeviceData_Router *DataLayer_JSON::ChildMatchingDeviceData(int nPK_Device_Parent, int nFK_DeviceData, string sValue)
{
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);

	for(std::map<int, DeviceData_Router *>::iterator it = m_mapDeviceData_Router.begin(),
		end = m_mapDeviceData_Router.end(); it != end; ++it)
	{
		DeviceData_Router *pDeviceData_Router = it->second;

		if(
			pDeviceData_Router->m_dwPK_Device_ControlledVia == nPK_Device_Parent && 
			pDeviceData_Router->m_mapParameters_Find(nFK_DeviceData) == sValue
		)
			return pDeviceData_Router;
	}
	
	return NULL;
}
//----------------------------------------------------------------------------------------------
DeviceData_Router *DataLayer_JSON::Device(int nPK_Device)
{
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);

	std::map<int, DeviceData_Router *>::iterator it = m_mapDeviceData_Router.find(nPK_Device);
	if(it != m_mapDeviceData_Router.end())
		return it->second;

	return NULL;
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ChildrenDevices(int nPK_Device_Parent, std::vector<DeviceData_Router *>& vectDevice_Children,
	bool bRecursive/* = false*/)
{
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);

	for(std::map<int, DeviceData_Router *>::iterator it = m_mapDeviceData_Router.begin(),
		end = m_mapDeviceData_Router.end(); it != end; ++it)
	{
		DeviceData_Router *pDeviceData_Router = it->second;

		if(pDeviceData_Router->m_dwPK_Device_ControlledVia == nPK_Device_Parent)
		{
			vectDevice_Children.push_back(pDeviceData_Router);

			if(bRecursive)
				ChildrenDevices(pDeviceData_Router->m_dwPK_Device, vectDevice_Children, true);
		}
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::SetDeviceData(int nPK_Device, int nFK_DeviceData, string sValue)
{
	DeviceData_Router *pDeviceData_Router = Device(nPK_Device);

	if(NULL != pDeviceData_Router)
		SetDeviceData(pDeviceData_Router, nFK_DeviceData, sValue);
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::SetDeviceData(DeviceData_Router *pDeviceData_Router, int nFK_DeviceData, string sValue)
{
	pDeviceData_Router->ReplaceParameter(nFK_DeviceData, sValue);
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::SaveDevicesList()
{
	//delete the old node
	if( m_root_json_obj_Devices )
		json_object_object_del(m_root_json_obj_Devices, "Device_ids");
	else
		m_root_json_obj_Devices = json_object_new_object();

	//create a new one
	struct json_object *devices_list_obj = json_object_new_array();
	for(std::map<int, DeviceData_Router *>::iterator it = m_mapDeviceData_Router.begin(),
		end = m_mapDeviceData_Router.end(); it != end; ++it)
	{
		DeviceData_Router *pDeviceData_Router = it->second;

		if(!pDeviceData_Router->IsDeleted())
			json_object_array_add(devices_list_obj, json_object_new_int(pDeviceData_Router->m_dwPK_Device));
	}

	//add it to root node
	json_object_object_add(m_root_json_obj_Devices, "Device_ids", devices_list_obj);
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::SaveDevices()
{
	//delete the old node
	if( m_root_json_obj_Devices )
		json_object_object_del(m_root_json_obj_Devices, "Devices");
	else
		m_root_json_obj_Devices = json_object_new_object();

	//create a new one
	struct json_object *devices_obj = json_object_new_object();

	int PK_Device_ZWave=0;
	int iUnassignedDevices=0; // Count of devices with no room
	int iAutomationDevices=0; // Count of devices that are a child of the automation system
	for(std::map<int, DeviceData_Router *>::iterator it = m_mapDeviceData_Router.begin(),
		end = m_mapDeviceData_Router.end(); it != end; ++it)
	{
		DeviceData_Router *pDeviceData_Router = it->second;

		if(pDeviceData_Router->IsDeleted())
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device %d is deleted. Skipping... ", pDeviceData_Router->m_dwPK_Device);
			continue;
		}

		/*
			Nasty temporary hack.  Vali needs the total number of ZWave devices.  Make this general */

		if( pDeviceData_Router->m_dwPK_DeviceTemplate==1754 )
			PK_Device_ZWave=pDeviceData_Router->m_dwPK_Device;
		if( PK_Device_ZWave && pDeviceData_Router->m_dwPK_Device_ControlledVia==PK_Device_ZWave && pDeviceData_Router->m_dwPK_DeviceTemplate!=1945 )
			iAutomationDevices++;

		if( pDeviceData_Router->m_dwPK_Room==0 )
			iUnassignedDevices++; 

        struct json_object *device_obj = json_object_new_object();

		ADD_STRING_CHILD(device_obj, "Description", pDeviceData_Router->m_sDescription);
		ADD_STRING_CHILD(device_obj, "FK_DeviceTemplate", StringUtils::ltos(pDeviceData_Router->m_dwPK_DeviceTemplate));
		ADD_STRING_CHILD(device_obj, "FK_Room", StringUtils::ltos(pDeviceData_Router->m_dwPK_Room));
		ADD_STRING_CHILD(device_obj, "FK_Device_ControlledVia", StringUtils::ltos(pDeviceData_Router->m_dwPK_Device_ControlledVia));

		//create DeviceData node
		struct json_object *dd_obj = json_object_new_object();

		{
			PLUTO_SAFETY_LOCK(dm_param, pDeviceData_Router->Mutex());
			for(std::map<int, string>::const_iterator it_params = pDeviceData_Router->Parameters().begin();
				it_params != pDeviceData_Router->Parameters().end(); ++it_params)
			{
				string sDDNodeName = "FK_DeviceData_" + StringUtils::ltos(it_params->first);
				json_object_object_add(dd_obj, const_cast<char *>(sDDNodeName.c_str()), json_object_new_string(const_cast<char *>(it_params->second.c_str())));
			}
		}

		//add DeviceData node
		json_object_object_add(device_obj, "DeviceData", dd_obj);

		//create params node
		struct json_object *params_obj = json_object_new_object();

		ADD_STRING_CHILD(params_obj, "IPaddress", pDeviceData_Router->m_sIPAddress);
		ADD_STRING_CHILD(params_obj, "MACaddress", pDeviceData_Router->m_sMacAddress);
		ADD_STRING_CHILD(params_obj, "IgnoreOnOff", StringUtils::ltos((long)pDeviceData_Router->m_bIgnoreOnOff));
		ADD_STRING_CHILD(params_obj, "State", pDeviceData_Router->m_sState_get());
		ADD_STRING_CHILD(params_obj, "State_NonTemporary", pDeviceData_Router->m_sState_NonTemporary_get());
		ADD_STRING_CHILD(params_obj, "Status", pDeviceData_Router->m_sStatus_get());
		ADD_STRING_CHILD(params_obj, "Disabled", StringUtils::ltos((long)pDeviceData_Router->m_bDisabled));

		if(NULL != pDeviceData_Router->m_pDevice_RouteTo && !pDeviceData_Router->m_pDevice_RouteTo->IsDeleted())
			ADD_STRING_CHILD(params_obj, "FK_Device_RouteTo", StringUtils::ltos(pDeviceData_Router->m_pDevice_RouteTo->m_dwPK_Device));

		//add params node
		json_object_object_add(device_obj, "params", params_obj);

		string sNodeName = "PK_Device_" + StringUtils::ltos(pDeviceData_Router->m_dwPK_Device);
		json_object_object_add(devices_obj, const_cast<char *>(sNodeName.c_str()), device_obj);
	}

	ADD_STRING_CHILD(m_root_json_obj_Devices, "UnassignedDevices", StringUtils::itos(iUnassignedDevices));
	ADD_STRING_CHILD(m_root_json_obj_Devices, "AutomationDevices", StringUtils::itos(iAutomationDevices));

	//add it to root node
	json_object_object_add(m_root_json_obj_Devices, "Device", devices_obj);
}
//----------------------------------------------------------------------------------------------
DeviceData_Router *DataLayer_JSON::CreateDevice(int iPK_DeviceTemplate, string sDescription, 
	string sIP_Address, string sMac_address, int iPK_Device_ControlledVia)
{
	int nPK_Device = ++m_dwPK_Device_Largest;

	DeviceData_Router *pDeviceData_Router = new DeviceData_Router(
		nPK_Device, iPK_DeviceTemplate, INSTALLATIONID, iPK_Device_ControlledVia);

	pDeviceData_Router->m_sDescription = sDescription;
	pDeviceData_Router->m_sMacAddress = sMac_address;
	pDeviceData_Router->m_sIPAddress = sIP_Address;

	PLUTO_SAFETY_LOCK(dm, m_DataMutex);
	m_mapDeviceData_Router[nPK_Device] = pDeviceData_Router;

	return pDeviceData_Router;
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::DevicesByTemplate(int nPK_DeviceTemplate, std::list<DeviceData_Router *>& listDevices)
{
	PLUTO_SAFETY_LOCK(dm, m_DataMutex);

	for(std::map<int, DeviceData_Router *>::iterator it = m_mapDeviceData_Router.begin(),
		end = m_mapDeviceData_Router.end(); it != end; ++it)
	{
		DeviceData_Router *pDeviceData_Router = it->second;

		if(pDeviceData_Router->m_dwPK_DeviceTemplate == nPK_DeviceTemplate)
			listDevices.push_back(pDeviceData_Router);
	}
}
//----------------------------------------------------------------------------------------------
