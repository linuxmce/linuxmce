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
 *  That class implements one hierarchy of dictionaries
 *
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef SettingsDictionaryTree_H_
#define SettingsDictionaryTree_H_

#include "SettingsDictionary.h"
#include <vector>

// Used for XML operations
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libxml/xmlreader.h>

/**
 *	
 */
class SettingsDictionaryTree
{

	SettingsDictionaryTree* Parent;

	bool ProcessNode(xmlTextReaderPtr Reader, int ParseDepth);

	void BuildPropertiesInfo(xmlTextWriterPtr Writer);

	SettingsDictionary* Dictionary;

	bool Deserialize(xmlTextReaderPtr Reader, int ParseDepth);
public:
	std::vector<SettingsDictionaryTree*> Children;

	SettingsDictionaryTree(void);
	virtual ~SettingsDictionaryTree(void);

	/**
	 *	Convert a specified input using a specified encoding
	 */
	char* ConvertInput(const char *in, const char *encoding);
	
	/**
	 *	Deserialize XML content to a file
	 */
	bool LoadFromXMLFile(std::string FileName);

	/**
	 *	Serialize the dictionary content to a XML file
	 */
	bool SaveToXMLFile(std::string FileName);

	/**
	 *	Add a child dictionary to the 
	 */
	void AddChild(SettingsDictionaryTree* Child);

	/**
	 *	Child to be removed
	 */
	void RemoveChild(SettingsDictionaryTree* Child);
	/**
	 *	Deletes all childrens. 
	 *	Warning note, RemoveChild does not delete them, only unrefers them
	 */
	void RemoveChildren();

	/**
	 *	Geter for attribute Dictionary 
	 */
	SettingsDictionary* GetDictionary();
	/**
	 *	Getter that gets the name of dictionary from current childrens. 
	 *  It search only one level. For getting recursive childrens, you shoud use
	 *  GetChildRecursive
	 */
	SettingsDictionaryTree* GetChild(std::string Child);
	/**
	 *	Getter that gets the name of dictionary from current childrens. 
	 *  It search based on DFS. 
	 */
	SettingsDictionaryTree* GetChildRecursive(std::string Child);
};

#endif
