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

const string acDelimiters = "|;/";
const char cDelimiter = ';';

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
					TagLib::ID3v2::GeneralEncapsulatedObjectFrame *geob = (TagLib::ID3v2::GeneralEncapsulatedObjectFrame *)(*it);
					if ( geob->description() == "" || geob->description() == "lmce-serialized" )
					{
						TagLib::ByteVector pGeneralData = geob->object();
						Size = (size_t)pGeneralData.size();
						pData = new char[Size];
						memcpy(pData, pGeneralData.data(), Size);

		cout << "GEOB GEOB GEOB -- " << "Size: " << Size << " Desc: " << geob->description() << endl;
						break;
					}
				}
			}
		}

		// is it an MPEG file? read pics like this // TODO: need to extend TagLib::MPEG:File to know about .id3? or just tell it?
		else if ( TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>( f->file()) )
		{
			TagLib::ID3v2::Tag *id3v2tag = mpegFile->ID3v2Tag();
			if ( id3v2tag )
			{
				TagLib::ID3v2::FrameList frameList = id3v2tag->frameListMap()["GEOB"];
				for(TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); ++it)
				{
					TagLib::ID3v2::GeneralEncapsulatedObjectFrame *geob = (TagLib::ID3v2::GeneralEncapsulatedObjectFrame *)(*it);
					if ( geob->description() == "" || geob->description() == "lmce-serialized" )
					{
						TagLib::ByteVector pGeneralData = geob->object();
						Size = (size_t)pGeneralData.size();
						pData = new char[Size];
						memcpy(pData, pGeneralData.data(), Size);

		cout << "GEOB GEOB GEOB -- " << "Size: " << Size << " Desc: " << geob->toString() << endl;
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
        pData = NULL;
        Size = 0;

	FileRef *f = new FileRef(sFilename.c_str());
	if(!f->isNull()) //ensure tag is present before trying to read and data.
	{
		// Pictures need to be handled differently depending on file type
		// is it a FLAC file? read pics like this
		if ( TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>( f->file()) )
		{
			TagLib::ID3v2::Tag *id3v2tag = flacFile->ID3v2Tag( true ); // create if not present
			if ( id3v2tag )
			{
				TagLib::ID3v2::FrameList frameList = id3v2tag->frameListMap()["GEOB"];
				if ( !frameList.isEmpty() )
				{
					for( TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); ++it)
					{
						cout << "SetUserDefinedData - removeFrame( *it );    [GEOB]" << endl;
						id3v2tag->removeFrame( *it );
					}
				}

				TagLib::ByteVector generalData( pData , Size );

				TagLib::ID3v2::GeneralEncapsulatedObjectFrame *geob = new TagLib::ID3v2::GeneralEncapsulatedObjectFrame;
				geob->setDescription("lmce-serialized");
				geob->setObject( generalData );

				cout << "SetUserDefinedData - addFrame( frame );     [GEOB]" << endl;

				id3v2tag->addFrame( geob );

				flacFile->save();
			}
		}

		// is it an MPEG file? read pics like this
		else if ( TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>( f->file()) )
		{
			TagLib::ID3v2::Tag *id3v2tag = mpegFile->ID3v2Tag( true );
			if ( id3v2tag )
			{
				TagLib::ID3v2::FrameList frameList = id3v2tag->frameListMap()["GEOB"];
				if ( !frameList.isEmpty() )
				{
					for( TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); ++it)
					{
						cout << "SetUserDefinedData - removeFrame( *it );    [GEOB]" << endl;
						id3v2tag->removeFrame( *it );
					}
				}

				TagLib::ByteVector generalData( pData , Size );

				TagLib::ID3v2::GeneralEncapsulatedObjectFrame *geob = new TagLib::ID3v2::GeneralEncapsulatedObjectFrame;
				geob->setDescription("lmce-serialized");
				geob->setObject( generalData );

				cout << "SetUserDefinedData - addFrame( frame );     [GEOB]" << endl;

				id3v2tag->addFrame( geob );
				mpegFile->save();
			}
		}
	}

	// TODO: MP4/ASF ?  may need external id3 files?
}
//-----------------------------------------------------------------------------------------------------
bool TagFileHandler::LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes,
									list<pair<char *, size_t> >& listPicturesForTags)
{
	string sFileWithAttributes = m_sDirectory + "/" + FileWithAttributes(pPlutoMediaAttributes, true);
	//string sFileWithAttributes = m_sFullFilename;

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

	        LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# LoadPlutoAttributes: pluto attributes loaded (from id3 file - general object tag) %d",
	                pPlutoMediaAttributes->m_mapAttributes.size());
	}

	//get common tag attributes
	map<int, std::vector<string> > mapAttributes;
	GetTagInfo(sFileWithAttributes, mapAttributes, listPicturesForTags);

	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::LoadAttributes: tag attributes loaded (from tag file - common tags) %d", mapAttributes.size());

	//merge attributes into PlutoMediaAttributes
	for( map<int, std::vector<string> >::iterator itAttribute = mapAttributes.begin(); itAttribute != mapAttributes.end(); ++itAttribute)
	{
		// merge this attribute type, and these values
		int nType = itAttribute->first;
		std::vector<string> vsAttributeValues = itAttribute->second;

		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::LoadAttributes, from file: type %d: '%s'", nType, vtos(vsAttributeValues,';').c_str() );

		// need to iterate through all values for this attributetype
		for (std::vector<string>::iterator itValue = vsAttributeValues.begin(); itValue != vsAttributeValues.end(); ++itValue )
		{
			// attributetype found, ensure this is not a duplicate value
			bool found = false;
			std::pair <MapPlutoMediaAttributes::iterator,MapPlutoMediaAttributes::iterator> itm = pPlutoMediaAttributes->m_mapAttributes.equal_range( nType );
			for (MapPlutoMediaAttributes::iterator itPlutoAttribute = itm.first; itPlutoAttribute != itm.second; ++itPlutoAttribute)
			{
				if ( itPlutoAttribute->second->m_sName == (*itValue) )
				{
					// value found, this is a duplicate value
					found = true;
					break;
				}
			}
			if ( !found )
			{
				// value was not found in multimap - add it
				pPlutoMediaAttributes->m_mapAttributes.insert( std::make_pair( nType, new PlutoMediaAttribute(0, nType, (*itValue) ) ) );
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool TagFileHandler::SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes)
{
	string sFileWithAttributes = m_sDirectory + "/" + FileWithAttributes(pPlutoMediaAttributes, true);
	//string sFileWithAttributes = m_sFullFilename;

	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::SaveAttributes: saving %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), sFileWithAttributes.c_str());

	if (pPlutoMediaAttributes->m_mapAttributes.size() != 0)
	{
		//Temporary map with attributes for common tags
		map<int, std::vector<string> > mapAttributes;
		for(MapPlutoMediaAttributes::iterator it = pPlutoMediaAttributes->m_mapAttributes.begin(); it != pPlutoMediaAttributes->m_mapAttributes.end(); ++it)
		{
			// create a map copy of the existing multi map
			mapAttributes[it->first].push_back( it->second->m_sName );
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

		//Save user defined text
		char *pDataStartPosition = NULL;
		unsigned long ulSize = 0;
		pPlutoMediaAttributes->SerializeWrite();
		ulSize = pPlutoMediaAttributes->CurrentSize();
		pDataStartPosition = pPlutoMediaAttributes->m_pcDataBlock;
		size_t Size = ulSize;

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::SaveAttributes: saving into GEOB frame size %d", Size);

		SetUserDefinedInformation(sFileWithAttributes, pDataStartPosition, Size);
		pPlutoMediaAttributes->FreeSerializeMemory();

		// clear our pictures vector
		listPictures.clear();
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::SaveAttributes: No attributes to save");

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool TagFileHandler::RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes)
{
	string sFileWithAttributes = m_sDirectory + "/" + FileWithAttributes(pPlutoMediaAttributes, true);
	//string sFileWithAttributes = m_sFullFilename;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::RemoveAttribute: removing %s type %d attribute from the attribute file %s",
		sValue.c_str(), nTagType, sFileWithAttributes.c_str());

	RemoveTag(sFileWithAttributes, nTagType, sValue);
	return true;
}
//-----------------------------------------------------------------------------------------------------
string TagFileHandler::FileWithAttributes(PlutoMediaAttributes *pPlutoMediaAttributes,
                                                                                  bool bCreateId3File)
{
        //no id3 file if the media file doesn't exist anymore.
        if(!FileUtils::FileExists(m_sFullFilename))
        {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "# No id3 file. The media file doesn't exist anymore.");
                return "";
        }

        string sFileWithAttributes = m_sFile;

        // This section permits us to check external .id3 files to use for media without tag support
        if(!IsSupported())
        {
                if(UpdateMediaFileUtils::IsDirectory(m_sFullFilename.c_str()))
                        sFileWithAttributes = m_sFile + "/" + m_sFile + ".id3";
                else
                        sFileWithAttributes = m_sFile + ".id3";

                if(FileUtils::FileExists(m_sDirectory + "/" + sFileWithAttributes))
                        return sFileWithAttributes;

                if(!bCreateId3File)
                        return "";

                if(
                        NULL != pPlutoMediaAttributes &&
                        pPlutoMediaAttributes->m_mapAttributes.size() == 0 &&
                        pPlutoMediaAttributes->m_mapLongAttributes.size() == 0 &&
                        pPlutoMediaAttributes->m_mapCoverarts.size() == 0 &&
                        pPlutoMediaAttributes->m_listBookmarks.size() == 0 &&
                        pPlutoMediaAttributes->m_sStartPosition == ""
                )
                {
                        LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# Won't create id3 file (the media file doesn't have attributes)");
                        return "";
                }

                if(!FileUtils::DirExists(m_sDirectory + "/" + sFileWithAttributes))
                        FileUtils::WriteTextFile(m_sDirectory + "/" + sFileWithAttributes, ""); //touch it/create .id3 file
        }

        return sFileWithAttributes;
}
//-----------------------------------------------------------------------------------------------------
string TagFileHandler::GetFileAttribute()
{
        return FileWithAttributes(NULL, false);
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
	return IsSupported() || FileUtils::FileExists(m_sFullFilename + ".id3");
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

	// Create a single string with all property values seperated by 'cDelimiter'
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
void TagFileHandler::GetTagInfo(string sFilename, map<int, std::vector<string> >& mapAttributes, list<pair<char *, size_t> >& listPictures)
{
	FileRef *f = new FileRef(sFilename.c_str());
	if(!f->isNull()) //ensure tag is present before trying to read and data.
	{
		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# TagFileHandler::GetTagInfo: tags present");

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
					mapAttributes[ATTRIBUTETYPE_Album_Artist_CONST].push_back( sProperty );
				}
				else if ( i->first == "ARTIST" )
				{
					mapAttributes[ATTRIBUTETYPE_Performer_CONST].push_back( sProperty );
				}
				else if ( i->first == "TITLE" )
				{
					mapAttributes[ATTRIBUTETYPE_Title_CONST].push_back( sProperty );
				}
				else if ( i->first == "GENRE" )
				{
					mapAttributes[ATTRIBUTETYPE_Genre_CONST].push_back( sProperty );
				}
				else if ( i->first == "ALBUM" )
				{
					mapAttributes[ATTRIBUTETYPE_Album_CONST].push_back( sProperty );
				}
				else if ( i->first == "TRACKNUMBER" )
				{
					mapAttributes[ATTRIBUTETYPE_Track_CONST].push_back( sProperty );
				}
				else if ( i->first == "DATE" )
				{
					mapAttributes[ATTRIBUTETYPE_Release_Date_CONST].push_back( sProperty );
				}
				else if ( i->first == "COMPOSER" )
				{
					mapAttributes[ATTRIBUTETYPE_ComposerWriter_CONST].push_back( sProperty );
				}
				else if ( i->first == "LABEL" || (i->first == "ORGANIZATION") )
				{
					mapAttributes[ATTRIBUTETYPE_Studio_CONST].push_back( sProperty );
				}
				else if ( i->first == "TOTALDISCS" )
				{
					mapAttributes[ATTRIBUTETYPE_Number_of_Discs_CONST].push_back( sProperty );
				}
				else if ( i->first == "DISCNUMBER" )
				{
					// TODO: find/fix issue with Disc_ID in datagrids
					// Media_Plugin/Orbiter/DGs seem to cound Disc_ID as a Track #, *really* screws up track lists.
					// The data shows properly in the database it is only once it is queried in the DGs
					std::vector<string> vsDisc;
					stov(sProperty, vsDisc, "; /");
					mapAttributes[ATTRIBUTETYPE_Disc_ID_CONST].push_back( vsDisc.front() );
				}
				else if ( i->first == "COMMENT" )
				{
					mapAttributes[ATTRIBUTETYPE_Synopsis_CONST].push_back( sProperty );
				}
				else if ( i->first == "CHANNEL" )
				{
					mapAttributes[ATTRIBUTETYPE_Channel_CONST].push_back( sProperty );
				}
				else if ( i->first == "EPISODE" )
				{
					mapAttributes[ATTRIBUTETYPE_Episode_CONST].push_back( sProperty );
				}
				else if ( i->first == "PROGRAM" )
				{
					mapAttributes[ATTRIBUTETYPE_Program_CONST].push_back( sProperty );
				}
			}
		}

		if ( mapAttributes[ATTRIBUTETYPE_Album_Artist_CONST].empty() && !mapAttributes[ATTRIBUTETYPE_Performer_CONST].empty() )
		{
			// if no Album Artist, use the first Artist/Performer that was added
			mapAttributes[ATTRIBUTETYPE_Album_Artist_CONST].push_back( mapAttributes[ATTRIBUTETYPE_Performer_CONST].front() );
		}


		// Handle pictures in the tag or in the directory
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
		// clear existing pics in file
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
			// clear existing pics in file
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

			// clear existing pics in file
			itemsListMap.erase("covr");

			for( list<pair<char *, size_t> >::const_iterator it = listPictures.begin(); it != listPictures.end(); it++)
			{
				TagLib::MP4::CoverArt coverArt = TagLib::MP4::CoverArt( TagLib::MP4::CoverArt::JPEG, *it->first);
				coverArtList.append(coverArt);
			}

			TagLib::MP4::Item coverItem(coverArtList);
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
void TagFileHandler::InsertTagValues(TagLib::FileRef *&f, string sName, std::vector<string> vsParameters)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::InsertTagValues: - %s = %s", sName.c_str(), vtos(vsParameters, ';').c_str());

	TagLib::PropertyMap property_map = f->file()->properties();

	// erase the existing tag information to ensure synchronization
	if ( property_map.find( sName ) != property_map.end() )
		property_map.erase( sName );

	TagLib::StringList property_value_list;
	for (std::vector<string>::iterator parameter = vsParameters.begin(); parameter != vsParameters.end(); ++parameter)
		property_value_list.append( String( (*parameter), String::UTF8) );

	cout << "Property " << sName << " doesn't exist in file, adding" << endl;
	property_map.insert( sName, property_value_list );

	f->file()->setProperties(property_map);
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::SetTagInfo(string sFilename, const map<int, std::vector<string> >& mapAttributes, const list<pair<char *, size_t> >& listPictures)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::SetTagInfo for %s", sFilename.c_str());

	FileRef *f = new FileRef(sFilename.c_str());
	if(NULL != f)
	{
		std::vector<string> vsParameters;

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Album_Artist_CONST );
		if ( !vsParameters.empty() )
		{
			InsertTagValues(f, string("ALBUMARTIST"), vsParameters );

			string sParameters = vtos( vsParameters, ';' );
			cout << "SetTagInfo - set Album Artist to: " << String(sParameters, String::UTF8).to8Bit(true) << endl;
		}

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Performer_CONST );
		if ( !vsParameters.empty() )
		{
			InsertTagValues(f, string("ARTIST"), vsParameters);

			string sParameters = vtos( vsParameters, ';' );
			cout << "SetTagInfo - set Artist to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Artist set to: " << f->tag()->artist().to8Bit( true ) << endl;
		}

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Title_CONST );
		if ( !vsParameters.empty() )
		{
			string sParameters = vtos( vsParameters, ';' );
			//f->tag()->setTitle( String(sParameters, String::UTF8));
			InsertTagValues(f, string("TITLE"), vsParameters);
		}

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Genre_CONST );
		if ( !vsParameters.empty() )
		{
			string sParameters = vtos( vsParameters, ';' );
			//f->tag()->setGenre( String(sParameters, String::UTF8));
			InsertTagValues(f, string("GENRE"), vsParameters);
			cout << "SetTagInfo - set Genre to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Genreset to: " << f->tag()->genre().to8Bit( true ) << endl;
		}

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Album_CONST );
		if ( !vsParameters.empty() )
		{
			string sParameters = vtos( vsParameters, ';' );
			//f->tag()->setAlbum( String(sParameters, String::UTF8));
			InsertTagValues(f, string("ALBUM"), vsParameters);
			cout << "SetTagInfo - set Album to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Album set to: " << f->tag()->album().to8Bit( true ) << endl;
		}

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Track_CONST );
		if ( !vsParameters.empty() )
		{
			// only send one Track value
			while ( vsParameters.size() > 1 )
				vsParameters.pop_back();

			string sParameters = vtos( vsParameters, ';' );
			if ( vsParameters.front() != "0" )
			{
				//f->tag()->setTrack( atoi(sParameters.c_str()));
				InsertTagValues(f, string("TRACKNUMBER"), vsParameters);
				cout << "SetTagInfo - set Track to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Track set to: " << f->tag()->track() << endl;
			}
		}

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Release_Date_CONST );
		if ( !vsParameters.empty() )
		{
			// only send one Date value
			while ( vsParameters.size() > 1 )
				vsParameters.pop_back();

			string sParameters = vtos( vsParameters, ';' );
			if (sParameters != "0" )
			{
				//f->tag()->setYear( atoi(sParameters.c_str()));
				InsertTagValues(f, string("DATE"), vsParameters);
				cout << "SetTagInfo - set Date to: " << String(sParameters, String::UTF8).to8Bit(true) << " -- Date set to: " << f->tag()->year() << endl;
			}
		}

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_ComposerWriter_CONST );
		if ( !vsParameters.empty() )
			InsertTagValues(f, string("COMPOSER"), vsParameters);

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Studio_CONST );
		if ( !vsParameters.empty() )
			InsertTagValues(f, string("LABEL"), vsParameters);

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Number_of_Discs_CONST );
		if ( !vsParameters.empty() )
			InsertTagValues(f, string("TOTALDISCS"), vsParameters);

		// TODO: verify this parameter name
//		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Disc_ID_CONST );
//		if ( !vsParameters.empty() )
//			InsertTagValues(f, string("DISCNUMBER"), vsParameters);

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Synopsis_CONST );
		if ( !vsParameters.empty() )
			InsertTagValues(f, string("COMMENT"), vsParameters);

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Channel_CONST );
		if ( !vsParameters.empty() )
			InsertTagValues(f, string("CHANNEL"), vsParameters);

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Episode_CONST );
		if ( !vsParameters.empty() )
			InsertTagValues(f, string("EPISODE"), vsParameters);

		vsParameters = ExtractAttribute( mapAttributes, ATTRIBUTETYPE_Program_CONST );
		if ( !vsParameters.empty() )
			InsertTagValues(f, string("PROGRAM"), vsParameters);

// */
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
		for(TagLib::StringList::Iterator property_value = property_value_list.begin(); property_value != property_value_list.end(); ++property_value)
		{
			if ( (*property_value) == sValue )
			{
				cout << "Property " << (*property).first << " exists in file with sName: " << (*property_value) << ", erasing" << endl;
				property_value = property_value_list.erase( property_value );
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

//			case ATTRIBUTETYPE_Disc_ID_CONST:
//				RemoveTagValue(f, "DISCNUMBER", sValue);
//				break;

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
std::vector<string> TagFileHandler::ExtractAttribute(const map<int, std::vector<string> >& mapAttributes, int key)
{
	map<int, std::vector<string> >::const_iterator it = mapAttributes.find(key);
	if(it != mapAttributes.end())
		return it->second;

	return std::vector<string>();
}
//-----------------------------------------------------------------------------------------------------
