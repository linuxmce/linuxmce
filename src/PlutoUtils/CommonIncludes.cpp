/* 
	CommonIncludes
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"	 
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
#ifdef WIN32
#include <conio.h>
#endif

bool AskYNQuestion(string Question,bool bDefault,int Timeout)
{
	while(true)
	{
		fflush(stdin);
		cout << Question << (bDefault ? " [Y/n] " : " [N/y] ");
		while (true)
		{

			#ifdef __APPLE_CC__
				char c;
				std::cin.get(c);
			#else
				#ifndef WIN32
					char c = Timeout ? getch_timeout(Timeout) : getch();
				#else
					char c = getch();
				#endif
			#endif
			cout << endl;
			if( c==3 )
				exit(1);
			if( c=='y' || c=='Y' )
				return true;
			if( c=='n' || c=='N' )
				return false;
			if( c=='\n' )
				return bDefault;
			if (c != EOF)
				break;
			Sleep(50);
		}
	}
}

string GetPrompt(int Timeout)
{
	string sResponse;
	char c=0;
	while (true)
	{
		
#ifdef __APPLE_CC__
		std::cin.get(c);
#else
		
	#ifndef WIN32
		c = Timeout ? getch_timeout(Timeout) : getch();
	#else
		c = getch();
	#endif
#endif
		if( c=='\n' || c=='\r' || c==3 || c==0 )
			return sResponse;

		sResponse += c;
	}
}

char AskMCQuestion(string Question,string Prompts,int Timeout)
{
	char cDefault=0;
	while(true)
	{
		fflush(stdin);
		cout << Question << " [";
		bool bFirst=true;
		for(size_t s=0;s<Prompts.length();++s)
		{
			string Char = " ";
			Char[0] = Prompts[s];
			Char = StringUtils::ToLower(Char); // Just to see if it was uppercase
			if( Char[0] != Prompts[s] ) // It was uppercase
				cDefault = Char[0];
			cout << (bFirst ? "" : "/") << Prompts[s];
			bFirst=false;
		}
		cout << "] ";
		
#ifdef __APPLE_CC__
		char c;
		std::cin.get(c);
#else
		
	#ifndef WIN32
			char c = Timeout ? getch_timeout(Timeout) : getch();
	#else
			char c = getch();
	#endif
#endif
//#pragma warning("need something unbuffered && need to clear the buffer, otherwise it uses old keystrokes")
//		cin.read( &c[0], 1 );
		if( c==3 )
			exit(1);
		cout << endl;
		if( c=='\n' && cDefault)
			return cDefault;

		if( StringUtils::ToLower(Prompts).find(c)!=string::npos )
			return c;

		// In case the user entered in upper case 
		if( StringUtils::ToLower(Prompts).find(c+32)!=string::npos )
			return c+32;
	}
}

void Sleep(int miliseconds)
{
#ifdef __APPLE_CC__
	usleep(miliseconds * 1000); // For xcode this uses sleep time in useconds
#else
	struct timeval t;
	t.tv_sec = (miliseconds / 1000);
	t.tv_usec = (miliseconds % 1000) * 1000;
	while (t.tv_sec > 0 || t.tv_usec > 0)
		select(0, NULL, NULL, NULL, &t);
#endif
}
