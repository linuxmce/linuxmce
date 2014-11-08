#include "VDRInfoFileHandler.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "pluto_media/Define_AttributeType.h"
#include "PlutoMediaAttributes.h"

#include "DCE/Logger.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
VDRInfoFileHandler::VDRInfoFileHandler(string sDirectory, string sFile) :
	GenericFileHandler(sDirectory, sFile)
{
}
//-----------------------------------------------------------------------------------------------------
VDRInfoFileHandler::~VDRInfoFileHandler(void)
{
}
//-----------------------------------------------------------------------------------------------------
bool VDRInfoFileHandler::LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes,
	list<pair<char *, size_t> >& /*listPicturesForTags*/)
{
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# VDRInfoFileHandler::LoadAttributes: loading %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), GetFileAttribute().c_str());
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# VDRInfoFileHandler::LoadAttributes: loading %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), GetFileAttribute().c_str());
#endif
	string sData;
	FileUtils::ReadTextFile(GetFileAttribute(), sData);

	vector<string> vectLines;
	StringUtils::Tokenize(sData, "\n", vectLines);

	for(vector<string>::iterator it = vectLines.begin(), end = vectLines.end(); it != end; ++it)
	{
		string sLine = *it;

		int nAttributeType = 0;
		int iStart = 2;
		
		if(sLine.size() >= 2)
		{

			switch(sLine[0])
			{
				case 'C':
					nAttributeType = ATTRIBUTETYPE_Channel_CONST;
					break;

				case 'T':
					nAttributeType = ATTRIBUTETYPE_Title_CONST;
					break;

				case 'S':
					nAttributeType = ATTRIBUTETYPE_Episode_CONST;
					break;

				case 'D':
					nAttributeType = ATTRIBUTETYPE_Synopsis_CONST;
					break;

				case 'X':
					// TODO: Do a proper check of the video format and assign aspect ratio information as well.
					if (sLine[2] == '1') 
						nAttributeType = ATTRIBUTETYPE_Format_CONST;
					else
						nAttributeType = ATTRIBUTETYPE_Audio_Encoding_CONST;
						
					// iStart = 4;
					break;

				default:
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "VDRInfoFileHandler: don't know attribute type for '%c'", sLine[0]);
					break;
			}

			string sValue = sLine.substr(iStart, sLine.length() - iStart);
			PlutoMediaAttribute *pma = new PlutoMediaAttribute(0, nAttributeType, sValue);

			if(nAttributeType == ATTRIBUTETYPE_Synopsis_CONST)
				pPlutoMediaAttributes->m_mapLongAttributes.insert(make_pair(nAttributeType, pma));
			else
				pPlutoMediaAttributes->m_mapAttributes.insert(make_pair(nAttributeType, pma));
		}
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool VDRInfoFileHandler::SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes)
{
	string sBuffer;

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# VDRInfoFileHandler::SaveAttributes: saving %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), GetFileAttribute().c_str());
#endif

	for(MapPlutoMediaAttributes::iterator it = pPlutoMediaAttributes->m_mapAttributes.begin();
		it != pPlutoMediaAttributes->m_mapAttributes.end(); ++it)
	{
		string cAttributeType = "?";

		switch(it->first)
		{
			case ATTRIBUTETYPE_Channel_CONST:
				cAttributeType = "C";
				break;

			case ATTRIBUTETYPE_Title_CONST:
				cAttributeType = "T";
				break;

			case ATTRIBUTETYPE_Episode_CONST:
				cAttributeType = "S";
				break;

			case ATTRIBUTETYPE_Audio_Encoding_CONST:
			case ATTRIBUTETYPE_Format_CONST:
				cAttributeType = "X";
				break;

			default:
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "VDRInfoFileHandler: don't know attribute type convention for %d", it->first);
				break;
		}
		AddAttributeToBuffer(cAttributeType, it->second->m_sName, sBuffer);
	}

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# VDRInfoFileHandler::SaveAttributes: saving %d long attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapLongAttributes.size(), GetFileAttribute().c_str());
#endif

	for(MapPlutoMediaAttributes::iterator it = pPlutoMediaAttributes->m_mapLongAttributes.begin();
		it != pPlutoMediaAttributes->m_mapLongAttributes.end(); ++it)
	{
		string cAttributeType = "?";

		switch(it->first)
		{
			case ATTRIBUTETYPE_Synopsis_CONST:
				cAttributeType = "D";
				break;
			default:
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "VDRInfoFileHandler: don't know attribute type convention for %d", it->first);
				break;
		}
		AddAttributeToBuffer(cAttributeType, it->second->m_sName, sBuffer);
	}

	FileUtils::WriteTextFile(GetFileAttribute(), sBuffer);
	return true;
}

void VDRInfoFileHandler::AddAttributeToBuffer(string cAttributeType, string sName, string &sBuffer)
{
	if(cAttributeType != "?")
	{
		sBuffer += cAttributeType + " " + sName + "\n";
	}

}
//-----------------------------------------------------------------------------------------------------
bool VDRInfoFileHandler::RemoveAttribute(int /*nTagType*/, string /*sValue*/, PlutoMediaAttributes * /*pPlutoMediaAttributes*/)
{
	//nothing to do here
	return true;
}
//-----------------------------------------------------------------------------------------------------
bool VDRInfoFileHandler::FileAttributeExists()
{
	return FileUtils::FileExists(GetFileAttribute());
}
//-----------------------------------------------------------------------------------------------------
string VDRInfoFileHandler::GetFileAttribute()
{
	// Newer VDR version let go of the vdr extension, and use plain file names
	string sInfofile;
	sInfofile = "/info";
	
	if ( FileUtils::FileExists(m_sDirectory + "/info.vdr" ) )
	{
#ifdef UPDATEMEDIA_DEBUG
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "# VDRInfoFileHandler::GetFileAttribute: %s is an old recording dir", m_sDirectory.c_str());
#endif
		sInfofile = "/info.vdr";
	} else {
#ifdef UPDATEMEDIA_DEBUG
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "# VDRInfoFileHandler::GetFileAttribute: %s is a new recording dir", m_sDirectory.c_str());
#endif
	}
	
	return m_sDirectory + sInfofile;
}
//-----------------------------------------------------------------------------------------------------
string VDRInfoFileHandler::GetFileSourceForDB()
{
	return "V";
}
//-----------------------------------------------------------------------------------------------------
