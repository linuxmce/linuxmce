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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "id3info.h"

#include <string.h>

#include "../../pluto_media/Define_AttributeType.h"

#ifdef ENABLE_TEST_UNIT
	#include "PlutoUtils/FileUtils.h"
#endif

#include <map>
using namespace std;

#include <tbytevector.h>

#include <mpegfile.h>

#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <taglib/textidentificationframe.h>
#include <taglib/generalencapsulatedobjectframe.h>
#include <taglib/attachedpictureframe.h>
#include <id3v1tag.h>
#include <sstream>
#include "PlutoUtils/FileUtils.h"

#include <apetag.h>
using namespace TagLib;


#define MAX_TAG_SIZE_ALLOWED 10 * 1024 * 1024 //10MB

/**
	void GetUserDefinedInformation(string sFilename, char *&pData, size_t& Size)

	@brief 			Read user defined information from id3 tag

	@param[in]  	sFilename 		Filename of mp3 file to read id3 tag from
	@param[out] 	pData 			Pointer on a data structure receiving the user information
	@param[out] 	Size 			Size of user defined data
*/
void GetUserDefinedInformation(string sFilename, char *&pData, size_t& Size)
{
 	pData = NULL;
	Size = 0;


	string sExtension = FileUtils::FindExtension(sFilename.c_str());

	if(sExtension == "mp3")
	{
		// Open mp3 file for reading
		MPEG::File file(sFilename.c_str());

		if (!file.isOpen())
			return;

		// Extract id3 tag
		ID3v2::Tag *id3v2tag = file.ID3v2Tag();

		if(id3v2tag)
		{
			// If a valid tag is found, extract all frames with user defined data (GEOB)
			TagLib::ID3v2::FrameList geoblist = id3v2tag->frameListMap()["GEOB"];

			// The function can be left, if there are no GEOB frames present
			if(geoblist.isEmpty())
				return;

			// Extract the first data frame. If none was found, leave this function
			TagLib::ID3v2::GeneralEncapsulatedObjectFrame *geobframe = static_cast<TagLib::ID3v2::GeneralEncapsulatedObjectFrame *>(geoblist.front());
			if(!geobframe)
				return;

			if(geobframe->object().isEmpty())
				return;

			// Extract data size from frames information and create a data structure with the given size. At least frame data is copied to this structure.
			Size = geobframe->object().size();
			pData = new char[Size];
			memcpy(pData, geobframe->object().data(), Size);
		}
	}
}

/**
	void SetUserDefinedInformation(string sFilename, char *pData, size_t& Size)

	@brief 			Write user defined information to ID3 tag

	@param[in]  	sFilename 		Filename of mp3 file to add user data
	@param[in] 		pData 			Pointer on a data structure with data to be written
	@param[in] 		Size 			Size of user defined data
*/
void SetUserDefinedInformation(string sFilename, char *pData, size_t& Size)
{
	string sExtension = FileUtils::FindExtension(sFilename.c_str());

	if(sExtension == "mp3")
	{
		// Open mp3 file for writing
		MPEG::File file(sFilename.c_str());

		if (!file.isOpen())
			return;

		// Extract id3 tag
		ID3v2::Tag *id3v2tag = file.ID3v2Tag();
		if(id3v2tag)
		{
			// Create a new object frame with given data and size
			TagLib::ID3v2::GeneralEncapsulatedObjectFrame *geobframe = new TagLib::ID3v2::GeneralEncapsulatedObjectFrame;
			geobframe->setObject(TagLib::ByteVector(pData, Size));
			file.ID3v2Tag()->addFrame(geobframe);

			file.save();
			return;
		}
	}
}

/**
	void GetInformationV2(const ID3v2::Tag *id3v2tag, map<int,string>& mapAttributes,
					list<pair<char *, size_t> >& listPictures)

	@brief 			Read data from ID3 tag version 2

	@param[in]  	id3v2tag 		Pointer on tag to read from
	@param[out] 	mapAttributes 	Map to write tag information
	@param[out] 	listPictures 	List of system known pictures

	@warning 		ID3 tag version 1 is not handled yet
*/
void GetInformationV2(const ID3v2::Tag *id3v2tag, map<int,string>& mapAttributes,
					list<pair<char *, size_t> >& listPictures)
{
	// Extract all frames from id3 tag and check each frame for usable information
	  ID3v2::FrameList::ConstIterator it = id3v2tag->frameList().begin();
	  for(; it != id3v2tag->frameList().end(); it++)
	  {
		int PK_Attr = 0;

		// Read the frame data
		string value = (*it)->toString().to8Bit(true);

		// We know the frame type, lets see if we have an applicable database attribute
		if((*it)->frameID() == "TALB")
			PK_Attr = ATTRIBUTETYPE_Album_CONST;
		else if((*it)->frameID() == "TIT2")
			PK_Attr = ATTRIBUTETYPE_Title_CONST;
		else if((*it)->frameID() == "TPE1")
			PK_Attr = ATTRIBUTETYPE_Performer_CONST;
		else if((*it)->frameID() == "TCON")
			PK_Attr = ATTRIBUTETYPE_Genre_CONST;
		else if((*it)->frameID() == "TRCK")
			PK_Attr = ATTRIBUTETYPE_Track_CONST;
		else if(((*it)->frameID() == "TYER") || ((*it)->frameID() == "TDRC"))
			PK_Attr = ATTRIBUTETYPE_Release_Date_CONST;
		else if((*it)->frameID() == "COMM") //comments
			PK_Attr = -2; // todo ask
		else if((*it)->frameID() == "TMED") //media
			PK_Attr = -3; // todo ask
		else if((*it)->frameID() == "TLEN") //length
			PK_Attr = -4; // todo ask;
	    else if((*it)->frameID() == "WXXX")
		    PK_Attr = ATTRIBUTETYPE_Website_CONST;
		else if((*it)->frameID() == "TCOM")
		    PK_Attr = ATTRIBUTETYPE_ComposerWriter_CONST;
		else if((*it)->frameID() == "TENC") //encoded by
	        PK_Attr = -5; // todo ask
	    else if((*it)->frameID() == "TCOP") //copyright
	        PK_Attr = -6; // todo ask
	    else if((*it)->frameID() == "TOPE") //Original artist(s)/performer(s)
	        PK_Attr = -7; // todo ask;
		else
			PK_Attr = -1000;

		// If tag has a picture attached, insert it into system wide picture list
		if((*it)->frameID() == "APIC")
		{
			TagLib::ID3v2::AttachedPictureFrame *pictureframe = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(*it);

			if(pictureframe)
			{
				char *pPictureData = NULL;
				size_t nBinSize = pictureframe->picture().size();
				pPictureData = new char[nBinSize];
				memcpy(pPictureData, pictureframe->picture().data(), nBinSize);

				listPictures.push_back(make_pair(pPictureData, nBinSize));

				value = string("(") + pictureframe->description().to8Bit(true) + ")[]: " + pictureframe->mimeType().to8Bit(true);
			}
		}

		mapAttributes[PK_Attr] = value;

	  }
}

/**
	void GetInformationV1(const ID3v2::Tag *id3v1tag, map<int,string>& mapAttributes

	@brief 			Read data from ID3 tag version 1

	@param[in]  	id3v1tag 		Pointer on tag to read from
	@param[out] 	mapAttributes 	Map to write tag information
*/
void GetInformationV1(const ID3v1::Tag *id3v1tag, map<int,string>& mapAttributes)
{

	if(!id3v1tag->title().isEmpty())
		mapAttributes[ATTRIBUTETYPE_Title_CONST] = id3v1tag->title().to8Bit(true);

	if(!id3v1tag->artist().isEmpty())
		mapAttributes[ATTRIBUTETYPE_Performer_CONST] = id3v1tag->artist().to8Bit(true);

	if(!id3v1tag->album().isEmpty())
		mapAttributes[ATTRIBUTETYPE_Album_CONST] = id3v1tag->album().to8Bit(true);

	if(!id3v1tag->genre().isEmpty())
		mapAttributes[ATTRIBUTETYPE_Genre_CONST] = id3v1tag->genre().to8Bit(true);

	if(id3v1tag->year() != 0)
	{
		ostringstream s;
		if(s << id3v1tag->year())
			mapAttributes[ATTRIBUTETYPE_Release_Date_CONST] = s.str();
	}

	if(id3v1tag->track() != 0)
	{
		ostringstream s;
		if(s << id3v1tag->track())
			mapAttributes[ATTRIBUTETYPE_Track_CONST] = s.str();
	}
}

/**
	void GetInformationAPE(const ID3v2::Tag *id3v1tag, map<int,string>& mapAttributes)

	@brief 			Read data from ape tag version 2

	@param[in]  	id3v1tag 		Pointer on tag to read from
	@param[out] 	mapAttributes 	Map to write tag information
*/
void GetInformationAPE(const APE::Tag *apetag, map<int,string>& mapAttributes)
{

	if(!apetag->title().isEmpty())
		mapAttributes[ATTRIBUTETYPE_Title_CONST] = apetag->title().to8Bit(true);

	if(!apetag->artist().isEmpty())
		mapAttributes[ATTRIBUTETYPE_Performer_CONST] = apetag->artist().to8Bit(true);

	if(!apetag->album().isEmpty())
		mapAttributes[ATTRIBUTETYPE_Album_CONST] = apetag->album().to8Bit(true);

	if(!apetag->genre().isEmpty())
		mapAttributes[ATTRIBUTETYPE_Genre_CONST] = apetag->genre().to8Bit(true);

	if(apetag->year() != 0)
	{
		ostringstream s;
		if(s << apetag->year())
			mapAttributes[ATTRIBUTETYPE_Release_Date_CONST] = s.str();
	}

	if(apetag->track() != 0)
	{
		ostringstream s;
		if(s << apetag->track())
			mapAttributes[ATTRIBUTETYPE_Track_CONST] = s.str();
	}
}

/**
	void GetId3Info(string sFilename, map<int,string>& mapAttributes,
					list<pair<char *, size_t> >& listPictures)

	@brief 			Open file and read tag depending on tag version

	@param[in]  	sFilename 		Filename of mp3 file to be read
	@param[out] 	mapAttributes 	Map to write tag information
	@param[out] 	listPictures 	List of system known pictures
*/
void GetId3Info(string sFilename, map<int,string>& mapAttributes,
				list<pair<char *, size_t> >& listPictures)
{
	string sExtension = FileUtils::FindExtension(sFilename.c_str());

	if(sExtension == "mp3")
	{
		MPEG::File file(sFilename.c_str());

		if (!file.isOpen())
			return;

		ID3v2::Tag *id3v2tag = file.ID3v2Tag();

		if((id3v2tag != NULL) && (!id3v2tag->isEmpty()))
		{
			GetInformationV2(id3v2tag, mapAttributes, listPictures);
			return;
		}

		ID3v1::Tag *id3v1tag = file.ID3v1Tag();

		if((id3v1tag != NULL) && (!id3v1tag->isEmpty()))
		{
			GetInformationV1(id3v1tag, mapAttributes);
			return;
		}

		APE::Tag *apetag = file.APETag();

		if((apetag != NULL) && (!apetag->isEmpty()))
		{
			GetInformationAPE(apetag, mapAttributes);
			return;
		}
	}
}

/**
	void AddID3PictureTag(const string sFilename, const char *pData, size_t nSize)

	@brief 			Append a picture to the given file

	@param[in]  	sFilename 		Filename of mp3 file to be written
	@param[in] 		pData 			Pointer on picture data
	@param[in] 		nSize 			Size of picture
*/
void AddID3PictureTag(const string sFilename, const char *pData, size_t nSize)
{
	string sExtension = FileUtils::FindExtension(sFilename.c_str());

	if(sExtension == "mp3")
	{
		MPEG::File file(sFilename.c_str());

		if (!file.isOpen())
			return;

		if(!file.ID3v2Tag())
			return;

		TagLib::ID3v2::AttachedPictureFrame *pictureframe = new TagLib::ID3v2::AttachedPictureFrame;
		pictureframe->setMimeType(TagLib::String("image/jpg", TagLib::String::Latin1));
		pictureframe->setPicture(TagLib::ByteVector(pData, nSize));
		pictureframe->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
		file.ID3v2Tag()->addFrame(pictureframe);

		file.save();
	}
}

/**
	void SetId3Value(const string sFilename, const string sAttribute, const string Value)

	@brief 			Write string data to file

	@param[in]  	sFilename 		Filename of mp3 file to be written
	@param[in] 		sAttribute 		Frame type
	@param[in] 		Value 			Data to be written
*/
void SetId3Value(const string sFilename, const string sAttribute, const string Value)
{
	ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::UTF16);

	string sExtension = FileUtils::FindExtension(sFilename.c_str());

	if(sExtension == "mp3")
	{
		MPEG::File file(sFilename.c_str());

		if (!file.isOpen())
			return;

		if(!file.ID3v2Tag())
			return;

		// Get frames depending on given attribute
		TagLib::ID3v2::FrameList framelist = file.ID3v2Tag()->frameListMap()[sAttribute.c_str()];

		// If there is no such frame yet, create a new one
		if(framelist.isEmpty())
		{
			TagLib::ID3v2::TextIdentificationFrame *tpeframe = new TagLib::ID3v2::TextIdentificationFrame(sAttribute.c_str(), TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding());
			file.ID3v2Tag()->addFrame(tpeframe);

			tpeframe->setText(Value.c_str());
		}
		else
		{
			TagLib::ID3v2::TextIdentificationFrame *frame = (TagLib::ID3v2::TextIdentificationFrame *)framelist.front();
			if (!frame)
			{
				TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame(sAttribute.c_str(), TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding());
				file.ID3v2Tag()->addFrame(frame);
			}
			frame->setTextEncoding(TagLib::String::UTF16);
			frame->fieldList().append(Value.c_str());
		}

		file.save();
	}
}


/**
	void SetId3Info(string sFilename, const map<int,string>& mapAttributes,
					const list<pair<char *, size_t> >& listPictures)

	@brief 			Transform from database attribute to frame type

	@param[in]  	sFilename 		Filename of mp3 file to be written
	@param[in] 		mapAttributes 	Map of attributes from database
	@param[in] 		listPictures 	List of pictures

*/
void SetId3Info(string sFilename, const map<int,string>& mapAttributes,
				const list<pair<char *, size_t> >& listPictures)
{
	// Cycle through the given attributes and insert data accordingly
	for(map<int,string>::const_iterator it = mapAttributes.begin(); it != mapAttributes.end(); it++)
	{
		cout << "Writing: PK_Attr = " << (*it).first << "\t\t" << (*it).second << endl;

		int PK_Attr = (*it).first;
		string sValue = (*it).second;

		switch(PK_Attr)
		{
			case ATTRIBUTETYPE_Performer_CONST:
				SetId3Value(sFilename.c_str(), "TPE1", sValue.c_str());
				break;

			case ATTRIBUTETYPE_Album_CONST:
				SetId3Value(sFilename.c_str(), "TALB", sValue.c_str());
				break;

			case ATTRIBUTETYPE_Title_CONST:
				SetId3Value(sFilename.c_str(), "TIT2", sValue.c_str());
				break;

			case ATTRIBUTETYPE_Release_Date_CONST:
				SetId3Value(sFilename.c_str(), "TDRC", sValue.c_str());
				break;

			case ATTRIBUTETYPE_Genre_CONST:
				SetId3Value(sFilename.c_str(), "TCON", sValue.c_str());
				break;

			case ATTRIBUTETYPE_Track_CONST:
				SetId3Value(sFilename.c_str(), "TRCK", sValue.c_str());
				break;

			case ATTRIBUTETYPE_ComposerWriter_CONST:
				SetId3Value(sFilename.c_str(), "TCOM", sValue.c_str());
				break;

			case ATTRIBUTETYPE_Website_CONST:
				SetId3Value(sFilename.c_str(), "WXXX", sValue.c_str());
				break;

			default:
				cout << "Don't know yet how to save tag with PK_Attr = " << PK_Attr << " and value " << sValue << endl;
		}
	}

	RemoveId3Tag(sFilename, -5000, "");

	for(list<pair<char *, size_t> >::const_iterator itp = listPictures.begin(); itp != listPictures.end(); itp++)
			AddID3PictureTag(sFilename, itp->first, itp->second);
}

/**
	void RemoveId3Tag(string sFilename, int nTagType, string sValue)

	@brief 			Remove every frame with the given type from ID3 V1 and V2 tags

	@param[in]  	sFilename 		Filename of mp3 file to be written
	@param[in] 		nTagType 		Map of attributes from database
	@param[in] 		sValue 			List of pictures

*/
void RemoveId3Tag(string sFilename, int nTagType, string sValue)
{
	string sExtension = FileUtils::FindExtension(sFilename.c_str());

	if(sExtension == "mp3")
	{
		TagLib::MPEG::File file(sFilename.c_str());

		if (!file.isOpen())
			return;

		// Get frame type depending on attribute type and remove both ID3 version 1 and 2 tags.
		// Both have to be removed to avoid frame reconstruction from other tag version
		switch(nTagType)
		{
			case ATTRIBUTETYPE_Performer_CONST:
				if(file.ID3v2Tag())
					file.ID3v2Tag()->removeFrames("TPE1");
				if(file.ID3v1Tag())
					file.ID3v1Tag()->setArtist(String::null);
				break;

			case ATTRIBUTETYPE_Album_CONST:
				if(file.ID3v2Tag())
					file.ID3v2Tag()->removeFrames("TALB");
				if(file.ID3v1Tag())
					file.ID3v1Tag()->setAlbum(String::null);
				break;

			case ATTRIBUTETYPE_Title_CONST:
				if(file.ID3v2Tag())
					file.ID3v2Tag()->removeFrames("TIT2");
				if(file.ID3v1Tag())
					file.ID3v1Tag()->setTitle(String::null);
				break;

			case ATTRIBUTETYPE_Release_Date_CONST:
				if(file.ID3v2Tag())
					file.ID3v2Tag()->removeFrames("TDRC");
				if(file.ID3v1Tag())
					file.ID3v1Tag()->setYear(0);
				break;

			case ATTRIBUTETYPE_Genre_CONST:
				if(file.ID3v2Tag())
					file.ID3v2Tag()->removeFrames("TCON");
				if(file.ID3v1Tag())
					file.ID3v1Tag()->setGenre(String::null);
				break;

			case ATTRIBUTETYPE_Track_CONST:
				if(file.ID3v2Tag())
					file.ID3v2Tag()->removeFrames("TRCK");
				if(file.ID3v1Tag())
					file.ID3v1Tag()->setTrack(0);
				break;

			case ATTRIBUTETYPE_ComposerWriter_CONST:
				if(file.ID3v2Tag())
					file.ID3v2Tag()->removeFrames("TCOM");
				break;

			case ATTRIBUTETYPE_Website_CONST:
				if(file.ID3v2Tag())
					file.ID3v2Tag()->removeFrames("WXXX");
				break;

			case -5000:
				if(file.ID3v2Tag())
					file.ID3v2Tag()->removeFrames("APIC");
				break;

			default:
				cout << "Don't know yet how to remove tags with PK_Attr = " << nTagType << endl;
		}

		file.save();
	}
}

