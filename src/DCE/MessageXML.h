

/** @file MessageXML.h
 Header file for the MessageXML class.
 @todo notcommented
 */
#ifndef __MESSAGE_XML_H__
#define __MESSAGE_XML_H__
//--------------------------------------------------------------------------------------------------
#include <string>
#include <map>
//--------------------------------------------------------------------------------------------------
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "Message.h"
//--------------------------------------------------------------------------------------------------
namespace DCE
{
    /**
    @class MessageXML
    This class ???
     */
	class MessageXML
	{
	public:
		MessageXML(Message *pMessage, bool bAddHeaders = true);
		MessageXML(std::string sXMLData, Message *pMessage);
		~MessageXML();

		Message *GetMessage();
		std::string GetXML();
		bool Failed() { return m_bFailed; }

	private:
		Message *m_pMessage;
		std::string m_sXMLData;
		bool m_bFailed;

		MessageXML(xmlNode *pNode, Message *pMessage);

		void ParseXML(xmlNode *pNode = NULL);
		void GenerateXML(bool bAddHeaders);

		//helpers
		static void ParseAttributes(xmlNode *pNode, map<string, string>& mapAttributes);

		void ParseFromNode(xmlNode *pNode);
		void ParseTargetNode(xmlNode *pNode);
		void ParseExpectedResponseNode(xmlNode *pNode);
		void ParseParametersNode(xmlNode *pNode);
		void ParseExtraMessagesNode(xmlNode *pNode);
	};
}
//--------------------------------------------------------------------------------------------------
#endif //__MESSAGE_XML_H__
