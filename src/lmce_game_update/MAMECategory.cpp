/**
 * MAMECategory - a data model to store Roms to MAME category.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "MAMECategory.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

using namespace DCE;

MAMECategory::MAMECategory(string sCategoryPath)
{
  m_sCategoryPath = sCategoryPath;
}

MAMECategory::~MAMECategory()
{

}

bool MAMECategory::Parse()
{
  if (!FileUtils::FileExists(m_sCategoryPath))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't find category file named %s",m_sCategoryPath.c_str());
      return false;
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Reading category file %s",m_sCategoryPath.c_str());
  FileUtils::ReadFileIntoVector(m_sCategoryPath,m_vectCategoryLines);

  for (vector<string>::iterator it=m_vectCategoryLines.begin(); it!=m_vectCategoryLines.end(); ++it)
    {
      ParseLine((string)*it);
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Done reading category file.");
  m_vectCategoryLines.clear();
  
  return true;

}

void MAMECategory::ParseLine(string sLine)
{
  if (sLine.empty())
    {
      ParseBlankLine(sLine);
    }
  else if (sLine[0]=='[')
    {
      ParseCategoryLine(sLine);
    }
  else
    {
      ParseRomLine(sLine);
    }
}

void MAMECategory::ParseBlankLine(string sLine)
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Parsing Blank Line.");
}

void MAMECategory::ParseCategoryLine(string sLine)
{
  size_t iStart = sLine.find('[')+1;
  size_t iEnd = sLine.find(']')-1;

  if (iEnd == string::npos)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No ending bracket for category line %s",sLine.c_str());
      return;
    }
  
  m_sCurrentCategory = sLine.substr(iStart, iEnd);
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Category changed to %s",m_sCurrentCategory.c_str());
}

void MAMECategory::ParseRomLine(string sLine)
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Adding ROM %s to category %s",sLine.c_str(), m_sCurrentCategory.c_str());
  sLine = StringUtils::TrimSpaces(sLine);
  m_mapRomToCategory[sLine] = m_sCurrentCategory;
}
