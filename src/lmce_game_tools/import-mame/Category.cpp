/**
 * Category.h - Implement a class for parsing
 * AntoPISA Category files, to aid in the
 * categorization of XML data from MAME.
 *
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @version 1.0
 */

#include "Category.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

using namespace DCE;

Category::Category(string sCategoryPath)
{
  m_sCategoryPath = sCategoryPath;
}

Category::~Category()
{

}

bool Category::Parse()
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

void Category::ParseLine(string sLine)
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

void Category::ParseBlankLine(string sLine)
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Parsing Blank Line.");
}

void Category::ParseCategoryLine(string sLine)
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

void Category::ParseRomLine(string sLine)
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Adding ROM %s to category %s",sLine.c_str(), m_sCurrentCategory.c_str());
  sLine = StringUtils::TrimSpaces(sLine);
  m_mapRomToCategory[sLine] = m_sCurrentCategory;
}
