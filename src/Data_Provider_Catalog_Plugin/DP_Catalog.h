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

#ifndef DP_Catalog_h
#define DP_Catalog_h

#include <map>

#include "Data_Provider_CallBack.h"
#include "../PlutoUtils/MultiThreadIncludes.h"

class RegDevice
{
public:
	bool m_bWildcard;
	int m_dwPK_Device;
	string m_sFormatIn;
	string m_sFormatOut;
};

typedef list<RegDevice *> ListRegDevice;

using namespace std;

namespace DCE
{
	class DP_Catalog
	{
		class Command_Impl *m_pCommand_Impl; // The Command_Impl we can use to send messages
		// Private member variables
		map<string, ListRegDevice *> m_mapRegisters;

	public:
		DP_Catalog( Command_Impl *pCommand_Impl );
		~DP_Catalog();

		void Register_Data_Provider(int iPK_Device,string sID,string sFormat_Input,string sFormat_Output);
		bool Request_Data_Provider(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String); // Return true if it was processed OK
	};
}

#endif
