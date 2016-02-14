/**
 * ImportMame - main class to import MAME data into metadata.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Start Date: 01/25/2016
 */

#ifndef IMPORTMAME_H
#define IMPORTMAME_H

#include "MAMEXMLParser.h"
#include "Category.h"
#include "MAMEMachine.h"

#include <string>
#include <map>
#include <iostream>

class ImportMAME
{
 public:
  ImportMAME(std::string sMamePath, std::string sCategoryPath);
  virtual ~ImportMAME();
  int Run();

  map<string, MAMEMachine *> m_mapMachineToMAMEMachine;
  MAMEMachine * m_mapMachineToMAMEMachine_Find(string sRomName)
  {
    map<string, MAMEMachine *>::iterator it=m_mapMachineToMAMEMachine.find(sRomName);
    return it == m_mapMachineToMAMEMachine.end() ? NULL : it->second;
  }

 private:
  std::string m_sMAMEPath;
  std::string m_sCategoryPath;
  std::string m_sROMPath;
  class MAMEXMLParser* m_pMAMEXMLParser;
  class Category* m_pCategory;
  void MergeGenresIntoMachines();
};

#endif
