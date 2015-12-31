/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef TEXT_SPLITTER
#define TEXT_SPLITTER

#include <string>
#include <list>
using namespace std;

typedef string Word;
typedef list<Word> Row;

class TextLineExtract
{
	protected:
		bool lineSplitter(char c);
		bool wordSplitter(char c);
		void AddRow(Row line);

	public:
		list<Row> lines;
		list<Row> & Extract(string text);
		void Clear();
		void AddWord(Row & line, Word word);
};

#endif
