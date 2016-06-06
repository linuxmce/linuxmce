#include "TagFileHandler.h"
#include "PlutoMediaAttributes.h"
#include "FileUtils/file_utils.h"
#include "DCE/Logger.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_media/Define_AttributeType.h"
using namespace DCE;

#include <cstdlib>
#include <tag.h>
#include <fileref.h>
#include <tpropertymap.h>

/////////////////////////////////////////////
// for FLAC files with Pics
#include <flacfile.h>
#include <flacpicture.h>

// for MP3 files with Pics
#include <id3v2tag.h>
#include <mpegfile.h>
#include <attachedpictureframe.h>
#include <generalencapsulatedobjectframe.h>

// for MP4 files with Pics
#include <mp4file.h>
#include <mp4coverart.h>

// for ASF files with Pics
#include <asffile.h>
#include <asfpicture.h>
////////////////////////////////////////////



using namespace TagLib;
//-----------------------------------------------------------------------------------------------------
namespace UpdateMediaVars
{
	extern string sUPnPMountPoint;
	extern string sLocalUPnPServerName;
};
//-----------------------------------------------------------------------------------------------------
TagFileHandler::TagFileHandler(string sDirectory, string sFile) :
	GenericFileHandler(sDirectory, sFile)
{
}
//-----------------------------------------------------------------------------------------------------
TagFileHandler::~TagFileHandler(void)
{
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::GetUserDefinedInformation(string sFilename, char *&pData, size_t& Size)
{
        pData = NULL;
        Size = 0;

	FileRef *f = new FileRef(sFilename.c_str());
	if(!f->isNull()) //ensure tag is present before trying to read and data.
	{
		// Pictures need to be handled differently depending on file type
		// is it a FLAC file? read pics like this
		if ( TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>( f->file()) )
		{
			TagLib::ID3v2::Tag *id3v2tag = flacFile->ID3v2Tag();
			if ( id3v2tag )
			{
				TagLib::ID3v2::FrameList frameList = id3v2tag->frameListMap()["GEOB"];
				for(TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); ++it)
				{
					TagLib::ID3v2::GeneralEncapsulatedObjectFrame *obj = (TagLib::ID3v2::GeneralEncapsulatedObjectFrame *)(*it);
					if ( obj->description() == "" || obj->description() == "lmce-serialized" )
					{
						TagLib::ByteVector pGeneralData = obj->render();
						Size = (size_t)pGeneralData.size();
						pData = new char[Size];
						memcpy(pData, pGeneralData.data(), Size);

		cout << "GEOB GEOB GEOB -- " << "Size: " << Size << " Desc: " << obj->description() << endl;
						break;
					}
				}
			}
		}

		// is it an MPEG file? read pics like this
		else if ( TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>( f->file()) )
		{
			TagLib::ID3v2::Tag *id3v2tag = mpegFile->ID3v2Tag();
			if ( id3v2tag )
			{
				TagLib::ID3v2::FrameList frameList = id3v2tag->frameListMap()["GEOB"];
				for(TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); ++it)
				{
					TagLib::ID3v2::GeneralEncapsulatedObjectFrame *obj = (TagLib::ID3v2::GeneralEncapsulatedObjectFrame *)(*it);
					if ( obj->description() == "" || obj->description() == "lmce-serialized" )
					{
						TagLib::ByteVector pGeneralData = obj->render();
						Size = (size_t)pGeneralData.size();
						pData = new char[Size];
						memcpy(pData, pGeneralData.data(), Size);

		cout << "GEOB GEOB GEOB -- " << "Size: " << Size << " Desc: " << obj->description() << endl;
						break;
					}
				}
			}
		}


	}
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::SetUserDefinedInformation(string sFilename, char *pData, size_t& Size)
{
/*
        ID3_Tag tag;
        tag.Link(sFilename.c_str());

        ID3_Frame* frame = tag.Find(ID3FID_GENERALOBJECT);
        if(NULL == frame)
        {
                frame = new ID3_Frame(ID3FID_GENERALOBJECT);
                tag.AttachFrame(frame);
        }

        if(NULL != frame)
        {
                ID3_Field* fld = frame->GetField(ID3FN_DATA);
                uchar *pFieldData = new uchar[Size];
                memcpy(pFieldData, pData, Size);
                fld->Set(pFieldData, Size);
                delete []pFieldData;
        }

    tag.Update(ID3TT_ID3);
*/
}
//-----------------------------------------------------------------------------------------------------
bool TagFileHandler::LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes,
									list<pair<char *, size_t> >& listPicturesForTags)
{
	string sFileWithAttributes = m_sFullFilename;

	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::LoadAttributes: loading %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), sFileWithAttributes.c_str());


	//deserialize data from user defined tag
	char *pData = NULL;
	size_t Size = 0;
	GetUserDefinedInformation(sFileWithAttributes, pData, Size);

	if(NULL != pData)
	{
		pPlutoMediaAttributes->SerializeRead((unsigned long)Size, pData);
		delete []pData;
		pData = NULL;
	}

        LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# LoadPlutoAttributes: pluto attributes loaded (from id3 file - general object tag) %d",
                pPlutoMediaAttributes->m_mapAttributes.size());

	// FIXME: refactor this with a multi-map, so it's the same as MapPlutoMediaAttributes
	//get common tag attributes
	map<int, string> mapAttributes;
	GetTagInfo(sFileWithAttributes, mapAttributes, listPicturesForTags);

	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::LoadAttributes: tag attributes loaded (from tag file - common tags) %d", mapAttributes.size());

	//merge attributes into PlutoMediaAttributes
	for(map<int, string>::iterator it = mapAttributes.begin(), end = mapAttributes.end(); it != end; ++it)
	{
		int nType = it->first;
		string sValue = it->second;

		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::LoadAttributes, from file: type %d: '%s'", nType, sValue.c_str());

		std::vector<string> vsValues;
		stov(sValue, vsValues);

		// need to iterate through all values for this attributetype
		for (std::vector<string>::iterator value = vsValues.begin(); value != vsValues.end(); ++value )
		{
			MapPlutoMediaAttributes::iterator itm = pPlutoMediaAttributes->m_mapAttributes.find(nType);
			if(itm == pPlutoMediaAttributes->m_mapAttributes.end())
			{
				pPlutoMediaAttributes->m_mapAttributes.insert( std::make_pair( nType, new PlutoMediaAttribute(0, nType, (*value) ) ) );
			}
			else
			{
				// ensure this is not a duplicate value
				bool found = false;
				for (MapPlutoMediaAttributes::iterator Values = pPlutoMediaAttributes->m_mapAttributes.begin();
					Values != pPlutoMediaAttributes->m_mapAttributes.end(); ++Values)
				{
					if ( itm->second->m_sName == (*value) )
					{
						found = true;
						break;
					}
				}
				if ( !found )
				{
					pPlutoMediaAttributes->m_mapAttributes.insert( std::make_pair( nType, new PlutoMediaAttribute(0, nType, (*value) ) ) );
				}
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool TagFileHandler::SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes)
{
	string sFileWithAttributes = m_sFullFilename;

	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::SaveAttributes: saving %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), sFileWithAttributes.c_str());

	if (pPlutoMediaAttributes->m_mapAttributes.size() != 0)
	{
		//Temporary map with attributes for common tags
		map<int, string> mapAttributes;
		for(MapPlutoMediaAttributes::iterator it = pPlutoMediaAttributes->m_mapAttributes.begin(), 
			end = pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
		{
			if ( !mapAttributes[it->first].empty() )
				mapAttributes[it->first] += "/";
			// create a map copy of the existing multi map
			mapAttributes[it->first] += it->second->m_sName;
		}


		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::SaveAttributes: saving %d pictures into APIC tags to %s",
			pPlutoMediaAttributes->m_mapCoverarts.size(), sFileWithAttributes.c_str());

		list<pair<char *, size_t> > listPictures;
		for(MapPictures::iterator itc = pPlutoMediaAttributes->m_mapCoverarts.begin(); itc != pPlutoMediaAttributes->m_mapCoverarts.end(); ++itc)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::SaveAttributes: saving into APIC picture size %d", itc->first);

			listPictures.push_back(make_pair(itc->second, itc->first));
		}
		//Save common tag tags
		SetTagInfo(sFileWithAttributes, mapAttributes, listPictures);

// setTagGEOB()

		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::SaveAttributes: listPictures.clear()");
		listPictures.clear();
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::SaveAttributes: No attributes to save");

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool TagFileHandler::RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes)
{
	string sFileWithAttributes = m_sFullFilename;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::RemoveAttribute: removing %s type %d attribute from the attribute file %s",
		sValue.c_str(), nTagType, sFileWithAttributes.c_str());

	RemoveTag(sFileWithAttributes, nTagType, sValue);
	return true;
}
//-----------------------------------------------------------------------------------------------------
string TagFileHandler::GetFileAttribute()
{
	return m_sFullFilename;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool TagFileHandler::IsSupported()
{
	const string csSupportedExtensions("mp3:ogg:flac:mp4:wv:wav::oga:mpc:spx:opus:tta:m4a:m4r:m4p:3g2:m4v:wma:asf:aif:aiff:afc:aifc:ape:s3m");
	string sExtension = StringUtils::ToLower(FileUtils::FindExtension(m_sFullFilename));

	if(sExtension.empty())
		return false;

	return csSupportedExtensions.find(sExtension) != string::npos;
}
//-----------------------------------------------------------------------------------------------------
bool TagFileHandler::FileAttributeExists()
{
	return true;
}
//-----------------------------------------------------------------------------------------------------
string TagFileHandler::GetFileSourceForDB()
{
	if(!UpdateMediaVars::sUPnPMountPoint.empty() && StringUtils::StartsWith(m_sDirectory, UpdateMediaVars::sUPnPMountPoint))
		return "U";
	else
		return "F";
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::stov(string s, std::vector<string> &v, string acDelimiters /* = "|;/" */ )
{
	string::size_type CurPos = 0;
	string sToken;

	while( ( sToken = StringUtils::Tokenize( s, acDelimiters.c_str(), CurPos )) != "" )
	{
		// Add to vector if it doesn't already exist
		sToken = StringUtils::TrimSpaces(sToken);
		if( !sToken.empty() )
		{
			std::vector<string>::iterator it = find (v.begin(), v.end(), sToken );
			if ( it == v.end() )
			{
				v.push_back( StringUtils::TrimSpaces(sToken));
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
string TagFileHandler::vtos(std::vector<string> v, const char cDelimiter /* = '/' */ )
{
	string s;

	// Create a single string with all property values seperated by ';'
	for (std::vector<string>::iterator it = v.begin(); it != v.end(); ++it)
	{
		string entry = *it;
		if ( s.size() )
		{
			s += cDelimiter;
		}
		s += entry;
	}
	return s;
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::GetTagPictures(TagLib::FileRef *&f, list<pair<char *, size_t> >& listPictures)
{
	// Pictures need to be handled differently depending on file type
	// is it a FLAC file? read pics like this
	if ( TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>( f->file()) )
	{
		cout << "FLAC FLAC FLAC - " << m_sFullFilename << endl;

		TagLib::List<TagLib::FLAC::Picture*> picList = flacFile->pictureList();
		if ( !picList.isEmpty())
		{
			for (TagLib::List<TagLib::FLAC::Picture*>::ConstIterator it = picList.begin(); it != picList.end(); ++it)
			{
				TagLib::FLAC::Picture *pic = *it;

				//if ( pic->type() == TagLib::FLAC::Picture::FrontCover)
				{
					if ( pic->mimeType() == "image/jpeg")
					{
						// get the image
						TagLib::ByteVector picData = pic->data();

						size_t nBinSize = (size_t)picData.size();
						char *pPictureData = new char[nBinSize];
						memcpy(pPictureData, picData.data(), nBinSize);

						cout << "Picture type: " << (int)pic->type() << ", mime:  '" << pic->mimeType() << "', Size: " << nBinSize << " bytes." << endl;

						// add this image to the lmce picture vector
						listPictures.push_back(make_pair(pPictureData, nBinSize));
					}
				}
			}
		}
	}

	// is it an MPEG file? read pics like this
	else if ( TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>( f->file()) )
	{
		cout << "MPEG MPEG MPEG - " << m_sFullFilename << endl;

		TagLib::ID3v2::Tag *id3v2tag = mpegFile->ID3v2Tag();
		if ( id3v2tag )
		{
			// picture frame
			TagLib::ID3v2::FrameList frameList = id3v2tag->frameListMap()["APIC"];
			if (!frameList.isEmpty() )
			{
				for(TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); ++it)
				{
					TagLib::ID3v2::AttachedPictureFrame *picFrame = (TagLib::ID3v2::AttachedPictureFrame *)(*it);

					//if ( picFrame->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover )
					{
						if ( picFrame->mimeType() == "image/jpeg")
						{
							TagLib::ByteVector picData = picFrame->picture();

							size_t nBinSize = (size_t)picData.size();
							char *pPictureData = new char[nBinSize];
							memcpy(pPictureData, picData.data(), nBinSize);

							cout << "Picture type: " << (int)picFrame->type() << ", mime: '" << picFrame->mimeType() << "', Size: " << nBinSize << " bytes." << endl;

							// the following adds this image to the lmce picture vector
							listPictures.push_back(make_pair(pPictureData, nBinSize));
						}
					}
				}
			}
		}
	}

	// is it an MP4 (AAC, ALAC, video) file? read pics like this
	else if ( TagLib::MP4::File* mp4File = dynamic_cast<TagLib::MP4::File*>( f->file()) )
	{
		cout << "MP4 MP4 MP4 - " << m_sFullFilename << endl;

		TagLib::MP4::Tag* tag = mp4File->tag();
		TagLib::MP4::ItemListMap itemListMap = tag->itemListMap();
		TagLib::MP4::Item coverItem = itemListMap["covr"];
		TagLib::MP4::CoverArtList coverArtList = coverItem.toCoverArtList();

		if ( !coverArtList.isEmpty() )
		{
			for(TagLib::MP4::CoverArtList::ConstIterator it = coverArtList.begin(); it != coverArtList.end(); ++it)
			{
				TagLib::MP4::CoverArt coverArt = *it;
				if ( coverArt.format() == TagLib::MP4::CoverArt::JPEG )
				{
					TagLib::ByteVector picData = coverArt.data();

					size_t nBinSize = (size_t)picData.size();
					char *pPictureData = new char[nBinSize];
					memcpy(pPictureData, picData.data(), nBinSize);

					cout << "Picture type: covr, mime: 'image/jpeg', Size: " << nBinSize << " bytes." << endl;

					// add this image to the lmce picture vector
					listPictures.push_back(make_pair(pPictureData, nBinSize));
				}
			}
		}
	}

	// is it an ASF (wma) file? read pics like this
	else if ( TagLib::ASF::File* asfFile = dynamic_cast<TagLib::ASF::File*>( f->file()) )
	{
		cout << "ASF ASF ASF - " << m_sFullFilename << endl;

		TagLib::ASF::Tag* tag = asfFile->tag();
		TagLib::ASF::AttributeListMap attrListMap = tag->attributeListMap();
		TagLib::ASF::AttributeList attrList = attrListMap["WM/Picture"];

		if ( !attrListMap.isEmpty() )
		{
			for(TagLib::ASF::AttributeList::ConstIterator it = attrList.begin(); it != attrList.end(); ++it)
			{
				TagLib::ASF::Attribute attr = *it;
				TagLib::ASF::Picture pic = attr.toPicture();

			//if ( pic.type() == TagLib::ASF::Picture::FrontCover)
			{
					TagLib::ByteVector picData = pic.picture();

					size_t nBinSize = (size_t)picData.size();
					char *pPictureData = new char[nBinSize];
					memcpy(pPictureData, picData.data(), nBinSize);

					cout << "Picture type: " << (int)pic.type() << ", mime: 'image/jpeg', Size: " << nBinSize << " bytes." << endl;

					// add this image to the lmce picture vector
					listPictures.push_back(make_pair(pPictureData, nBinSize));
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::GetTagInfo(string sFilename, map<int,string>& mapAttributes, list<pair<char *, size_t> >& listPictures)
{
	FileRef *f = new FileRef(sFilename.c_str());
	if(!f->isNull()) //ensure tag is present before trying to read and data.
	{
		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::GetTagInfo: tags present");

		std::vector<string> vsAlbumArtist;
		std::vector<string> vsPerformer;
		std::vector<string> vsGenre;
		std::vector<string> vsTitle;
		std::vector<string> vsAlbum;
		std::vector<string> vsTrack;
		std::vector<string> vsDate;
		std::vector<string> vsComposerWriter;
		std::vector<string> vsStudio;
		std::vector<string> vsTotalDiscs;
		std::vector<string> vsDisc;
		std::vector<string> vsSynopsis;
		std::vector<string> vsChannel;
		std::vector<string> vsEpisode;
		std::vector<string> vsProgram;

/*
		stov( f->tag()->artist().to8Bit( true ), vsPerformer );
		stov( f->tag()->title().to8Bit( true ) , vsTitle );
		stov( f->tag()->genre().to8Bit( true ) , vsGenre );
		stov( f->tag()->album().to8Bit( true ) , vsAlbum );
		stov( StringUtils::ltos( f->tag()->track() ), vsTrack );
		stov( StringUtils::ltos( f->tag()->year() ) , vsDate );
*/
		// Get Album Artist, from file properties i==propertyname, j==propertyvalue
		TagLib::PropertyMap tags = f->file()->properties();
		for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i)
		{
			for(TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j)
			{
				TagLib::String field_data = *j;
				string sProperty = field_data.to8Bit( true );
				sProperty = StringUtils::TrimSpaces(sProperty);

				if ( (i->first == "ALBUMARTIST") || (i->first == "ALBUM ARTIST") || (i->first == "ENSEMBLE") )
				{
					stov(sProperty, vsAlbumArtist);
				}
				else if ( i->first == "ARTIST" )
				{
					stov(sProperty, vsPerformer);
				}
				else if ( i->first == "GENRE" )
				{
					stov(sProperty, vsGenre);
				}
				else if ( i->first == "TITLE" )
				{
					stov(sProperty, vsTitle, "|");
				}
				else if ( i->first == "ALBUM" )
				{
					stov(sProperty, vsAlbum, "|");
				}
				else if ( i->first == "TRACKNUMBER" )
				{
					stov(sProperty, vsTrack, "|");
				}
				else if ( i->first == "DATE" )
				{
					stov(sProperty, vsDate, "|");
				}
				else if ( i->first == "COMPOSER" )
				{
					stov(sProperty, vsComposerWriter);
				}
				else if ( i->first == "LABEL" || (i->first == "ORGANIZATION") )
				{
					stov(sProperty, vsStudio);
				}
				else if ( i->first == "TOTALDISCS" )
				{
					stov(sProperty, vsTotalDiscs, "|");
				}
				else if ( i->first == "DISCNUMBER" )
				{
					// TODO: split this properly when it comes in as 'x/y' or 'x of y'
					stov(sProperty, vsDisc, "|");
				}
				else if ( i->first == "COMMENT" )
				{
					stov(sProperty, vsSynopsis, "|");
				}
				else if ( i->first == "CHANNEL" )
				{
					stov(sProperty, vsChannel, "|");
				}
				else if ( i->first == "EPISODE" )
				{
					stov(sProperty, vsEpisode);
				}
				else if ( i->first == "PROGRAM" )
				{
					stov(sProperty, vsSynopsis, "|");
				}
			}
		}

		if ( !vsAlbumArtist.size() && vsPerformer.size() )
		{
			// if no Album Artist, use the first Artist/Performer that was added
			stov( *vsPerformer.begin(), vsAlbumArtist );
		}

		// assign compiled strings to attributes
		if ( !vsAlbumArtist.empty() )		mapAttributes[ATTRIBUTETYPE_Album_Artist_CONST] = vtos( vsAlbumArtist );
		if ( !vsPerformer.empty() )		mapAttributes[ATTRIBUTETYPE_Performer_CONST] = vtos( vsPerformer );
		if ( !vsTitle.empty() )			mapAttributes[ATTRIBUTETYPE_Title_CONST] = vtos( vsTitle );
		if ( !vsGenre.empty() )			mapAttributes[ATTRIBUTETYPE_Genre_CONST] = vtos( vsGenre );
		if ( !vsAlbum.empty() )			mapAttributes[ATTRIBUTETYPE_Album_CONST] = vtos( vsAlbum );
		if ( !vsTrack.empty() )			mapAttributes[ATTRIBUTETYPE_Track_CONST] = vtos( vsTrack );
		if ( !vsDate.empty() )			mapAttributes[ATTRIBUTETYPE_Release_Date_CONST] = vtos( vsDate );
		if ( !vsComposerWriter.empty() )	mapAttributes[ATTRIBUTETYPE_ComposerWriter_CONST] = vtos( vsComposerWriter );
		if ( !vsStudio.empty() )		mapAttributes[ATTRIBUTETYPE_Studio_CONST] = vtos( vsStudio );
		if ( !vsTotalDiscs.empty() )		mapAttributes[ATTRIBUTETYPE_Number_of_Discs_CONST] = vtos( vsTotalDiscs );
		if ( !vsDisc.empty() )			mapAttributes[ATTRIBUTETYPE_Disc_ID_CONST] = vtos( vsDisc );
		if ( !vsSynopsis.empty() )		mapAttributes[ATTRIBUTETYPE_Synopsis_CONST] = vtos( vsSynopsis );
		if ( !vsChannel.empty() )		mapAttributes[ATTRIBUTETYPE_Channel_CONST] = vtos( vsChannel );
		if ( !vsEpisode.empty() )		mapAttributes[ATTRIBUTETYPE_Episode_CONST] = vtos( vsEpisode );
		if ( !vsProgram.empty() )		mapAttributes[ATTRIBUTETYPE_Program_CONST] = vtos( vsProgram );

		GetTagPictures(f, listPictures);
		// get pic from a .jpg in the same directory
		string coverfilename = sFilename.substr(0,sFilename.find_last_of("/\\")) + "/cover.jpg";
		if ( !FileUtils::FileExists( coverfilename ) )
			coverfilename = sFilename.substr(0,sFilename.find_last_of("/\\")) + "/Cover.jpg";
		if ( !FileUtils::FileExists( coverfilename ) )
			coverfilename = sFilename.substr(0,sFilename.find_last_of("/\\")) + "/folder.jpg";
		if ( !FileUtils::FileExists( coverfilename ) )
			coverfilename = sFilename.substr(0,sFilename.find_last_of("/\\")) + "/Folder.jpg";
		if ( FileUtils::FileExists( coverfilename ) )
		{
			cout << "fetching cover: "<< coverfilename << "\n";
			FILE *coverart = fopen(coverfilename.c_str(), "rb");
			if (coverart != NULL)
			{
				fseek(coverart, 0, SEEK_END);
				size_t nBinSize = (size_t)ftell(coverart);
				if (nBinSize > 0)
				{
					rewind(coverart);
					char *pPictureData = new char[nBinSize];
					fread(pPictureData, 1,nBinSize,coverart);
					listPictures.push_back(make_pair(pPictureData, nBinSize));
				}
			}
		}

		delete f;
	}
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::InsertTagPictures(TagLib::FileRef *&f, const list<pair<char *, size_t> >& listPictures)
{
	if ( listPictures.empty() )
		return;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::InsertPictures: inserting");

	// is it a FLAC file? write pics like this
	if ( TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>( f->file()) )
	{
		flacFile->pictureList().clear();

		for( list<pair<char *, size_t> >::const_iterator it = listPictures.begin(); it != listPictures.end(); it++)
		{
			TagLib::ByteVector picData( it->first, (unsigned int)it->second );

			TagLib::FLAC::Picture *pic = new TagLib::FLAC::Picture( picData );
			pic->setType(TagLib::FLAC::Picture::FrontCover);
			pic->setMimeType("image/jpeg");

			flacFile->pictureList().append(pic);
		}
	}


	// is it an MPEG file? write pics like this
	else if ( TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>( f->file()) )
	{
		TagLib::ID3v2::Tag *tag = mpegFile->ID3v2Tag(true);

		if ( tag )
		{
			// remove all pre-existing picture frames from the file?!?!
			TagLib::ID3v2::FrameList frameList = tag->frameListMap()["APIC"];
			if ( !frameList.isEmpty() )
			{
				for( TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); ++it)
				{
					cout << "InsertTagPictures - removeFrame( *it );" << endl;
					tag->removeFrame( *it );
				}
			}

			for( list<pair<char *, size_t> >::const_iterator it = listPictures.begin(); it != listPictures.end(); it++)
			{
				TagLib::ByteVector picData( (*it).first , (*it).second );
				TagLib::ID3v2::AttachedPictureFrame *frame = new TagLib::ID3v2::AttachedPictureFrame;
				frame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
				frame->setMimeType("image/jpeg");
				frame->setPicture( picData );

				cout << "InsertTagPictures - addFrame( frame );" << endl;
				tag->addFrame( frame );
			}
			mpegFile->save();
		}
	}

	// is it an MP4 (AAC, ALAC, video) file? access pics like this
	else if ( TagLib::MP4::File* mp4File = dynamic_cast<TagLib::MP4::File*>( f->file()) )
	{
		TagLib::MP4::Tag *tag = mp4File->tag();

		if ( tag )
		{
			TagLib::MP4::ItemListMap itemsListMap = tag->itemListMap();
			TagLib::MP4::CoverArtList coverArtList;

			for( list<pair<char *, size_t> >::const_iterator it = listPictures.begin(); it != listPictures.end(); it++)
			{
				TagLib::MP4::CoverArt coverArt = TagLib::MP4::CoverArt( TagLib::MP4::CoverArt::JPEG, *it->first);
				coverArtList.append(coverArt);
			}

			TagLib::MP4::Item coverItem(coverArtList);
			itemsListMap.erase("covr");
			itemsListMap.insert("covr", coverItem);
			tag->save();
		}
	}
/*
	// TODO: is it an ASF (wma) file? access pics like this
	else if ( TagLib::ASF::File* asfFile = dynamic_cast<TagLib::ASF::File*>( f->file()) )
	{

		for( list<pair<char *, size_t> >::const_iterator it = listPictures.begin(); it != listPictures.end(); it++)
		{
		}
	}
*/
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::InsertTagValues(TagLib::FileRef *&f, string sName, string sParameters)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::InsertTagValues: - %s = %s", sName.c_str(), sParameters.c_str());

	std::vector<string> vsParameters;
	stov(sParameters, vsParameters);

	TagLib::PropertyMap property_map = f->file()->properties();
	TagLib::PropertyMap::Iterator property = property_map.find( sName );
	if ( property != property_map.end() )
	{
		for (std::vector<string>::iterator parameter = vsParameters.begin(); parameter != vsParameters.end(); ++parameter)
		{
			bool bFound = false;
			TagLib::StringList property_value_list = property->second;
			for(TagLib::StringList::ConstIterator property_values = property_value_list.begin(); property_values != property_value_list.end(); ++property_values)
			{
				// FIXME: compare each StringList 'property_values' as a whole prior to checking de-tokenized values, may not need to check them
				std::vector<string> vsPropertyValues;
				stov( (*property_values).to8Bit( true ) , vsPropertyValues );

				for(std::vector<string>::iterator value = vsPropertyValues.begin(); value != vsPropertyValues.end(); ++value)
				{
					cout << "InsertTagValue - Comparing " << (*value) << " to " << (*parameter) << endl;

					if ( (*value)  == (*parameter) )
					{
						cout << "Property " << sName << " exists in file with value: " << (*parameter) << endl;
						//cout << "Property " << (*property).first << " exists in file in StringList: " << (*property_values).to8Bit( true ) << endl;
						bFound = true;
					}
				}
			}
			if (bFound != true)
			{
				cout << "Property " << sName << " doesn't exist in file with value " << (*parameter) << ", adding" << endl;
				property_value_list.append( String( (*parameter), String::UTF8) );
				property_map.replace( sName, property_value_list );
			}
		}
	}
	else
	{
		cout << "Property " << sName << " doesn't exist in file, adding" << endl;
		property_map.insert( sName, String( sParameters, String::UTF8 ) );
	}
	f->file()->setProperties(property_map);
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::SetTagInfo(string sFilename, const map<int,string>& mapAttributes, const list<pair<char *, size_t> >& listPictures)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::SetTagInfo for %s", sFilename.c_str());

	FileRef *f = new FileRef(sFilename.c_str());
	if(NULL != f)
	{
		string sParameters;
		std::vector<string> vsParameters;

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Album_Artist_CONST);
		if ( !sParameters.empty() )
		{
			cout << "SetTagInfo - set Album Artist to: " << String(sParameters, String::UTF8).to8Bit(true) << endl;
			InsertTagValues(f, string("ALBUMARTIST"), sParameters );
		}

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Performer_CONST);
		if ( !sParameters.empty() )
		{
			//f->tag()->setArtist( String(sParameters, String::UTF8 ));
			InsertTagValues(f, string("ARTIST"), sParameters);
			cout << "SetTagInfo - set Artist to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Artist set to: " << f->tag()->artist().to8Bit( true ) << endl;
		}

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Title_CONST);
		if ( !sParameters.empty() )
		{
			//f->tag()->setTitle( String(sParameters, String::UTF8));
			InsertTagValues(f, string("TITLE"), sParameters);
			cout << "SetTagInfo - set Title to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Title set to: " << f->tag()->title().to8Bit( true ) << endl;
		}

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Genre_CONST);
		if ( !sParameters.empty() )
		{
			//f->tag()->setGenre( String(sParameters, String::UTF8));
			InsertTagValues(f, string("GENRE"), sParameters);
			cout << "SetTagInfo - set Genre to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Genreset to: " << f->tag()->genre().to8Bit( true ) << endl;
		}

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Album_CONST);
		if ( !sParameters.empty() )
		{
			//f->tag()->setAlbum( String(sParameters, String::UTF8));
			InsertTagValues(f, string("ALBUM"), sParameters);
			cout << "SetTagInfo - set Album to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Album set to: " << f->tag()->album().to8Bit( true ) << endl;
		}

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Track_CONST);
		if ( !sParameters.empty() )
		{
			//f->tag()->setTrack( atoi(sParameters.c_str()));
			InsertTagValues(f, string("TRACKNUMBER"), sParameters);
			cout << "SetTagInfo - set Track to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Track set to: " << f->tag()->track() << endl;
		}

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Release_Date_CONST);
		if ( !sParameters.empty() )
		{
			//f->tag()->setYear( atoi(sParameters.c_str()));
			InsertTagValues(f, string("DATE"), sParameters);
			cout << "SetTagInfo - set Date to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Date set to: " << f->tag()->title().to8Bit( true ) << endl;
		}

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_ComposerWriter_CONST);
		if ( !sParameters.empty() )
			InsertTagValues(f, string("COMPOSER"), sParameters);

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Studio_CONST);
		if ( !sParameters.empty() )
			InsertTagValues(f, string("ORGANIZATION"), sParameters);

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Number_of_Discs_CONST);
		if ( !sParameters.empty() )
			InsertTagValues(f, string("TOTALDISCS"), sParameters);

		// TODO: verify this parameter name
		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Disc_ID_CONST);
		if ( !sParameters.empty() )
			InsertTagValues(f, string("DISCNUMBER"), sParameters);

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Synopsis_CONST);
		if ( !sParameters.empty() )
			InsertTagValues(f, string("COMMENT"), sParameters);

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Channel_CONST);
		if ( !sParameters.empty() )
			InsertTagValues(f, string("CHANNEL"), sParameters);

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Episode_CONST);
		if ( !sParameters.empty() )
			InsertTagValues(f, string("EPISODE"), sParameters);

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Program_CONST);
		if ( !sParameters.empty() )
			InsertTagValues(f, string("PROGRAM"), sParameters);


		// Store pictures in file tag
		InsertTagPictures(f, listPictures);

		f->save();
		delete f;
	}
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::RemoveTagValue(TagLib::FileRef *&f, const string sName, string sValue)
{
	cout << "Property - sName: " << sName << ", sValue: " << sValue << endl;
	TagLib::PropertyMap property_map = f->file()->properties();

	TagLib::PropertyMap::Iterator property = property_map.find(sName);
	if ( property != property_map.end() )
	{
		bool bFound = false;
		TagLib::StringList property_value_list = property->second;
		for(TagLib::StringList::Iterator property_values = property_value_list.begin(); property_values != property_value_list.end(); ++property_values)
		{
			if ( (*property_values) == sValue )
			{
				cout << "Property " << (*property).first << " exists in file with sName: " << (*property_values) << ", erasing" << endl;
				property_values = property_value_list.erase( property_values );
				bFound = true;
			}
		}
		if ( bFound )
		{
			if ( property_value_list.isEmpty() )
				property_map.erase( (*property).first );
			else
				property_map.replace( (*property).first , property_value_list );

			cout << "Properties being writing to file" << endl;
			f->file()->setProperties(property_map);
		}
		else
		{
			cout << "Property " << sName << ", with value " << sValue << " doesn't exist in file to remove" << endl;;
		}
	}
	else
	{
		cout << "Property " << sName << " doesn't exist in file to remove" << endl;
	}
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::RemoveTag(string sFilename, int nTagType, string sValue)
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "# TagFileHandler::RemoveTag");

	FileRef *f = new FileRef(sFilename.c_str());
	if(NULL != f)
	{
		switch(nTagType)
		{
			case ATTRIBUTETYPE_Album_Artist_CONST:
				RemoveTagValue(f, "ALBUMARTIST", sValue);
				RemoveTagValue(f, "ALBUM ARTIST", sValue);
				RemoveTagValue(f, "ENSEMBLE", sValue);
				break;

			case ATTRIBUTETYPE_Performer_CONST:
				RemoveTagValue(f, "ARTIST", sValue);
//				if(f->tag()->artist() == sValue)
//					f->tag()->setArtist("");
				break;

			case ATTRIBUTETYPE_Title_CONST:
				RemoveTagValue(f, "TITLE", sValue);
//				if(f->tag()->title() == sValue)
//					f->tag()->setTitle("");
				break;

			case ATTRIBUTETYPE_Genre_CONST:
				RemoveTagValue(f, "GENRE", sValue);
//				if(f->tag()->genre() == sValue)
//					f->tag()->setGenre("");
				break;

			case ATTRIBUTETYPE_Album_CONST:
				RemoveTagValue(f, "ALBUM", sValue);
//				if(f->tag()->album() == sValue)
//					f->tag()->setAlbum("");
				break;

			case ATTRIBUTETYPE_Track_CONST:
				RemoveTagValue(f, "TRACKNUMBER", sValue);
//				if(f->tag()->track() == atoi(sValue.c_str()))
//					f->tag()->setTrack(0);
				break;

			case ATTRIBUTETYPE_Release_Date_CONST:
				RemoveTagValue(f, "DATE", sValue);
//				if(f->tag()->year() == atoi(sValue.c_str()))
//					f->tag()->setYear(0);
				break;

			case ATTRIBUTETYPE_ComposerWriter_CONST:
				RemoveTagValue(f, "COMPOSER", sValue);
				break;

			case ATTRIBUTETYPE_Studio_CONST:
				RemoveTagValue(f, "LABEL", sValue);
				RemoveTagValue(f, "ORGANIZATION", sValue);
				break;

			case ATTRIBUTETYPE_Number_of_Discs_CONST:
				RemoveTagValue(f, "TOTALDISCS", sValue);
				break;

			case ATTRIBUTETYPE_Disc_ID_CONST:
				RemoveTagValue(f, "DISCNUMBER", sValue);
				break;

			case ATTRIBUTETYPE_Synopsis_CONST:
				RemoveTagValue(f, "COMMENT", sValue);
				break;

			case ATTRIBUTETYPE_Channel_CONST:
				RemoveTagValue(f, "CHANNEL", sValue);
				break;
			case ATTRIBUTETYPE_Episode_CONST:
				RemoveTagValue(f, "EPISODE", sValue);
				break;
			case ATTRIBUTETYPE_Program_CONST:
				RemoveTagValue(f, "PROGRAM", sValue);
				break;

		}
		f->save();
		delete f;
	}
}
//-----------------------------------------------------------------------------------------------------
string TagFileHandler::ExtractAttribute(const map<int,string>& mapAttributes, int key)
{
	map<int,string>::const_iterator it = mapAttributes.find(key);
	if(it != mapAttributes.end())
		return it->second;

	return "";
}
//-----------------------------------------------------------------------------------------------------
