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

using namespace std;

/**
 * A simple container for Roms
 */
class MAMERom
{
 private:
  string m_sRomTitle;
  string m_sTitleHash;
  string m_sRomSubtitle;
  string m_sRomManufacturer;
  string m_sRomYear;
  string m_sRomStatus;
  string m_sRomCloneOf;

 public:
  MAMERom(string sRomTitle,
	  string sTitleHash,
	  string sRomSubtitle, 
	  string sRomManufacturer, 
	  string sRomYear, 
	  string sRomStatus,
	  string sRomCloneOf);

  virtual ~MAMERom();

  string RomTitle_get() {return m_sRomTitle;}
  string TitleHash_get() {return m_sTitleHash;}
  string RomSubtitle_get() {return m_sRomSubtitle;}
  string RomManufacturer_get() {return m_sRomManufacturer;}
  string RomYear_get() {return m_sRomYear;}
  string RomStatus_get() {return m_sRomStatus;}
  string RomCloneOf_get() {return m_sRomCloneOf;}

};

class MAMEXMLParser
{

 public:
  MAMEXMLParser(string sMAMEPath);
  virtual ~MAMEXMLParser();

  map<string, MAMERom *> m_mapRomToMAMERom;
  MAMERom * m_mapRomToMAMERom_Find(string sRomName)
  {
    map<string, MAMERom *>::iterator it=m_mapRomToMAMERom.find(sRomName);
    return it == m_mapRomToMAMERom.end() ? NULL : it->second;
  }

  bool run();

 private:
  string m_sMAMEPath;
  MAMERom *m_pMAMERom;
  string m_sRomName, m_sDescription, m_sTitleHash, m_sRomTitle, m_sRomSubtitle, m_sRomManufacturer,m_sRomYear,m_sRomStatus,m_sRomCloneOf;

  bool getMAMEOutput();
  bool parseMAMEOutput();
  bool mameOutputIsSane();

};

#endif
