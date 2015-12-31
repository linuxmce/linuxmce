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
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//---------------------------------------------------------------------------

#ifndef Resolution_H
#define Resolution_H

#include "RefreshRates.h"

class Resolution
{
	int Width, Height;

    RefreshRates Refreshes;
public:
	Resolution(int Width, int Height);

	void AddRefreshRate(int RefreshRate);

	int GetWidth() {
		return Width;
	}

	int GetHeight() {
		return Height;
	}

	/**
	 *	Gives the closest refresh rate relative to the value stored
	 *	That function is used that when is changed the screen resolution will offer the 
	 *	same refresh (as much as possible)
	 */
	int GetClosestRefresh(int Refresh);

	/**
	 *	Gives the closest higher refresh from a given refresh
	 *	If that refresh doesn't exist, that function will return -1
	 */
	int GetHigherRefresh(int Refresh);

	/**
	 *	Gives the closest lower refresh from a given refresh
	 *	If that refresh doesn't exist, that function will return -1
	 */
	int GetLowerRefresh(int Refresh);


	void ToString();
};

#endif
