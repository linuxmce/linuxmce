/**
 * Category.h - Implement a class for parsing
 * AntoPISA Category files, to aid in the
 * categorization of XML data from MAME.
 *
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @version 1.0
 */

#ifndef CATEGORY_H
#define CATEGORY_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class Category
{
 private:
  vector<string> m_vectCategoryLines;
  map<string, string> m_mapMachineToCategory;
  string m_sCategoryPath;
  string m_sCurrentCategory;
  void ParseLine(string sLine);
  void ParseCategoryLine(string sLine);
  void ParseMachineLine(string sLine);
  void ParseBlankLine(string sLine);

 public:
  Category(string sCategoryPath);
  virtual ~Category();
  bool Parse();
  
  string m_mapMachineToCategory_Find(string sMachine)
  {
    map<string, string>::iterator it = m_mapMachineToCategory.find(sMachine);
    return it == m_mapMachineToCategory.end() ? "" : it->second;
  }

};

#endif /** CATEGORY_H */
