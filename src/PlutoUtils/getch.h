/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file getch.h
 Header file for some char stuff.
 */
#ifndef getch_h
#define getch_h
#ifndef WIN32
// Note: this conflicts with getch from nCurses (but we don't use nCurses anyway)
int getch();
int getche();

// timeout versions of getch and getche
int getch_timeout(int seconds);
int getche_timeout(int seconds);
#endif
#endif
