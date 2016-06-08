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
#include "PlutoMediaAttributes.h"

#include <algorithm>
using namespace std;

#include "Logger.h"
using namespace DCE;

//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaAttribute class
//
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttribute::PlutoMediaAttribute() : 
	SerializeClass(), 
	m_nPK_Attribute(0), m_nType(0), m_nTrack(0), m_nSection(0)
{
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttribute::PlutoMediaAttribute(int nPK_Attribute,int nType, string sName, int nTrack/* = 0*/, int nSection/* = 0*/) :
	SerializeClass(), 
	m_nPK_Attribute(nPK_Attribute), m_nType(nType), m_sName(sName), m_nTrack(nTrack), m_nSection(nSection)
{

}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttribute::~PlutoMediaAttribute()
{
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaAttribute::SerializeClassClassName() 
{ 
	return "PlutoMediaAttribute"; 
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaAttribute::SetupSerialization(int /*iSC_Version*/)
{
	StartSerializeList() + m_nType + m_sName + m_nTrack + m_nSection;
}
//-----------------------------------------------------------------------------------------------------
bool PlutoMediaAttribute::operator ==(PlutoMediaAttribute& attribute)
{
	return 
		attribute.m_nSection	== m_nSection	&&
		attribute.m_nTrack		== m_nTrack		&&
		attribute.m_nType		== m_nType		&&
		attribute.m_sName		== m_sName;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttribute& PlutoMediaAttribute::operator =(PlutoMediaAttribute& attribute)
{
	m_nSection = attribute.m_nSection;
	m_nTrack = attribute.m_nTrack;
	m_nType = attribute.m_nType;
	m_sName = attribute.m_sName;

	return *this;
}
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaBookmark class
//
//-----------------------------------------------------------------------------------------------------
PlutoMediaBookmark::PlutoMediaBookmark() : SerializeClass()
{
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaBookmark::PlutoMediaBookmark(string sDescription, string sPosition) :
	SerializeClass(), m_sDescription(sDescription), m_sPosition(sPosition)
{
	m_iSC_Version = SERIALIZE_PLUTO_MEDIA_BOOKMARKS_VERSION;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaBookmark::~PlutoMediaBookmark()
{
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaBookmark::SerializeClassClassName() 
{ 
	return "PlutoMediaBookmark"; 
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaBookmark::SetupSerialization(int /*iSC_Version*/)
{
	StartSerializeList() + m_sDescription + m_sPosition + m_dataPicture + m_dataPictureThumb;
}
//-----------------------------------------------------------------------------------------------------
bool PlutoMediaBookmark::operator ==(PlutoMediaBookmark& bookmark)
{
	return m_sDescription == bookmark.m_sDescription && m_sPosition == bookmark.m_sPosition;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaBookmark& PlutoMediaBookmark::operator =(PlutoMediaBookmark& bookmark)
{
	m_sDescription = bookmark.m_sDescription;
	m_sPosition = bookmark.m_sPosition;
	m_dataPicture = bookmark.m_dataPicture;
	m_dataPictureThumb = bookmark.m_dataPictureThumb;

	return *this;
}
//-----------------------------------------------------------------------------------------------------
bool PlutoMediaBookmark::OkayToDeserialize(int iSC_Version)
{
	return SERIALIZE_PLUTO_MEDIA_BOOKMARKS_VERSION == iSC_Version;
}
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaAttributes class
//
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes::PlutoMediaAttributes() : 
	SerializeClass(), 
	m_nInstallationID(0), m_nFileID(0), m_nPictureID(0),
	m_nFileFormat(0), m_nMediaSubType(0)
{
	m_iSC_Version = SERIALIZE_PLUTO_MEDIA_ATTRIBUTES_VERSION;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes::~PlutoMediaAttributes()
{
	for(MapPlutoMediaAttributes::iterator it = m_mapAttributes.begin(), end = m_mapAttributes.end(); it != end; ++it)
		delete it->second;
	for(MapPlutoMediaAttributes::iterator it = m_mapLongAttributes.begin(), end = m_mapLongAttributes.end(); it != end; ++it)
		delete it->second;

	//coverarts
	ClearCoverarts();
	
	//bookmarks
	ClearBookmarks();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaAttributes::ClearBookmarks()
{
	//bookmarks
	for(ListBookmarks::iterator itb = m_listBookmarks.begin(), endb = m_listBookmarks.end(); itb != endb; ++itb)
		delete *itb;
	
	m_listBookmarks.clear();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaAttributes::ClearCoverarts()
{
	//coverarts
	for(MapPictures::iterator itp = m_mapCoverarts.begin(), endp = m_mapCoverarts.end(); itp != endp; ++itp)
		delete [] itp->second;
	for(MapPictures::iterator itp = m_mapCoverartsThumbs.begin(), endp = m_mapCoverartsThumbs.end(); itp != endp; ++itp)
		delete [] itp->second;

	m_mapCoverarts.clear();
	m_mapCoverartsThumbs.clear();
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaAttributes::SerializeClassClassName() 
{ 
	return "PlutoMediaAttributes"; 
}
//-----------------------------------------------------------------------------------------------------
bool PlutoMediaAttributes::OkayToDeserialize(int iSC_Version)
{
	return SERIALIZE_PLUTO_MEDIA_ATTRIBUTES_VERSION == iSC_Version;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaAttributes::SetupSerialization(int /*iSC_Version*/)
{
	StartSerializeList() + m_nInstallationID + m_nFileID + m_nPictureID + m_sPictureUrl;
	
	(*this) + m_mapAttributes;
	(*(static_cast<SerializeClass *>(this))) + m_sStartPosition;
	(*this) + m_mapLongAttributes;

	//coverarts
	(*this) + m_mapCoverarts;
	(*this) + m_mapCoverartsThumbs;
	
	//bookmarks
	(*this) + m_listBookmarks;

	//misc
	(*(static_cast<SerializeClass *>(this))) + m_nFileFormat;
	(*(static_cast<SerializeClass *>(this))) + m_nMediaSubType;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes &PlutoMediaAttributes::operator+ (MapPlutoMediaAttributes &i) 
{ 
	m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE,(void *) &i)); 
	return (*this); 
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes &PlutoMediaAttributes::operator+ (MapPictures &i) 
{ 
	m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MULTIMAP_PICTURES,(void *) &i)); 
	return (*this); 
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes &PlutoMediaAttributes::operator+ (ListBookmarks &i)
{
	m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_LIST_BOOKMARKS,(void *) &i)); 
	return (*this); 
}
//-----------------------------------------------------------------------------------------------------
bool PlutoMediaAttributes::UnknownSerialize(ItemToSerialize *pItem,bool bWriting,
	char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition)
{
	m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
	if( bWriting )
	{
		switch(pItem->m_iSerializeDataType)
		{
			case SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE:
				{
					MapPlutoMediaAttributes *pMap = (MapPlutoMediaAttributes *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
					for(MapPlutoMediaAttributes::iterator it = pMap->begin(); it!=pMap->end(); ++it)
					{
						Write_long((unsigned long) it->first);
						it->second->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					}
					return true;  // We handled it
				}
				break;

			case SERIALIZE_DATA_TYPE_MULTIMAP_PICTURES:
				{
					//disable coverart serialization in our tags
					//we'll write them into PIC tags
					Write_unsigned_long(0);
					return true;  // We handled it
				}
				break;

			case SERIALIZE_DATA_TYPE_LIST_BOOKMARKS:
				{
					ListBookmarks *pList = (ListBookmarks *) pItem->m_pItem;

					LoggerWrapper::GetInstance()->Write(LV_MEDIA, "Serialize write bookmarks: %d", pList->size());

					Write_unsigned_long((unsigned long) pList->size());
					for(ListBookmarks::iterator it = pList->begin(); it!=pList->end(); ++it)
					{
						(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					}
					return true;  // We handled it
				}
				break;
		};
	}
	else
	{
		switch(pItem->m_iSerializeDataType)
		{
			case SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE:
				{
					MapPlutoMediaAttributes *pMap = (MapPlutoMediaAttributes *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
					for(unsigned long i = 0; i<count; ++i)
					{
						long ID = Read_long();
						PlutoMediaAttribute *pPlutoMediaAttribute = new PlutoMediaAttribute();
						pPlutoMediaAttribute->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
						pMap->insert(std::make_pair(ID, pPlutoMediaAttribute));
					}
					return true;  // We handled it
				}
				break;

			case SERIALIZE_DATA_TYPE_MULTIMAP_PICTURES:
				{
					MapPictures *pMap = (MapPictures *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
					for(unsigned long i = 0; i<count; ++i)
					{
                        unsigned long size = Read_unsigned_long();
						char *pData = Read_block(size);
						if(NULL != pData)
							pMap->insert(std::make_pair(size, pData));
					}
					return true;  // We handled it
				}
				break;

			case SERIALIZE_DATA_TYPE_LIST_BOOKMARKS:
				{
					ListBookmarks *pList = (ListBookmarks *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();

					LoggerWrapper::GetInstance()->Write(LV_MEDIA, "Serialize read bookmarks: %d", count);

					for(unsigned long i = 0; i<count; ++i)
					{
						PlutoMediaBookmark *pPlutoMediaBookmark = new PlutoMediaBookmark();
						if(!pPlutoMediaBookmark->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition))
						{
							//something is wrong -- we cannot deserialize this
							LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot deserialize list with bookmarks; the attribute file seems to be corrupted! Skipping deserialization....");
							delete pPlutoMediaBookmark;
							return true;
						}

						pList->push_back(pPlutoMediaBookmark);
					}
					return true;  // We handled it
				}
				break;
		};
	}

	pDataBlock=m_pcDataBlock; iAllocatedSize=m_dwAllocatedSize; pCurrentPosition=m_pcCurrentPosition;
	return true;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaAttributes::GenerateMd5SumsForCoverarts(list<string>& listMD5SumsCoverarts, bool bRemoveDuplicates/* = false*/)
{
	MapPictures::iterator it = m_mapCoverarts.begin();
	MapPictures::iterator end = m_mapCoverarts.end();

	for( ; it != end; )
	{
		string sMd5Sums = FileUtils::FileChecksum(it->second, it->first);

		if(bRemoveDuplicates && std::find(listMD5SumsCoverarts.begin(), listMD5SumsCoverarts.end(), sMd5Sums) != listMD5SumsCoverarts.end())
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenerateMd5SumsForCoverarts: picture size %d md5sum %s is a duplicate for this entry. Not adding...",
					it->first, sMd5Sums.c_str());

			delete [] it->second;
			m_mapCoverarts.erase(it++);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenerateMd5SumsForCoverarts: picture size %d md5sum %s is unique for this entry. Adding...",
					it->first, sMd5Sums.c_str());

			listMD5SumsCoverarts.push_back(sMd5Sums);
			++it;
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaAttributes::DumpCoverarts()
{
	MapPictures::iterator it = m_mapCoverarts.begin();
	MapPictures::iterator end = m_mapCoverarts.end();

	for( ; it != end ; ++it)
	{
		string sMd5Sums = FileUtils::FileChecksum(it->second, it->first);

		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "DumpCoverarts: picture size %d md5sum %s",
			it->first, sMd5Sums.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------
