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
#ifndef __SELF_UPDATE_H__
#define __SELF_UPDATE_H__

#include <string>
#include "Orbiter.h"
//-----------------------------------------------------------------------------------------------------
class OrbiterSelfUpdate
{
	Orbiter *m_pOrbiter;
	string m_sOrbiterFilePath;
    string m_sUpdateBinaryPath;

public:
	OrbiterSelfUpdate(Orbiter *pOrbiter);
	~OrbiterSelfUpdate();

	static void GetProcessFilePath(char *pProcessFilePath);
	//bool IsTheNewOrbiter();
	string GetOrbiterCheckSum();
	bool UpdateAvailable();
	bool DownloadUpdateBinary();
	bool CreateCommunicationFile();
	bool SpawnUpdateBinaryProcess();
	bool LastUpdateFailed();

	bool Run();
};
//-----------------------------------------------------------------------------------------------------
#endif //__SELF_UPDATE_H__
