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
#include "PlutoUtils/ProcessUtils.h"
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

RomFileHandler::RomFileHandler(string sDirectory, string sFile, int iRomType) :
	ID3FileHandler(sDirectory, sFile)
{
	m_iRomType = iRomType;
}

//-----------------------------------------------------------------------------------------------------
RomFileHandler::~RomFileHandler(void)
{
}
//-----------------------------------------------------------------------------------------------------



bool RomFileHandler::LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, 
		list<pair<char *, size_t> >& listPicturesForTags)
{
  
	string sFileWithAttributes = m_sFullFilename;
	string sROMName = FileUtils::FilenameWithoutPath(sFileWithAttributes,false);

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# RomFileHandler::LoadAttributes: loading %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), sFileWithAttributes.c_str());
#endif
	//get common tag attributes
	map<int, string> mapAttributes;
	GetRomInfo(sROMName, mapAttributes, listPicturesForTags);	

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: tag attributes loaded (from tag file - common tags) %d", 
		mapAttributes.size());
#endif
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
  
  // Pass it back once we have figured out the ROM attributes here.
  return ID3FileHandler::LoadAttributes(pPlutoMediaAttributes, listPicturesForTags);
}

//-----------------------------------------------------------------------------------------------------
bool RomFileHandler::SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes)
{
  // Call superclass.
  return ID3FileHandler::SaveAttributes(pPlutoMediaAttributes);
}

//-----------------------------------------------------------------------------------------------------
bool RomFileHandler::RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes)
{
  // We do not remove attributes from files at the moment.
  return ID3FileHandler::RemoveAttribute(nTagType, sValue, pPlutoMediaAttributes);
}

//-----------------------------------------------------------------------------------------------------
string RomFileHandler::GetFileAttribute()
{
  return ID3FileHandler::GetFileAttribute();
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
  return ID3FileHandler::FileAttributeExists();
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
  //ID3FileHandler::SetTagInfo(sFilename, mapAttributes, listPictures);
}
//-----------------------------------------------------------------------------------------------------
void RomFileHandler::RemoveTag(string sFilename, int nTagType, string sValue) {
  // We do not modify the game database, currently.
  //ID3FileHandler::RemoveTag(sFilename, nTagType, sValue);
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

//-----------------------------------------------------------------------------------------------------
void RomFileHandler::getMAMEData(string sRomName)
{
	string sOutput, sStdErr;
	char csRomName[100];
	strcpy(csRomName,sRomName.c_str());
	char * const args[] = {"/usr/pluto/bin/GAMEROM",csRomName,NULL};
	if (ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr) == 0)
	{
		vector<string> vectOutput_Rows;
		vector<string>::iterator it;
		StringUtils::Tokenize(sOutput,"|",vectOutput_Rows);
		if (vectOutput_Rows.size() == 5)
		{
			if (vectOutput_Rows[0] != "NOMATCH")
			{
				m_sROMTitle = vectOutput_Rows[0];
			}
			else
			{
				m_sROMTitle.clear();
			}
			if (vectOutput_Rows[1] != "NOMATCH")
			{
				m_sROMYear = vectOutput_Rows[1];
			}
			else
			{
				m_sROMYear.clear();
			}
			if (vectOutput_Rows[2] != "NOMATCH")
			{
				m_sROMManufacturer = vectOutput_Rows[2];
			}
			else
			{
				m_sROMManufacturer.clear();
			}
			if (vectOutput_Rows[3] != "NOMATCH")
			{
				m_sROMGenre = vectOutput_Rows[3];
			}
			else
			{
				m_sROMGenre.clear();
			}
			if (vectOutput_Rows[4] != "NOMATCH")
			{
				m_sROMSubtitle=vectOutput_Rows[4];
			}
			else
			{
				m_sROMSubtitle.clear();
			}
			m_sROMSystem = "Arcade"; // FIXME: deal with this?
		}
	}

}
//-----------------------------------------------------------------------------------------------------
void RomFileHandler::getCoweringData(string sRomName)
{
	m_sROMTitle.clear();
	m_sROMYear.clear();
	m_sROMManufacturer.clear();
	m_sROMGenre.clear();
	m_sROMSystem.clear();
	m_sROMSubtitle.clear();

	// Cowering format ROM data is of the format: A Game Title (Year) (Manufacturer) [c] [c] [c]
	// where [c] are ancilliary codes not currently parsed. 
	
	// Assume that the first parenthesis ends the title.
	int iTitleEnd = sRomName.find("(");

	if (iTitleEnd == string::npos) 
	{
	  m_sROMTitle = sRomName;
	} 
	else 
	{
	    m_sROMTitle = sRomName.substr(0,iTitleEnd);
	    m_sROMTitle = trim(m_sROMTitle);
	}


	// Assume that (1 or (2 is the start of a year, with the ) being the end delimiter.
	int iYearStart = sRomName.find("(19");
	int iYearEnd;

	if (iYearStart == string::npos) 
	{
		iYearStart = sRomName.find("(20");
	}

	if (iYearStart == string::npos)
	{
	  //cout << "No Year was found." << endl;
	} else 
	{
		iYearEnd = sRomName.find(")");
		m_sROMYear = sRomName.substr(iYearStart+1,iYearEnd-iYearStart-1);
		//cout << "Year parsed to: " << sYear << endl;
	}

	// Attempt to find manufacturer, and weed out memory size specifications.
	// FIXME: Are there roms that are Berzerk (Bally Midway).rom ???
	if (iYearEnd != string::npos) 
	{
		int iMemorySize = sRomName.find("k)",iYearEnd); // FIXME: Manufacturers that end in k?
		if (iMemorySize != string::npos) 
		{
		  //cout << "Possible memory size detection!" << endl;
			string sD = sRomName.substr(iMemorySize-1,1);
			//cout << "Character before the k: " << sD << endl;
			if (sD == "0" || sD == "1" || sD == "2" || sD == "3" || sD == "4" || sD == "5" ||  
					sD == "6" || sD == "7" || sD == "8" || sD == "9") // Yeah yeah... 
			{
			  //cout << "Memory Size detected, ignoring." << endl;
			} 
			else
			{
				// Not a memory size, let's fall back.
			}
		}
		else
		{
			int iManufacturerStart = sRomName.find("(",iYearEnd);
			if (iManufacturerStart != string::npos) 
			{
				int iManufacturerEnd = sRomName.find(")",iManufacturerStart);
				m_sROMManufacturer = sRomName.substr(iManufacturerStart+1,iManufacturerEnd-iManufacturerStart-1);
				//cout << "Manufacturer Parsed to: " << sManufacturer << endl;
			}
		}
	}

}
//-----------------------------------------------------------------------------------------------------
void RomFileHandler::getSystem(string sFilename)
{
	m_sROMSystem = "NOMATCH";

	if (sFilename.find("/a2600") != string::npos ) 
		m_sROMSystem = "Atari 2600";
	if (sFilename.find("/a5200") != string::npos )
		m_sROMSystem = "Atari 5200";
	if (sFilename.find("/a7800") != string::npos )
		m_sROMSystem = "Atari 7800";
	if (sFilename.find("/coleco") != string::npos )
		m_sROMSystem = "ColecoVision";
	if (sFilename.find("/intv") != string::npos )
		m_sROMSystem = "Intellivision";
	if (sFilename.find("/sg1000") != string::npos )
		m_sROMSystem = "Sega SG-1000";
	if (sFilename.find("/sms") != string::npos )
		m_sROMSystem = "Sega Master System";
	if (sFilename.find("/nes") != string::npos )
		m_sROMSystem = "Nintendo Entertainment System";
	if (sFilename.find("/famicom") != string::npos )
		m_sROMSystem = "Nintendo Famicom";
	if (sFilename.find("/snes") != string::npos )
		m_sROMSystem = "Super Nintendo Entertainment System";
	if (sFilename.find("/genesis") != string::npos )
		m_sROMSystem = "Sega Genesis";
	if (sFilename.find("/megadriv") != string::npos )
		m_sROMSystem = "Sega MegaDrive";
	if (sFilename.find("/vectrex") != string::npos )
		m_sROMSystem = "GCE Vectrex";
	if (sFilename.find("/tg16") != string::npos )
		m_sROMSystem = "TurboGrafx-16";
	if (sFilename.find("/pce") != string::npos )
		m_sROMSystem = "PC Engine";
	if (sFilename.find("/sgx") != string::npos )
		m_sROMSystem = "SuperGrafx";
	if (sFilename.find("/vectrex") != string::npos )
		m_sROMSystem = "Vectrex";
	if (sFilename.find("/apple2") != string::npos )
		m_sROMSystem = "Apple ][";
	if (sFilename.find("/jaguar") != string::npos )
		m_sROMSystem = "Atari Jaguar";
	if (sFilename.find("/vic20") != string::npos)
		m_sROMSystem = "Commodore VIC-20";
	if (sFilename.find("/c64") != string::npos)
		m_sROMSystem = "Commodore 64";
	if (sFilename.find("/a800") != string::npos || 
	    sFilename.find("/a130xe") != string::npos)
	  m_sROMSystem = "Atari 800";
	if (sFilename.find("/ti99") != string::npos ||
	sFilename.find(".rpk") != string::npos)
	  m_sROMSystem = "TI 99/4A";

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"RomFileHandler::getSystem(%s) = %s",sFilename.c_str(),m_sROMSystem.c_str());

}
//-----------------------------------------------------------------------------------------------------
void RomFileHandler::GetRomInfo(string sFilename, map<int,string>& mapAttributes, list<pair<char *, size_t> >& listPictures)
{

  string sSnapFilename, sROMSystem, sROMTitle, sROMSubtitle, sROMYear, sROMManufacturer, sROMGenre;
  string sROMName = FileUtils::FilenameWithoutPath(sFilename,false);

  // Use MAMEROM to grab text attributes.

  switch (m_iRomType)
  {
  	case ROMTYPE_DEFAULT:
		getMAMEData(sROMName);
		break;
	case ROMTYPE_COWERING:
		getCoweringData(sROMName);
		getSystem(m_sFullFilename); // The system is part of the path. 
		break;
  }

  sROMTitle = m_sROMTitle;
  sROMYear =  m_sROMYear;
  sROMManufacturer = m_sROMManufacturer;
  sROMGenre = m_sROMGenre;
  sROMSystem = m_sROMSystem;
  sROMSubtitle = m_sROMSubtitle;

  if (!sROMTitle.empty())
    mapAttributes[ATTRIBUTETYPE_Title_CONST] = sROMTitle;

  if (!sROMSubtitle.empty())
    mapAttributes[ATTRIBUTETYPE_Episode_CONST] = sROMSubtitle;

  if (!sROMYear.empty())
    mapAttributes[ATTRIBUTETYPE_Year_CONST] = sROMYear;

  if (!sROMManufacturer.empty())
    mapAttributes[ATTRIBUTETYPE_Manufacturer_CONST] = sROMManufacturer;

  if (!sROMGenre.empty())
    mapAttributes[ATTRIBUTETYPE_Genre_CONST] = sROMGenre;

  if (!sROMSystem.empty())
	  mapAttributes[ATTRIBUTETYPE_System_CONST] = sROMSystem;

  // Grab Cover art.

  if (m_iRomType == ROMTYPE_DEFAULT)
  {
  	sSnapFilename = "/home/snap/" + sROMName + ".jpeg";
  }

  if (m_iRomType == ROMTYPE_COWERING)
  {
	string sTmpFile = StringUtils::ToLower(sFilename);

	if (sTmpFile.find(".j64") != string::npos)
		sSnapFilename = "/home/snap/jaguar/"+sROMTitle+".jpg";

	if (m_sFullFilename.find("/vic20") != string::npos ||
		sTmpFile.find(".20") != string::npos || 
		sTmpFile.find(".40") != string::npos || 
		sTmpFile.find(".60") != string::npos ||
		sTmpFile.find(".70") != string::npos ||
		sTmpFile.find(".80") != string::npos || 
		sTmpFile.find(".a0") != string::npos ||
		sTmpFile.find(".b0") != string::npos)
		sSnapFilename = "/home/snap/vic20/"+sROMTitle+".jpg";

	if (m_sFullFilename.find("/c64") != string::npos || 
		sTmpFile.find(".prg") != string::npos ||
		sTmpFile.find(".p00") != string::npos ||
		sTmpFile.find(".t64") != string::npos ||
		sTmpFile.find(".g64") != string::npos ||
		sTmpFile.find(".d64") != string::npos ||
		sTmpFile.find(".crt") != string::npos)
		sSnapFilename = "/home/snap/c64/"+sROMTitle+".jpg";
	
	if (m_sFullFilename.find("/a800") != string::npos ||
            sTmpFile.find(".atr") != string::npos ||
	    sTmpFile.find(".xfd") != string::npos ||
	    sTmpFile.find(".rom") != string::npos)
	  sSnapFilename = "/home/snap/a800/"+sROMTitle+".jpg";

	if (m_sFullFilename.find("/apple2") != string::npos)
		sSnapFilename = "/home/snap/apple2/" + sROMTitle + ".jpg";

	if (sTmpFile.find(".dsk") != string::npos || sTmpFile.find(".do") != string::npos
		|| sTmpFile.find(".po") != string::npos || sTmpFile.find(".nib") != string::npos)
		sSnapFilename = "/home/snap/apple2/" + sROMTitle + ".jpg";

	if (m_sFullFilename.find("/ti99") != string::npos)
	  sSnapFilename = "/home/snap/ti99/" + sROMTitle + ".jpg";

  	if (m_sFullFilename.find("/a2600") != string::npos || sTmpFile.find(".a26") != string::npos)
		sSnapFilename = "/home/snap/a2600/" + sROMTitle + ".jpg";

	if (m_sFullFilename.find("/a5200") != string::npos || sTmpFile.find(".a52") != string::npos)
		sSnapFilename = "/home/snap/a5200/" + sROMTitle + ".jpg";

	if (m_sFullFilename.find("/a7800") != string::npos || sTmpFile.find(".a78") != string::npos)
		sSnapFilename = "/home/snap/a7800/" + sROMTitle + ".jpg";

	if (m_sFullFilename.find("/intv") != string::npos || sTmpFile.find(".int") != string::npos)
		sSnapFilename = "/home/snap/intv/" + sROMTitle + ".jpg";
	
	if (m_sFullFilename.find("/coleco") != string::npos || sTmpFile.find(".col") != string::npos)
		sSnapFilename = "/home/snap/coleco/" + sROMTitle + ".jpg";

	if (m_sFullFilename.find("/sg1000") != string::npos || sTmpFile.find(".SG") != string::npos)
		sSnapFilename = "/home/snap/sg1000/" + sROMTitle + ".jpg";

	if (m_sFullFilename.find("/sms") != string::npos || sTmpFile.find(".sms") != string::npos)
	       sSnapFilename = "/home/snap/sms/" + sROMTitle + ".jpg";

	if (m_sFullFilename.find("/nes") != string::npos || sTmpFile.find(".nes") != string::npos)
		sSnapFilename = "/home/snap/nes/" + sROMTitle + ".jpg";

	if (m_sFullFilename.find("/famicom") != string::npos || sTmpFile.find(".fam") != string::npos
							     || sTmpFile.find(StringUtils::ToLower(".fds")) != string::npos)
		sSnapFilename = "/home/snap/famicom/" + sROMTitle + ".jpg";


        if (m_sFullFilename.find("/snes") != string::npos || sTmpFile.find(".smc") != string::npos
							  || sTmpFile.find(".sfc") != string::npos
							  || sTmpFile.find(".fig") != string::npos
							  || sTmpFile.find(".swc") != string::npos)
                sSnapFilename = "/home/snap/snes/" + sROMTitle + ".jpg";

	if (m_sFullFilename.find("/genesis") != string::npos
		|| sTmpFile.find(".gen") != string::npos)
		sSnapFilename = "/home/snap/genesis/" +sROMTitle+".jpg";

	if (m_sFullFilename.find("/megadriv") != string::npos 
		|| sTmpFile.find(".smd") != string::npos
		|| sTmpFile.find(".md") != string::npos)
		sSnapFilename = "/home/snap/megadriv/"+sROMTitle+".jpg";

	if (m_sFullFilename.find("/tg16") != string::npos
		|| sTmpFile.find(".pce") != string::npos)
		sSnapFilename = "/home/snap/tg16/"+sROMTitle+".jpg";

	if (m_sFullFilename.find("/pce") != string::npos
		|| sTmpFile.find(".pce") != string::npos)
		sSnapFilename = "/home/snap/pce/"+sROMTitle+".jpg";

	if (m_sFullFilename.find("/sgx") != string::npos
		|| sTmpFile.find(".pce") != string::npos)
		sSnapFilename = "/home/snap/sgx/"+sROMTitle+".jpg";

	if (m_sFullFilename.find("/vectrex") != string::npos
		|| sTmpFile.find(".vec") != string::npos
		|| sTmpFile.find(".gam") != string::npos)
		sSnapFilename = "/home/snap/vectrex/"+sROMTitle+".jpg";

  }

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "# RomFileHandler: Adding ROM Picture: %s",sSnapFilename.c_str());

  size_t nSnapSize = 0;
  char *pSnapData;
  pSnapData = FileUtils::ReadFileIntoBuffer(sSnapFilename,nSnapSize);
  if (nSnapSize > 0)
    listPictures.push_back(make_pair(pSnapData,nSnapSize));

}
