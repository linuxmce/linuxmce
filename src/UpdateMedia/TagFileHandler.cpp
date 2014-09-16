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
	const string csSupportedExtensions("ogg:flac");
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
void TagFileHandler::GetTagInfo(string sFilename, map<int,string>& mapAttributes, list<pair<char *, size_t> >& listPictures)
{
	FileRef *f = new FileRef(m_sFullFilename.c_str());
	if(!f->isNull()) //ensure tag is present before trying to read and data.
	{
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# TagFileHandler::GetTagInfo: tags present");
#endif
		mapAttributes[ATTRIBUTETYPE_Performer_CONST] = f->tag()->artist().to8Bit(true);
		mapAttributes[ATTRIBUTETYPE_Title_CONST] = f->tag()->title().to8Bit(true);
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
		f->tag()->setArtist(String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Performer_CONST), String::UTF8));
		f->tag()->setTitle(String(ExtractAttribute(mapAttributes, ATTRIBUTETYPE_Title_CONST), String::UTF8));
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
			case ATTRIBUTETYPE_Performer_CONST:
				if(f->tag()->artist() == sValue)
					f->tag()->setArtist("");
				break;

			case ATTRIBUTETYPE_Title_CONST:
				if(f->tag()->title() == sValue)
					f->tag()->setTitle("");
				break;

			case ATTRIBUTETYPE_Genre_CONST:
				if(f->tag()->genre() == sValue)
					f->tag()->setGenre("");
				break;

			case ATTRIBUTETYPE_Album_CONST:
				if(f->tag()->album() == sValue)
					f->tag()->setAlbum("");
				break;

			case ATTRIBUTETYPE_Track_CONST:
				if(f->tag()->track() == atoi(sValue.c_str()))
					f->tag()->setTrack(0);
				break;

			case ATTRIBUTETYPE_Release_Date_CONST:
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
