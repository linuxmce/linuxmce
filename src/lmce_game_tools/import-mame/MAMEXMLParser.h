/**
 * MAMEXMLParser - A pugixml based parser to extract relevant data
 * from mame's -listxml output.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Start Date: 01/25/2016
 */

#ifndef MAMEXMLPARSER_H
#define MAMEXMLPARSER_H

#include <string>
#include <map>

#include "pugixml.hpp"
#include "ImportMame.h"
#include "MAMEMachine.h"

using namespace std;

class MAMEXMLParser
{

 public:
  MAMEXMLParser(string sMAMEPath, class ImportMAME* pImportMAME);
  virtual ~MAMEXMLParser();

  bool run();

 private:
  string m_sMAMEPath;
  string m_sROMPath;
  MAMEMachine *m_pMAMEMachine;
  ImportMAME* m_pImportMAME;
  string m_sRomName, m_sDescription, m_sTitleHash, m_sRomTitle, m_sRomSubtitle, m_sRomManufacturer,m_sRomYear,m_sRomStatus,m_sRomCloneOf;

  bool getMAMEOutput();
  bool parseMAMEOutput();
  bool mameOutputIsSane();

};

#endif
