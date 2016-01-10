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
#ifndef __PLUTO_MEDIA_ATTRIBUTES_H__
#define __PLUTO_MEDIA_ATTRIBUTES_H__
//-----------------------------------------------------------------------------------------------------
#include "../SerializeClass/SerializeClass.h"
#define SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE	2100
#define SERIALIZE_DATA_TYPE_MULTIMAP_PICTURES		2101
#define SERIALIZE_DATA_TYPE_LIST_BOOKMARKS			2102
#define SERIALIZE_PLUTO_MEDIA_ATTRIBUTES_VERSION	1
#define SERIALIZE_PLUTO_MEDIA_BOOKMARKS_VERSION		1
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaAttribute class
//
//----------------------------------------------------------------------------------------------------
class PlutoMediaAttribute : public SerializeClass
{
public:

	int m_nPK_Attribute;
	int m_nType;
	string m_sName;
	int m_nTrack;
	int m_nSection;

	PlutoMediaAttribute();
	PlutoMediaAttribute(int nPK_Attribute,int nType, string sName, int nTrack = 0, int nSection = 0);
	~PlutoMediaAttribute();

	string SerializeClassClassName();
	void SetupSerialization(int iSC_Version);

	bool operator ==(PlutoMediaAttribute& attribute);
	PlutoMediaAttribute& operator =(PlutoMediaAttribute& attribute);
};
//-----------------------------------------------------------------------------------------------------
class PlutoMediaBookmark: public SerializeClass
{
public:

	string m_sDescription;
	string m_sPosition;
	PlutoDataBlock m_dataPicture;
	PlutoDataBlock m_dataPictureThumb;

	PlutoMediaBookmark();
	PlutoMediaBookmark(string sDescription, string sPosition);
	~PlutoMediaBookmark();

	string SerializeClassClassName();
	void SetupSerialization(int iSC_Version);

	bool operator ==(PlutoMediaBookmark& bookmark);
	PlutoMediaBookmark& operator =(PlutoMediaBookmark& bookmark);

	bool OkayToDeserialize(int iSC_Version);
};
//-----------------------------------------------------------------------------------------------------
typedef multimap<long, PlutoMediaAttribute *> MapPlutoMediaAttributes;
typedef multimap<unsigned long, char *> MapPictures;
typedef list<PlutoMediaBookmark *> ListBookmarks;
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaAttributes class
//
//-----------------------------------------------------------------------------------------------------
class PlutoMediaAttributes : public SerializeClass
{
public:

	int m_nInstallationID;
	int m_nFileID;
	int m_nPictureID;
	string m_sPictureUrl;
	string m_sStartPosition;
	MapPlutoMediaAttributes m_mapAttributes;
	MapPlutoMediaAttributes m_mapLongAttributes;
	MapPictures m_mapCoverarts;
	MapPictures m_mapCoverartsThumbs; 
	ListBookmarks m_listBookmarks;
	int m_nFileFormat;
	int m_nMediaSubType;

	PlutoMediaAttributes();
	~PlutoMediaAttributes();

	void ClearBookmarks();
	void ClearCoverarts();

	bool OkayToDeserialize(int iSC_Version);
	string SerializeClassClassName();
	void SetupSerialization(int iSC_Version);

	PlutoMediaAttributes &operator+ (MapPlutoMediaAttributes &i);
	PlutoMediaAttributes &operator+ (MapPictures &i);
	PlutoMediaAttributes &operator+ (ListBookmarks &i);

	bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,
		unsigned long &iAllocatedSize,char *&pCurrentPosition);

	void GenerateMd5SumsForCoverarts(list<string>& listMD5SumsCoverarts,
		bool bRemoveDuplicates = false);
	void DumpCoverarts();
};
//-----------------------------------------------------------------------------------------------------
#endif
