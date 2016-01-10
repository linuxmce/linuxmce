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
#ifndef OrbiterFileBrowser_h
#define OrbiterFileBrowser_h

// Here is a collection of OrbiterFileBrowser_Entry objects which the orbiter
// uses in response to the 'Show File List' command so it knows what screen
// to present the user to pick a file based on a given media type

#include "SerializeClass/SerializeClass.h"
#define SERIALIZE_DATA_TYPE_MAP_OBJFILEBROWSER		1000
#define SERIALIZE_OBJECT_FILE_BROWSER_VERSION		2

class OrbiterFileBrowser_Entry : public SerializeClass
{
public:
	int m_PK_Screen,m_MediaType;
	string m_sFilename;

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_PK_Screen + m_MediaType + m_sFilename;
	}
};

// int=MediaType
typedef map<int,OrbiterFileBrowser_Entry *> MapOrbiterFileBrowser;

class OrbiterFileBrowser_Collection : public SerializeClass
{
public:
	MapOrbiterFileBrowser m_mapOrbiterFileBrowser;
	map<string, pair<int,int> > m_mapRemoteControls;
	map<int,int> m_mapRemoteID_Device;
	map<int,string> m_mapUsernames;

	int m_mapRemoteID_Device_Find(int RemoteID)	{ map<int,int>::iterator it = m_mapRemoteID_Device.find(RemoteID); return it==m_mapRemoteID_Device.end() ? 0 : (*it).second; }
	pair<int,int> m_mapRemoteControls_Find(string sSerialNumber) { map<string, pair<int,int> >::iterator it = m_mapRemoteControls.find(sSerialNumber); return it==m_mapRemoteControls.end() ? make_pair<int,int> (0,0) : (*it).second; }
	string m_mapUsernames_Find(int PK_Users)	{ map<int,string>::iterator it = m_mapUsernames.find(PK_Users); return it==m_mapUsernames.end() ? "" : (*it).second; }


	OrbiterFileBrowser_Collection()
	{
		m_iSC_Version = SERIALIZE_OBJECT_FILE_BROWSER_VERSION;
	}

	~OrbiterFileBrowser_Collection()
	{
		for(MapOrbiterFileBrowser::iterator it=m_mapOrbiterFileBrowser.begin();it!=m_mapOrbiterFileBrowser.end();++it)
			delete it->second;
	}

	OrbiterFileBrowser_Collection &operator+ (MapOrbiterFileBrowser &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MAP_OBJFILEBROWSER,(void *) &i)); return (*this); }
	virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition);

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_mapRemoteControls + m_mapUsernames;
		(*this) + m_mapOrbiterFileBrowser; // this is serialized custom
	}
	bool OkayToDeserialize(int iSC_Version)
	{
		return SERIALIZE_OBJECT_FILE_BROWSER_VERSION == iSC_Version;
	}
};


#endif
