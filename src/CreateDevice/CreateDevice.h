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
#ifndef CreateDevice_h
#define CreateDevice_h

#include "PlutoUtils/DBHelper.h"

class CreateDevice : public DBHelper
{
	int m_iPK_Installation;
public:
	bool m_bDontInstallPackages,m_bInstallPackagesInBackground;

	CreateDevice(int PK_Installation,string host, string user, string pass, string db_name, int port=3306) 
		: DBHelper(host, user, pass, db_name, port) { m_iPK_Installation=PK_Installation; m_bDontCallConfigureScript=false; m_bInstallPackagesInBackground=false; }

	int DoIt(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sDescription,string sIPAddress,string sMacAddress,int PK_Device_ControlledVia=0,string sDeviceData="",int iPK_Device_RelatedTo=0,int iPK_Room=0,bool *bReusedExistingDevice=NULL);
	void CreateChildrenByCategory(int iPK_Device,int iPK_DeviceCategory);
	void CreateChildrenByTemplate(int iPK_Device,int iPK_DeviceTemplate);
	// For CreateAutoCreateChildDevice specify either PK_DeviceTemplate_DeviceTemplate_ControlledVia or PK_DeviceTemplate_DeviceCategory_ControlledVia
	// but not both.  Which one is specified determines which table the relationship was taken from
	void CreateAutoCreateChildDevice(int iPK_Device_Parent,int PK_DeviceTemplate,bool bRerouteMessagesToParent,int PK_DeviceTemplate_DeviceTemplate_ControlledVia,int PK_DeviceTemplate_DeviceCategory_ControlledVia);
	void ConfirmAllRelations();
	void ConfirmRelations(int PK_Device,int iPK_Device_RelatedTo=0,list<int> *plistSisterDevices=NULL,bool bRecurseChildren=true,bool bOnlyAddDevicesOnCore=false); // If mapSisterDevices is not null, it will have a list of the devices which are sisters, but where the parent is not yet known because ConfirmRelations is called before the ControlledVia

	void AssignDeviceData(int PK_Device,int PK_DeviceData,string sValue);
	void CreateRelation(int PK_Device,int PK_Device_Related);
	void GetAllDevicesInTree(int PK_Device,map<int,int> &mapDeviceTree,bool bCheckParent=true,int PK_Device_ChildExclude=0);

	void FixControlledViaIfEmbeddedIsMoreValid(int PK_DeviceTemplate,int &PK_Device_ControlledVia);

	bool m_bDontCallConfigureScript;
};
#endif
