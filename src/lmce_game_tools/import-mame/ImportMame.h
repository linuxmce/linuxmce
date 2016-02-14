/**
 * ImportMame - main class to import MAME data into metadata.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Start Date: 01/25/2016
 */

#ifndef IMPORTMAME_H
#define IMPORTMAME_H

#include "MAMEXMLParser.h"

#include <string>
#include <map>
#include <iostream>

class ImportMAME
{
 public:
  ImportMAME(std::string sMamePath, std::string sCategoryPath);
  virtual ~ImportMAME();
  int Run();
 private:
  std::string m_sMAMEPath;
  std::string m_sCategoryPath;
  std::string m_sROMPath;
  MAMEXMLParser* m_pMAMEXMLParser;
};

#endif
