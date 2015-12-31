/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "PackageXMLParser.h"

#include <algorithm>

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

#define REQUIRED_LIST_VERSION "1.1"

//--------------------------------------------------------------------------------------------------
string NodeName[] =
{
	"",
	"icon",
	"title",
	"description",
	"homeurl",
	"category",
	"downloadurl",
	"repositoryname",
	"packagename",
	"distro",
	"misc",
	"version",
	"target",
	"importance",
	"PC_Type",
	"Required_Version_Min",
	"Required_Version_Max",
	"md5sum",
	"sha1sum"
};
//--------------------------------------------------------------------------------------------------
PackageXMLParser::PackageXMLParser() : m_CurrentNode(niNone)
{
}
//--------------------------------------------------------------------------------------------------
PackageXMLParser::~PackageXMLParser()
{
	ProcessPackage();

	xmlCleanupParser();
	xmlMemoryDump();
}
//--------------------------------------------------------------------------------------------------
bool PackageXMLParser::ProcessNode(xmlTextReaderPtr reader) 
{
	const char *name, *value;

	name = (char *)xmlTextReaderConstName(reader);
	if(name == NULL)
		return false;

	value = (char *)xmlTextReaderConstValue(reader);

	string sName(name);
	int type = xmlTextReaderNodeType(reader);

	if (sName == "packages")
	{
		cout << "Found packages tag: " << endl;
		char *version = (char*) xmlTextReaderGetAttribute(reader, (xmlChar*) "version");
		if (!version) 
		{
			DCE::LoggerWrapper::GetInstance()->Write( LV_WARNING, "Cannot read packages list version, skipping this XML file");
			return false;
		}
		else
		{
			DCE::LoggerWrapper::GetInstance()->Write( LV_STATUS, "Detected version of packages list: %s", version);
			if (strcmp(version, REQUIRED_LIST_VERSION) != 0)
			{
				DCE::LoggerWrapper::GetInstance()->Write( LV_WARNING, "Version of packages list doesn't match required one " REQUIRED_LIST_VERSION ", skipping this XML file");
				return false;
			}
		}
	}
	else if(sName == "package")
	{
		ProcessPackage();
	}
	else if(sName == "#text")
	{
		if(NULL != value && m_CurrentNode != niNone && type == XML_READER_TYPE_TEXT)
            m_mapCurrentPackageInfo[m_CurrentNode] = value;
	}
	else
	{
		m_CurrentNode = niNone;
		for(int i = 0; i < niNumNodes; ++i)
		{
			if(NodeName[i] == sName)
			{
				m_CurrentNode = (NodeIndex)i;
			}
		}
	}

	return true;
}
//--------------------------------------------------------------------------------------------------
void PackageXMLParser::ProcessPackage()
{
	if(!m_mapCurrentPackageInfo[niTitle].empty() && !m_mapCurrentPackageInfo.empty())
	{
		m_listPackages.push_back(m_mapCurrentPackageInfo);
		m_mapCurrentPackageInfo.clear();
		m_CurrentNode = niNone;
	}
}
//--------------------------------------------------------------------------------------------------
void PackageXMLParser::Parse(string sXmlData)
{
	xmlTextReaderPtr Reader;
	Reader = xmlReaderForMemory(sXmlData.c_str(), static_cast<int>(sXmlData.length()), NULL, NULL, 0);

	int ret;

	if (Reader != NULL) 
	{
		ret = xmlTextReaderRead(Reader);
		while (ret != -1)
		{
			if(!ProcessNode(Reader))
				ret = -1;
			else
				ret = xmlTextReaderRead(Reader);
		}
		
		xmlFreeTextReader(Reader);
		if (ret != 0) 
		{
			DCE::LoggerWrapper::GetInstance()->Write( LV_WARNING, "Failed to parse: %s", sXmlData.c_str() );
		}
	} 

	return;
}
//--------------------------------------------------------------------------------------------------
const list<PackageInfo>& PackageXMLParser::GetParsedData() const
{
	return m_listPackages;
}
//--------------------------------------------------------------------------------------------------
#ifdef UNIT_TEST_PARSER
int main()
{
	string sFileName = "/tmp/packages.xml";

	string sXmlData;
	FileUtils::ReadTextFile(sFileName, sXmlData);
	sXmlData = StringUtils::Replace(sXmlData, "&", "&amp;");

	PackageXMLParser parser;
	parser.Parse(sXmlData);
	const list<PackageInfo>& listPackages = parser.GetParsedData();

	for(list<PackageInfo>::const_iterator it = listPackages.begin(); it != listPackages.end(); ++it)
		for(PackageInfo::const_iterator itp = it->begin(); itp != it->end(); ++itp)
			cout << NodeName[itp->first] << ": " << itp->second << endl;

	return 0;
}
#endif
//--------------------------------------------------------------------------------------------------

