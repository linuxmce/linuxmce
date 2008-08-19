/*
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include "Generic_Mysql_Provider.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Command_Impl.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/DBHelper.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_media/Define_AttributeType.h"

#include <vector>
#include <iostream>
using namespace std;
using namespace DCE;

#include "../Data_Provider_Catalog_Plugin/Data_Provider_Factory.h"
#include "Media_Data_Provider.h"


Generic_Mysql_Provider *Generic_Mysql_Provider::m_pthis;

bool Generic_Mysql_Provider::Populate(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String, string sQuery)
{
	if( sQuery.empty() )
	{
		map< string, pair<string,string> >::iterator it=m_mapQueries.find(sID);
		if( it==m_mapQueries.end() )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Generic_Mysql_Provider::Populate no query: %s", sID.c_str());
			return false;
		}

		sQuery = ParseQueryParameters(it->second.first,sParameters,it->second.second);
	}
	else
		sQuery = ParseQueryParameters(sQuery,sParameters,"");

	PlutoSqlResult result;
	DB_ROW row;
	if( ( result.r=m_pMedia_Provider_InstantiationInfo->m_pDBHelper->db_wrapper_query_result( sQuery ) )==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Generic_Mysql_Provider::Populate error in query: %s", sID.c_str());
		return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Generic_Mysql_Provider::Populate ID: %s Parameters: %s Now: %s",
		sID.c_str(), sParameters.c_str(), sQuery.c_str());

	*sData_String="[";

	// TODO -- they are looking for rows and columsn reversed.  This isn't efficient
	vector<string> vectString;
	for(unsigned int i=0;i<result.r->field_count;++i)
		vectString.push_back("[");

	bool bFirst=true;
	while( ( row=db_wrapper_fetch_row( result.r ) ) )
    {
		vector<string>::iterator it = vectString.begin();
		for(unsigned int i=0;i<result.r->field_count;++i)
		{
			if( !bFirst )
				*it += ",";

			*it += string("\"");
			if( row[i] )
			{
				string s = row[i];
				if( s.size()>7 && s.substr(0,7)=="file://" )
				{
					StringUtils::Replace(&s," ","%20");
				}
				else
				{
					StringUtils::Replace(&s,"\"","");
					StringUtils::Replace(&s,"'","");
					StringUtils::Replace(&s,"[","");
					StringUtils::Replace(&s,"]","");
					StringUtils::Replace(&s,"(","");
					StringUtils::Replace(&s,")","");
					StringUtils::Replace(&s,"%","");
					StringUtils::Replace(&s,"<","");
					StringUtils::Replace(&s,">","");
					StringUtils::Replace(&s,"^","");
					StringUtils::Replace(&s,"$","");
					StringUtils::Replace(&s,"@","");
					StringUtils::Replace(&s,";","");
					StringUtils::Replace(&s,"?","");
					StringUtils::Replace(&s,"!","");
					StringUtils::Replace(&s,"#","");
					StringUtils::Replace(&s,"=","");
					StringUtils::Replace(&s,"&","");
					StringUtils::Replace(&s,"+","");
					StringUtils::Replace(&s,"\\","");
					StringUtils::Replace(&s,"`","");
					StringUtils::Replace(&s,",","");
					StringUtils::Replace(&s,"ë","");
					StringUtils::Replace(&s,"ï","");

	if( s!=row[i] )
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Generic_Mysql_Provider::Populate ESCAPED %s", row[i]);
				}

				if( s.find("http://")!=string::npos )
					*it += StringUtils::URLEncode(s);
				else
					*it += s;
			}
			*it += "\"";

//			*it += string("\"") + (row[i] ? row[i] : "*NULL") + "\"";
			it++;
		}
		bFirst=false;
	}

	bFirst=true;
	for(vector<string>::iterator it = vectString.begin();it != vectString.end();++it)
	{
		if( bFirst )
			bFirst=false;
		else
			*sData_String+=",";

		*sData_String+=*it + "]";
	}

	*sData_String+= "]";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Generic_Mysql_Provider::Populate %s", sData_String->c_str());

	return true;
}

string Generic_Mysql_Provider::ParseQueryParameters(string sQuery, string sParameterValues,string sParameterNames)
{
	string::size_type pos=0,pos_name=0;
	int iCount=1;
	while(pos<sParameterValues.size())
	{
		string sParm = StringUtils::Tokenize(sParameterValues,",",pos);
		if( pos_name<sParameterNames.size() )
		{
			string sName = StringUtils::Tokenize(sParameterNames,",",pos_name);
			PreprocessQueryParameters(sName,sParm,sQuery);
		}

		if( sParm.empty()==false )
			StringUtils::Replace(&sQuery, "%" + StringUtils::itos(iCount++), sParm);
	}
	return sQuery;
}

void Generic_Mysql_Provider::PreprocessQueryParameters(string sName,string &sParm,string &sQuery)
{
	if( sName=="HCPrefix" )
	{
		int iValue = atoi(sParm.c_str());
		if( iValue==3 )
		{
			StringUtils::Replace(&sQuery,"%mt",TOSTRING(MEDIATYPE_pluto_Pictures_CONST));
			StringUtils::Replace(&sQuery,"%at",TOSTRING(ATTRIBUTETYPE_Album_CONST));
		}
		else if( iValue==5 )
		{
			StringUtils::Replace(&sQuery,"%mt",TOSTRING(MEDIATYPE_pluto_StoredVideo_CONST));
			StringUtils::Replace(&sQuery,"%at",TOSTRING(ATTRIBUTETYPE_Album_CONST));
		}
		else if( iValue==0 )
		{
			StringUtils::Replace(&sQuery,"%mt",TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST));
			StringUtils::Replace(&sQuery,"%at",TOSTRING(ATTRIBUTETYPE_Album_CONST));
		}
		else if( iValue==1 )
		{
			StringUtils::Replace(&sQuery,"%mt",TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST));
			StringUtils::Replace(&sQuery,"%at",TOSTRING(ATTRIBUTETYPE_Performer_CONST));
		}
		else if( iValue==2 )
		{
			StringUtils::Replace(&sQuery,"%mt",TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST));
			StringUtils::Replace(&sQuery,"%at",TOSTRING(ATTRIBUTETYPE_Title_CONST));
		}
	}
}

bool Generic_Mysql_Provider::RegisterQueries(string sQueries,class Data_Provider_Factory *pData_Provider_Factory)
{
	vector<string> vectQueries;
	StringUtils::Tokenize(sQueries,"\r\n",vectQueries);
	for(vector<string>::iterator it=vectQueries.begin();it!=vectQueries.end();++it)
	{
		string::size_type pos=0;
		string sName = StringUtils::Tokenize(*it,"\t",pos);
		string sQuery = StringUtils::Tokenize(*it,"\t",pos);
		string sParameters = StringUtils::Tokenize(*it,"\t",pos);

		m_mapQueries[ sName ] = make_pair<string,string> (sQuery,sParameters);
		pData_Provider_Factory->Register( Generic_Mysql_Provider::Instantiate, sName, "*", "json", NULL, Data_Provider_Source_Info::cache_never, false );
	}
	pData_Provider_Factory->RegisterCatalog();
	return true;
}
