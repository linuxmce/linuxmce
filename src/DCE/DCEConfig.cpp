/* 
	DCEConfig
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU Lesser General Public License for more details.
*/

#include "DCEConfig.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "Logger.h"

using namespace DCE;

//------------------------------------------------------------------------------------------------------
DCEConfig::DCEConfig(string sFilename) : RA_Config()
{
	m_sConfigFile=sFilename;
    m_sDBHost="dcerouter"; m_sDBUser="root"; m_sDBPassword=""; m_sDBName="pluto_main"; m_sDCERouter="dcerouter";
    m_iPK_Device_Computer=0; m_iDBPort=3306; m_iDCERouterPort=3450;
    m_iPK_Distro=m_iPK_Installation=-1;

	vector<string> vectString;
	FileUtils::ReadFileIntoVector( m_sConfigFile, vectString );
	ParseFile(vectString);
}

DCEConfig::DCEConfig(const char *pBuffer,size_t size) : RA_Config()
{
	string sData = pBuffer;
	vector<string> vectString;
	StringUtils::Tokenize(sData,"\n",vectString);
	for(size_t s=0;s<vectString.size();++s)
		vectString[s] = StringUtils::Replace(&vectString[s],"\r","");

	ParseFile(vectString);
}

void DCEConfig::ParseFile(vector<string> &vectString)
{
	for(size_t s=0;s<vectString.size();++s)
	{
		string::size_type pos_Equal;
        // Skip over any lines that start with # or /, or don't have an =
		if( vectString[s][0]=='#' || vectString[s][0]=='/' || (pos_Equal=vectString[s].find('='))==string::npos )
            continue;

		string::size_type pos_Slash = vectString[s].find("//");

        string Token=vectString[s].substr(0,pos_Equal);
		StringUtils::TrimSpaces(Token);
        string Value;
		if( pos_Slash==string::npos )
			Value = vectString[s].substr(pos_Equal+1);
		else
			Value = vectString[s].substr(pos_Equal+1,pos_Slash-pos_Equal-1);

		m_mapParameters[Token]=StringUtils::TrimSpaces(Value);
		m_mapModifiedParameters[Token]=false;
    }

    vectString.clear();

    m_sDBHost               = ReadString("MySqlHost",m_sDBHost);
    m_sDBUser               = ReadString("MySqlUser",m_sDBUser);
    m_sDBPassword           = ReadString("MySqlPassword",m_sDBPassword);
    m_sDBName               = ReadString("MySqlDBName",m_sDBName);
    m_sDCERouter            = ReadString("DCERouter",m_sDCERouter);
    m_iDBPort               = ReadInteger("MySqlPort",m_iDBPort);
    m_iDCERouterPort        = ReadInteger("DCERouterPort",m_iDCERouterPort);
    m_iPK_Device_Computer   = ReadInteger("PK_Device",m_iPK_Device_Computer);
    m_iPK_Installation      = ReadInteger("PK_Installation",m_iPK_Installation);
    m_iPK_Distro            = ReadInteger("PK_Distro",m_iPK_Distro);

    m_sMenuPath             = ReadString("MenuPath",m_sMenuPath);
    m_sPicturePath          = ReadString("PicturePath",m_sPicturePath);
    m_sSisFilesPath         = ReadString("SisFilesPath",m_sSisFilesPath);
}
//------------------------------------------------------------------------------------------------------
/*virtual*/ DCEConfig::~DCEConfig()
{
    m_mapParameters.clear();
	m_mapModifiedParameters.clear();
}
//------------------------------------------------------------------------------------------------------
bool DCEConfig::WriteSettings()
{
	vector<string> vectString;
	FileUtils::ReadFileIntoVector( m_sConfigFile, vectString );

	// Make a copy of the parameters.  We'll go through the existing file and modify any changed values
	map<string,string> mapParameters_Copy = m_mapParameters;

	for(size_t s=0;s<vectString.size();++s)
	{
		string::size_type pos_Equal;
        // Skip over any lines that start with # or /, or don't have an =
		if( vectString[s][0]=='#' || vectString[s][0]=='/' || (pos_Equal=vectString[s].find('='))==string::npos )
            continue;

		string::size_type pos_Slash = vectString[s].find("//");
        
		string Token = vectString[s].substr(0,pos_Equal);
		StringUtils::TrimSpaces(Token);
				
        string Value;

		if( pos_Slash==string::npos )
			Value = vectString[s].substr(pos_Equal+1);
		else
			Value = vectString[s].substr(pos_Equal+1,pos_Slash-pos_Equal-1);

		if( mapParameters_Copy.find(Token)==mapParameters_Copy.end() )
			continue; // This token exists in the file but not our map.  We won't touch it

		if( StringUtils::TrimSpaces(mapParameters_Copy[Token])==StringUtils::TrimSpaces(Value) )
		{
			mapParameters_Copy.erase(Token); // We're taking care of this one here since the token exists
			continue; // The value is the same as what's in the file.  Skip it
		}

		if(m_mapModifiedParameters[Token]) //we've modified this
		{
			// We need to update what's in the file to reflect this value
			if( pos_Slash==string::npos )
				vectString[s].replace(pos_Equal+1, vectString[s].length()-pos_Equal-1, mapParameters_Copy[Token]);
			else // Preserve the // and anything after it
				vectString[s].replace(pos_Equal+1, pos_Slash-pos_Equal-2, mapParameters_Copy[Token]);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DCEConfig::WriteSettings modified %s=%s",Token.c_str(),vectString[s].c_str());
		}

		mapParameters_Copy.erase(Token); // We're taking care of this one here since the token exists
    }

	// Add the new ones
	for(map<string,string>::iterator it=mapParameters_Copy.begin();it!=mapParameters_Copy.end();++it)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DCEConfig::WriteSettings new %s=%s",(*it).first.c_str(),(*it).second.c_str());
		vectString.push_back((*it).first + "=" + (*it).second);
	}

	// Rewrite the config file
	return FileUtils::WriteVectorToFile(m_sConfigFile, vectString);
}
//------------------------------------------------------------------------------------------------------
void DCEConfig::AddString(string sToken, string sValue) 
{ 
	m_mapModifiedParameters[sToken] = true;
	m_mapParameters[sToken]=sValue; 
}
//------------------------------------------------------------------------------------------------------
void DCEConfig::AddInteger(string sToken, int iValue) 
{ 
	m_mapModifiedParameters[sToken] = true;
	m_mapParameters[sToken]=StringUtils::itos(iValue); 
}
//------------------------------------------------------------------------------------------------------
const map<string, string>& DCEConfig::Parameters() const
{
	return m_mapParameters;
}
//------------------------------------------------------------------------------------------------------
