/**
 * MAMECategory - MAME Category.ini data model.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include <string>
#include <vector>
#include <map>

using namespace std;

class MAMECategory
{
 private:
  vector<string> m_vectCategoryLines;
  map<string, string> m_mapRomToCategory;
  string m_sCategoryPath;
  string m_sCurrentCategory;
  void ParseLine(string sLine);
  void ParseCategoryLine(string sLine);
  void ParseRomLine(string sLine);
  void ParseBlankLine(string sLine);

 public:
  MAMECategory(string sCategoryPath);
  virtual ~MAMECategory();
  bool Parse();

  string m_mapRomToCategory_Find(string sRom)
    {
      map<string, string>::iterator it = m_mapRomToCategory.find(sRom);
      return it == m_mapRomToCategory.end() ? "" : it->second;
    }

};
