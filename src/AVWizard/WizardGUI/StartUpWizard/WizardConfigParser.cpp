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
#include "WizardConfigParser.h"
//---------------------------------------------------------------------------
#include <stdlib.h>
//---------------------------------------------------------------------------
WizardConfigParser* WizardConfigParser::Instance = NULL;
//---------------------------------------------------------------------------
WizardConfigParser::WizardConfigParser(void)
{

}
//---------------------------------------------------------------------------
WizardConfigParser::~WizardConfigParser(void)
{

}
//---------------------------------------------------------------------------
WizardConfigParser* WizardConfigParser::GetInstance()
{
	if(Instance == NULL)
		Instance = new WizardConfigParser();
	return Instance;
}
//---------------------------------------------------------------------------
void WizardConfigParser::SetOption(std::string OptionName, 
												std::string OptionValue)
{

}

std::string WizardConfigParser::GetOption(std::string OptionName)
{
	return this->Options[OptionName];
}

void WizardConfigParser::CleanUpOptions()
{
	this->Options.clear();
}

void WizardConfigParser::Serialize(std::string XMLFile)
{

}

void WizardConfigParser::Deserialize(std::string XMLFile)
{

}
