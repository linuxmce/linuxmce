#include "UpdatesXML.h"
#include "DCE/Logger.h"
#include "PlutoUtils/StringUtils.h"

using namespace std;
using namespace DCE;

char * UpdatesXML::tagUpdate = "update";
char * UpdatesXML::tagId = "id";
char * UpdatesXML::tagPriority = "priority";
char * UpdatesXML::tagDescription = "description";
char * UpdatesXML::tagModels = "model";
char * UpdatesXML::tagRelease = "release";
char * UpdatesXML::tagFiles = "file";
char * UpdatesXML::tagOptions = "option";
char * UpdatesXML::tagRequires = "require";
char * UpdatesXML::attrOptionsType = "type";
char * UpdatesXML::attrOptionsPre = "pre";
char * UpdatesXML::attrOptionsPost = "post";
char * UpdatesXML::attrRequiresModel = "model";
char * UpdatesXML::attrRequiresUpdate = "update";

// class Update Node
UpdateNode::UpdateNode(unsigned id)
	: updateId(id),
	  priority(UpdateNode::Normal)
{
}

UpdateNode::~UpdateNode()
{
	// remove the elements
	for(map<string, UpdateProperty*>::iterator it=properties.begin(); it!=properties.end(); ++it)
	{
		delete (*it).second;
	}
	properties.clear();
	
	// remove the file elements
	for(vector<UpdateProperty*>::iterator it1=files.begin(); it1!=files.end(); ++it1)
	{
		delete (*it1);
	}
	files.clear();
}

bool UpdateNode::IsModel(const string & model) const
{
	ModelMapConstIt it = models.find(model);
	if( it != models.end() )
		return true;
	
	return false;
}

void UpdateNode::SetModels(const string & sText)
{
	string text = sText;
	vector<string> modelList;
	StringUtils::Tokenize(text, ",", modelList);
	for(vector<string>::const_iterator it=modelList.begin(); it!=modelList.end(); ++it)
	{
		models[*it] = 0;
	}
}

string UpdateNode::GetAttribute(const string& attr)
{
	StringMapIt it = attributesMap.find(attr);
	if( it != attributesMap.end() )
		return (*it).second;
	
	return "";
}

//--- class Update XML -----------------------------------------------------------------------------
UpdatesXML::UpdatesXML(std::string sXmlFile)
	: m_sXmlFile(sXmlFile),
	  m_bFailed(false)
{
}

UpdatesXML::~UpdatesXML()
{
	for(vector<UpdateNode*>::iterator it=m_updates.begin(); it!=m_updates.end(); ++it)
	{
		delete (*it);
	}
	m_updates.clear();
}

void UpdatesXML::Clean()
{
	m_bFailed = false;
	for(vector<UpdateNode*>::iterator it=m_updates.begin(); it!=m_updates.end(); ++it)
	{
		delete (*it);
	}
	m_updates.clear();
}

void UpdatesXML::Updates(vector<UpdateNode*> & releaseUpdates, const string & sRelease) const
{
	UpdateNode * pNode;
	for(vector<UpdateNode*>::const_iterator it=m_updates.begin(); it!=m_updates.end(); ++it)
	{
		pNode = (*it);
		if( pNode != NULL )
		{
			if( pNode->IsRelease(sRelease) )
			{
				releaseUpdates.push_back(pNode);
			}
		}
		else
		{
			// error
		}
	}
}

void UpdatesXML::ParseUpdate(xmlNode *pNode)
{
	if( pNode != NULL )
	{
		map<string, string> mapAttributes;
		ParseAttributes(pNode, mapAttributes);
		
		UpdateNode * pUpdateNode = new UpdateNode( atoi(mapAttributes["id"].c_str()) );
		if( pUpdateNode != NULL )
		{
			pUpdateNode->Attributes() = mapAttributes;
			for(xmlNode *cur_node = pNode->children; cur_node; cur_node = cur_node->next) 
			{
				if(cur_node->type == XML_ELEMENT_NODE)
				{
					string sNodeName(reinterpret_cast<const char *>(cur_node->name));
				
					map<string, string> mapAttributesProp;
					ParseAttributes(cur_node, mapAttributesProp);
					
					UpdateProperty * pProperty = new UpdateProperty();
					if( pProperty != NULL )
					{
						pProperty->attributesMap = mapAttributesProp;
						pProperty->name = sNodeName;
						
						xmlChar * value = xmlNodeGetContent(cur_node);
						pProperty->value = reinterpret_cast<const char *>(value);
						xmlFree(value);
						
						// add it to properties map or to files
						if( pProperty->name == UpdatesXML::tagFiles )
						{
							pUpdateNode->AddFile(pProperty);
						}
						else if( pProperty->name == UpdatesXML::tagOptions )
						{
							pUpdateNode->AddOption(pProperty);
						}
						else if( pProperty->name == UpdatesXML::tagRequires )
						{
							pUpdateNode->AddRequire(pProperty);
						}
						else
						{
							pUpdateNode->AddProperty(pProperty->name, pProperty);
						}
						
						// extra info
						if(sNodeName == UpdatesXML::tagPriority)
						{
							if( pProperty->value == "normal" )
							{
								pUpdateNode->SetPriority(UpdateNode::Normal);
							}
							else if( pProperty->value == "asap" )
							{
								pUpdateNode->SetPriority(UpdateNode::ASAP);
							}
							else if( pProperty->value == "urgent" )
							{
								pUpdateNode->SetPriority(UpdateNode::Urgent);
							}
							else
							{
								pUpdateNode->SetPriority(UpdateNode::Normal);
							}
						}
						else if(sNodeName == UpdatesXML::tagDescription)
						{
							pUpdateNode->SetDescription(pProperty->value.c_str());
						}
						else if(sNodeName == UpdatesXML::tagModels)
						{
							pUpdateNode->SetModels(pProperty->value);
						}
						else if(sNodeName == UpdatesXML::tagRelease)
						{
							pUpdateNode->SetRelease(pProperty->value);
						}
					}
				}
			}
			
			m_updates.push_back(pUpdateNode);
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "NULL XML node!");
	}
}

void UpdatesXML::ParseXML(xmlNode *pNode/* = NULL*/)
{
	//use libxml2 to parse the xml and create a message object from it
	xmlNode *update_node = NULL;
	xmlDoc *doc = NULL;

	if(NULL == pNode)
	{
		doc = xmlParseFile(m_sXmlFile.c_str());
		if(NULL == doc) 
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to parse xml %s", m_sXmlFile.c_str());
			m_bFailed = true;
			return;
		}

		update_node = xmlDocGetRootElement(doc);
		
		// TODO: check if the root node is lmce_update
	}
	else
	{
		update_node = pNode;
	}
	
	for(xmlNode *cur_node = update_node->children; cur_node != NULL; cur_node = cur_node->next)
	{
		if( NULL != cur_node && cur_node->type == XML_ELEMENT_NODE &&
			string(reinterpret_cast<const char *>(cur_node->name)) == UpdatesXML::tagUpdate )
		{
			ParseUpdate(cur_node);
		}
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

void UpdatesXML::GenerateXML(const char * outFileName)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	
	// create new xml document with version 1.0 
	doc = xmlNewDoc((const xmlChar*)"1.0");
	// create a new root node  
	root = xmlNewDocNode(doc, NULL, (const xmlChar*)"lmce_update", NULL);
	xmlDocSetRootElement(doc, root);
	
	for(vector<UpdateNode*>::const_iterator it=m_updates.begin(); it!=m_updates.end(); ++it)
	{
		GenerateUpdateXML(*it, outFileName, root);
	}
	
	xmlSaveFormatFile(outFileName, doc, 1);
	xmlFreeDoc(doc);
}

void UpdatesXML::GenerateUpdateXML(UpdateNode * pUpdateNode, const char * outFileName, xmlNodePtr pNode)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr cur;
	
	if( pNode == NULL )
	{
		// create new xml document with version 1.0 
		doc = xmlNewDoc((const xmlChar*)"1.0");
		// create a new root node  
		root = xmlNewDocNode(doc, NULL, (const xmlChar*)"lmce_update", NULL);
		xmlDocSetRootElement(doc, root);
	}
	else
	{
		root = pNode;
	}
	
	cur = xmlNewChild(root, NULL, (const xmlChar*)"update", NULL);
	// attributes
	for(StringMapConstIt it=pUpdateNode->Attributes().begin(); it!=pUpdateNode->Attributes().end(); ++it)
	{
		xmlAttrPtr pAttr;
		pAttr = xmlNewProp(cur, (const xmlChar*)(*it).first.c_str(), (const xmlChar*)(*it).second.c_str());
	}
	// properties
	for(map<string, UpdateProperty*>::const_iterator itProp=pUpdateNode->Properties().begin();
		itProp!=pUpdateNode->Properties().end(); ++itProp)
	{
		UpdateProperty * pUpdateProp = (*itProp).second;
		xmlNodePtr pProp = xmlNewTextChild(cur, NULL,
								(const xmlChar*)pUpdateProp->name.c_str(),
								(const xmlChar*)pUpdateProp->value.c_str());
		for(StringMapConstIt itAttr=pUpdateProp->attributesMap.begin(); itAttr!=pUpdateProp->attributesMap.end(); ++itAttr)
		{
			//xmlAttrPtr pAttr =
				xmlNewProp(pProp, (const xmlChar*)(*itAttr).first.c_str(), (const xmlChar*)(*itAttr).second.c_str());
		}
	}
	// files
	for(vector<UpdateProperty*>::const_iterator itFile=pUpdateNode->Files().begin(); itFile!=pUpdateNode->Files().end(); ++itFile)
	{
		UpdateProperty * pUpdateProp = (*itFile);
		xmlNodePtr pProp = xmlNewTextChild(cur, NULL,
										   (const xmlChar*)pUpdateProp->name.c_str(),
										   (const xmlChar*)pUpdateProp->value.c_str());
		for(StringMapConstIt itAttr=pUpdateProp->attributesMap.begin(); itAttr!=pUpdateProp->attributesMap.end(); ++itAttr)
		{
			//xmlAttrPtr pAttr =
			xmlNewProp(pProp, (const xmlChar*)(*itAttr).first.c_str(), (const xmlChar*)(*itAttr).second.c_str());
		}
	}
	// options
	for(vector<UpdateProperty*>::const_iterator itOpt=pUpdateNode->Options().begin(); itOpt!=pUpdateNode->Options().end(); ++itOpt)
	{
		UpdateProperty * pUpdateProp = (*itOpt);
		xmlNodePtr pProp = xmlNewTextChild(cur, NULL,
										   (const xmlChar*)pUpdateProp->name.c_str(),
										   (const xmlChar*)pUpdateProp->value.c_str());
		for(StringMapConstIt itAttr=pUpdateProp->attributesMap.begin(); itAttr!=pUpdateProp->attributesMap.end(); ++itAttr)
		{
			//xmlAttrPtr pAttr =
			xmlNewProp(pProp, (const xmlChar*)(*itAttr).first.c_str(), (const xmlChar*)(*itAttr).second.c_str());
		}
	}
	// requires
	for(vector<UpdateProperty*>::const_iterator itReq=pUpdateNode->Requires().begin(); itReq!=pUpdateNode->Requires().end(); ++itReq)
	{
		UpdateProperty * pUpdateProp = (*itReq);
		xmlNodePtr pProp = xmlNewTextChild(cur, NULL,
										   (const xmlChar*)pUpdateProp->name.c_str(),
										   (const xmlChar*)pUpdateProp->value.c_str());
		for(StringMapConstIt itAttr=pUpdateProp->attributesMap.begin(); itAttr!=pUpdateProp->attributesMap.end(); ++itAttr)
		{
			//xmlAttrPtr pAttr =
			xmlNewProp(pProp, (const xmlChar*)(*itAttr).first.c_str(), (const xmlChar*)(*itAttr).second.c_str());
		}
	}
	
	if( pNode == NULL )
	{
		xmlSaveFormatFile(outFileName, doc, 1);
		xmlFreeDoc(doc);
	}
}

/*static*/ void UpdatesXML::ParseAttributes(xmlNode *pNode, map<string, string>& mapAttributes)
{
	if(NULL == pNode)
		return;

	xmlAttr *cur_attr = NULL;
	for(cur_attr = pNode->properties; NULL != cur_attr; cur_attr = cur_attr->next)
		mapAttributes[reinterpret_cast<const char *>(cur_attr->name)] =
			NULL != cur_attr->children && cur_attr->children->content ? 
				reinterpret_cast<const char *>(cur_attr->children->content) : "";
}

void UpdatesXML::xmlExample(const char * name)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr cur;
	
	// create new xml document with version 1.0 
	doc = xmlNewDoc((const xmlChar*)"1.0");
	// create a new root node  
	root = xmlNewDocNode(doc, NULL, (const xmlChar*)"lmce_update", NULL);
	xmlDocSetRootElement(doc, root);
	
	cur = xmlNewChild(root, NULL, (const xmlChar*)"update", NULL);
	char attr[256];
	for(int i=0; i<3; i++)
	{
		snprintf(attr, sizeof(attr), "attr%03d", i);
		xmlNewProp(cur, (const xmlChar*)attr, (const xmlChar*)attr);
	}
	for(int j=0; j<4; j++)
	{
		snprintf(attr, sizeof(attr), "prop%03d", j);
		xmlNodePtr pProp = xmlNewTextChild(cur, NULL, (const xmlChar*)attr, (const xmlChar*)attr);
		for(int i=0; i<3; i++)
		{
			snprintf(attr, sizeof(attr), "attr_%03d", i);
			xmlNewProp(pProp, (const xmlChar*)attr, (const xmlChar*)attr);
		}
	}
	
	xmlSaveFormatFile(name, doc, 1);
	xmlFreeDoc(doc);
}
