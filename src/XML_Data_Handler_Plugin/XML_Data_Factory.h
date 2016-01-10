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

#ifndef XML_Data_Factory_h
#define XML_Data_Factory_h

#include <map>

#include "XML_Data_CallBack.h"
#include "../PlutoUtils/MultiThreadIncludes.h"

using namespace std;

namespace DCE
{
	class XML_Data_Factory
	{
		// A foreign data factory can register with RegisterForeignFactory, it's device id and data id will be stored m_mapForeignFactories
		// and Populate requests will be forwarded to it
		map<string,int> m_mapForeignFactories;

		// XML_Data_Source's that Register() will be stored here
		map<string,XML_Data_Source_Info *> m_mapXML_Data_Source_Info;

		pluto_pthread_mutex_t m_DataFactoryMutex;  // Our mutex to protect our member variables

		class Command_Impl *m_pCommand_Impl; // The Command_Impl we can use to send messages

	public:
		XML_Data_Factory( Command_Impl *pCommand_Impl );
		~XML_Data_Factory();
		int RegisterForeignFactory(const char *szDataID,int PK_Device);
		int Register(XML_Data_Source_Instantantiate _XML_Data_Source_Instantantiate,
							   XML_Data_Source_DataID _XML_Data_Source_DataID,
							   XML_Data_InstantiationInfo *pXML_Data_InstantiationInfo,
							   XML_Data_Source_Info::enumCanCache _enumCanCache,bool bSpecificToRequestor);

		int Populate(int PK_Device_Requestor,string sDataID,string sXmlIn,string *sXmlOut);  // The entry point to populate data
	};
}

#endif
