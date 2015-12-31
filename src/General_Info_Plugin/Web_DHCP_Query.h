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
#ifndef WEB_DHCP_QUERY
#define WEB_DHCP_QUERY

#include <string>
#include <map>
#include <vector>
using namespace std;

namespace nsWeb_DHCP_Query
{
	typedef map<string, string> Web_DHCP_Query_Params;
	typedef vector<string> Web_DHCP_Query_Row;
	typedef vector<Web_DHCP_Query_Row> Web_DHCP_Query_Result;

	class Web_DHCP_Query
	{
		private:
			string m_sURL_Base;
			string QueryString_Pair(const string & sVar, const string & sValue);
			
		public:
			Web_DHCP_Query(const string & sURL_Base) : m_sURL_Base(sURL_Base) {};
			void Query(Web_DHCP_Query_Params Params, Web_DHCP_Query_Result & Result);
	};
}

#endif
