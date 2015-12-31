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

#ifndef ScreenModes_H
#define ScreenModes_H

#include <vector>

#include "Resolution.h"
/**
 *	Helper function that store the current resolutions
 */
class ScreenModes
{
	std::vector<Resolution> Resolutions;

	int CurrentWidth, CurrentHeight, CurrentRefresh;
public:
	ScreenModes();

	virtual ~ScreenModes();

	/**
     * Adds one new known resolution
	 */
	Resolution* AddNewResolution(int Width, int Height);

	/**
	 * Add a screen mode (resolution + refresh rates object)
	 * @result returns false if the object already exist
	 */
	bool AddScreenMode(Resolution Mode);

	/**
     * Gets the number of known resolution
	 */
	int GetResolutionCount();

	/**
     * Gets one specified resolution pointer
	 */
	Resolution* GetResolution(int Index); 

	/**
	 * Gets one specified resolution pointer
	 */
	int GetResolutionIndex(int Width, int Height); 

	/**
	 *	Clean up the resolutions stored
	 */
	void DeleteAllResolutions();

	/**
	 *	Setup the default screen mode
	 */
	void SetDefault(int Width, int Height, int Refresh);

	/**
	 *	That method gives a brief of the current object
	 */
	void ToString();

	/**
	 *	That method gives the default screen mode index
	 */
	int GetDefaultResolutionIndex();

	int GetCurrentWidth() {
		return this->CurrentWidth;
	}

	int GetCurrentHeight() {
		return this->CurrentHeight;
	}

	int GetCurrentRefresh() {
		return this->CurrentRefresh;
	}

};

bool operator < (Resolution Left, Resolution Right);

#endif
