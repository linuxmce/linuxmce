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

#ifndef WIZARD_SETTINGS_H_
#define WIZARD_SETTINGS_H_

#ifdef WIN32
#define WIZARD_SETTINGS_FILE_NAME "OrbiterSettings.txt"
#else 
#define WIZARD_SETTINGS_FILE_NAME "OrbiterSettings.xml"
#endif

#define WIZARD_SETTINGS_XML_ENCODING "UTF-8"

#include <libxml/xmlreader.h>

#include "SettingsDictionary.h"

class WizardSettings
{
	/**
	 *	Screen sizes
	 */
	int Width, Height, Refresh;

	/**
	 *	Volume data
	 */
	int MasterVolumeLow, MasterVolumeHigh, MasterVolumeValue;

	/**
	 *		
	 */
	SettingsDictionary Dictionary;

	void ProcessNode(xmlTextReaderPtr Reader);

public:
	/**
	 *	Creates the option defaults
	 */
	WizardSettings();

	/**
	 *	Getters
	 */
	int GetWidth();
	int GetHeight();
	int GetRefresh();
	int GetMasterVolumeLow();
	int GetMasterVolumeHigh();
	int GetMasterVolumeValue();

	/**
	 *	Setters
	 */
	void SetWidth(int Value);
	void SetHeight(int Value);
	void SetRefresh(int Value);
	void SetMasterVolumeLow(int Value);
	void SetMasterVolumeHigh(int Value);
	void SetMasterVolumeValue(int Value);

	/**
	 *	Save/restore the settings using a specified XML file
	 */
	bool SaveSettingsToXMLFile(char* FileName);
	bool LoadSettingsFromXMLFile(char* FileName);

	void SetUpDefaults();

};

/**
 *	That function will convert one input using specified encoding
 *	xmlChar * ConvertInput(const char *in, const char *encoding);
 */

#endif
