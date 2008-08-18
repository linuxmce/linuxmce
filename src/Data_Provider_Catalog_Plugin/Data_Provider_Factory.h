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

#ifndef Data_Provider_Factory_h
#define Data_Provider_Factory_h

#include <map>

#include "Data_Provider_CallBack.h"
#include "../PlutoUtils/MultiThreadIncludes.h"

using namespace std;

typedef list<Data_Provider_Source_Info *> ListData_Provider_Source_Info;
namespace DCE
{
	class Data_Provider_Factory
	{
		// Data_Provider_Source's that Register() will be stored here
		map<string,ListData_Provider_Source_Info *> m_mapListData_Provider_Source_Info;

		pluto_pthread_mutex_t m_DataFactoryMutex;  // Our mutex to protect our member variables

		class Command_Impl *m_pCommand_Impl; // The Command_Impl we can use to send messages

	public:
		Data_Provider_Factory( Command_Impl *pCommand_Impl );
		~Data_Provider_Factory();
		int Register(Data_Provider_Source_Instantantiate _Data_Provider_Source_Instantantiate,
							   string sID,string sFormatIn, string sFormatOut,
							   Data_Provider_InstantiationInfo *pData_Provider_InstantiationInfo,
							   Data_Provider_Source_Info::enumCanCache _enumCanCache,bool bSpecificToRequestor);

		bool Populate(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String);

		void RegisterCatalog();
	};
}

#endif
