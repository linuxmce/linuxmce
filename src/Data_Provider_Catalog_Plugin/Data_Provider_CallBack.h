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

#ifndef Data_Provider_CallBack_h
#define Data_Provider_CallBack_h

#include <map>
#include <string>
using namespace std;

/*
	Devices can provide data by including classes derived from Data_Provider_Source_Base.  Each separate device with data sources needs it's own Data_Provider_Factory.
	The Data_Provider_Catalog keeps track of all the the devices with factories, and when a Populate command comes in, it forwards it to the appropriate device with 
	the corresponding factory.

	Data_Provider_Source_Base -- Classes derived from here provide the actual data in the specified format by implementing Populate.

	Data_Provider_InstantiationInfo -- The base class is empty.  Derived classes can have data that Data_Provider_Source_Base may need to do the population,
	such as pointers to database info.  If there is data that Data_Provider_Source_Base will need, create a derived class of Data_Provider_InstantiationInfo,
	set the data, and then pass it on the constructor to Data_Provider_Source_Info.

	Data_Provider_Source_Instantantiate is a typedef for a pointer to a function that returns a pointer to a Data_Provider_Source_Base

	The software's startup should create an instance of Data_Provider_Factory, and an instance of a class derivied from Data_Provider_InstantiationInfo if there's data to pass.
	Then call Data_Provider_Factory::Register passing in the pointer to the Data_Provider_Source_Instantantiate function that will create the instance, 
		the pointer to the Data_Provider_Source_DataID function that returns the ID of the data,
		the Data_Provider_InstantiationInfo, the enumCanCache to indicate if the results can be cached 
		and bSpecificToRequestor==true then a separate instance of the Data_Provider_Source_Base will be created for each requesting device ID.

	The factory will create an instance of Data_Provider_Source_Info to hold all this information, and it stores them in m_mapData_Provider_Source_Info.

	When all Data_Provider_Source_Base's have registered with the factory, call the Data_Provider_Factory::RegisterCatalog to register the factory with the catalog.

	A class derived from Data_Provider_Source_Base can provide data for a given data id.
	It must implement an Instantiate function (Data_Provider_Source_Instantantiate) to create a copy of itself,
	DataID() to return the data id it provides, and also the Populate() method.
*/

class Data_Provider_Source_Base
{
public:

	Data_Provider_Source_Base() { }

	// Return true if the populate went ok
	virtual bool Populate(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String)=0;

	/*
		When the data source registers if the cache setting is cache_optional and a set of 
		data has already been created, then before calling Populate
		the Data_Provider_Source_Factory will call RepopulateData with the parms and the existing
		data.  If the data is ok as is, this function should return false and the factory 
		will use the existing data.  If it returns true, the factory will delete the existing
		xml doc and call Populate again with an empty one.  If the data just needs a little
		refresh and not a full populate, this function can make changes to p_xmlDocPtr_Data 
		and return false.
		xmlNodePtr_Root will contain the root document the data should be created under
	bool RepopulateData(string sParameterFormat, string sParameters, string &sOutputFormat, string &sOutput) { return false; }

	xmlDocPtr m_p_xmlDocPtr_Data_Last_get() { return m_p_xmlDocPtr_Data_Last; }
	void m_p_xmlDocPtr_Data_Last_set(xmlDocPtr p_xmlDocPtr_Data_Last)
	{ 
		if( m_p_xmlDocPtr_Data_Last )
			xmlFreeDoc(m_p_xmlDocPtr_Data_Last);
		m_p_xmlDocPtr_Data_Last=p_xmlDocPtr_Data_Last; 
	}
	*/
};

// When registering a derived class can be passed in which contains
// info the data source needs upon instantiation, such as pointers to databases
class Data_Provider_InstantiationInfo
{
public:
	virtual const char *Type() { return "Base"; }
};


// Typedef the function which returns a Data_Provider_Source_Base for a given ID
typedef Data_Provider_Source_Base *( *Data_Provider_Source_Instantantiate )(Data_Provider_InstantiationInfo *pData_Provider_InstantiationInfo,string sID);

// Point
//typedef const char *( *Data_Provider_Source_DataID ) ();

class Data_Provider_Source_Info
{
public:
	enum enumCanCache { cache_never, cache_always, cache_optional };

	bool m_bSpecificToRequestor;  // If true, the logic handler creates a separate XMLDoc for each requestor, which is usually an Orbiter
	bool m_bWildcard; // If true, we match wildcards for the ID's
	Data_Provider_Source_Base *m_pData_Provider_Source_Base;  // This is used if m_bSpecificToRequestor is false
	map<int,Data_Provider_Source_Base *> m_mapData_Provider_Source_Base;  // This is used if m_bSpecificToRequestor is true, and the int is the id of the requestor
	string m_sID; // The ID we have registered
	string m_sFormatIn, m_sFormatOut; // The formats we can handle

	Data_Provider_Source_Instantantiate m_Data_Provider_Source_Instantantiate; // The function which returns a new instance of the data source
	Data_Provider_InstantiationInfo *m_pData_Provider_InstantiationInfo; // Info to be passed in when instantiating the device

	/*	If this is cache_never, the logic handler will delete the document each time.  If cache_always is true, it 
		will automatically return the last grid generated.  Cache_optional means the generator is passed an existing grid and 
		has the option of returning without making any changes, modifying it, or creating a new grid */
	enumCanCache m_enumCanCache;

public:
	Data_Provider_Source_Info(Data_Provider_Source_Instantantiate _Data_Provider_Source_Instantantiate,
			string sID, string sFormatIn, string sFormatOut,
			Data_Provider_InstantiationInfo *pData_Provider_InstantiationInfo,
			enumCanCache _enumCanCache,bool bSpecificToRequestor,bool bWildcard);

	Data_Provider_Source_Base *Data_Provider_Source_Base_get(int PK_Device_Requestor);

//	int Populate(int PK_Device_Requestor, string sParameterFormat, string sParameters, string &sOutputFormat, string &sOutput);
};

#endif
