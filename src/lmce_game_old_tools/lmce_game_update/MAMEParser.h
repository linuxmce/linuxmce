/**
 * MAME Parser - Parse XML output of -listsoftware
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include <libxml++/libxml++.h>
#include <string>
#include <map>

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

class MAMEParser : public xmlpp::SaxParser
{

 private:
  bool GetMAMEOutput(string &sMameOutput);
  string m_sMamePath;
  MAMERom *m_pMAMERom;
  string m_sRomName, m_sDescription, m_sTitleHash, m_sRomTitle, m_sRomSubtitle, m_sRomManufacturer,m_sRomYear,m_sRomStatus,m_sRomCloneOf;

  enum CurrentTag { GAME=0, DESCRIPTION, YEAR, MANUFACTURER, DRIVER };
  CurrentTag m_eCurrentTag;

  void ProcessDescription(string sDescription);

 public:
  map<string, MAMERom *> m_mapRomToMAMERom;
  MAMERom * m_mapRomToMAMERom_Find(string sRomName)
  {
    map<string, MAMERom *>::iterator it=m_mapRomToMAMERom.find(sRomName);
    return it == m_mapRomToMAMERom.end() ? NULL : it->second;
  }

  MAMEParser();
  virtual ~MAMEParser();

 protected:
  // virtual void on_start_document();
  // virtual void on_end_document();
  virtual void on_start_element(const Glib::ustring& name,
                                const AttributeList& properties);
  virtual void on_end_element(const Glib::ustring& name);
  virtual void on_characters(const Glib::ustring& characters);
  // virtual void on_comment(const Glib::ustring& text);
  // virtual void on_warning(const Glib::ustring& text);
  // virtual void on_error(const Glib::ustring& text);
  // virtual void on_fatal_error(const Glib::ustring& text);

  // MAMERom* m_mapRomNameToMAMERom_Find(string sRomName)
  // {
  //   map<string, MAMERom *>::iterator it=m_mapRomNameToMAMERom.find(sRomName);
  //  return it == m_mapRomNameToMAMERom.end() ? NULL : it->second;
  //}


};
