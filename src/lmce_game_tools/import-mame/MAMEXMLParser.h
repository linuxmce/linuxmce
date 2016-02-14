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
#include "MAMEMachine.h"

using namespace std;

class MAMEXMLParser
{

 public:
  MAMEXMLParser(string sMAMEPath);
  virtual ~MAMEXMLParser();

  map<string, MAMEMachine *> m_mapMachineToMAMEMachine;
  MAMEMachine * m_mapMachineToMAMEMachine_Find(string sRomName)
  {
    map<string, MAMEMachine *>::iterator it=m_mapMachineToMAMEMachine.find(sRomName);
    return it == m_mapMachineToMAMEMachine.end() ? NULL : it->second;
  }

  bool run();

 private:
  string m_sMAMEPath;
  string m_sROMPath;
  MAMEMachine *m_pMAMEMachine;
  string m_sRomName, m_sDescription, m_sTitleHash, m_sRomTitle, m_sRomSubtitle, m_sRomManufacturer,m_sRomYear,m_sRomStatus,m_sRomCloneOf;

  bool getMAMEOutput();
  bool parseMAMEOutput();
  bool mameOutputIsSane();

};

#endif
