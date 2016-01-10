/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __PACKAGE_XML_PARSER_H__
#define __PACKAGE_XML_PARSER_H__
//--------------------------------------------------------------------------------------------------
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
//--------------------------------------------------------------------------------------------------
#include <string>
#include <map>
#include <list>
#include <iostream>
using namespace std;
//--------------------------------------------------------------------------------------------------
enum NodeIndex
{
	niNone,
	niIcon,
	niTitle,
	niDescription,
	niHomeurl,
	niCategory,
	niDownloadurl,
	niRepositoryname,
	niPackagename,
	niDistro,
	niMisc,
	niVersion,
	niTarget,
	niImportance,
	niPC_Type,
	niRequired_Version_Min,
	niRequired_Version_Max,
	niSum_MD5,
	niSum_SHA1,
	niNumNodes
};
//--------------------------------------------------------------------------------------------------
typedef map<NodeIndex, string> PackageInfo;
//--------------------------------------------------------------------------------------------------
class PackageXMLParser
{
private:

	PackageInfo m_mapCurrentPackageInfo;
	list<PackageInfo> m_listPackages;
	NodeIndex m_CurrentNode;
	
	void ProcessPackage();
	bool ProcessNode(xmlTextReaderPtr reader);

public:

	PackageXMLParser();
	~PackageXMLParser();
	void Parse(string sXmlData);
	const list<PackageInfo>& GetParsedData() const;
};
//--------------------------------------------------------------------------------------------------
#endif //__PACKAGE_XML_PARSER_H__

