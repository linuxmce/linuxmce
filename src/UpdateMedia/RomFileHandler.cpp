/**
  ROM File Handler for UpdateMedia.

  Basically, looks for any files in the Games folder, and
  anything that matches .zip, it will try to match it, and
  grab metadata for it from the lmce_game database.

  Author: Thom Cherryhomes <thomas@localeconcept.com>
  Version: 1.0

*/

#include "PlutoMediaAttributes.h"
#include "FileUtils/file_utils.h"
#include "DCE/Logger.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_media/Define_AttributeType.h"
#include "RomFileHandler.h"
using namespace DCE;

#include <cstdlib>

#include <tag.h>
#include <fileref.h>
//-----------------------------------------------------------------------------------------------------
namespace UpdateMediaVars
{
	extern string sUPnPMountPoint;
	extern string sLocalUPnPServerName;
};
//-----------------------------------------------------------------------------------------------------

RomFileHandler::RomFileHandler(string sDirectory, string sFile) :
	GenericFileHandler(sDirectory, sFile)
{

  m_pGAMEROM = NULL;

}

//-----------------------------------------------------------------------------------------------------
RomFileHandler::~RomFileHandler(void)
{
  m_pGAMEROM = NULL;
}
//-----------------------------------------------------------------------------------------------------



bool RomFileHandler::LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, 
		list<pair<char *, size_t> >& listPicturesForTags)
{

	string sFileWithAttributes = m_sFullFilename;
	string sROMName = FileUtils::FilenameWithoutPath(sFileWithAttributes,false);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# RomFileHandler::LoadAttributes: loading %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), sFileWithAttributes.c_str());

	//get common tag attributes
	map<int, string> mapAttributes;
	GetRomInfo(sROMName, mapAttributes, listPicturesForTags);	

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: tag attributes loaded (from tag file - common tags) %d", 
		mapAttributes.size());

	//merge attributes
	for(map<int, string>::iterator it = mapAttributes.begin(), end = mapAttributes.end(); it != end; ++it)
	{
		int nType = it->first;
		string sValue = it->second;

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
void RomFileHandler::GetRomInfo(string sFilename, map<int,string>& mapAttributes, list<pair<char *, size_t> >& listPictures)
{

  string sROMTitle, sROMYear, sROMManufacturer, sROMGenre;
  string sROMName = FileUtils::FilenameWithoutPath(sFilename,false);
  m_pGAMEROM = new(GAMEROM);
  m_pGAMEROM->InitDatabase();

  // Use MAMEROM to grab text attributes.

  sROMTitle = m_pGAMEROM->getTitleForROM(sROMName);
  sROMYear = m_pGAMEROM->getYearForROM(sROMName);
  sROMManufacturer = m_pGAMEROM->getManufacturerForROM(sROMName);
  sROMGenre = m_pGAMEROM->getGenreForROM(sROMName);

  if (sROMTitle != "NOMATCH")
    mapAttributes[ATTRIBUTETYPE_Title_CONST] = sROMTitle;

  if (sROMYear != "NOMATCH")
    mapAttributes[ATTRIBUTETYPE_Year_CONST] = sROMYear;

  if (sROMManufacturer != "NOMATCH")
    mapAttributes[ATTRIBUTETYPE_Manufacturer_CONST] = sROMManufacturer;

  if (sROMGenre != "NOMATCH")
    mapAttributes[ATTRIBUTETYPE_Genre_CONST] = sROMGenre;

  // Grab Cover art.

  string sSnapfilename = "/home/snap/" + sROMName + ".jpeg";
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "# RomFileHandler: Adding ROM Picture: %s",sSnapfilename.c_str());

  size_t nSnapSize = 0;
  char *pSnapData;
  pSnapData = FileUtils::ReadFileIntoBuffer(sSnapfilename,nSnapSize);
  if (nSnapSize > 0)
    listPictures.push_back(make_pair(pSnapData,nSnapSize));

  delete(m_pGAMEROM);
  m_pGAMEROM = NULL;

}

//-----------------------------------------------------------------------------------------------------
bool RomFileHandler::SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes)
{
  // No save attributes at the moment.
}

//-----------------------------------------------------------------------------------------------------
bool RomFileHandler::RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes)
{
  // We do not remove attributes from files at the moment.
}

//-----------------------------------------------------------------------------------------------------
string RomFileHandler::GetFileAttribute()
{
	return m_sFullFilename;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool RomFileHandler::IsSupported()
{
	const string csSupportedExtensions("zip");
	string sExtension = StringUtils::ToLower(FileUtils::FindExtension(m_sFullFilename));

	if(sExtension.empty())
		return false;

    return csSupportedExtensions.find(sExtension) != string::npos;
}
//-----------------------------------------------------------------------------------------------------
bool RomFileHandler::FileAttributeExists()
{
	return true;
}
//-----------------------------------------------------------------------------------------------------
string RomFileHandler::GetFileSourceForDB()
{
	if(!UpdateMediaVars::sUPnPMountPoint.empty() && StringUtils::StartsWith(m_sDirectory, UpdateMediaVars::sUPnPMountPoint))
		return "U";
	else 
		return "F";
}

//-----------------------------------------------------------------------------------------------------
void RomFileHandler::SetTagInfo(string sFilename, const map<int,string>& mapAttributes, const list<pair<char *, size_t> >& listPictures)
{
  // We do not modify the game database, currently.
}
//-----------------------------------------------------------------------------------------------------
void RomFileHandler::RemoveTag(string sFilename, int nTagType, string sValue) {
  // We do not modify the game database, currently.
}
//-----------------------------------------------------------------------------------------------------
string RomFileHandler::ExtractAttribute(const map<int,string>& mapAttributes, int key)
{
	map<int,string>::const_iterator it = mapAttributes.find(key);
	if(it != mapAttributes.end())
		return it->second;

	return "";
}
//-----------------------------------------------------------------------------------------------------
