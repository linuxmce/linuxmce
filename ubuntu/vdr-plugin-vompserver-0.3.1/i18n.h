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

#ifndef I18N_H
#define I18N_H

#include <string>
#include <map>

class I18n
{
  public:
    I18n(char* tconfigDir);
    typedef std::map<std::string,std::string> lang_code_list;
    typedef std::pair<std::string,std::string> lang_code;
    typedef std::map<std::string,std::string> trans_table;
    typedef std::pair<std::string,std::string> trans_entry;

    void findLanguages(void);
    trans_table getLanguageContent(const std::string code);
    const lang_code_list& getLanguageList(void);

  private:
    char* configDir;
    std::string LanguageCode;
    lang_code_list CodeList;

    typedef std::multimap<std::string,std::string> lang_file_list;
    typedef std::pair<std::string,std::string> lang_file;
    lang_file_list FileList; 
};
#endif
