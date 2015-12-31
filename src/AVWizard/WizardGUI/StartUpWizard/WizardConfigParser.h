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
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef WizardConfigParser_H_
#define WizardConfigParser_H_

#include <string>
#include <map>
/**
 *	Singleton class that get configuration to
 *  build the wizard.
 */

class WizardConfigParser
{
	/**
	 *	Singleton class = private constructor	
	 */
	WizardConfigParser();
	static WizardConfigParser* Instance;

	/**
	 *	AV Wizard XML saved options
	 */
	std::map <std::string, std::string> Options;

public:
	virtual ~WizardConfigParser(void);

	/**
	 *	Get singleton's instance
	 */
	WizardConfigParser* GetInstance();

	/**
	 *	Set 
	 */
	void SetOption(std::string OptionName, std::string OptionValue);
	std::string GetOption(std::string OptionName);

	/**
	 *	Cleanup stored options from memory
	 */
	void CleanUpOptions();

	/**
	 *	Serialize = store current object in a specified XML file
	 */
	void Serialize(std::string XMLFile);

	/**
	 *	Deserialize = recover current object from a specified XML file
	 */
	void Deserialize(std::string XMLFile);
};

#endif
