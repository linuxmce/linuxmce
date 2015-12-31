/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __DATABASE_HELPER_H__
#define __DATABASE_HELPER_H__
//--------------------------------------------------------------------------------------------------
#include "PackageXMLParser.h"
//--------------------------------------------------------------------------------------------------
class Database_pluto_main;
//--------------------------------------------------------------------------------------------------
enum PackageStatus
{
	psNone,
	psNotInstalled,
	psNotUpToDate,
	psUpToDate
};
//--------------------------------------------------------------------------------------------------
class DatabaseHelper
{
	Database_pluto_main	*m_pDatabase_pluto_main;

public:

	DatabaseHelper(string sHost = "dcerouter", string sUser = "root", string sPasswd = "", 
		string sDB = "pluto_main", int nPort = 3306);
	~DatabaseHelper();

	void ProcessPackages(const list<PackageInfo>& listPackages, const vector<string>& virusFree);

private:

	static string GetValue(const PackageInfo& package, NodeIndex key);
	
	PackageStatus GetPackageStatus(const PackageInfo& package, int DeviceID);
	void UpdatePackage(const PackageInfo& package, int DeviceID);
	void InstallPackage(const PackageInfo& package, int DeviceID);
	void SaveRawIcon(int nPK_Software, char *pData, size_t nSize);
};
//--------------------------------------------------------------------------------------------------
#endif //__DATABASE_HELPER_H__

