/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file DCEConfig.h
 *@brief The DCEConfig class.
 */
#ifndef DCEConfig_H
#define DCEConfig_H
//------------------------------------------------------------------------------------------------------
#include "RA/RA_Config.h"
#include "PlutoUtils/StringUtils.h"
//------------------------------------------------------------------------------------------------------
#include <string>
#include <map>

using namespace std;

//------------------------------------------------------------------------------------------------------

    /**
    @class DCEConfig
     Defines config stuff.
     */
class DCEConfig : public RA_Config
{
private:

	map<string,bool> m_mapModifiedParameters;
public: // HACK needed by lmce_launch_managerwidget.cpp -- dtk 2007-11-06 HACK
	map<string,string> m_mapParameters;

public:
	// serialized values
	string m_sDBHost,m_sDBUser,m_sDBPassword,m_sDBName,m_sDCERouter;
	string m_sMenuPath,m_sPicturePath;  // Used by the server to find the menus and pictures.  Picture Path may be used by the Establishment also
	string m_sSisFilesPath;
	string m_sConfigFile;

	int m_iPK_Device_Computer,m_iDBPort,m_iDCERouterPort,m_iPK_Installation;
	int m_iPK_Distro;

	string m_mapParameters_Find(string Token) {	map<string,string>::iterator it = m_mapParameters.find( Token ); return it == m_mapParameters.end() ? string() : (*it).second; }
	bool m_mapParameters_Exists(string Token) {	map<string,string>::iterator it = m_mapParameters.find( Token ); return it != m_mapParameters.end(); }

	inline int ReadInteger(string sToken, int iDefaultValue = 0);
	inline string ReadString(string sToken, string sDefaultValue = "");

	const map<string, string>& Parameters() const;

	bool WriteSettings();

	void AddString(string sToken, string sValue);
	void AddInteger(string sToken, int iValue);
	void ParseFile(vector<string> &vectString);

	DCEConfig(string sFilename="/etc/pluto.conf");
	DCEConfig(const char *pBuffer,size_t size);
	virtual ~DCEConfig();
};
//------------------------------------------------------------------------------------------------------
#endif

