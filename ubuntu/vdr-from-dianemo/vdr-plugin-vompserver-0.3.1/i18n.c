/*
    Copyright 2007 Mark Calderbank

    This file is part of VOMP.

    VOMP is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VOMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VOMP; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "i18n.h"

#include <stdio.h>
#include <string.h>
#include <glob.h>

using namespace std;

I18n::I18n(char* tconfigDir)
{
  configDir = tconfigDir;
}

void I18n::findLanguages(void)
{
  glob_t globbuf;
  char line[1000];

  CodeList.clear();
  FileList.clear();
  
  string l10nGlob = configDir;
  l10nGlob += "/l10n/*";
  glob(l10nGlob.c_str(), 0, NULL, &globbuf);
  for (unsigned int i=0; i < globbuf.gl_pathc; i++)
  {
    FILE *f = fopen(globbuf.gl_pathv[i], "r");
    if (f)
    {
      while (fgets(line, 1000, f) && strncmp(line, "l10n-vomp:", 10) == 0)
      {
        string langline = line;
        string code, name;

        string::size_type pos_start, pos_end;
        pos_start = langline.find_first_not_of(" \t\r\n", 10);
        if (pos_start == string::npos) break;
        pos_end = langline.find_first_of(" \t", pos_start);
        if (pos_end == string::npos) break;
        code = langline.substr(pos_start, pos_end - pos_start);
        pos_start = langline.find_first_not_of(" \t\r\n", pos_end);
        if (pos_start == string::npos) break;
        pos_end = langline.find_last_not_of(" \t\r\n");
        name = langline.substr(pos_start, pos_end + 1 - pos_start);
        CodeList[code] = name;
        FileList.insert(lang_file(code, globbuf.gl_pathv[i]));
      }
      fclose(f);
    }
  }
  globfree(&globbuf);
}

I18n::trans_table I18n::getLanguageContent(const string code)
{
  trans_table Translations;
  if (CodeList.count(code) == 0) return Translations;
  LanguageCode = code;

  pair<lang_file_list::const_iterator, lang_file_list::const_iterator> range;
  range = FileList.equal_range(code);
  lang_file_list::const_iterator iter;
  for (iter = range.first; iter != range.second; ++iter)
  {
    FILE *f;
    char line[1000];
    string key; 
    f = fopen((*iter).second.c_str(), "r");
    if (f)
    {
      while (fgets(line, 1000, f))
      {
        int linetype = 0;
        string::size_type offset = 0;
        string fileline = line;
        if (fileline.compare(0, 2, "x:") == 0)
        { // New key to be translated
          linetype = 1; offset = 2;
        }
        if (fileline.compare(0, code.size() + 1, code + ":") == 0)
        { // Translation for previous key
          if (key.empty()) continue; // Translation without preceding key
          linetype = 2; offset = code.size() + 1;
        }
        if (linetype != 0)
        {
          string::size_type start, end;
          start = fileline.find_first_not_of(" \t\r\n",offset);
          if (start == string::npos)
          {
            if (linetype == 2) Translations[key].clear();
            continue;
          }
          end = fileline.find_last_not_of(" \t\r\n");
          string text = fileline.substr(start, end + 1 - start);
          if (text.length() > 1) // Strip quotes if at both ends
          {
            if (text[0] == '"' && text[text.length()-1] == '"')
              text = text.substr(1, text.length()-2);
          }
          if (linetype == 1) key = text;
          if (linetype == 2) Translations[key] = text;
        }
      }
      fclose(f);
    }
  }
  return Translations;
}

const I18n::lang_code_list& I18n::getLanguageList(void)
{
  return CodeList;
}
