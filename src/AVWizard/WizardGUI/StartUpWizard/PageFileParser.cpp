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
//---------------------------------------------------------------------------
#include "PageFileParser.h"
//---------------------------------------------------------------------------
#include <iostream>
#include <string.h>
//---------------------------------------------------------------------------
#include "WizardWidgetsFactory.h"
#include "GUIWizardUtils.h"
//---------------------------------------------------------------------------
WizardWidgetBase* PageFileParser::ParseObjectLine(char* LineToParse, WizardWidgetBase* Parent)
{
	char WidgetName[1024], WidgetType[128];
	char* Buffer;

	Buffer = strtok(LineToParse, " ");
	Buffer = strtok(NULL, ":");

	strncpy(WidgetName, Buffer, 1024);

	Buffer = strtok(NULL, " ");

	strncpy(WidgetType, Buffer, 128);

	WizardWidgetsFactory* Factory = WizardWidgetsFactory::GetInstance();

	int WidgetTypeID = Factory->GetWidgetType(WidgetType);

	WizardWidgetBase* Result = Factory->
		CreateWidget(WidgetTypeID, WidgetName);
	if(Result)
		Result->SetParent(Parent);

	return Result;
}

bool PageFileParser::IgnoreCurrentLine(char* LineToParse)
{
	if(LineToParse == NULL)
		return true;
	while(LineToParse[0] == '\r')
		LineToParse++;
	if(strlen(LineToParse) == 0)
		return true;
	if(LineToParse[0] == '#')
		return true;
	return false;
}

void PageFileParser::ParseLine(char* LineToParse, WizardWidgetBase* & Control)
{
	if(strstr(LineToParse, "object") != NULL)
		{
			Control = ParseObjectLine(LineToParse, Control);
			return;
		}

	if(strstr(LineToParse, "end") != NULL)
	{
		if(Control && Control->Parent)
			Control = Control->Parent;
		return;
	}
    #ifdef DEBUG
	std::cout<<LineToParse<<std::endl;
    #endif
}

WizardWidgetBase* PageFileParser::LoadFromFile(char* FileName)
{
	//reads a line of maximum 1023 characters
	char Buffer[1024];

	std::ifstream FInput;
	FInput.open(FileName);
	if(!FInput)
		return NULL;

	WizardWidgetBase* Result = NULL;

	while(!FInput.eof())
	{
		FInput.getline(Buffer, 1024);
		if (!IgnoreCurrentLine(Buffer))
		{
			if(strstr(Buffer, "=") != NULL)
				ParseAttribute(FInput, Buffer, Result);
			else
				ParseLine(Buffer, Result);
		}
	}

	FInput.close();

	return Result;
}

void PageFileParser::ParseAttribute(std::ifstream &FInput, char* LineToParse, WizardWidgetBase* & Control)
{
    if(Control == NULL)
    {
#ifdef DEBUG
        std::cout<<"Warning, attribute defined without being on a control defined"<<std::endl;
        std::cout<<"Line: "<<LineToParse<<std::endl;
#endif
        return;
    }
	char AttributeName[128], AttributeValue[1024];
	char* Buffer;
	char LineToRead[256];

#ifdef DEBUG
    // defined for clean output
    char* pAttributeName = AttributeName;
    char* pAttributeValue = AttributeValue;
#endif

	Buffer = strtok(LineToParse, "=");
	strncpy(AttributeName, Buffer, 128);

	Buffer = Utils::AllTrim(AttributeName);
    strncpy(AttributeName, Buffer, 128);
	free(Buffer);
	Buffer = NULL;

	Buffer = strtok(NULL, "");
	Buffer = Utils::AllTrim(Buffer);
	strcpy(AttributeValue, Buffer);
    free(Buffer);
    Buffer = NULL;
	if (AttributeValue[0] == '(') 
	{
		do
		{
            if(Buffer)
            {
                free(Buffer);
                Buffer = NULL;
            }
			FInput.getline(LineToRead, 256);
			Buffer = Utils::AllTrim(LineToRead);
			strcat(AttributeValue, Buffer);

		}while(AttributeValue[strlen(AttributeValue)-1] != ')');
	}
    if(Buffer)
    {
        free(Buffer);
        Buffer = NULL;
    }                 

#ifdef DEBUG
    Control->SetUpAttribute(pAttributeName, pAttributeValue);
    std::cout<<AttributeName<<":"<<AttributeValue<<std::endl;

#else
    Control->SetUpAttribute(AttributeName, AttributeValue);
#endif
}

PageFileParser* PageFileParser::Instance = NULL;

/*static*/ PageFileParser* PageFileParser::GetInstance()
{
	if(Instance == NULL)
		Instance = new PageFileParser();
	return Instance;
}

void PageFileParser::CleanUp()
{
	if (Instance)
	{
		delete Instance;
		Instance = NULL;
	}
}
