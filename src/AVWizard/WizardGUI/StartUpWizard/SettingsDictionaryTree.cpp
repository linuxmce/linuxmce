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
#include "SettingsDictionaryTree.h"
#include <cstring>

#define WIZARD_SETTINGS_XML_ENCODING "UTF8" 

SettingsDictionaryTree::SettingsDictionaryTree(void)
{
	Parent = NULL;
	Dictionary = new SettingsDictionary();
}

SettingsDictionaryTree::~SettingsDictionaryTree(void)
{
	delete Dictionary;
	RemoveChildren();
	Dictionary = NULL;
}

/**
* ConvertInput:
* @in: string in a given encoding
* @encoding: the encoding used
*
* Converts @in into UTF-8 for processing with libxml2 APIs
*
* Returns the converted UTF-8 string, or NULL in case of error.
*/
char*
SettingsDictionaryTree::ConvertInput(const char *in, const char *encoding)
{
	xmlChar *out;
	int ret;
	int size;
	int out_size;
	int temp;
	xmlCharEncodingHandlerPtr handler;

	if (in == 0)
		return 0;

	handler = xmlFindCharEncodingHandler(encoding);

	if (!handler) {
		printf("ConvertInput: no encoding handler found for '%s'\n",
			encoding ? encoding : "");
		return 0;
	}

	size = (int) strlen(in) + 1;
	out_size = size * 2 - 1;
	out = (unsigned char *) xmlMalloc((size_t) out_size);

	if (out != 0) {
		temp = size - 1;
		ret = handler->input(out, &out_size, (const xmlChar *) in, &temp);
		if ((ret < 0) || (temp - size + 1)) {
			if (ret < 0) {
				printf("ConvertInput: conversion wasn't successful.\n");
			} else {
				printf
					("ConvertInput: conversion wasn't successful. converted: %i octets.\n",
					temp);
			}

			xmlFree(out);
			out = 0;
		} else {
			out = (unsigned char *) xmlRealloc(out, out_size + 1);
			out[out_size] = 0;  /*null terminating out */
		}
	} else {
		printf("ConvertInput: no mem\n");
	}

	return (char*)out;
}

bool SettingsDictionaryTree::SaveToXMLFile(std::string FileName)
{
	int Result;
	xmlTextWriterPtr Writer;

	/* Create a new XmlWriter for FileName, with no compression. */
	Writer = xmlNewTextWriterFilename(FileName.c_str(), 0);

	if (Writer == NULL) 
	{
		printf("testXmlwriterFilename: Error creating the xml writer\n");
		return false;
	}

	Result = xmlTextWriterStartDocument(Writer, NULL, WIZARD_SETTINGS_XML_ENCODING, NULL);
	if (Result < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterStartDocument\n");
		return false;
	}

	BuildPropertiesInfo(Writer);

	Result = xmlTextWriterEndDocument(Writer);
	if (Result < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterEndDocument\n");
		return false;
	}

	xmlFreeTextWriter(Writer);

	return true;
}

void SettingsDictionaryTree::BuildPropertiesInfo(xmlTextWriterPtr Writer)
{
	int Result;
	std::string Text;

	/* Start an element named "Object". Since thist is the first
	* element, this will be the root element of the document. */
	Result = xmlTextWriterStartElement(Writer, BAD_CAST "Object");
	if (Result < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
		return;
	}

	Dictionary->Serialize(Writer);


	std::vector<SettingsDictionaryTree*>::iterator Item;
	for(Item = Children.begin(); Item != Children.end(); ++Item)
	{
		(*Item)->BuildPropertiesInfo(Writer);
	}

	xmlTextWriterEndElement(Writer);
}

void SettingsDictionaryTree::AddChild(SettingsDictionaryTree* Child)
{
	Children.push_back(Child);
	Child->Parent = this;
}

void SettingsDictionaryTree::RemoveChild(SettingsDictionaryTree* Child)
{
	std::vector<SettingsDictionaryTree*>::iterator Item;
	for(Item = Children.begin(); Item != Children.end(); )
	{
		if (*Item == Child)
			Item = Children.erase(Item);
		else
			Item++;
	}
}

bool SettingsDictionaryTree::Deserialize(xmlTextReaderPtr Reader, int ParseDepth)
{
	int Result;
	Result = xmlTextReaderRead(Reader);
	while (Result == 1) {
		if (!ProcessNode(Reader, ParseDepth))
			return true;
		Result = xmlTextReaderRead(Reader);
	}
	if (Result != 0) 
		return false;
	return true;
}

bool SettingsDictionaryTree::LoadFromXMLFile(std::string FileName)
{
	xmlTextReaderPtr Reader;

	Reader = xmlReaderForFile(FileName.c_str(), NULL, 0);
	if (Reader != NULL) 
	{
		if(!Deserialize(Reader, 1))
		{
			fprintf(stderr, "Unable to parse %s\n", FileName.c_str());
			return false;
		}
		xmlFreeTextReader(Reader);
	}
	else
	{
		fprintf(stderr, "Unable to open %s\n", FileName.c_str());
		return false;
	}

	/*
	* Cleanup function for the XML library.
	*/
	xmlCleanupParser();

	/*
	* this is to debug memory for regression tests
	*/
	xmlMemoryDump();

	return true;
}

bool SettingsDictionaryTree::ProcessNode(xmlTextReaderPtr Reader, int ParseDepth)
{
	char* pText;
	int NodeType, Depth;
	std::string StringName, StringValue; 

	pText = (char*)xmlTextReaderConstName(Reader);
	if(pText == NULL)
		pText = "";

	StringName = pText;


	NodeType = xmlTextReaderNodeType(Reader);
	Depth = xmlTextReaderDepth(Reader);

	pText = (char*)xmlTextReaderConstValue(Reader);
	if(pText == NULL)
		pText = "";

	StringValue = pText;
	
	if ((NodeType == 1) && (StringName == "Type"))
	{
		xmlTextReaderRead(Reader);

		pText = (char*)xmlTextReaderConstValue(Reader);
		if(pText == NULL)
			pText = "";

		if(pText != NULL)
			Dictionary->SetType(pText);
		else
			Dictionary->SetType("");
	}

	if ((NodeType == 1) &&(StringName == "Name"))
	{
		xmlTextReaderRead(Reader);

		pText = (char*)xmlTextReaderConstValue(Reader);
		if(pText != NULL)
			Dictionary->SetName(pText);
		else
			Dictionary->SetName("");
	}

	if ((NodeType == 1) &&(StringName == "Attributes"))
	{
		bool FinishReadAttributes = false;
		while (!FinishReadAttributes)
		{
			xmlTextReaderRead(Reader);
			NodeType = xmlTextReaderNodeType(Reader);
			StringName = (char*)xmlTextReaderConstName(Reader);
			FinishReadAttributes = (NodeType == 15) || (StringName == "Object");

			if (!FinishReadAttributes)
			{
				xmlTextReaderRead(Reader);
				pText = (char*)xmlTextReaderConstValue(Reader);

				Dictionary->Set(StringName, pText);
				xmlTextReaderRead(Reader);
			}
		}
	}

	if(StringName == "Object")
	{
		if(NodeType == 15)
			return false;

		if ((NodeType == 1) && (0 != Depth))
		{
			SettingsDictionaryTree* DictionaryTree = new SettingsDictionaryTree();

			AddChild(DictionaryTree);
			DictionaryTree->Deserialize(Reader, ParseDepth+1);
		}
	}
	return true;
}

void SettingsDictionaryTree::RemoveChildren()
{
	std::vector<SettingsDictionaryTree*>::iterator Item;
	for(Item = Children.begin(); Item != Children.end(); ++Item)
        delete (*Item);
	Children.clear();
}

SettingsDictionary* SettingsDictionaryTree::GetDictionary()
{
	return this->Dictionary;
}

SettingsDictionaryTree* SettingsDictionaryTree::GetChild(std::string ChildName)
{
	std::vector<SettingsDictionaryTree*>::iterator Item;
	for(Item = Children.begin(); Item != Children.end(); ++Item)
		if((*Item)->GetDictionary()->GetName() == ChildName)
			return *Item;
	return NULL;
}

SettingsDictionaryTree* SettingsDictionaryTree::GetChildRecursive(std::string ChildName)
{
	SettingsDictionaryTree* Result = GetChild(ChildName);
	if(Result != NULL)
		return Result;

	std::vector<SettingsDictionaryTree*>::iterator Item;
	for(Item = Children.begin(); Item != Children.end(); ++Item)
	{
		Result = (*Item)->GetChildRecursive(ChildName);
		if (Result != NULL)
			return Result;
	}

	return NULL;
}
