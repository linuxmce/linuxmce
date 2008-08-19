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

#ifndef Generic_Mysql_Provider_h
#define Generic_Mysql_Provider_h

#include <map>
#include <string>

using namespace std;

#include "../Data_Provider_Catalog_Plugin/Data_Provider_CallBack.h"
class Media_Provider_InstantiationInfo;

namespace DCE
{
	class Media_Provider_InstantiationInfo;

	class Generic_Mysql_Provider : public Data_Provider_Source_Base
	{
	public:
		Media_Provider_InstantiationInfo *m_pMedia_Provider_InstantiationInfo;
		map< string, pair<string,string> > m_mapQueries;
		static Generic_Mysql_Provider *m_pthis;

		Generic_Mysql_Provider( Media_Provider_InstantiationInfo *pMedia_Provider_InstantiationInfo ) : Data_Provider_Source_Base()
		{
			m_pMedia_Provider_InstantiationInfo=pMedia_Provider_InstantiationInfo;
			m_pthis=this;
		}
		~Generic_Mysql_Provider();
		/*
		static Data_Provider_Source_Base *Instantiate(Data_Provider_InstantiationInfo *pData_Provider_InstantiationInfo,string sID)
		{ 
			return new DPS_MainJSONData_Devices( (JSON_Provider_InstantiationInfo *) pData_Provider_InstantiationInfo);
		}
*/
		static Data_Provider_Source_Base *Instantiate(Data_Provider_InstantiationInfo *pData_Provider_InstantiationInfo,string sID)
		{ 
			return m_pthis;
		}

		// Return true if the populate went ok
		virtual bool Populate(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String) { return false; }
		virtual bool Populate(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String, string sQuery);

		bool RegisterQueries(string sQueries,class Data_Provider_Factory *pData_Provider_Factory);

		string ParseQueryParameters(string sQuery, string sParameterValues,string sParameterNames);
		void PreprocessQueryParameters(string sName,string &sParm,string &sQuery);
	};
}

#endif
