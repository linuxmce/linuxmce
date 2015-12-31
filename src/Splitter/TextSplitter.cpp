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
#include "TextSplitter.h"

#include <iostream>
using namespace std;

bool TextLineExtract::lineSplitter(char c)
{
	static string splitters = "lcrtmbW";

	for (string::iterator i = splitters.begin(); i != splitters.end() ; i++)
		if (* i == c)
			return true;
	return false;
}

/**
 * iulian.d: This function is locale-dependent, it won't work with
 * French characters, for example. It's also not Unicode-aware;
 * it should work with std::wstring and wchar_t
 * 
 */ 
bool TextLineExtract::wordSplitter(char c)
{
	return c == '-' || c == ' ' || c == '.';
}

void TextLineExtract::AddRow(Row line)
{
	if (line.size() > 0)
	{
		lines.insert(lines.end(), line);
	}
}

void TextLineExtract::AddWord(Row & line, Word word)
{
	if (word.length() > 0)
	{
		line.insert(line.end(), word);
	}
}

list<Row> & TextLineExtract::Extract(string text)
{
	string flags = "";
	Word word = "";
	Row line;
	char c;
	bool inflags = false/*, splitter = true*/;

	for (string::iterator i = text.begin(); i != text.end(); i++)
	{
		c = * i;
		if (c == '\r')
			continue;

		if (c == '~')
		{
			inflags = !inflags;
			if (!inflags) //just existed a flags section
				word += '~';
			AddWord(line, word);
			word = "";
		}
		
		if (inflags)
		{
			if (lineSplitter(c))
			{
				AddRow(line);
				line.clear();
			}
			word += c;
		}
		else if (c != '~' && c != '\n')
		{
			/* Avoid adding space characters at the beginning of a line */
			if((c != ' ' && c != '\t')  || line.size() > 0 || word.size() > 0 )
			{
				word += c;
			}
		}
		else if (c == '\n')
		{
			AddWord(line, word);
			word = "";
			AddRow(line);
			line.clear();
		}
		
		if (!inflags && wordSplitter(c))
		{
			//if (! splitter)
			{
				AddWord(line, word);
				//splitter = true;
				word = "";
			}
		}
		else
		{
			//splitter = false;
		}
	}
	AddWord(line, word);
	AddRow(line);

	return lines;
}

void TextLineExtract::Clear()
{
	lines.clear();
}
