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

#ifndef XML_Data_CallBack_h
#define XML_Data_CallBack_h

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <map>
#include <string>
using namespace std;

/*
	A class derived from XML_Data_Source can provide data for a given data id.
	It must implement the static function Instantiate() to create a copy of itself,
	DataID() to return the data id it provides, and also the Populate() method.
*/
class XML_Data_Source_Base
{
	xmlDocPtr m_p_xmlDocPtr_Data_Last;
public:

	XML_Data_Source_Base() { m_p_xmlDocPtr_Data_Last=NULL; }

	// xmlDocPtr_Parms_In contains any parameters the requestor provides
	// xmlDocPtr_Data will be an empty XML doc to populate with the data
	// xmlNodePtr_Root will contain the root document the data should be created under
	virtual int Populate(xmlDocPtr xmlDocPtr_Parms_In,xmlDocPtr xmlDocPtr_Data,xmlNodePtr xmlNodePtr_Root)=0;

	/*
		When the data source registers if the cache setting is cache_optional and a set of 
		data has already been created, then before calling Populate
		the XML_Data_Source_Factory will call RepopulateData with the parms and the existing
		data.  If the data is ok as is, this function should return false and the factory 
		will use the existing data.  If it returns true, the factory will delete the existing
		xml doc and call Populate again with an empty one.  If the data just needs a little
		refresh and not a full populate, this function can make changes to p_xmlDocPtr_Data 
		and return false.
		xmlNodePtr_Root will contain the root document the data should be created under
	*/
	bool RepopulateData(xmlDocPtr xmlDocPtr_Parms_In,xmlDocPtr xmlDocPtr_Data,xmlNodePtr xmlNodePtr_Root) { return false; }

	xmlDocPtr m_p_xmlDocPtr_Data_Last_get() { return m_p_xmlDocPtr_Data_Last; }
	void m_p_xmlDocPtr_Data_Last_set(xmlDocPtr p_xmlDocPtr_Data_Last)
	{ 
		if( m_p_xmlDocPtr_Data_Last )
			xmlFreeDoc(m_p_xmlDocPtr_Data_Last);
		m_p_xmlDocPtr_Data_Last=p_xmlDocPtr_Data_Last; 
	}
};

// When registering a derived class can be passed in which contains
// info the data source needs upon instantiation, such as pointers to databases
class XML_Data_InstantiationInfo
{
};


typedef XML_Data_Source_Base *( *XML_Data_Source_Instantantiate )(XML_Data_InstantiationInfo *pXML_Data_InstantiationInfo);
typedef const char *( *XML_Data_Source_DataID ) ();

class XML_Data_Source_Info
{
public:
	enum enumCanCache { cache_never, cache_always, cache_optional };

private:
	bool m_bSpecificToRequestor;  // If true, the logic handler creates a separate XMLDoc for each requestor, which is usually an Orbiter
	XML_Data_Source_Base *m_pXML_Data_Source_Base;  // This is used if m_bSpecificToRequestor is false
	map<int,XML_Data_Source_Base *> m_mapXML_Data_Source_Base;  // This is used if m_bSpecificToRequestor is true, and the int is the id of the requestor

	XML_Data_Source_Instantantiate m_XML_Data_Source_Instantantiate; // The function which returns a new instance of the data source
	XML_Data_Source_DataID m_XML_Data_Source_DataID; // The function that returns the ID
	XML_Data_InstantiationInfo *m_pXML_Data_InstantiationInfo; // Info to be passed in when instantiating the device

	/*	If this is cache_never, the logic handler will delete the document each time.  If cache_always is true, it 
		will automatically return the last grid generated.  Cache_optional means the generator is passed an existing grid and 
		has the option of returning without making any changes, modifying it, or creating a new grid */
	enumCanCache m_enumCanCache;

public:
	XML_Data_Source_Info(XML_Data_Source_Instantantiate XML_Data_Source_Instantantiate,
			XML_Data_Source_DataID XML_Data_Source_DataID,
			XML_Data_InstantiationInfo *pXML_Data_InstantiationInfo,
			enumCanCache _enumCanCache,bool bSpecificToRequestor);

	XML_Data_Source_Base *XML_Data_Source_Base_get(int PK_Device_Requestor);

	int Populate(int PK_Device_Requestor,string sXmlIn,string *sXmlOut);
};

#endif
