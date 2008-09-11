/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef DCE_Gen_h
#define DCE_Gen_h

#include "PlutoUtils/DBHelper.h"
#include "../pluto_main/Database_pluto_main.h"
#include "../pluto_main/Table_Command.h"

#include <memory>

class CommandInfo
{
public:
	class Row_Command *m_pRow_Command;
	class Row_DeviceCommandGroup_Command *m_pRow_DeviceCommandGroup_Command;
	string m_sParmsWithType_In,m_sParmsWithType_Out,m_sParmsWithTypePointers_Out,m_sAssignParmToLocal_In,m_sAssignParmToLocal_Out,m_sParmsWithNoType_In,m_sParmsWithNoType_Out,m_sPassingToMessage_In,m_sPassingToMessage_Out,m_sAssignLocalToParm;
	vector<Row_Command_CommandParameter *> m_vectRow_Command_CommandParameter_In,m_vectRow_Command_CommandParameter_Out;


	string CPPName() { return FileUtils::ValidCPPName(m_pRow_Command->Description_get()); }
};

class DataInfo
{
public:
	class Row_DeviceData *m_pRow_DeviceData;
	bool m_bCanSet;
};

class DeviceInfo
{
public:
	map<int,class CommandInfo *> m_mapCommandInfo;

	list<class Row_Event *> m_listRow_Event;
	map<int,string> m_mapEventDeclarations;
	map<int,string> m_mapEventParms;
	map<int,class Row_DeviceTemplate_Event *> m_mapRow_DeviceTemplate_Event;
	map<int,class DataInfo *> m_mapDataInfo;
	class Row_DeviceTemplate *m_pRow_DeviceTemplate;
	DeviceInfo(class Row_DeviceTemplate *pRow_DeviceTemplate) { m_pRow_DeviceTemplate=pRow_DeviceTemplate; }
};

//forward declarations
class Row_EventParameter;
class Row_CommandParameter;

class DCEGen : public DBHelper
{
	string m_sGeneratedOutput,m_sTemplateInput,m_sTemplateOutput;

	//helper functions

	/*
	 *	Returns a string with pluto_main's generated const for that device template
	 */
	string GetDeviceTemplateConstStr(Row_DeviceTemplate* pRow_DeviceTemplate);

	/*
	 *	Returns a string with pluto_main's generated const for that command
	 */
	string GetCommandConstStr(Row_Command* pRow_Command);

	/*
	*	Returns a string with pluto_main's generated const for that event parameter
	*/
	string GetEventParameterConstStr(Row_EventParameter* pRow_EventParameter);

	/*
	*	Returns a string with pluto_main's generated const for that command parameter
	*/
	string GetCommandParameterConstStr(Row_CommandParameter* pRow_CommandParameter);

	/*
	*	Returns a string with pluto_main's generated const for that event
	*/
	string GetEventConstStr(Row_Event* pRow_Event);

	/*
	*	Returns a string with pluto_main's generated const for that device data
	*/
	string GetDeviceDataConstStr(Row_DeviceData* pRow_DeviceData);

public:
	map<int,int> m_mapGeneratedDevices;  // Keep track of the devices we've already generated
	int m_dwPK_DeviceTemplate;
	auto_ptr<Database_pluto_main> m_spDatabase_pluto_main;

	DCEGen(int PK_DeviceTemplate,string GeneratedOutput,string TemplateInput,string TemplateOutput,string DBHost,string DBUser,string DBPassword,string DBName,int DBPort);

	void GenerateDevice(int PK_DeviceTemplate,bool bTemplates);
	void AddChildrenByCategory(class Row_DeviceCategory *p_Row_DeviceCategory,map<int,class Row_DeviceTemplate *> *p_mapRow_MasterDevice_Children);
	void CreateDeviceFile(class Row_DeviceTemplate *p_Row_DeviceTemplate,map<int,class Row_DeviceTemplate *> *p_mapRow_MasterDevice_Children,bool bTemplates=true);
	void SearchAndReplace(string InputFile,string OutputFile,string Classname,class DeviceInfo *pDeviceInfo,string sMergeTo,string sDesription);
	void WriteGlobals(); 

	// sParmsWithType: int Parm1, string Parm2, etc.
	// sAssignParmsToLocal: int Parm1=pMessage->m_mapParameters[3]; \n string Parm2= ..., etc.
	// sParmsWithNoType: Parm1, Parm2, etc.
	void CreateFunctionParms(Row_Command *pRow_Command,CommandInfo *pCommandInfo);
	void CreateFunctionParms(class Row_Event *pRow_Event,int &ParmCount,string &sParmsWithType,string &sAssignParmToLocal,string &sParmsWithNoType,string &sPassingToMessage);
	void CreateFunctionParms(class Row_Request *pRow_Request,string &sParmsWithType,string &sAssignParmToLocal,string &sParmsWithNoType,string &sPassingToMessage);
	// bByReference is used for the Requests where some parameters are passed by reference instead
	void CreateFunctionParms(int iParameterID, string ParameterConstName, int iParameterType,string ParameterName,string &sParmsWithType,string &sAssignParmToLocal,string &sParmsWithNoType,string &sPassingToMessage,bool bByReference=false);
	string GetPrefix(int PK_ParameterType); // Get the prefix to put on local variables based on this type of parameter
	string CastTypeToChar(string s,int PK_ParameterType); // assume s is a string, cast it as the correct type
	string CastStringToType(string s,int PK_ParameterType); // assume s is the given type, cast it to a const char
};

#endif
