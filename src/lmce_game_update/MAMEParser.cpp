/** 
 * MAMEParser - Parses -listsoftware output from MAME into useful data.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version 1.0
 */

#include "MAMEParser.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

using namespace DCE;

MAMERom::MAMERom(string sRomTitle,
		 string sTitleHash,
		 string sRomSubtitle,
		 string sRomManufacturer,
		 string sRomYear,
		 string sRomStatus,
		 string sRomCloneOf)
{
  m_sRomTitle=sRomTitle;
  m_sTitleHash=sTitleHash;
  m_sRomSubtitle=sRomSubtitle;
  m_sRomManufacturer=sRomManufacturer;
  m_sRomYear=sRomYear;
  m_sRomStatus=sRomStatus;
  m_sRomCloneOf=sRomCloneOf;
}

MAMERom::~MAMERom()
{
}

MAMEParser::MAMEParser()
  : xmlpp::SaxParser()
{
}

MAMEParser::~MAMEParser()
{
}

void MAMEParser::on_start_element(const Glib::ustring& name,
				  const AttributeList& properties)
{
  string sElementName = name;

  if (name=="game")
    {
      m_eCurrentTag = MAMEParser::GAME;
      for (xmlpp::SaxParser::AttributeList::const_iterator iter = properties.begin(); iter != properties.end(); ++iter)
	{
	  if (iter->name == "name")
	    {
	      m_sRomName = iter->value;
	    }
	  if (iter->name == "cloneof")
	    {
	      m_sRomCloneOf = iter->value;
	    }
	}
    }
  else if (name=="description")
    {
      m_eCurrentTag = MAMEParser::DESCRIPTION;
    }
  else if (name=="manufacturer")
    {
      m_eCurrentTag = MAMEParser::MANUFACTURER;
    }
  else if (name=="year")
    {
      m_eCurrentTag = MAMEParser::YEAR;
    }
  else if (name=="driver")
    {
      m_eCurrentTag = MAMEParser::DRIVER;
      for (xmlpp::SaxParser::AttributeList::const_iterator iter= properties.begin(); iter != properties.end(); ++iter)
	{
	  if (iter->name=="status")
	    {
	      m_sRomStatus=iter->value;
	    }
	}
    }

}

void MAMEParser::on_end_element(const Glib::ustring& name)
{
  if (name=="description")
    {
      ProcessDescription(m_sDescription);
      m_eCurrentTag=MAMEParser::GAME;
    }
  else if (name=="manufacturer" || name=="year" || name=="driver")
    {
      m_eCurrentTag = MAMEParser::GAME;
    }
  else if (name=="game")
    {
      m_mapRomToMAMERom[m_sRomName]=new MAMERom(m_sRomTitle,
						m_sTitleHash,
						m_sRomSubtitle,
						m_sRomManufacturer,
						m_sRomYear,
						m_sRomStatus,
						m_sRomCloneOf);
      m_sRomTitle=""; m_sDescription=""; m_sTitleHash=""; m_sRomSubtitle=""; m_sRomManufacturer=""; m_sRomYear="";
      m_sRomStatus=""; m_sRomCloneOf="";
    }
}

void MAMEParser::on_characters(const Glib::ustring& characters)
{
  string sCharacters=characters;
  switch (m_eCurrentTag)
    {
    case MAMEParser::GAME:
      break;
    case MAMEParser::DESCRIPTION:
      m_sDescription+=characters;
      break;
    case MAMEParser::MANUFACTURER:
      m_sRomManufacturer+=characters;
      break;
    case MAMEParser::YEAR:
      m_sRomYear+=characters;
      break;
    case MAMEParser::DRIVER:
      break;
    default:
      break;
    }
}

void MAMEParser::ProcessDescription(string sDescription)
{
  m_sTitleHash=StringUtils::TitleHash(sDescription);

  size_t iSubtitleBegPos=sDescription.find('(');
  if (iSubtitleBegPos != string::npos)
    {
      size_t iSubtitleEndPos=sDescription.rfind(')');
      string sTitleTmp=sDescription.substr(0,iSubtitleBegPos-1);
      string sSubtitleTmp=sDescription.substr(iSubtitleBegPos+1,
					      (iSubtitleEndPos-1)-iSubtitleBegPos);
      sSubtitleTmp=StringUtils::Replace(sSubtitleTmp,"(","");
      sSubtitleTmp=StringUtils::Replace(sSubtitleTmp,")","");
      m_sRomTitle=StringUtils::TrimSpaces(sTitleTmp);
      m_sRomSubtitle=StringUtils::TrimSpaces(sSubtitleTmp);
    }
  else
    {
      m_sRomTitle=sDescription;
    }
}
