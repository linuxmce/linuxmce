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
#ifndef FLOORPLAN_H
#define FLOORPLAN_H

#include <string>
#include <map>

using namespace std;

typedef vector<class FloorplanObject *> FloorplanObjectVector;
typedef map<int,FloorplanObjectVector *> FloorplanObjectVectorMap;
typedef map<int,class FloorplanItemStatus *> FloorplanItemStatusMap;

namespace DCE
{
	class DeviceData_Router;
	class DeviceData_Base;
	class EntertainArea;
};

using namespace DCE;

class FloorplanObject
{
public:
	int PK_Device,Page,Type,FillX,FillY;
	string DeviceDescription;  // Only used by Controller
	string ObjectTypeDescription;
	string ObjectID;
	string Status;
#ifdef ORBITER
	DCE::DeviceData_Base *m_pDeviceData_Base;
	int m_dwPK_DesignObj_Toolbar; // The toolbar to control this object
#else
	DCE::DeviceData_Router *m_pDeviceData_Router;
	DCE::EntertainArea *m_pEntertainArea;
#endif
	class DesignObj_Orbiter *pObj; // Only used by the orbiter

	FloorplanObject()
	{
		PK_Device=0;
		Page=0;
		Type=0;
		FillX=0;
		FillY=0;
#ifdef ORBITER
		m_pDeviceData_Base=NULL;
		m_dwPK_DesignObj_Toolbar=0;
#else
		m_pDeviceData_Router=NULL;
		m_pEntertainArea=NULL;
#endif
	}

	FloorplanObject(FloorplanObject *pCopy)
	{
		PK_Device=pCopy->PK_Device;
		Page=pCopy->Page;
		Type=pCopy->Type;
		FillX=pCopy->FillX;
		FillY=pCopy->FillY;
		DeviceDescription=pCopy->DeviceDescription;
		ObjectTypeDescription=pCopy->ObjectTypeDescription;
		ObjectID=pCopy->ObjectID;
		Status=pCopy->Status;
#ifdef ORBITER
		m_pDeviceData_Base=pCopy->m_pDeviceData_Base;
		m_dwPK_DesignObj_Toolbar=pCopy->m_dwPK_DesignObj_Toolbar;
#else
		m_pDeviceData_Router=pCopy->m_pDeviceData_Router;
		m_pEntertainArea=pCopy->m_pEntertainArea;
#endif
	}
};

class FloorplanItemStatus
{
public:
	string Description;
	int Color,PKID_C_FloorplanObjectType_Color;
};

class FloorplanInfoProvider
{
public:

	virtual ~FloorplanInfoProvider() {}

	// Color is the color to fill the icon with, Description is the status which
	// appears at the bottom of the floorplan when the item is selected, OSD is
	// the text will be put into any text object within the icon (like the temperature
	// next to a thermastat, and PK_DesignObj_Toolbar is the toolbar to activate
	// when the object is selected.
	virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar)=0;
};

#endif

