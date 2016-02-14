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
  m_pCategory = new Category(sCategoryPath);
  m_pMAMEXMLParser = new MAMEXMLParser(sMAMEPath);
}

ImportMAME::~ImportMAME()
{
  if (m_pMAMEXMLParser)
    delete m_pMAMEXMLParser;

  if (m_pCategory)
    delete m_pCategory;
}

void ImportMAME::MergeGenresIntoMachines()
{
  cout << "Merging Genres into Machine entries...Please wait...";
  for (map<string, MAMEMachine*>::iterator it=m_pMAMEXMLParser->m_mapMachineToMAMEMachine.begin(); it!=m_pMAMEXMLParser->m_mapMachineToMAMEMachine.end(); ++it)
    {
      MAMEMachine* m = it->second;
      m->MachineGenre_set(m_pCategory->m_mapMachineToCategory_Find(m->MachineName_get()));
    }
  cout << endl;
}

int ImportMAME::Run()
{
  if (!m_pMAMEXMLParser || !m_pCategory)
    return false;

  m_pCategory->Parse();
  m_pMAMEXMLParser->run();
  MergeGenresIntoMachines();

  return 0;
}
