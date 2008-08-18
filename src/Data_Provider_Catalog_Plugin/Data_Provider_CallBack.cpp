/*
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include "Data_Provider_CallBack.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

Data_Provider_Source_Info::Data_Provider_Source_Info(Data_Provider_Source_Instantantiate _Data_Provider_Source_Instantantiate,
	string sID, string sFormatIn, string sFormatOut,
	Data_Provider_InstantiationInfo *pData_Provider_InstantiationInfo,
	enumCanCache _enumCanCache,bool bSpecificToRequestor,bool bWildcard)
{
	m_Data_Provider_Source_Instantantiate=_Data_Provider_Source_Instantantiate;
	m_sID=sID;
	m_sFormatIn=sFormatIn;
	m_sFormatOut=sFormatOut;
	m_pData_Provider_InstantiationInfo=pData_Provider_InstantiationInfo;
	m_enumCanCache=_enumCanCache;
	m_bSpecificToRequestor=bSpecificToRequestor;
	m_bWildcard=bWildcard;
	m_pData_Provider_Source_Base=NULL;
}

Data_Provider_Source_Base *Data_Provider_Source_Info::Data_Provider_Source_Base_get(int PK_Device_Requestor)
{
	if( m_bSpecificToRequestor )
	{
		map<int,Data_Provider_Source_Base *>::iterator it=m_mapData_Provider_Source_Base.find( PK_Device_Requestor );
		if( it==m_mapData_Provider_Source_Base.end() )
		{
			Data_Provider_Source_Base *pData_Provider_Source_Base = (*m_Data_Provider_Source_Instantantiate)(m_pData_Provider_InstantiationInfo,"");
			m_mapData_Provider_Source_Base[PK_Device_Requestor] = pData_Provider_Source_Base;
			return pData_Provider_Source_Base;
		}
		else
			return it->second;
	}
	else
	{
		if( m_pData_Provider_Source_Base==NULL )
			m_pData_Provider_Source_Base = (*m_Data_Provider_Source_Instantantiate)(m_pData_Provider_InstantiationInfo,"");
		return m_pData_Provider_Source_Base;
	}
}
/*
int Data_Provider_Source_Info::Populate(int PK_Device_Requestor,string sXmlIn,string *sXmlOut)
{
	Data_Provider_Source_Base *pData_Provider_Source_Base = Data_Provider_Source_Base_get(PK_Device_Requestor);
	xmlDocPtr pxmlDocPtr_Out = pData_Provider_Source_Base->m_p_xmlDocPtr_Data_Last_get();

	// Unless we have an existing pxmlDocPtr_Out and m_enumCanCache==cache_always, we 
	// may need to rebuild the data
	if( pxmlDocPtr_Out==NULL || m_enumCanCache!=cache_always )
	{
		// Parse the incoming parameters since we may need to rebuild the data
		xmlDocPtr pxmlDocPtr_In = xmlParseMemory(sXmlIn.c_str(),(int) sXmlIn.size());

		// We need to rebuild it if we don't have existing data in pxmlDocPtr_Out, or cache_never
		// or RepopulateData is true
		if( pxmlDocPtr_Out==NULL ||
			m_enumCanCache==cache_never || pData_Provider_Source_Base->RepopulateData(pxmlDocPtr_In,pxmlDocPtr_Out,xmlDocGetRootElement(pxmlDocPtr_Out)) )
		{
			// We need to repopulate the data
			pxmlDocPtr_Out = xmlNewDoc(BAD_CAST "1.0");

			xmlNodePtr xmlNodePtr_root = NULL;
			xmlNodePtr_root = xmlNewNode(NULL, BAD_CAST "PlutoXmlData");
			xmlDocSetRootElement(pxmlDocPtr_Out, xmlNodePtr_root);

			pData_Provider_Source_Base->Populate(pxmlDocPtr_In,pxmlDocPtr_Out,xmlNodePtr_root);
			pData_Provider_Source_Base->m_p_xmlDocPtr_Data_Last_set( pxmlDocPtr_Out );
		}
	}

	xmlSaveFormatFileEnc("/tmp.xml", pxmlDocPtr_Out, "UTF-8", 1);
	size_t size;
	char *pBuf = FileUtils::ReadFileIntoBuffer("/tmp.xml",size);
	*sXmlOut = pBuf;
	/*
	// This doesn't work
	xmlOutputBufferPtr buf = xmlAllocOutputBuffer (NULL);
	int	iResult = xmlSaveFormatFileTo(buf, 
			pxmlDocPtr_Out, 
			"", 
			1);
		*/

/*
	return 0;
}
*/