/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include <string>
#include <vector>

using namespace std;

#include "DatabaseHelper.h"
#include "IconDecoder.h"

#include "../DCE/Logger.h"
#include "../pluto_main/Table_Device.h"
#include "../pluto_main/Table_Software.h"
#include "../pluto_main/Table_Software_Source.h"
//--------------------------------------------------------------------------------------------------
using namespace DCE;
//--------------------------------------------------------------------------------------------------
DatabaseHelper::DatabaseHelper(string sHost, string sUser, string sPasswd, string sDB, int nPort)
{
	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());

	if(!m_pDatabase_pluto_main->Connect(sHost, sUser, sPasswd, sDB, nPort))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		return;
	}
}
//--------------------------------------------------------------------------------------------------
DatabaseHelper::~DatabaseHelper()
{
	delete m_pDatabase_pluto_main;
	m_pDatabase_pluto_main = NULL;
}
//--------------------------------------------------------------------------------------------------
/*static*/ string DatabaseHelper::GetValue(const PackageInfo& package, NodeIndex key)
{
	PackageInfo::const_iterator it = package.find(key);
	return it != package.end() ? StringUtils::SQLEscape(it->second) : string();
}
//--------------------------------------------------------------------------------------------------
void DatabaseHelper::ProcessPackages(const list<PackageInfo>& listPackages, const vector<string>& virusFree)
{
	// adding all packages if they do not exist yet, storing their primary keys
	
	for(list<PackageInfo>::const_iterator it = listPackages.begin(); it != listPackages.end(); ++it)
	{
		const PackageInfo& package = *it;

		string sPackageName = GetValue(package, niPackagename);
		string sVersion = GetValue(package, niVersion);
		string sDistro = GetValue(package, niDistro);
		
		vector<Row_Software *> vSoftware;
		m_pDatabase_pluto_main->Software_get()->GetRows("WHERE PackageName=\"" + sPackageName + "\"", &vSoftware);
		
		int iSoftwareID = -1;
		
		if ( vSoftware.empty() )
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, ("No package '" + sPackageName + "' exist, adding it").c_str() );
			Row_Software *pRow = m_pDatabase_pluto_main->Software_get()->AddRow();
			pRow->PackageName_set(sPackageName);
			pRow->Table_Software_get()->Commit();
			LoggerWrapper::GetInstance()->Write( LV_STATUS, ( "Added with PKID=" + StringUtils::itos(pRow->PK_Software_get()) ).c_str() );
			iSoftwareID = pRow->PK_Software_get();
		}
		else
		{
			Row_Software *pRow = *vSoftware.begin();
			LoggerWrapper::GetInstance()->Write( LV_STATUS, ( "Package '" + sPackageName + " exists with PKID=" + StringUtils::itos(pRow->PK_Software_get()) ).c_str() );
			iSoftwareID = pRow->PK_Software_get();
		}
		
		// adding source for software if not added yet
		if (iSoftwareID != -1)
		{
			vector<Row_Software_Source *> vRows;
			m_pDatabase_pluto_main->Software_Source_get()->GetRows("WHERE FK_Software=" + StringUtils::itos(iSoftwareID) +" AND Version=\"" + 
				sVersion + "\" AND Distro=\"" + sDistro +"\" AND Downloadurl=\"" + GetValue(package, niDownloadurl) + "\"", &vRows);
			
			Row_Software_Source *pRow=NULL;
			
			if ( vRows.empty() )
			{
				pRow = m_pDatabase_pluto_main->Software_Source_get()->AddRow();
			}
			else
			{
				pRow = *vRows.begin();
			}
			
			if (pRow)
			{
				// checking if package is virus-free
				bool isVirusFree=false;
				string sInfo = sPackageName + "," + GetValue(package, niDistro) + "," + GetValue(package, niVersion) + ","  + GetValue(package, niSum_MD5) + "," + GetValue(package, niSum_SHA1);
				for (vector<string>::const_iterator it=virusFree.begin(); it!=virusFree.end(); it++)
				{
					if (*it == sInfo)
					{
						isVirusFree = true;
						break;
					}
				}
				
				// saving information about package source
				pRow->FK_Software_set(iSoftwareID);
				pRow->Description_set(GetValue(package, niDescription));
				pRow->Title_set(GetValue(package, niTitle));
				pRow->HomeURL_set(GetValue(package, niHomeurl));
				pRow->Category_set(GetValue(package, niCategory));
				pRow->Downloadurl_set(GetValue(package, niDownloadurl));
				pRow->RepositoryName_set(GetValue(package, niRepositoryname));
				pRow->Distro_set(GetValue(package, niDistro));
				pRow->Misc_set(GetValue(package, niMisc));
				pRow->Version_set(GetValue(package, niVersion));
				pRow->Target_set(GetValue(package, niTarget));
				pRow->Importance_set(GetValue(package, niImportance));
				pRow->PC_Type_set(GetValue(package, niPC_Type));
				pRow->Required_Version_Min_set(GetValue(package, niRequired_Version_Min));
				pRow->Required_Version_Max_set(GetValue(package, niRequired_Version_Max));
				pRow->Virus_Free_set(isVirusFree?1:0);
				pRow->Sum_md5_set(GetValue(package, niSum_MD5));
				pRow->Sum_sha_set(GetValue(package, niSum_SHA1));
				pRow->Table_Software_Source_get()->Commit();
		
				size_t unIconSize = 0;
				char *pIconData = DecodeIcon(GetValue(package, niIcon), unIconSize);
				SaveRawIcon(pRow->PK_Software_Source_get(), pIconData, unIconSize);
			}
		}
		
	}

/*	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows(
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"WHERE FK_DeviceCategory IN (6,7,8) AND IPaddress is not NULL and IPaddress <> \'\'",
		&vectRow_Device);


	for(list<PackageInfo>::const_iterator it = listPackages.begin(); it != listPackages.end(); ++it)
	{
		const PackageInfo& package = *it;

		string sPackageName = GetValue(package, niPackagename);
		string sVersion = GetValue(package, niVersion);

		if(sPackageName.empty() || sVersion.empty())
			continue;

		for(vector<Row_Device *>::iterator it = vectRow_Device.begin(); it != vectRow_Device.end(); ++it)
		{
			Row_Device *pRow_Device = *it;

			switch(GetPackageStatus(package, pRow_Device->PK_Device_get()))
			{
				case psNotInstalled:
					InstallPackage(package, pRow_Device->PK_Device_get());
					break;

				case psNotUpToDate:
					UpdatePackage(package, pRow_Device->PK_Device_get());
					break;

				case psUpToDate:
					break;

				default:
					break;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------
PackageStatus DatabaseHelper::GetPackageStatus(const PackageInfo& package, int DeviceID)
{
	/ *
	string sPackageName = GetValue(package, niPackagename);
	string sVersion = GetValue(package, niVersion);

	vector<Row_Software *> vectRow_Software;
	m_pDatabase_pluto_main->Software_get()->GetRows(
		"WHERE PackageName = \"" + sPackageName + "\" AND FK_Device = " + StringUtils::ltos(DeviceID),
		&vectRow_Software);

	if(vectRow_Software.size() >= 1)
	{
		Row_Software *pRow_Software = vectRow_Software[0];
		string sCurrentVersion = pRow_Software->Version_get();
		if(sCurrentVersion != sVersion)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Package %s for device %d not to the latest version %s",
				sPackageName.c_str(), DeviceID, sVersion.c_str());
			
			return psNotUpToDate;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Package %s for device %d already to the latest version %s",
				sPackageName.c_str(), DeviceID, sVersion.c_str());
			
			return psUpToDate;
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Package %s for device %d not installed!",
			sPackageName.c_str(), DeviceID);

		return psNotInstalled;
	}
	return psNone;
*/
}
//--------------------------------------------------------------------------------------------------
void DatabaseHelper::UpdatePackage(const PackageInfo& package, int DeviceID)
{
	/*
	string sPackageName = GetValue(package, niPackagename);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Updating package %s for device %d", sPackageName.c_str(), DeviceID);

	vector<Row_Software *> vectRow_Software;
	m_pDatabase_pluto_main->Software_get()->GetRows(
		"WHERE PackageName = \"" + sPackageName + "\" AND FK_Device = " + StringUtils::ltos(DeviceID),
		&vectRow_Software);

	if(vectRow_Software.size() >= 1)
	{
		Row_Software *pRow_Software = vectRow_Software[0];
		pRow_Software->Description_set(GetValue(package, niDescription));
		pRow_Software->Title_set(GetValue(package, niTitle));
		pRow_Software->HomeURL_set(GetValue(package, niHomeurl));
		pRow_Software->Category_set(GetValue(package, niCategory));
		pRow_Software->Downloadurl_set(GetValue(package, niDownloadurl));
		pRow_Software->PackageName_set(GetValue(package, niPackagename));
		pRow_Software->RepositoryName_set(GetValue(package, niRepositoryname));
		pRow_Software->Misc_set(GetValue(package, niMisc));
		pRow_Software->Version_set(GetValue(package, niVersion));
		pRow_Software->Target_set(GetValue(package, niTarget));
		pRow_Software->Importance_set(GetValue(package, niImportance));
		pRow_Software->PC_Type_set(GetValue(package, niPC_Type));
		pRow_Software->Required_Version_Min_set(GetValue(package, niRequired_Version_Min));
		pRow_Software->Required_Version_Max_set(GetValue(package, niRequired_Version_Max));
		pRow_Software->FK_Device_set(DeviceID);
		pRow_Software->Table_Software_get()->Commit();

		size_t unIconSize = 0;
		char *pIconData = DecodeIcon(GetValue(package, niIcon), unIconSize);
		SaveRawIcon(pRow_Software->PK_Software_get(), pIconData, unIconSize);
	}
	*/
}
//--------------------------------------------------------------------------------------------------
void DatabaseHelper::InstallPackage(const PackageInfo& package, int DeviceID)
{
	/*
	string sPackageName = GetValue(package, niPackagename);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Installing package %s for device %d", sPackageName.c_str(), DeviceID);

	Row_Software *pRow_Software = m_pDatabase_pluto_main->Software_get()->AddRow();

	pRow_Software->Description_set(GetValue(package, niDescription));
	pRow_Software->Title_set(GetValue(package, niTitle));
	pRow_Software->HomeURL_set(GetValue(package, niHomeurl));
	pRow_Software->Category_set(GetValue(package, niCategory));
	pRow_Software->Downloadurl_set(GetValue(package, niDownloadurl));
	pRow_Software->PackageName_set(GetValue(package, niPackagename));
	pRow_Software->RepositoryName_set(GetValue(package, niRepositoryname));
	pRow_Software->Misc_set(GetValue(package, niMisc));
	pRow_Software->Version_set(GetValue(package, niVersion));
	pRow_Software->Target_set(GetValue(package, niTarget));
	pRow_Software->Importance_set(GetValue(package, niImportance));
	pRow_Software->PC_Type_set(GetValue(package, niPC_Type));
	pRow_Software->Required_Version_Min_set(GetValue(package, niRequired_Version_Min));
	pRow_Software->Required_Version_Max_set(GetValue(package, niRequired_Version_Max));
	pRow_Software->FK_Device_set(DeviceID);
	pRow_Software->Table_Software_get()->Commit();

	size_t unIconSize = 0;
	char *pIconData = DecodeIcon(GetValue(package, niIcon), unIconSize);
	SaveRawIcon(pRow_Software->PK_Software_get(), pIconData, unIconSize);
	*/
}
//--------------------------------------------------------------------------------------------------
void DatabaseHelper::SaveRawIcon(int nPK_Software, char *pData, size_t nSize)
{
	if(nSize != 0)
	{
		string sHeadQuery = "UPDATE `Software_Source` set Iconstr = \"";
		string sEndQuery = "\" WHERE PK_Software_Source = " + StringUtils::ltos(nPK_Software);

		int nGoodDataSize = 0;
		char *pGoodData = new char[nSize * 2 + 1];

		size_t length = nGoodDataSize = db_wrapper_real_escape_string(m_pDatabase_pluto_main->m_pDB, pGoodData, pData, (unsigned long)nSize);

		delete [] pData;
		pData = NULL;

		length += sHeadQuery.length();
		length += sEndQuery.length();

		char *pQueryData = new char[length];
		memcpy(pQueryData, sHeadQuery.c_str(), sHeadQuery.length());
		char *pCurrentPos = pQueryData + sHeadQuery.length();
		memcpy(pCurrentPos, pGoodData, nGoodDataSize);
		pCurrentPos += nGoodDataSize;
		memcpy(pCurrentPos, sEndQuery.c_str(), sEndQuery.length());
		delete [] pGoodData;

		if(db_wrapper_real_query(m_pDatabase_pluto_main->m_pDB, pQueryData, (unsigned long)length))
		{
			//LoggerWrapper::GetInstance()->Write(LV_STATUS, "Failed to save icon in db!");
		}

		delete [] pQueryData;
	}
}
//--------------------------------------------------------------------------------------------------
#ifdef UNIT_TEST_DATABASE_HELPER
int main()
{
	string sFileName = "/temp/packages.xml";

	string sXmlData;
	FileUtils::ReadTextFile(sFileName, sXmlData);
	sXmlData = StringUtils::Replace(sXmlData, "&", "&amp;");

	PackageXMLParser parser;
	parser.Parse(sXmlData);
	const list<PackageInfo>& listPackages = parser.GetParsedData();

	DatabaseHelper dbhelper("10.0.0.150", "root", "", "pluto_main_devel");
	dbhelper.ProcessPackages(listPackages);

	return 0;
}
#endif
