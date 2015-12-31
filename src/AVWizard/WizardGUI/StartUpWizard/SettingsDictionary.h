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
 *  That class implements one string->string dictionary, it is great for keep
 *	settings 
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef SettingsDictionary_H_
#define SettingsDictionary_H_

#include <map>
#include <string>

// Used for XML operations
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libxml/xmlreader.h>

class SettingsDictionary
{
public:
	std::map <std::string, std::string> Options;
	std::string Name, Type;

	SettingsDictionary();
	~SettingsDictionary();

	void Set(std::string OptionName, const char* Value);
	void Set(std::string OptionName, std::string Value);
	void Set(std::string OptionName, int Value);
	bool Exists(std::string OptionName);

	std::string GetValue(std::string OptionName);

	void SetName(std::string Name);
	void SetType(std::string Type);

	bool HasAttributes();

	std::string GetName();
	std::string GetType();

	void Serialize(xmlTextWriterPtr Writer);
	void Deserialize(xmlTextReaderPtr Reader);

};

#endif
