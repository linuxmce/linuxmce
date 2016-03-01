/**
 * ImportMame - main class to import MAME data into metadata
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Start Date: 01/25/2015
 */

#include "ImportMame.h"

ImportMAME::ImportMAME(std::string sMAMEPath, std::string sCategoryPath, std::string sROMPath, std::string sPicturePath, std::string sHistoryPath)
{
  m_sMAMEPath=sMAMEPath;
  m_sCategoryPath=sCategoryPath;
  m_sROMPath=sROMPath;
  m_sPicturePath=sPicturePath;
  m_sHistoryPath=sHistoryPath;
  m_pCategory = new Category(sCategoryPath);
  m_pHistory = new History(sHistoryPath);
  m_pMAMEXMLParser = new MAMEXMLParser(sMAMEPath, this);
  m_pDatabase = new Database();
  m_pROMScraper = new ROMScraper(this,sROMPath);
  m_pPictureScraper = new PictureScraper(sPicturePath);
}

ImportMAME::~ImportMAME()
{
  if (m_pMAMEXMLParser)
    delete m_pMAMEXMLParser;

  if (m_pCategory)
    delete m_pCategory;
  
  if (m_pHistory)
    delete m_pHistory;

  for (map<string, MAMEMachine*>::iterator it=m_mapMachineToMAMEMachine.begin(); it!=m_mapMachineToMAMEMachine.end(); ++it)
    {
      delete(it->second);
    }

  if (m_pROMScraper)
    delete m_pROMScraper;

  if (m_pPictureScraper)
    delete m_pPictureScraper;

}

void ImportMAME::MergeGenresIntoMachines()
{
  cout << "Merging Genres into Machine entries...Please wait...";
  for (map<string, MAMEMachine*>::iterator it=m_mapMachineToMAMEMachine.begin(); it!=m_mapMachineToMAMEMachine.end(); ++it)
    {
      MAMEMachine* m = it->second;
      m->MachineGenre_set(m_pCategory->m_mapMachineToCategory_Find(m->MachineName_get()));
    }
  cout << endl;
}

void ImportMAME::MergeHistoriesIntoMachines()
{
  cout << "Merging Histories into Machine entries...Please wait...";

  for (map<string, MAMEMachine*>::iterator it=m_mapMachineToMAMEMachine.begin(); it!=m_mapMachineToMAMEMachine.end(); ++it)
    {
      MAMEMachine* m = it->second;
      m->MachineHistory_set(m_pHistory->m_mapMachineAndGameToHistory_Find(make_pair("info",m->MachineName_get())));
    }  

  cout << endl;
}

int ImportMAME::Run()
{
  if (!m_pMAMEXMLParser || !m_pCategory || !m_pDatabase)
    return false;

  m_pDatabase->Init();
  m_pCategory->Parse();
  m_pHistory->Parse();
  
  m_pMAMEXMLParser->run();
  MergeGenresIntoMachines();
  MergeHistoriesIntoMachines();

  cout << "Processing each machine...Please wait..." << endl << endl;

  for (map<string, MAMEMachine*>::iterator it=m_mapMachineToMAMEMachine.begin(); it!=m_mapMachineToMAMEMachine.end(); ++it)
    {
      MAMEMachine* m = it->second;
      m_pROMScraper->processROM(m);
      m_pPictureScraper->processPicture(m);
      m_pDatabase->ProcessMachine(m);
      m_pPictureScraper->exportPicture(m);
      cout << "." << flush;
    }

  return 0;
}
