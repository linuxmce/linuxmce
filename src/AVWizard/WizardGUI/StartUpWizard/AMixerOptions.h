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
 *  That class stores the 'amixer' command settings
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef AMIXER_OPTIONS_H
#define AMIXER_OPTIONS_H

#include <vector>

#include "SettingsDictionary.h"
/**
 *	Singleton class which keeps the options of AMixer command
 *	based on Dictionary
 */
class AMixerOptions
{
	/**
	 *	Keept the mixer options	
	 */
	SettingsDictionary Options;

	static AMixerOptions* Instance;
public:
	AMixerOptions();

	static AMixerOptions* GetInstance();

	/**
	 *	Cleanup resources attached with singleton class	
	 */
	void CleanUp();
	
	/**
	 *	Virtual destructor	
	 */
	virtual ~AMixerOptions(void);

	void AddOption(char* OptionName);

	bool Exists(char* OptionName);

};

#endif
