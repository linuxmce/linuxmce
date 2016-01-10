/* 
	getch
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/


#ifdef WIN32
#include <conio.h>
#endif

#if !defined(WIN32) && !defined(__APPLE_CC__)
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>

#define EOF -1

#include <stdio.h> 
#include <iostream>
using namespace std;

int THE_getch(bool echo, struct timeval * timeout) // :P
{
	struct termios ts;
	struct termios new_ts;
	char c;
	fd_set fdset;
	size_t bytes;
	
	fflush(stdout);
	
	// ioctl(0, TCGETS, &ts); // deprecated and replaced by below code
	tcgetattr(0, &ts);
	
	new_ts = ts;
	new_ts.c_lflag &= !ICANON;
	if (! echo)
		new_ts.c_lflag &= !ECHO;
	// ioctl(0, TCSETS, &new_ts); //deprecated and replaced by below code
	tcsetattr(0, 0, &new_ts);

	c = 0;
	do
	{
		FD_ZERO(&fdset);
		FD_SET(0, &fdset);
		bytes = 0;
		if (select(1, &fdset, NULL, NULL, timeout) > 0)
			bytes = read(0, &c, 1);
	} while (/*bytes == 0 ||*/ c == '\r');

//	while ((c = getchar()) == '\r') {}
//	cin >> c;

	ioctl(0, TCSETS, &ts);

	if (bytes <= 0)
		return EOF;
	
	return c;
}

int getch()
{
	return THE_getch(false, NULL);
}

int getche()
{
	return THE_getch(true, NULL);
}

// extentions:
// getch and getche with timeout
// return: -1 on timeout, key code on received key
int getch_timeout(int seconds)
{
	struct timeval timeout = {seconds, 0};
	return THE_getch(false, &timeout);
}

int getche_timeout(int seconds)
{
	struct timeval timeout = {seconds, 0};
	return THE_getch(true, &timeout);
}
#endif

