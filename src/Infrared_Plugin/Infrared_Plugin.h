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
//<-dceag-d-b->
#ifndef Infrared_Plugin_h
#define Infrared_Plugin_h

//	DCE Implemenation for #39 Infrared Plug-in

#include "Gen_Devices/Infrared_PluginBase.h"
//<-dceag-d-e->

#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DCERouter.h"
#include "pluto_main/Database_pluto_main.h"
class IRDevice;

//<-dceag-decl-b->!
namespace DCE
{
	class Infrared_Plugin : public Infrared_Plugin_Command, public DataGridGeneratorPlugIn
	{
//<-dceag-decl-e->
		// Private member variables
		string m_sRemoteMapping; // Key mappings for i/r remote controls

	private:
		    class Orbiter_Plugin *m_pOrbiter_Plugin;
		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Infrared_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Infrared_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		class Datagrid_Plugin *m_pDatagrid_Plugin;
		Database_pluto_main *m_pDatabase_pluto_main;

		class DataGridTable *DevicesGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
		class DataGridTable *CommandsGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
		class DataGridTable *Manufacturers(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
		class DataGridTable *DeviceTemplateByMfrModel(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
		class DataGridTable *InfraredGroups(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
		class DataGridTable *InfraredCodes(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
		class DataGridTable *IRGroupCategories(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
		class DataGridTable *AvailableInputs(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
		class DataGridTable *AVDeviceTypes(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

		void GetInfraredCodes(int iPK_Device,IRDevice &irDevice,bool bNoIRData=false);
		
//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #188 - Get Infrared Codes */
	/** Retrieves all the infrared codes for a given device. */
		/** @param #2 PK_Device */
			/** The device to retrieve the infrared codes for. */
		/** @param #19 Data */
			/** A serialized map(int,string) with the commands/codes. */

	virtual void CMD_Get_Infrared_Codes(int iPK_Device,char **pData,int *iData_Size) { string sCMD_Result; CMD_Get_Infrared_Codes(iPK_Device,pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Get_Infrared_Codes(int iPK_Device,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #250 - Store Infrared Code */
	/** Store a learned infrared code for a "Device" + "Command" pair */
		/** @param #2 PK_Device */
			/** Device this code was learned for */
		/** @param #5 Value To Assign */
			/** IR code in Philips pronto format */
		/** @param #71 PK_Command_Input */
			/** Command this code launches */

	virtual void CMD_Store_Infrared_Code(int iPK_Device,string sValue_To_Assign,int iPK_Command_Input) { string sCMD_Result; CMD_Store_Infrared_Code(iPK_Device,sValue_To_Assign.c_str(),iPK_Command_Input,sCMD_Result,NULL);};
	virtual void CMD_Store_Infrared_Code(int iPK_Device,string sValue_To_Assign,int iPK_Command_Input,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #276 - Add GC100 */
	/** Add a GC100 Device */

	virtual void CMD_Add_GC100() { string sCMD_Result; CMD_Add_GC100(sCMD_Result,NULL);};
	virtual void CMD_Add_GC100(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #688 - Get Remote Control Mapping */
	/** Returns a list of all the commands and mapping information for i/r remotes */
		/** @param #5 Value To Assign */
			/** A list with ScreenType\tRemoteLayout\tMapping */

	virtual void CMD_Get_Remote_Control_Mapping(string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Remote_Control_Mapping(sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Remote_Control_Mapping(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #793 - Get Sibling Remotes */
	/** Return a list of remote remote IDs with remote data. */
		/** @param #5 Value To Assign */
			/** Tilde delimited list, Remote DeviceID, Remote Configuration Data */
		/** @param #209 PK_DeviceCategory */
			/** Device category to search for remotes */

	virtual void CMD_Get_Sibling_Remotes(int iPK_DeviceCategory,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Sibling_Remotes(iPK_DeviceCategory,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Sibling_Remotes(int iPK_DeviceCategory,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
