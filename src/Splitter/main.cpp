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
#include "TextWrapper.h"

#include <iostream>
using namespace std;

extern void DoSDL();

extern "C"
int main()
{
	string sample_string = "sample text ~B~with bold~XB~text in the middle\n"
	"~c~centered line~b~bottom aligned text\n"
	"another bottom aligned line of text"
	"~tr~right aligned line\n"
	"~m~middle    aligned text"
	"~l~still in the ~I~middle~xI~, but to the ~U~left~xU~~c~~bi~centered~xbxi~ in the middle"
	"~bc~bottom center alligned text "
	"~B~bold~xB~ ~I~italic~xI~~BWl~bold wrapped text~xB~ bold out";

	TextLineExtract T;
	TextLineWrap W;

	T.Extract(sample_string);

	string FontPath;

#ifdef WIN32
	FontPath = "C:/Windows/Fonts/";
#else
	FontPath = "/usr/share/pluto/fonts/";
#endif

	W.Wrap(sample_string, 300, 400, FontPath, "arial", 18);

	list<Row>::iterator i;
	list<Word>::iterator j;

	for (i = T.lines.begin(); i != T.lines.end(); i++)
	{
		for (j = i->begin(); j != i->end(); j++)
			cout << * j << ":";
		cout << endl;
	}

	cout << endl;

	for (i = W.lines.begin(); i != W.lines.end(); i++)
	{
		for (j = i->begin(); j != i->end(); j++)
			cout << * j << ":";
		cout << endl;
	}

	W.RenderToScreen();

	DoSDL();

	return 0;
}
