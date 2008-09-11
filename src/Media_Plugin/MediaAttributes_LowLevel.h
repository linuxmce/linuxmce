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
 *
 * @file MediaAttributes.h
 * @brief header file for the MediaAttribute, MediaPicture, MediaAttributes class
 *
 */

#ifndef MEDIAATTRIBUTES_LOWLEVEL_H
#define MEDIAATTRIBUTES_LOWLEVEL_H

#include <deque>
using namespace std;

#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_PlaylistEntry.h"
#include "pluto_media/Table_Attribute.h"
#include "MediaSection.h"

/**
 * @brief Contains a media file
 */


class MediaFile;
class Row_Attribute;
class Row_Disc;
class Row_Picture;
class Row_DiscLocation;

/**
 * @brief the attributes of the media
 * @todo ask
 */

class MediaAttribute
{
public:
	int m_Title_Track,m_Section;  // For CD's Title_Track is the track for DVD's the title, for DVD's Section is the chapter
    int m_PK_Attribute;
    int m_PK_AttributeType;
    string m_sName;

	MediaAttribute() { m_Section=m_Title_Track=m_PK_Attribute=m_PK_AttributeType; }
	MediaAttribute(int Title_Track,int Section,int PK_AttributeType,int PK_Attribute,string Name) 
	{ 
		m_Section=Section;
		m_Title_Track=Title_Track;
		m_PK_Attribute=PK_Attribute;
		m_PK_AttributeType=PK_AttributeType; 
		m_sName=Name;
	}
};

/**
 * @brief documenatation
 * @todo ask
 */

class MediaPicture
{
public:
    int PK_Picture;
    int PK_File;
    int PK_Attribute;
    string Path;
    string Extension;
};

typedef list<MediaAttribute *> listMediaAttribute;
typedef list<MediaPicture *> listMediaPicture;

/* For table: MediaType_AttributeType, Identifier means:
1=this attribute should be included when identifying the media.  For music, song, artist
2=this attribute should be included when identifying a collection of the media.  For music, album
3=this attribute is worth mentioning, but don't include it as a primary identifier (genre, etc.)

CombineAsOne, if 1, then if there are 2 files with the same attribute they will share the same entry in
the attribute table.  If 0, there will be 2 entries in the attribute table with the same value
*/

/**
 * @brief documentation
 * @todo ask
 */

class MediaAttributes_LowLevel
{
	friend class MediaFile;

private:
    Database_pluto_media *m_pDatabase_pluto_media;

public:
    /** @brief constructor */
	MediaAttributes_LowLevel(Database_pluto_media *pDatabase_pluto_media) 
	{ 
		m_pDatabase_pluto_media = pDatabase_pluto_media; 
	};

	//void ChangeAttribute(int OldPK_AttributeType,int NewPK_AttributeType,string OldName,string NewName);
    //void ChangeAttribute(int PK_Attribute,string NewName);
	Row_Picture *AddPicture(char *pData,int iData_Size,string sFormat,string sURL);
	Row_Picture *AddPicture(string sFile);
    void UpdateSearchTokens(Row_Attribute *pRow_Attribute);
    void ScanDirectory(string Path);

    // Utility functions
	string GetFilePathFromFileID( int PK_File, int *PK_MediaType=NULL ); // If PK_MediaType is specified it will have the media type
    string GetFilePathsFromAttributeID(int PK_Atribute);
    int GetFileIDFromFilePath(string File);
    string GetAnyPictureUnderDirectory(string File,int *PK_Picture,int MaxDepthToSearch);
    string GetPictureFromFilePath(string File,int *PK_Picture);
    string GetPictureFromFileID(int PK_File,int *PK_Picture);
    string GetPictureFromDiscID(int PK_Disc,int *PK_Picture);
    string GetPictureFromAttributeID(int PK_Attribute,int *PK_Picture);
    int GetAttributeFromFileID(int PK_File);
    int GetAttributeFromFilePath(string File);
    int GetFileIDFromAttributeID(int PK_Attribute);
	int GetAttributeFromFile(int PK_File,int PK_AttributeType,string &sAttribute);  // Get the attribute of PK_AttributeType for PK_File
	int GetAttributeFromDisc(int PK_Disc,int PK_AttributeType,string &sAttribute);  // Get the attribute of PK_AttributeType for PK_Disc
	string GetDefaultDescriptionForMediaFile(MediaFile *pMediaFile);
	
	// First name can either be a separate parameter, or part of Name delimited by a tab
	// Albums are special situations because they should be combined as one only when the performer is the same.  
	// Pass in the performer as PK_Attribute_Related to have album attributes consolidated
    Row_Attribute *GetAttributeFromDescription(int PK_MediaType,int PK_AttributeType,string sName,int PK_Attribute_Related=0); 
	void AddAttributeToFile(Row_File *pRow_File,Row_Attribute *pRow_Attribute,int Track,int Section);
	void AddAttributeToDisc(Row_Disc *pRow_Disc,Row_Attribute *pRow_Attribute,int Track,int Section);

	MediaSection *GetMediaSection(deque<MediaSection *> *p_dequeMediaSection,unsigned int Section) 
	{
		while( Section>p_dequeMediaSection->size() )
			p_dequeMediaSection->push_back(new MediaSection());
		return (*p_dequeMediaSection)[Section-1];
	}
    void MarkAsMissing(int iKey, string fileName);

	static void PurgeDequeMediaFile(deque<MediaFile *> &dequeMediaFile);
	static listMediaAttribute *AttributesFromString(string Input);
    static string AttributesToString(listMediaAttribute *plistMediaAttribute);

    static listMediaPicture *PicturesFromString(string Input);
    static string PicturesToString(listMediaPicture *plistMediaPicture);

	Row_File *AddDirectoryToDatabase(int PK_MediaType,string sDirectory);
	void AddAttributesToDisc(Row_Disc *pRow_Disc,int iFileOrTrack,int iSection,map<int,int> *p_mapPK_Attribute);
	int AddRippedDiscToDatabase(int PK_Disc,int PK_MediaType,string sDestination,string sTracks);
	void AddDiscAttributesToFile(int PK_File,int PK_Disc,int Track);
	bool FixMediaAttributes(listMediaAttribute &listMediaAttribute_,int &Tracks);

	string GetAttributeName(int PK_Attribute)
	{
		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);
		if( pRow_Attribute )
			return pRow_Attribute->Name_get();
		return "";
	}

	int IsDiscAlreadyIdentified(string sIdentifiedDisc,listMediaAttribute &listMediaAttribute_);
	int IsDiscAlreadyIdentified(int PK_Device,int Slot,listMediaAttribute &listMediaAttribute_);
	int IsDiscAlreadyIdentified(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_);
	int Parse_CDDB_Media_ID(int PK_MediaType,listMediaAttribute &listMediaAttribute_,string sValue);
	int Parse_Misc_Media_ID(int PK_MediaType,listMediaAttribute &listMediaAttribute_,string sValue,int PK_File);
	MediaAttribute *GetMediaAttribute(int Title_Track,int Section,int PK_AttributeType,listMediaAttribute &listMediaAttribute_);
	int GetMaxTracks(listMediaAttribute &listMediaAttribute_);
	int AddIdentifiedDiscToDB(int PK_MediaType,string sIdentifiedDisc,listMediaAttribute &listMediaAttribute_);
	int AddIdentifiedFileToDB(int PK_MediaType,int PK_File,listMediaAttribute &listMediaAttribute_);
	int AddPictureToDisc(int PK_Disc,char *pPictureData,size_t sizePicture,string sURL);
	int AddPictureToFile(int PK_File,char *pPictureData,size_t sizePicture,string sURL);
	int AddPictureToAttribute(int PK_File,char *pPictureData,size_t sizePicture,string sURL);
	void AddPictureToDisc(int PK_Disc,int PK_Picture);
	void AddPictureToFile(int PK_File,int PK_Picture);
	void AddPictureToAttribute(int PK_Attribute,int PK_Picture);
	int GetMediaType(Row_DiscLocation *pRow_DiscLocation);
	void GetDefaultRippingName(Row_Disc *pRow_Disc,string *sFilename,string *sDirectory);

	void PurgeListMediaAttribute(listMediaAttribute &listMediaAttribute_)
	{
		for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
			delete *it;
		listMediaAttribute_.clear();
	}

	static int GetOwnerForPath(string sPath);
};

#endif
