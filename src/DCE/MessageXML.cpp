#include "MessageXML.h"
#include "PlutoUtils/uuencode.h"
#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------
using namespace std;
using namespace DCE;
//--------------------------------------------------------------------------------------------------
MessageXML::MessageXML(Message *pMessage, bool bAddHeaders/* = true*/) : m_pMessage(pMessage), 
	m_bFailed(false)
{
	GenerateXML(bAddHeaders);	
}
//--------------------------------------------------------------------------------------------------
MessageXML::MessageXML(std::string sXMLData, Message *pMessage) : m_sXMLData(sXMLData), 
	m_pMessage(pMessage), m_bFailed(false)
{
	ParseXML();
}
//--------------------------------------------------------------------------------------------------
MessageXML::MessageXML(xmlNode *pNode, Message *pMessage) : m_bFailed(false)
{
	ParseXML(pNode);
}
//--------------------------------------------------------------------------------------------------
MessageXML::~MessageXML()
{
}
//--------------------------------------------------------------------------------------------------
Message *MessageXML::GetMessage()
{
	return m_pMessage;
}
//--------------------------------------------------------------------------------------------------
std::string MessageXML::GetXML()
{
	return m_sXMLData;
}
//--------------------------------------------------------------------------------------------------
void MessageXML::ParseXML(xmlNode *pNode/* = NULL*/)
{
	//use libxml2 to parse the xml and create a message object from it
	xmlNode *msg_node = NULL;
	xmlDoc *doc = NULL;

	if(NULL == pNode)
	{
		xmlDoc *doc = xmlReadMemory(m_sXMLData.c_str(), static_cast<int>(m_sXMLData.length()), NULL, NULL, 0);
		if(NULL == doc) 
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to parse xml %s", m_sXMLData.c_str());
			m_bFailed = true;
			return;
		}

		msg_node = xmlDocGetRootElement(doc);
	}
	else
	{
		msg_node = pNode;
	}

	if(NULL != msg_node && msg_node->type == XML_ELEMENT_NODE && 
		string(reinterpret_cast<const char *>(msg_node->name)) == "message")
	{
		map<string, string> mapAttributes;
		ParseAttributes(msg_node, mapAttributes);

		m_pMessage->m_dwMessage_Type = atoi(mapAttributes["type"].c_str());
		m_pMessage->m_dwID = atoi(mapAttributes["id"].c_str());

		xmlNode *cur_node = NULL;
		for(cur_node = msg_node->children; cur_node; cur_node = cur_node->next) 
		{
			if(cur_node->type == XML_ELEMENT_NODE) 
			{
				string sNodeName(reinterpret_cast<const char *>(cur_node->name));
				
				if(sNodeName == "from")
					ParseFromNode(cur_node);
				else if(sNodeName == "target")
					ParseTargetNode(cur_node);
				else if(sNodeName == "expected_response")
					ParseExpectedResponseNode(cur_node);
				else if(sNodeName == "parameters")
					ParseParametersNode(cur_node);
				else if(sNodeName == "extra_messages")
					ParseExtraMessagesNode(cur_node);
			}
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot find 'message' node");
	}

	if(NULL != doc)
	{
		//free the document
		xmlFreeDoc(doc);

		//Free the global variables that may
		//have been allocated by the parser.
		xmlCleanupParser();
	}
}
//--------------------------------------------------------------------------------------------------
void MessageXML::GenerateXML(bool bAddHeaders)
{
	m_sXMLData = "<message type=\"" + StringUtils::ltos(m_pMessage->m_dwMessage_Type) + 
		"\" id=\"" + StringUtils::ltos(m_pMessage->m_dwID) + "\" >";
	
	//from
	m_sXMLData += "<from id=\"" + StringUtils::ltos(m_pMessage->m_dwPK_Device_From) + "\" />";

	//target
	string sTargetType;
	string sTargetID;

	switch(m_pMessage->m_dwPK_Device_To)
	{
		case DEVICEID_CATEGORY:
		{
			sTargetType = "category";
			sTargetID = StringUtils::ltos(m_pMessage->m_dwPK_Device_Category_To);
		}
		break;

		case DEVICEID_MASTERDEVICE:
		{
			sTargetType = "template";
			sTargetID = StringUtils::ltos(m_pMessage->m_dwPK_Device_Template);
		}
		break;

		case DEVICEID_LIST:
		{
			sTargetType = "device_list";
			sTargetID = m_pMessage->m_sPK_Device_List_To;
		}
		break;

		default:
		{
			sTargetType = "device";
			sTargetID = StringUtils::ltos(m_pMessage->m_dwPK_Device_To);
		}
		break;
	}

	m_sXMLData += "<target id=\"" + sTargetID + "\" type=\"" + sTargetType + "\" />";    

	//expected response
	m_sXMLData += string("<expected_response type=\"") + 
		(m_pMessage->m_eExpectedResponse == ER_DeliveryConfirmation ? "delivery_confirmation" :
			(m_pMessage->m_eExpectedResponse == ER_ReplyString ? "reply_string" : "default")) + "\" />";

	//parameters
	m_sXMLData += "<parameters>";
	//string params
	for(map<long, string>::iterator itP = m_pMessage->m_mapParameters.begin(); itP != m_pMessage->m_mapParameters.end(); ++itP)
		m_sXMLData += "<parameter id=\"" + StringUtils::ltos(itP->first) + "\" type=\"text\" value=\"" + itP->second + "\" />";

	//data params
	for(map<long, char *>::iterator itD = m_pMessage->m_mapData_Parameters.begin(); itD != m_pMessage->m_mapData_Parameters.end(); ++itD)
	{
		size_t SizeRaw = m_pMessage->m_mapData_Lengths[itD->first];
		size_t SizeEncoded = MaxEncodedSize(SizeRaw);
		char *pDataEncoded = new char[SizeEncoded];
		int Bytes = Ns_HtuuEncode((unsigned char *) itD->second, (unsigned int)SizeRaw, (unsigned char *) pDataEncoded);
		pDataEncoded[Bytes] = 0;
		m_sXMLData += "<parameter id=\"" + StringUtils::ltos(itD->first) + "\" type=\"binary\" value=\"" + pDataEncoded + "\" />";
		delete[] pDataEncoded;
	}

	m_sXMLData += "</parameters>";

	//extra messages
	m_sXMLData += "<extra_messages>";

    vector<class Message *>::iterator itExtras;
    for( itExtras = m_pMessage->m_vectExtraMessages.begin(); itExtras != m_pMessage->m_vectExtraMessages.end(); ++itExtras )
    {
        Message *pMessage_Child = *itExtras;
        if(NULL != pMessage_Child)  // The embedded message could have been deleted
		{
			MessageXML msg_xml(pMessage_Child, false);
			m_sXMLData += msg_xml.GetXML();
		}
    }

	m_sXMLData += "</extra_messages>";
	m_sXMLData += "</message>";

	if(bAddHeaders)
	{
		m_sXMLData = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + m_sXMLData;  
		m_sXMLData = "MESSAGEXML " + StringUtils::itos(static_cast<int>(m_sXMLData.length())) + "\n" + m_sXMLData;
	}
}
//--------------------------------------------------------------------------------------------------
/*static*/ void MessageXML::ParseAttributes(xmlNode *pNode, map<string, string>& mapAttributes)
{
	if(NULL == pNode)
		return;

	xmlAttr *cur_attr = NULL;
	for(cur_attr = pNode->properties; NULL != cur_attr; cur_attr = cur_attr->next)
		mapAttributes[reinterpret_cast<const char *>(cur_attr->name)] =
			NULL != cur_attr->children && cur_attr->children->content ? 
				reinterpret_cast<const char *>(cur_attr->children->content) : "";
}
//--------------------------------------------------------------------------------------------------
void MessageXML::ParseFromNode(xmlNode *pNode)
{
	map<string, string> mapAttributes;
	ParseAttributes(pNode, mapAttributes);

	m_pMessage->m_dwPK_Device_From = atoi(mapAttributes["id"].c_str());
}
//--------------------------------------------------------------------------------------------------
void MessageXML::ParseTargetNode(xmlNode *pNode)
{
	m_pMessage->m_dwPK_Device_Category_To = 0;
	m_pMessage->m_dwPK_Device_Template = 0;
	m_pMessage->m_sPK_Device_List_To = "";
	m_pMessage->m_dwPK_Device_To = 0;

	map<string, string> mapAttributes;
	ParseAttributes(pNode, mapAttributes);

	string sTargetType = mapAttributes["type"];
	string sTargetID = mapAttributes["id"];

	if(sTargetType == "category")
	{
		m_pMessage->m_dwPK_Device_To = DEVICEID_CATEGORY;
		m_pMessage->m_dwPK_Device_Category_To = atoi(sTargetID.c_str());
	}
	else if(sTargetType == "template")
	{
		m_pMessage->m_dwPK_Device_To = DEVICEID_MASTERDEVICE;
		m_pMessage->m_dwPK_Device_Template = atoi(sTargetID.c_str());
	}
	else if(sTargetType == "device_list")
	{
		m_pMessage->m_dwPK_Device_To = DEVICEID_LIST;
		m_pMessage->m_sPK_Device_List_To = sTargetID;
	}
	else
	{
		m_pMessage->m_dwPK_Device_To = atoi(sTargetID.c_str());
	}
}
//--------------------------------------------------------------------------------------------------
void MessageXML::ParseExpectedResponseNode(xmlNode *pNode)
{
	map<string, string> mapAttributes;
	ParseAttributes(pNode, mapAttributes);

	string sType = mapAttributes["type"];

	if(sType == "delivery_confirmation")
		m_pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;
	else if(sType == "reply_string")
		m_pMessage->m_eExpectedResponse = ER_ReplyString;
	else 
		m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
}
//--------------------------------------------------------------------------------------------------
void MessageXML::ParseParametersNode(xmlNode *pNode)
{
	m_pMessage->m_mapParameters.clear();
	m_pMessage->m_mapData_Parameters.clear();
	m_pMessage->m_mapData_Lengths.clear();

	xmlNode *cur_node = NULL;
	for(cur_node = pNode->children; cur_node; cur_node = cur_node->next) 
	{
		if(cur_node->type == XML_ELEMENT_NODE) 
		{
			string sNodeName(reinterpret_cast<const char *>(cur_node->name));
			
			if(sNodeName == "parameter")
			{
				map<string, string> mapAttributes;
				ParseAttributes(cur_node, mapAttributes);

				string sType = mapAttributes["type"];
				int nID = atoi(mapAttributes["id"].c_str());
				string sValue = mapAttributes["value"];

				if(sType == "text")
				{
					m_pMessage->m_mapParameters[nID] = sValue;
				}
				else if(sType == "binary")
				{
					int iUUSize = static_cast<int>(sValue.length());
					unsigned char *pBinaryValue = new unsigned char[MaxDecodedSize(iUUSize)];
					size_t tSizeParmValue = Ns_HtuuDecode(
						reinterpret_cast<unsigned char *>(const_cast<char *>(sValue.c_str())), 
						pBinaryValue, iUUSize);
					char *pParmValue = (char *) pBinaryValue;

					m_pMessage->m_mapData_Parameters[nID] = pParmValue;
					m_pMessage->m_mapData_Lengths[nID] = static_cast<unsigned long>(tSizeParmValue);
				}
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------
void MessageXML::ParseExtraMessagesNode(xmlNode *pNode)
{
	m_pMessage->m_vectExtraMessages.clear();

	xmlNode *cur_node = NULL;
	for(cur_node = pNode; NULL != cur_node; cur_node = cur_node->next)
	{
		Message *pExtraMessage = new Message();
		MessageXML msg_xml(cur_node, pExtraMessage);
		m_pMessage->m_vectExtraMessages.push_back(pExtraMessage);
	}
}
//--------------------------------------------------------------------------------------------------
