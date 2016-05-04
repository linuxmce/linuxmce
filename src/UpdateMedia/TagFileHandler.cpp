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
//#include <tstring.h>
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
	//get common tag attributes
	map<int, string> mapAttributes;
	GetTagInfo(sFileWithAttributes, mapAttributes, listPicturesForTags);	
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: tag attributes loaded (from tag file - common tags) %d", 
		mapAttributes.size());
#endif
	//merge attributes
	for(map<int, string>::iterator it = mapAttributes.begin(), end = mapAttributes.end(); it != end; ++it)
	{
		int nType = it->first;
		string sValue = it->second;
// FIXME: Need to de-tokenize items (sValue) as attributes can hold multiple entries
// This becomes a little more complicated as we have to de-tokenize sValue 
// for comparasin(multiple if's?) and, if required, re-tokenize for saving(else)
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "# LoadAttributes, from file: type %d: '%s'", nType, sValue.c_str());

		MapPlutoMediaAttributes::iterator itm = pPlutoMediaAttributes->m_mapAttributes.find(nType);
		if(itm == pPlutoMediaAttributes->m_mapAttributes.end())
			pPlutoMediaAttributes->m_mapAttributes.insert(
				std::make_pair(
					nType, 
					new PlutoMediaAttribute(0,nType, sValue)
				)
			);
		else
			itm->second->m_sName = sValue;
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
// FIXME: Need to tokenize items as attributes can hold multiple entries
			mapAttributes[it->first] = it->second->m_sName;
		}


		LoggerWrapper::GetInstance()->Write(LV_WARNING, "# TagFileHandler::SaveAttributes: saving %d pictures into APIC tags to %s",
			pPlutoMediaAttributes->m_mapCoverarts.size(), sFileWithAttributes.c_str());

		list<pair<char *, size_t> > listPictures;
		for(MapPictures::iterator itc = pPlutoMediaAttributes->m_mapCoverarts.begin();
			itc != pPlutoMediaAttributes->m_mapCoverarts.end(); ++itc)
		{
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::SaveAttributes: saving into APIC picture size %d", itc->first);
			listPictures.push_back(make_pair(itc->second, itc->first));
#endif
		}

		//Save common tag tags
		SetTagInfo(sFileWithAttributes, mapAttributes, listPictures);
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
	const string csSupportedExtensions("ogg:flac:mp3");
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
string getAlbumArtist(string sFilename)
{

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

		// Get Album Artist, from file properties
		TagLib::PropertyMap tags = f->file()->properties();
		for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
			if ( (i->first == "ALBUMARTIST") || (i->first == "ALBUM ARTIST") )
			{
				for(TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j) {
					TagLib::String album_artist = *j;
// FIXME: Need to tokenize/de-tokenize items as mapAttributes[] can hold multiple entries
					mapAttributes[ATTRIBUTETYPE_Album_Artist_CONST] = album_artist.to8Bit(true);
					break;
				}
			}
		}

// FIXME: Need to de-tokenize/tokenize items as mapAttributes[] can hold multiple entries
		mapAttributes[ATTRIBUTETYPE_Performer_CONST] = f->tag()->artist().to8Bit(true);
		mapAttributes[ATTRIBUTETYPE_Title_CONST] = f->tag()->title().to8Bit(true);
// FIXME: Need to de-tokenize/tokenize items as mapAttributes[] can hold multiple entries
		mapAttributes[ATTRIBUTETYPE_Genre_CONST] = f->tag()->genre().to8Bit(true);
		mapAttributes[ATTRIBUTETYPE_Album_CONST] = f->tag()->album().to8Bit(true);

		if(f->tag()->track() > 0)
			mapAttributes[ATTRIBUTETYPE_Track_CONST] = StringUtils::ltos(f->tag()->track());

		if(f->tag()->year() > 0)
			mapAttributes[ATTRIBUTETYPE_Release_Date_CONST] = StringUtils::ltos(f->tag()->year());

		// TODO: FIXME: use routines from PlutoUtils
		//if (FileExists(BasePath(m_sFullFilename) + "/cover.jpg")) {
		//}

		string coverfilename = m_sFullFilename.substr(0,m_sFullFilename.find_last_of("/\\")) + "/cover.jpg";
		cout << "fetching cover: "<< coverfilename << "\n";
		FILE *coverart = fopen(coverfilename.c_str(), "rb");
		if (coverart != NULL) {
			fseek(coverart, 0, SEEK_END);
			size_t nBinSize = (size_t)ftell(coverart);
			if (nBinSize > 0) {
				rewind(coverart);
				char *pPictureData = new char[nBinSize];
				fread(pPictureData, 1,nBinSize,coverart);
				listPictures.push_back(make_pair(pPictureData, nBinSize));
			}
		}
		delete f;
	}
}
//-----------------------------------------------------------------------------------------------------
void TagFileHandler::SetTagInfo(string sFilename, const map<int,string>& mapAttributes, const list<pair<char *, size_t> >& listPictures)
{
	FileRef *f = new FileRef(m_sFullFilename.c_str());
	if(NULL != f)
	{
		// Set Album Artist, in properties
// FIXME: Need to de-tokenize items as properties can be specified multiple times with single entries in this tag type
		f->file()->properties().insert("ALBUMARTIST",String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Album_Artist_CONST), String::UTF8));
// FIXME: Need to de-tokenize items as properties can be specified multiple times with single entries in this tag type
		f->file()->properties().insert("ARTIST", String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Performer_CONST), String::UTF8));
		f->file()->properties().insert("TITLE", String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Title_CONST), String::UTF8));
// FIXME: Need to de-tokenize items as properties can be specified multiple times with single entries in this tag type
		f->file()->properties().insert("GENRE", String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Genre_CONST), String::UTF8));
		f->file()->properties().insert("TRACKNUMBER", String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Track_CONST), String::UTF8));
		f->file()->properties().insert("DATE", String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Release_Date_CONST), String::UTF8));

// No need to de-tokenize items as single attribute will hold currently tokenized values
		f->tag()->setArtist(String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Performer_CONST), String::UTF8));
		f->tag()->setTitle(String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Title_CONST), String::UTF8));
// No need to de-tokenize items as single attribute will hold currently tokenized values
		f->tag()->setGenre(String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Genre_CONST), String::UTF8));
		f->tag()->setAlbum(String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Album_CONST), String::UTF8));
		f->tag()->setTrack(atoi(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Track_CONST).c_str()));
		f->tag()->setYear(atoi(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Release_Date_CONST).c_str()));
		f->save();

		delete f;
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
// FIXME: Need to check specific value as multiple attributes of the same type can exist
				f->file()->properties().erase("ALBUMARTIST");
				f->file()->properties().erase("ALBUM ARTIST");
				break;
			case ATTRIBUTETYPE_Performer_CONST:
// FIXME: Need to add properties handling
// FIXME: Need to tokenize/de-tokenize items as attribute can hold multiple entries
				if(f->tag()->artist() == sValue)
					f->tag()->setArtist("");
				break;

			case ATTRIBUTETYPE_Title_CONST:
// FIXME: Need to add properties handling
				if(f->tag()->title() == sValue)
					f->tag()->setTitle("");
				break;

			case ATTRIBUTETYPE_Genre_CONST:
// FIXME: Need to tokenize/de-tokenize items as attribute can hold multiple entries
				if(f->tag()->genre() == sValue)
					f->tag()->setGenre("");
				break;

			case ATTRIBUTETYPE_Album_CONST:
// FIXME: Need to add properties handling
				if(f->tag()->album() == sValue)
					f->tag()->setAlbum("");
				break;

			case ATTRIBUTETYPE_Track_CONST:
// FIXME: Need to add properties handling
				if(f->tag()->track() == atoi(sValue.c_str()))
					f->tag()->setTrack(0);
				break;

			case ATTRIBUTETYPE_Release_Date_CONST:
// FIXME: Need to add properties handling
				if(f->tag()->year() == atoi(sValue.c_str()))
					f->tag()->setYear(0);
				break;
		}

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
