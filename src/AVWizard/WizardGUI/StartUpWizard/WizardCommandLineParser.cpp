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
#include "WizardCommandLineParser.h"

#include <cstring>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

#include "GUIWizardUtils.h"

WizardCommandLineParser* WizardCommandLineParser::Instance = NULL;

WizardCommandLineParser* WizardCommandLineParser::GetInstance()
{
	if(Instance == NULL)
		Instance = new WizardCommandLineParser();

	return Instance;
}

WizardCommandLineParser::WizardCommandLineParser(void)
{
	NeedQuit = QUIT_REASON_NONE;
	this->Version = "0.01 Alpha";
	this->ConfigFileDefault = "/tmp/avwizardsettings.xml";
	ArgumentTypes["--help"] = ARGUMENT_GET_HELP;
	ArgumentTypes["-step"] = ARGUMENT_STEP;
	ArgumentTypes["-set"] = ARGUMENT_SET;
	ArgumentTypes["-get"] = ARGUMENT_GET;
	this->StartStep = "Welcome";
}

WizardCommandLineParser::~WizardCommandLineParser(void)
{
}

void WizardCommandLineParser::ParseArguments(std::vector<std::string> Arguments, 
											 SettingsDictionary& Options)
{
	for(unsigned int i = 1; i<Arguments.size(); i++)
	{
		std::string Argument = Arguments[i];
		int KindOfArgument = GetArgumentType(Argument);
		
		switch(KindOfArgument) {
		case ARGUMENT_GET_HELP:
			PrintCommandLineHelp();
			NeedQuit = QUIT_REASON_OTHER;
			break;
		case ARGUMENT_GET:
			std::cout<<Options.GetValue(Arguments[i+1])<<std::endl;
			NeedQuit = QUIT_REASON_GET;
			i++;
			break;
		case ARGUMENT_SET:
			Options.Set(Arguments[i+1], Arguments[i+2]);
			NeedQuit = QUIT_REASON_OTHER;
			i+= 2;
			break;
		case ARGUMENT_STEP:
			this->StartStep = Arguments[i+1];
			i++;
			break;
		default:
			DisplayParseCommandError(Arguments[i], ARGUMENT_ERROR_TYPE_NOT_FOUND);
		}
	}
}

void WizardCommandLineParser::PrintCommandLineHelp()
{
	std::cout <<"A(udio)V(ideo) Wizard  v."<<Version<<" "<<std::endl;
	std::cout<<" Command line usage:"<<std::endl;
	std::cout<<" --help\t\t That screen of help";
	std::cout<<" -screen <screenname>\t Display from wizard only one screen";
	std::cout<<" That screen may be: "<<std::endl;
	std::cout<<" (+) VideoOutputChoose"<<std::endl;
	std::cout<<" (+) VideoResolution"<<std::endl;
	std::cout<<" (+) AudioVolumeSetup"<<std::endl;
	std::cout<<" (+) DolbyCheck"<<std::endl;
	std::cout<<" -set <screenname>\t Which set different components without"<<std::endl;
	std::cout<<"showing the wizard screen for that."<<std::endl;
	std::cout<<"chosed screens will not be displayed. If you want to display it"<<std::endl;
	std::cout<<"you must use too the -screen command"<<std::endl;
	std::cout<<"(overwrites the defaults)"<<std::endl;
	std::cout<<" -get <screenname>\t Which get different components without"<<std::endl;
	std::cout<<"showing the wizard screen for that. The wizard will exit at that moment."<<std::endl;
	std::cout<<"(gets the defaults)"<<std::endl;
	std::cout<<" -config <configname>\t Uses a special config file"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"Note: in case that AVWizard is launched with no parameter"<<std::endl;
	std::cout<<"it will check after the config file <"<<ConfigFileDefault<<"> "<<std::endl;
}

void WizardCommandLineParser::CleanUp()
{
	if(Instance != NULL)
	{
		delete Instance;
		Instance = NULL;
	}
}

int WizardCommandLineParser::GetArgumentType(std::string ArgumentName)
{
	return ArgumentTypes[ArgumentName];
}

void WizardCommandLineParser::DisplayParseCommandError(std::string Argument, int ErrorMode)
{
	switch(ErrorMode) {
	case ARGUMENT_ERROR_TYPE_NOT_FOUND:
		std::cout<<"Unknown type of argument parsing argument: "<<Argument<<std::endl;
		break;
	default:
		std::cout<<"Warning: unknown type of error"<<std::endl;
	}
}


std::vector<std::string> WizardCommandLineParser::CommandLineSplit(std::string CommandLine)
{
	std::vector<std::string> Arguments;
	char ApplicationName[1024];

#ifdef WIN32
	GetModuleFileName(GetModuleHandle(NULL), ApplicationName, 1024);
#else
	strcpy(ApplicationName, "/usr/bin/AVWizard");
#endif

	Arguments.push_back(ApplicationName);

	char* Buffer = strtok((char*)CommandLine.c_str(), " ");
	while(Buffer != NULL)
	{
		Arguments.push_back(Buffer);
		Buffer = strtok(NULL, " ");
	}
	return Arguments;
}

std::vector<std::string> WizardCommandLineParser::CommandLineSplit(int Argc, char** Argv)
{
	std::vector<std::string> Arguments;
	for(int i = 0; i<Argc; i++)
		Arguments.push_back(Argv[i]);

	return Arguments;
}
