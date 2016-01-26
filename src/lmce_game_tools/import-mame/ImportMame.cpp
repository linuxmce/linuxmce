/**
 * ImportMame - main class to import MAME data into metadata
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Start Date: 01/25/2015
 */

#include "ImportMame.h"

ImportMAME::ImportMAME(std::string sMAMEPath, std::string sCategoryPath)
{
  m_sMAMEPath=sMAMEPath;
  m_sCategoryPath=sCategoryPath;
  m_pMAMEXMLParser = new MAMEXMLParser(sMAMEPath);
}

ImportMAME::~ImportMAME()
{
  if (m_pMAMEXMLParser)
    delete m_pMAMEXMLParser;
}

int ImportMAME::Run()
{
  if (!m_pMAMEXMLParser)
    return false;

  m_pMAMEXMLParser->run();

  return 0;
}
