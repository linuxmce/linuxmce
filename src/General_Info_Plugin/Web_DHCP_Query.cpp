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
#include "Web_DHCP_Query.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

using namespace DCE;

namespace nsWeb_DHCP_Query
{
	void Web_DHCP_Query::Query(Web_DHCP_Query_Params Params, Web_DHCP_Query_Result & Result)
	{
		Result.clear();

		// build HTTP GET query string
		Web_DHCP_Query_Params::iterator it;
		string sQueryString = "";
		if (Params.size() != 0)
		{
			it = Params.begin();
			sQueryString = QueryString_Pair(it->first, it->second);
			it++;
			for (; it != Params.end(); it++)
			{
				sQueryString += "&" + QueryString_Pair(it->first, it->second);
			}
		}
		
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Running Web Query: %s?%s", m_sURL_Base.c_str(), sQueryString.c_str());
		
		string sURL = m_sURL_Base + "?" + sQueryString;

		size_t size;
		char * buffer = NULL;
		buffer = FileUtils::ReadURL(sURL,size,true);
		
		if (size==0)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error fetching page: %s", sURL.c_str());
			return;
		}
		else
		{
			Web_DHCP_Query_Row Rows;
			string sBuffer = buffer;
			free(buffer); // allocated by http_fetch

			StringUtils::Tokenize(sBuffer, "\r\n", Rows);

			for (size_t i = 0; i < Rows.size(); i++)
			{
				Web_DHCP_Query_Row Fields;
				
				// StringUtils::Tokenize(..., vector) doesn't return empty fields while this other one does, but it's more "manual labor" (tm)
				size_t CurPos = 0;
				do
				{
					string sField = StringUtils::Tokenize(Rows[i], "\t", CurPos);
					Fields.push_back(sField);
				} while (CurPos < Rows[i].length());
			
				Result.push_back(Fields);
			}
		}
	}

	string Web_DHCP_Query::QueryString_Pair(const string & sVar, const string & sValue)
	{
		return StringUtils::URLEncode(sVar) + "=" + StringUtils::URLEncode(sValue);
	}
}
