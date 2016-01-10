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
#include "WizardSettings.h"

// Used for XML operations
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <string.h>
/**
* ConvertInput:
* @in: string in a given encoding
* @encoding: the encoding used
*
* Converts @in into UTF-8 for processing with libxml2 APIs
*
* Returns the converted UTF-8 string, or NULL in case of error.
*/
xmlChar *
ConvertInput(const char *in, const char *encoding)
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

	return out;
}

WizardSettings::WizardSettings()
{
	SetUpDefaults();
}

int WizardSettings::GetWidth()
{
	return Width;
}

int WizardSettings::GetHeight()
{
	return Height;
}

int WizardSettings::GetRefresh()
{
	return Refresh;
}

int WizardSettings::GetMasterVolumeLow()
{
    return this->MasterVolumeLow;
}

int WizardSettings::GetMasterVolumeHigh()
{
    return this->MasterVolumeHigh;
}

int WizardSettings::GetMasterVolumeValue()
{
	return this->MasterVolumeValue;
}

void WizardSettings::SetWidth(int Value)
{
	this->Width = Value;
}

void WizardSettings::SetHeight(int Value)
{
	this->Height = Value;
}

void WizardSettings::SetRefresh(int Value)
{
	this->Refresh = Value;
}

void WizardSettings::SetMasterVolumeLow(int Value)
{
	this->MasterVolumeLow = Value;
}

void WizardSettings::SetMasterVolumeHigh(int Value)
{
    this->MasterVolumeHigh = Value;
}

void WizardSettings::SetMasterVolumeValue(int Value)
{
	this->MasterVolumeValue = Value;	
}

bool WizardSettings::SaveSettingsToXMLFile(char* FileName)
{
	int rc;
	xmlTextWriterPtr writer;
	xmlChar *tmp;

	/* Create a new XmlWriter for FileName, with no compression. */
	writer = xmlNewTextWriterFilename(FileName, 0);

	if (writer == NULL) 
	{
		printf("testXmlwriterFilename: Error creating the xml writer\n");
		return false;
	}

	/* Start the document with the xml default for the version,
	* encoding ISO 8859-1 and the default for the standalone
	* declaration. */
	rc = xmlTextWriterStartDocument(writer, NULL, WIZARD_SETTINGS_XML_ENCODING, NULL);
	if (rc < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterStartDocument\n");
		return false;
	}

	/* Start an element named "settings". Since thist is the first
	* element, this will be the root element of the document. */
	rc = xmlTextWriterStartElement(writer, BAD_CAST "settings");
	if (rc < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
		return false;
	}

	/* Write a comment as child of EXAMPLE.
	* Please observe, that the input to the xmlTextWriter functions
	* HAS to be in UTF-8, even if the output XML is encoded
	* in iso-8859-1 */
	tmp = ConvertInput("This is a comment",
		WIZARD_SETTINGS_XML_ENCODING);


	rc = xmlTextWriterWriteComment(writer, tmp);
	if (rc < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterWriteComment\n");
		return false;
	}
	if (tmp != NULL) 
		xmlFree(tmp);

	/* Write an element named "TEXT" as child of FOOTER. */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "Title",
		BAD_CAST "Orbiter Configuration Wizard 0.1 beta");
	if (rc < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
		return false;
	}
	/* Start an element named "Options". Since thist is the first
	* element, this will be the root element of the document. */
	rc = xmlTextWriterStartElement(writer, BAD_CAST "Options");
	if (rc < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
		return false;
	}

	rc = xmlTextWriterStartElement(writer, BAD_CAST "Resolution");
	if (rc < 0) {
		printf("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
		return false;
	}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Width",
		"%d", GetWidth());

	if (rc < 0) {
		printf("xmlTextWriterWriteFormatElement: Error at write Width\n");
		return false;
	}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Height",
		"%d", GetHeight());

	if (rc < 0) {
		printf("xmlTextWriterWriteFormatElement: Error at write Height\n");
		return false;
	}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Refresh",
		"%d", GetRefresh());

	if (rc < 0) {
		printf("xmlTextWriterWriteFormatElement: Error at write Height\n");
		return false;
	}

	/* Close the element named "Resolution". */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {
		printf("testXmlwriterFilename: Error at xmlTextWriterEndElement\n");
		return false;
	}

	rc = xmlTextWriterStartElement(writer, BAD_CAST "MasterVolume");
	if (rc < 0) {
		printf("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
		return false;
	}

	
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "LowerBound",
		"%d", this->GetMasterVolumeLow());

	if (rc < 0) {
		printf("xmlTextWriterWriteFormatElement: Error at write LowerBound\n");
		return false;
	}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "UpperBound",
		"%d", this->GetMasterVolumeHigh());

	if (rc < 0) {
		printf("xmlTextWriterWriteFormatElement: Error at write UpperBound\n");
		return false;
	}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Value",
		"%d", this->GetMasterVolumeValue());

	if (rc < 0) {
		printf("xmlTextWriterWriteFormatElement: Error at write Value\n");
		return false;
	}

	/* Close the element named "Resolution". */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {
		printf("testXmlwriterFilename: Error at xmlTextWriterEndElement\n");
		return false;
	}

	/* Close the element named "Options". */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterEndElement\n");
		return false;
	}

    /* Close the element named "settings". */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterEndElement\n");
		return false;
	}


	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterEndDocument\n");
		return false;
	}

	xmlFreeTextWriter(writer);

	return true;
}

bool WizardSettings::LoadSettingsFromXMLFile(char* FileName)
{

	xmlTextReaderPtr Reader;
	int ret;

	Reader = xmlReaderForFile(FileName, NULL, 0);
	if (Reader != NULL) {
		ret = xmlTextReaderRead(Reader);
		while (ret == 1) {
			ProcessNode(Reader);
			ret = xmlTextReaderRead(Reader);
		}
		xmlFreeTextReader(Reader);
		if (ret != 0) {
			fprintf(stderr, "%s : failed to parse\n", FileName);
			return false;
		}
	} else {
		fprintf(stderr, "Unable to open %s\n", FileName);
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

void WizardSettings::ProcessNode(xmlTextReaderPtr Reader)
{
	const xmlChar *Name, *Value;

	Name = xmlTextReaderConstName(Reader);
	if (Name == NULL)
		Name = BAD_CAST "--";

	std::string StringName = (char*)Name;
	if (!Dictionary.Exists(StringName))
		return;

	int ret = xmlTextReaderRead(Reader);
	if (ret != 1)
		return;

	Value = xmlTextReaderConstValue(Reader);

	int NodeType, Depth, IsEmptyElement, HasValue;

	NodeType = xmlTextReaderDepth(Reader);
	Depth = xmlTextReaderNodeType(Reader);
	IsEmptyElement = xmlTextReaderIsEmptyElement(Reader);
	HasValue = xmlTextReaderHasValue(Reader);

	printf("%d %d %s %d %d", 
		NodeType,
		Depth,
		Name,
		IsEmptyElement,
		HasValue);


	if (Value == NULL)
		printf("\n");
	else {
		if (xmlStrlen(Value) > 40)
			printf(" %.40s...\n", Value);
		else
			printf(" %s\n", Value);

	}
}

void WizardSettings::SetUpDefaults()
{
	this->Width = 640;
	this->Height = 480;
	this->Refresh = 0;
	this->MasterVolumeLow = 0;
	this->MasterVolumeHigh = 0;
	this->MasterVolumeValue = 0;
}
