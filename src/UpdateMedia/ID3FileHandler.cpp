#include "ID3FileHandler.h"
#include "id3info/id3info.h"
#include "PlutoMediaAttributes.h"
#include "FileUtils/file_utils.h"
#include "DCE/Logger.h"
#include "pluto_main/Table_MediaType.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
namespace UpdateMediaVars
{
	extern string sUPnPMountPoint;
	extern string sLocalUPnPServerName;
};
//-----------------------------------------------------------------------------------------------------
ID3FileHandler::ID3FileHandler(string sDirectory, string sFile) :
	GenericFileHandler(sDirectory, sFile)
{
}
//-----------------------------------------------------------------------------------------------------
ID3FileHandler::~ID3FileHandler(void)
{
}
//-----------------------------------------------------------------------------------------------------
bool ID3FileHandler::LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes,
									list<pair<char *, size_t> >& listPicturesForTags)
{
	string sFileWithAttributes = m_sDirectory + "/" + FileWithAttributes(pPlutoMediaAttributes, false);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# ID3FileHandler::LoadAttributes: loading %d attributes in the attribute file %s",
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

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: pluto attributes loaded (from id3 file - general object tag) %d", 
		pPlutoMediaAttributes->m_mapAttributes.size());

	//get common id3 attributes
	map<int, string> mapAttributes;
	GetId3Info(sFileWithAttributes, mapAttributes, listPicturesForTags);	

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: id3 attributes loaded (from id3 file - common tags) %d", 
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
bool ID3FileHandler::SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes)
{
	string sFileWithAttributes = m_sDirectory + "/" + FileWithAttributes(pPlutoMediaAttributes, true);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# ID3FileHandler::SaveAttributes: saving %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), sFileWithAttributes.c_str());

	//Temporary map with attributes for common tags
	map<int, string> mapAttributes;
	for(MapPlutoMediaAttributes::iterator it = pPlutoMediaAttributes->m_mapAttributes.begin(), 
		end = pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
	{
		mapAttributes[it->first] = it->second->m_sName;
	}


	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# ID3FileHandler::SaveAttributes: saving %d pictures into APIC tags to %s",
		pPlutoMediaAttributes->m_mapCoverarts.size(), sFileWithAttributes.c_str());

	list<pair<char *, size_t> > listPictures;
	for(MapPictures::iterator itc = pPlutoMediaAttributes->m_mapCoverarts.begin();
		itc != pPlutoMediaAttributes->m_mapCoverarts.end(); ++itc)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# ID3FileHandler::SaveAttributes: saving into APIC picture size %d", itc->first);
		listPictures.push_back(make_pair(itc->second, itc->first));
	}

	//Save common id3 tags
	SetId3Info(sFileWithAttributes, mapAttributes, listPictures);
	listPictures.clear();

	//Save user defined text
	char *pDataCurrentPosition = NULL;
	char *pDataStartPosition = NULL;
	unsigned long ulSize = 0;

	pPlutoMediaAttributes->SerializeWrite();
	ulSize = pPlutoMediaAttributes->CurrentSize();
	pDataCurrentPosition = pDataStartPosition = pPlutoMediaAttributes->m_pcDataBlock;

	size_t Size = ulSize;
	SetUserDefinedInformation(sFileWithAttributes, pDataStartPosition, Size);
	pPlutoMediaAttributes->FreeSerializeMemory();

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool ID3FileHandler::RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes)
{
	string sFileWithAttributes = m_sDirectory + "/" + FileWithAttributes(pPlutoMediaAttributes, true);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# ID3FileHandler::RemoveAttribute: removing %s type %d attribute from the attribute file %s",
		sValue.c_str(), nTagType, sFileWithAttributes.c_str());

	RemoveId3Tag(sFileWithAttributes, nTagType, sValue);
	return true;
}
//-----------------------------------------------------------------------------------------------------
string ID3FileHandler::FileWithAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, 
										  bool bCreateId3File)
{
	//no id3 file if the media file doesn't exist anymore.
	if(!FileUtils::FileExists(m_sFullFilename))
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# No id3 file. The media file doesn't exist anymore.");
		return "";
	}

	string sFileWithAttributes = m_sFile;
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
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# Won't create id3 file (the media file doesn't have attributes)");
			return "";
		}

		if(!FileUtils::DirExists(m_sDirectory + "/" + sFileWithAttributes))
			FileUtils::WriteTextFile(m_sDirectory + "/" + sFileWithAttributes, ""); //touch it
	}

	return sFileWithAttributes;
}
//-----------------------------------------------------------------------------------------------------
string ID3FileHandler::GetFileAttribute()
{
	return FileWithAttributes(NULL, false);
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool ID3FileHandler::IsSupported()
{
	const string csSupportedExtensions("mp3");
	string sExtension = StringUtils::ToLower(FileUtils::FindExtension(m_sFullFilename));

	if(sExtension.empty())
		return false;

    return csSupportedExtensions.find(sExtension) != string::npos;
}
//-----------------------------------------------------------------------------------------------------
bool ID3FileHandler::FileAttributeExists()
{
	return IsSupported() || FileUtils::FileExists(m_sFullFilename + ".id3");
}
//-----------------------------------------------------------------------------------------------------
string ID3FileHandler::GetFileSourceForDB()
{
	if(!UpdateMediaVars::sUPnPMountPoint.empty() && StringUtils::StartsWith(m_sDirectory, UpdateMediaVars::sUPnPMountPoint))
		return "U";
	else 
		return "F";
}
//-----------------------------------------------------------------------------------------------------
