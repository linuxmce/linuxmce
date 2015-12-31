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

#ifndef XML_Data_Source_h
#define XML_Data_Source_h

#include "../XML_Data_Handler_Plugin/XML_Data_Factory.h"
#include "../XML_Data_Handler_Plugin/XML_Data_CallBack.h"

class Database_pluto_main;

namespace DCE
{
	class XML_Data_InstantiationInfo_Basic : public XML_Data_InstantiationInfo
	{
	public:
		class Basic_XML_Data_Source_Plugin *m_pBasic_XML_Data_Source_Plugin;
		Database_pluto_main *m_pDatabase_pluto_main;

		XML_Data_InstantiationInfo_Basic(Basic_XML_Data_Source_Plugin *pBasic_XML_Data_Source_Plugin);
	};

	// DeviceList Lists all the devices in the system, including all rooms, device templates and device categories with devices
	class DeviceList : public XML_Data_Source_Base
	{
		Database_pluto_main *m_pDatabase_pluto_main;
		XML_Data_InstantiationInfo_Basic *m_pXML_Data_InstantiationInfo_Basic;
	public:
		DeviceList(XML_Data_InstantiationInfo_Basic *pXML_Data_InstantiationInfo_Basic) { m_pXML_Data_InstantiationInfo_Basic=pXML_Data_InstantiationInfo_Basic; m_pDatabase_pluto_main=m_pXML_Data_InstantiationInfo_Basic->m_pDatabase_pluto_main; }
		static const char *DataID() { return "Devices"; }
		static XML_Data_Source_Base *Instantiate(XML_Data_InstantiationInfo *pXML_Data_InstantiationInfo) { return new DeviceList( (XML_Data_InstantiationInfo_Basic *) pXML_Data_InstantiationInfo); }

		virtual int Populate(xmlDocPtr xmlDocPtr_Parms_In,xmlDocPtr xmlDocPtr_Data,xmlNodePtr xmlNodePtr_Root);
	};

	// CommandGroupList Lists all the command groups in the system by room, including all rooms, and arrays (ie command group categories)
	class CommandGroupList : public XML_Data_Source_Base
	{
		Database_pluto_main *m_pDatabase_pluto_main;
		XML_Data_InstantiationInfo_Basic *m_pXML_Data_InstantiationInfo_Basic;
	public:
		CommandGroupList(XML_Data_InstantiationInfo_Basic *pXML_Data_InstantiationInfo_Basic) { m_pXML_Data_InstantiationInfo_Basic=pXML_Data_InstantiationInfo_Basic; m_pDatabase_pluto_main=m_pXML_Data_InstantiationInfo_Basic->m_pDatabase_pluto_main; }
		static const char *DataID() { return "CommandGroups"; }
		static XML_Data_Source_Base *Instantiate(XML_Data_InstantiationInfo *pXML_Data_InstantiationInfo) { return new CommandGroupList( (XML_Data_InstantiationInfo_Basic *) pXML_Data_InstantiationInfo); }

		virtual int Populate(xmlDocPtr xmlDocPtr_Parms_In,xmlDocPtr xmlDocPtr_Data,xmlNodePtr xmlNodePtr_Root);
	};
}

#endif
