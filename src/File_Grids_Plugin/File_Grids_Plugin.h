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
#ifndef File_Grids_Plugin_h
#define File_Grids_Plugin_h

//	DCE Implemenation for #10 File Grids Plug-in

#include "Gen_Devices/File_Grids_PluginBase.h"
//<-dceag-d-e->

#include "Datagrid_Plugin/Datagrid_Plugin.h"
class Database_pluto_main;

//<-dceag-decl-b->! custom
namespace DCE
{
class File_Grids_Plugin : public File_Grids_Plugin_Command, public DataGridGeneratorPlugIn
{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		File_Grids_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~File_Grids_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	// Private member variables
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Media_Plugin *m_pMedia_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;

	// Private methods
public:
	// Public member variables

	// Datagrids
	class DataGridTable *FileList(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);

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


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
