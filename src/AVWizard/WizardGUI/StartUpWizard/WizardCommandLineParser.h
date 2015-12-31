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
/**
 *	Class that handle the command line parsing
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WizardCommandLineParser_H_
#define WizardCommandLineParser_H_

#include <map>
#include <string>
#include <vector>

#include "SettingsDictionary.h"

/**
 *	Argument type = --help
 */
#define ARGUMENT_GET_HELP	1
/**
 *	Argument type = -step
 */
#define ARGUMENT_STEP		2
/**
 *	Argument type = -set
 */
#define ARGUMENT_SET		3
/**
 *	Argument type = -get
 */
#define ARGUMENT_GET		4

/**
 *	Error mode: argument type not known
 */

#define ARGUMENT_ERROR_TYPE_NOT_FOUND 1

/**
 * Reason to quit
 */
#define QUIT_REASON_NONE	0
#define QUIT_REASON_GET		1
#define QUIT_REASON_OTHER	2


/**
 *	Singleton that parte the command line
 */
class WizardCommandLineParser
{

	//<ArgumentName, KindOfArgument> ArgumentTypes;
	std::map <std::string, int> ArgumentTypes;

	/**
	 *	The version of AV wizard
	 */
	std::string Version;

	/**
	 *	Offers basic help for user
	 */
	void PrintCommandLineHelp();

	/**
	 *	Singleton class = private constructor, to use that object use GetInstance() static method
	 */
	WizardCommandLineParser();

	/**
	 *	Instance to the current object
	 */
	static WizardCommandLineParser* Instance;

	/**
	 *	Get argument type
	 *	@return Function return 0 if is an unknown argument type
	 */
	int GetArgumentType(std::string ArgumentName);
	
public:
	/**
	 *	Default FileName where AVWizarg gets the config
	 */
	std::string ConfigFileDefault;

	std::string StartStep;

	/**
	 *	NeedQuit is true if it gets commands as Get/Set or --help
	 */
	int NeedQuit;

	/**
	 *	Parse the command line arguments
	 */
	void ParseArguments(std::vector<std::string> Arguments, SettingsDictionary& Options);

	/**
	 *	Default destructor	
	 */
	virtual ~WizardCommandLineParser(void);

	/**
	 *	Singleton based methods
	 */
	static WizardCommandLineParser* GetInstance();

	/**
	 *	CleanUp the command line resources
	 */
	void CleanUp();

	/**
	 *	Display coresponding message if cannot parse parameter
	 */
	void DisplayParseCommandError(std::string Argument, int ErrorMode);

	/**
	 *	Overloading function to create a vector with string from 
	 *	application's parameters
	 */
	std::vector<std::string> CommandLineSplit(std::string CommandLine);

	/**
	 *	Overloading function to create a vector with string from 
	 *	application's parameters
	 */
	std::vector<std::string> CommandLineSplit(int Argc, char** Args);
};

#endif
