/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file DatabaseUtils.h
 Header file for the DatabaseUtils namespace
 * @todo notcommented
 */



#ifndef DatabaseUtils_h
#define DatabaseUtils_h

#include "DBHelper.h"

#include <string>
#include <vector>
#include <map>

using namespace std;

/**
@namespace DatabaseUtils
Utilities for Database access.
*/
namespace DatabaseUtils
{
    /** SQL to get the device ID ???
    @param pDBHelper is the database pointer.
    @param PK_Device is the device in question.
    @returns the device ID.

    FK_Device_ControlledVia entry of Devices table.
    */
	int GetTopMostDevice(DBHelper *pDBHelper,int PK_Device);

    /** SQL to get IP address of device.
    @param pDBHelper is the database pointer.
    @param PK_Device is the device in question.
    @returns a string containing the ip address.
    */
	string GetIpOfDevice(DBHelper *pDBHelper,int PK_Device);

	int GetNumberOfChildDevices(DBHelper *pDBHelper,int PK_Device);

    void SetDeviceTemplateForDevice(DBHelper *pDBHelper,int PK_Device,int PK_DeviceTemplate);

    int GetDeviceTemplateForDevice(DBHelper *pDBHelper,int PK_Device);

    string GetDeviceCategoryForDeviceTemplate(DBHelper *pDBHelper,int PK_DeviceTemplate,bool IncludeParents=true);

    bool IsValidControlledVia(DBHelper *pDBHelper,int PK_DeviceTemplate,int PK_Device_ControlledVia);

    /** Return a string naming the port.
    @param pDBHelper is the database pointer.
    @param PK_Device is the device in question.
    @param sPort is a name like '/dev/ttyS0'.
    @returns 'COM1'...
    */
	string HumanReadablePort(DBHelper *pDBHelper,int PK_Device,string sPort);

	void GetUnusedPortsOnPC(DBHelper *pDBHelper,int PK_Device,vector<string> &vectPorts);

    void GetUnusedPortsOnAllPCs(DBHelper *pDBHelper,vector< pair<int,string> > &vectAllPorts, long nFK_Installation = 0);

    void GetAllDevicesInTree(DBHelper *pDBHelper,int PK_Device,map<int,pair<int,int> > &mapDeviceTree,bool bCheckParent=true,int PK_Device_ChildExclude=0,int Generation=0);

    void SetDeviceData(DBHelper *pDBHelper,int PK_Device,int PK_DeviceData,string IK_DeviceData,bool bPreserveModTime=false);

    string GetDeviceData(DBHelper *pDBHelper,int PK_Device,int PK_DeviceData);

    string GetDescriptionForDevice(DBHelper *pDBHelper,int PK_Device);

    string GetDescriptionFromTable(DBHelper *pDBHelper,string sTable,int PK);

    void SetDescriptionForDevice(DBHelper *pDBHelper,int PK_Device,string sDescription);

    bool SetDeviceInRoom(DBHelper *pDBHelper,int PK_Device,int PK_Room);

    bool AddDeviceToEntertainArea(DBHelper *pDBHelper,int PK_Device,int PK_EntertainArea);

    bool SetDeviceControlledVia(DBHelper *pDBHelper,int PK_Device,int PK_Device_ControlledVia);
	int GetCommMethodForDeviceTemplate(DBHelper *pDBHelper,int PK_DeviceTemplate);
	int FindControlledViaCandidate(DBHelper *pDBHelper,int iPK_Device,int iPK_DeviceTemplate,int iPK_Device_RelatedTo,int iPK_Installation);
	int FindControlledViaCandidate(DBHelper *pDBHelper,int iPK_Device,int iPK_DeviceTemplate,int iPK_Device_RelatedTo,PlutoSqlResult &result_cv);
	int ViolatesDuplicateRules(DBHelper *pDBHelper,int PK_Device_ControlledVia_temp,int iPK_DeviceTemplate);
	string GetNameForRoom(DBHelper *pDBHelper,int PK_Room);
	bool AlreadyHasUsers(DBHelper *pDBHelper,int PK_Installation);
	bool AlreadyHasRooms(DBHelper *pDBHelper,int PK_Installation);
	long GetRoomForDevice(DBHelper *pDBHelper, int nPK_Device);
	long GetRoomByName(DBHelper *pDBHelper, string sDescription, int PK_RoomType);
	bool DeviceIsWithinCategory(DBHelper *pDBHelper,int PK_Device,int PK_DeviceCategory);
	void LockTable(DBHelper *pDBHelper,string sTable);
	void UnLockTables(DBHelper *pDBHelper);
};

#endif //#ifndef _DatabaseUtils
