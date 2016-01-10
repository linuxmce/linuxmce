/*
 CommonFunctions
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

 
/**
 *
 * @file CommonFunctions.cpp
 * @brief source file for the GlobalConfig, SerializeableStrings classes; it uses the Table, Repository and Database classes
 *
 */
 

#include "PlutoUtils/CommonIncludes.h"	
#include "CommonFunctions.h"
#include <iostream>

namespace sqlCVS
{
	void NewSection()
	{ 
		cout << endl << "==============================================================================" << endl; 
	}
}

using namespace sqlCVS;

string int2string(int i)
{
	char s[1000];
	sprintf(s, "%i", i);
	return s;
}

string getTableFromForeignKey(string field,map<string,class TableInfo_Generator *> *mapTableNames)
{
	/** 
	 * Skip fields that are not foreign keys, and also the _RecordInfo.  Otherwise
	 * Users will reference every other table because of the FK_Users_RecordInfo
	 * and create a .cpp file that is too big to compile
	 */
	 
	if( field.find("FK_")!=0 || field.find("_RecordInfo")!=string::npos )
		return "";

	string table = field.substr(3);


	/**
	 * See if table is a valid Table Name
	 */
	 
	if( mapTableNames->find(table)!=mapTableNames->end() )
	{
		return table;
	}

	/**
	 * No, see if it has a trailing _
	 */
	 
	size_t last_underscore=string::npos,s=-1;  /**< s is a temporary value, start with -1 since we always search for the last pos +1 */
	while( (s=table.find('_',s+1)) != string::npos )
		last_underscore = s;

	if( last_underscore!=string::npos )
	{
		table = table.substr(0,last_underscore);
		if( mapTableNames->find(table)!=mapTableNames->end() )
			return table;
	}

	/** @test cout << "**ERROR** Cannot determine referenced table in field: " << field << endl; */
	return "";
}

string GlobalConfig::csvUserID()
{
	string sResponse="0";  // The anonymous user is always valid
	bool bFirst=true;
	for(map<int,ValidatedUser *>::iterator it=m_mapValidatedUsers.begin();it!=m_mapValidatedUsers.end();++it)
	{
		sResponse += "," + StringUtils::itos( (*it).first );
	}
	return sResponse;
}
