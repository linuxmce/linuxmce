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
bool TagFileHandler::LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes,
									list<pair<char *, size_t> >& listPicturesForTags)
{
	string sFileWithAttributes = m_sFullFilename;

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::LoadAttributes: loading %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), sFileWithAttributes.c_str());
#endif
// FIXME: refactor this with a multi-map, so it's the same as MapPlutoMediaAttributes
	//get common tag attributes
	map<int, string> mapAttributes;
	GetTagInfo(sFileWithAttributes, mapAttributes, listPicturesForTags);

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: tag attributes loaded (from tag file - common tags) %d", mapAttributes.size());
#endif
	//merge attributes into PlutoMediaAttributes
	for(map<int, string>::iterator it = mapAttributes.begin(), end = mapAttributes.end(); it != end; ++it)
	{
		int nType = it->first;
		string sValue = it->second;

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "# LoadAttributes, from file: type %d: '%s'", nType, sValue.c_str());

		std::vector<string> vsValues;
		stov(sValue, vsValues);

		// need to iterate through all values for this attributetype
		for (std::vector<string>::iterator it = vsValues.begin(); it != vsValues.end(); ++it )
		{
			MapPlutoMediaAttributes::iterator itm = pPlutoMediaAttributes->m_mapAttributes.find(nType);
			if(itm == pPlutoMediaAttributes->m_mapAttributes.end())
			{
				pPlutoMediaAttributes->m_mapAttributes.insert( std::make_pair( nType, new PlutoMediaAttribute(0, nType, *it) ) );
			}
			else
			{
				// ensure this is not a duplicate value
				bool found = false;
				for (MapPlutoMediaAttributes::iterator Values = pPlutoMediaAttributes->m_mapAttributes.begin();
					Values != pPlutoMediaAttributes->m_mapAttributes.end(); ++Values)
				{
					if ( itm->second->m_sName == *it )
					{
						found = true;
						break;
					}
				}
				if ( !found )
				{
					pPlutoMediaAttributes->m_mapAttributes.insert( std::make_pair( nType, new PlutoMediaAttribute(0, nType, *it) ) );
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

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::SaveAttributes: saving %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), sFileWithAttributes.c_str());
#endif
	if (pPlutoMediaAttributes->m_mapAttributes.size() != 0)
	{
		//Temporary map with attributes for common tags
		map<int, string> mapAttributes;
		for(MapPlutoMediaAttributes::iterator it = pPlutoMediaAttributes->m_mapAttributes.begin(), 
			end = pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
		{
			if ( !mapAttributes[it->first].empty() )
				mapAttributes[it->first] += ";";
			// create a map copy of the existing multi map
			mapAttributes[it->first] += it->second->m_sName;
		}


		LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::SaveAttributes: saving %d pictures into APIC tags to %s",
			pPlutoMediaAttributes->m_mapCoverarts.size(), sFileWithAttributes.c_str());

		list<pair<char *, size_t> > listPictures;
		for(MapPictures::iterator itc = pPlutoMediaAttributes->m_mapCoverarts.begin(); itc != pPlutoMediaAttributes->m_mapCoverarts.end(); ++itc)
		{
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::SaveAttributes: saving into APIC picture size %d", itc->first);
#endif
			listPictures.push_back(make_pair(itc->second, itc->first));
		}
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::SaveAttributes: SetTagInfo");
#endif
		//Save common tag tags
// TODO: FIXME: fix this not working so attributes save to the file
//		SetTagInfo(sFileWithAttributes, mapAttributes, listPictures);
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::SaveAttributes: listPictures.clear()");
#endif
		listPictures.clear();
	}
	else
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::SaveAttributes: No attributes to save");
#endif

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool TagFileHandler::RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes)
{
	string sFileWithAttributes = m_sFullFilename;
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::RemoveAttribute: removing %s type %d attribute from the attribute file %s",
		sValue.c_str(), nTagType, sFileWithAttributes.c_str());
#endif
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
		std::vector<string>::iterator it = find (v.begin(), v.end(), StringUtils::TrimSpaces(sToken) );
		if ( it == v.end() )
		{
			v.push_back( StringUtils::TrimSpaces(sToken));
		}
	}
}
//-----------------------------------------------------------------------------------------------------
string TagFileHandler::vtos(std::vector<string> v, const char cDelimiter /* = ';' */ )
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
void TagFileHandler::GetTagInfo(string sFilename, map<int,string>& mapAttributes, list<pair<char *, size_t> >& listPictures)
{
	FileRef *f = new FileRef(m_sFullFilename.c_str());
	if(!f->isNull()) //ensure tag is present before trying to read and data.
	{

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::GetTagInfo: tags present");
#endif
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

		// Get Album Artist, from file properties i==propertyname, j==propertyvalue
		TagLib::PropertyMap tags = f->file()->properties();
		for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i)
		{
			for(TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j)
			{
				TagLib::String field_data = *j;
				string sProperty = field_data.to8Bit(true);
				sProperty = StringUtils::TrimSpaces(sProperty);

				if ( (i->first == "ALBUMARTIST") || (i->first == "ALBUM ARTIST") )
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
					stov(sProperty, vsTitle, "");
				}
				else if ( i->first == "ALBUM" )
				{
					stov(sProperty, vsAlbum, "");
				}
				else if ( i->first == "TRACKNUMBER" )
				{
					stov(sProperty, vsTrack, "");
				}
				else if ( i->first == "DATE" )
				{
					stov(sProperty, vsDate, "");
				}
				else if ( i->first == "COMPOSER" )
				{
					stov(sProperty, vsComposerWriter);
				}
				else if ( i->first == "LABEL" )
				{
					stov(sProperty, vsStudio);
				}
				else if ( i->first == "TOTALDISCS" )
				{
					stov(sProperty, vsTotalDiscs, "");
				}
				else if ( i->first == "DISCNUMBER" )
				{
					// TODO: split this properly when it comes in as 'x/y' or 'x of y'
					stov(sProperty, vsDisc, "");
				}
				else if ( i->first == "COMMENT" )
				{
					stov(sProperty, vsSynopsis, "");
				}
			}
		}
		stov(f->tag()->artist().to8Bit(true), vsPerformer);
		if ( !vsAlbumArtist.size() && vsPerformer.size() )
		{
			// if no Album Artist, use the first Artist/Performer that was added
			stov( *vsPerformer.begin(), vsAlbumArtist );
		}

		stov(f->tag()->title().to8Bit(true), vsTitle);
		stov(f->tag()->genre().to8Bit(true), vsGenre);
		stov(f->tag()->album().to8Bit(true), vsAlbum);

		if(f->tag()->track() > 0)
			stov( StringUtils::ltos( f->tag()->track() ), vsTrack);
		if(f->tag()->year() > 0)
			stov( StringUtils::ltos( f->tag()->year() ), vsDate);

		// assign compiled strings to attributes
		mapAttributes[ATTRIBUTETYPE_Album_Artist_CONST] = vtos( vsAlbumArtist );
		mapAttributes[ATTRIBUTETYPE_Performer_CONST] = vtos( vsPerformer );
		mapAttributes[ATTRIBUTETYPE_Title_CONST] = vtos( vsTitle );
		mapAttributes[ATTRIBUTETYPE_Genre_CONST] = vtos( vsGenre );
		mapAttributes[ATTRIBUTETYPE_Album_CONST] = vtos( vsAlbum );
		mapAttributes[ATTRIBUTETYPE_Track_CONST] = vtos( vsTrack );
		mapAttributes[ATTRIBUTETYPE_Release_Date_CONST] = vtos( vsDate );
		mapAttributes[ATTRIBUTETYPE_ComposerWriter_CONST] = vtos( vsComposerWriter );
		mapAttributes[ATTRIBUTETYPE_Studio_CONST] = vtos( vsStudio );
		mapAttributes[ATTRIBUTETYPE_Number_of_Discs_CONST] = vtos( vsTotalDiscs );
		mapAttributes[ATTRIBUTETYPE_Disc_ID_CONST] = vtos( vsDisc );
		mapAttributes[ATTRIBUTETYPE_Synopsis_CONST] = vtos( vsSynopsis );

		// Pictures need to be handled differently depending on file type
		// is it a FLAC file? access pics like this
		if ( TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>( f->file()) )
		{
			cout << "FLAC FLAC FLAC" << endl;

			TagLib::List<TagLib::FLAC::Picture*>picList = flacFile->pictureList();
			if ( !picList.isEmpty())
			{
				for (TagLib::List<TagLib::FLAC::Picture*>::ConstIterator it = picList.begin(); it != picList.end(); ++it)
				{
					TagLib::FLAC::Picture *pic = *it;

					//TagLib::FLAC::Picture::Type picType = pic->type();
					if ( pic->type() == TagLib::FLAC::Picture::FrontCover)
					{
						int iType = (int)pic->type();
						cout << "Picture found of type: " << iType << endl;

						if ( pic->mimeType() == "image/jpeg")
						{
							// get the image
							TagLib::ByteVector picData = pic->data();

							size_t nBinSize = (size_t)picData.size();
							char *pPictureData = new char[nBinSize];
							memcpy(pPictureData, picData.data(), nBinSize);

							cout << "Picture is 'image/jpeg' and is " << nBinSize << " bytes in size." << endl;

							// the following adds this image to the lmce picture vector
							listPictures.push_back(make_pair(pPictureData, nBinSize));
						}
					}
				}
			}
		}

		// is it an MPEG file? access pics like this
		else if ( TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>( f->file()) )
		{
			cout << "MPEG MPEG MPEG" << endl;

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
						if ( picFrame->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover)
					        {
							int iType = (int)picFrame->type();
							cout << "Picture found of type: " << iType << endl;

							if ( picFrame->mimeType() == "image/jpeg")
							{
								TagLib::ByteVector picData = picFrame->picture();

								size_t nBinSize = (size_t)picData.size();
								char *pPictureData = new char[nBinSize];
								memcpy(pPictureData, picData.data(), nBinSize);

								cout << "Picture is 'image/jpeg' and is " << nBinSize << " bytes in size." << endl;

								// the following adds this image to the lmce picture vector
								listPictures.push_back(make_pair(pPictureData, nBinSize));
							}
						}
					}
				}
			}
		}

		// is it an MP4 (AAC, ALAC, video) file? access pics like this
		else if ( TagLib::MP4::File* mp4File = dynamic_cast<TagLib::MP4::File*>( f->file()) )
		{
			cout << "MP4 MP4 MP4" << endl;
/*
			TagLib::MP4::Tag* tag = mp4File->tag();
			TagLib::MP4::ItemListMap itemListMap = tag->itemListMap();
			TagLib::MP4::Item coverItem = itemListMap["covr"];
			TagLib::MP4::CoverArtList coverArtList = coverItem.toCoverArtList();

			if ( !coverArtList.isEmpty() )
			{
				for(TagLib::MP4::CoverArtList::ConstIterator it = coverArtList.begin(); it != coverArtList.end(); ++it)
				{
					TagLib::MP4::CoverArt coverArt = *it;

					cout << "Picture found of type: 'covr'" << endl;
					if ( coverArt.format() == TagLib::MP4::CoverArt::JPEG )
				        {

						TagLib::ByteVector picData = coverArt.data();

						size_t nBinSize = (size_t)picData.size();
						char *pPictureData = new char[nBinSize];
						memcpy(pPictureData, picData.data(), nBinSize);

						cout << "Picture is 'image/jpeg' and is " << nBinSize << " bytes in size." << endl;

						// the following adds this image to the lmce picture vector
//						listPictures.push_back(make_pair(pPictureData, nBinSize));
					}
				}
			}
*/
		}

/*
		// is it an ASF (wma) file? access pics like this
		else if ( TagLib::ASF::File* asfFile = dynamic_cast<TagLib::ASF::File*>( f->file()) )
		{
			cout << "ASF ASF ASF" << endl;
		}
*/

		// get pic from a .jpg in the same directory
		string coverfilename = m_sFullFilename.substr(0,m_sFullFilename.find_last_of("/\\")) + "/cover.jpg";
		if ( !FileUtils::FileExists( coverfilename ) )
			coverfilename = m_sFullFilename.substr(0,m_sFullFilename.find_last_of("/\\")) + "/folder.jpg";
		if ( !FileUtils::FileExists( coverfilename ) )
			coverfilename = m_sFullFilename.substr(0,m_sFullFilename.find_last_of("/\\")) + "/folder2.jpg";
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
void TagFileHandler::InsertTagValues(TagLib::FileRef *&f, string sName, string sParameters)
{
	std::vector<string> vsParameters;
	stov(sParameters, vsParameters);

	for (std::vector<string>::iterator it = vsParameters.begin(); it != vsParameters.end(); ++it)
	{
		f->file()->properties().insert(sName, String(*it, String::UTF8));
	}
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::SetTagInfo(string sFilename, const map<int,string>& mapAttributes, const list<pair<char *, size_t> >& listPictures)
{
	FileRef *f = new FileRef(m_sFullFilename.c_str());
	if(NULL != f)
	{
		string sParameters;
		std::vector<string> vsParameters;

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Album_Artist_CONST);
		InsertTagValues(f, string("ALBUMARTIST"), sParameters);

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Performer_CONST);
		InsertTagValues(f, string("ARTIST"), sParameters);
		f->tag()->setArtist(String(sParameters, String::UTF8));

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Title_CONST);
		InsertTagValues(f, string("TITLE"), sParameters);
		f->tag()->setTitle(String(sParameters, String::UTF8));

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Genre_CONST);
		InsertTagValues(f, string("GENRE"), sParameters);
		f->tag()->setGenre(String(sParameters, String::UTF8));

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Album_CONST);
		InsertTagValues(f, string("ALBUM"), sParameters);
		f->tag()->setAlbum(String(sParameters, String::UTF8));

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Track_CONST);
		InsertTagValues(f, string("TRACKNUMBER"), sParameters);
		f->tag()->setTrack(atoi(sParameters.c_str()));

		sParameters=ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Release_Date_CONST);
		InsertTagValues(f, string("DATE"), sParameters);
		f->tag()->setYear(atoi(sParameters.c_str()));

		f->save();
		delete f;
	}
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::RemoveTagValue(TagLib::FileRef *&f, const string sName, string sValue)
{
	TagLib::PropertyMap::Iterator tag = f->file()->properties().find(sName);
	if ( tag != f->file()->properties().end() )
	{
/*
	// FIXME: the following line segfaults. Perhaps an empty value? Needs investigation.
		TagLib::StringList::Iterator value = tag->second.find(sValue);
		if ( value != tag->second.end() )
		{
			*tag->second.erase( value );

			if ( !tag->second.size() )
			{
				f->file()->properties().erase(sName);
			}
		}
*/
	}
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::RemoveTag(string sFilename, int nTagType, string sValue)
{
	FileRef *f = new FileRef(m_sFullFilename.c_str());
	if(NULL != f)
	{
		switch(nTagType)
		{
			case ATTRIBUTETYPE_Album_Artist_CONST:
				RemoveTagValue(f, "ALBUMARTIST", sValue);
				RemoveTagValue(f, "ALBUM ARTIST", sValue);
				break;

			case ATTRIBUTETYPE_Performer_CONST:
				RemoveTagValue(f, "ARTIST", sValue);
				if(f->tag()->artist() == sValue)
					f->tag()->setArtist("");
				break;

			case ATTRIBUTETYPE_Title_CONST:
				RemoveTagValue(f, "TITLE", sValue);
				if(f->tag()->title() == sValue)
					f->tag()->setTitle("");
				break;

			case ATTRIBUTETYPE_Genre_CONST:
				RemoveTagValue(f, "GENRE", sValue);
				if(f->tag()->genre() == sValue)
					f->tag()->setGenre("");
				break;

			case ATTRIBUTETYPE_Album_CONST:
				RemoveTagValue(f, "ALBUM", sValue);
				if(f->tag()->album() == sValue)
					f->tag()->setAlbum("");
				break;

			case ATTRIBUTETYPE_Track_CONST:
				RemoveTagValue(f, "TRACKNUMBER", sValue);
				if(f->tag()->track() == atoi(sValue.c_str()))
					f->tag()->setTrack(0);
				break;

			case ATTRIBUTETYPE_Release_Date_CONST:
				RemoveTagValue(f, "DATE", sValue);
				if(f->tag()->year() == atoi(sValue.c_str()))
					f->tag()->setYear(0);
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
