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
#include "ScreenModes.h"

#include <algorithm>

Resolution* ScreenModes::AddNewResolution(int Width, int Height)
{
	Resolution* Result = new Resolution(Width, Height);

    Resolutions.push_back(*Result);

	sort(Resolutions.begin(), Resolutions.end());
	return Result;
}

int ScreenModes::GetResolutionCount()
{
	return (int)this->Resolutions.size();
}

Resolution* ScreenModes::GetResolution(int Index)
{
	if((Index<0) || (Index>= GetResolutionCount()))
		return NULL;
	return &(Resolutions[Index]);
}

bool ScreenModes::AddScreenMode(Resolution Mode)
{
	Resolutions.push_back(Mode);
	sort(Resolutions.begin(), Resolutions.end());
	
	return true;
}

ScreenModes::ScreenModes()
{
}

ScreenModes::~ScreenModes()
{
	DeleteAllResolutions();
}

void ScreenModes::DeleteAllResolutions()
{
	std::vector<Resolution>::iterator Item;
	for(Item = Resolutions.begin(); Item!=Resolutions.end();)
		Item = Resolutions.erase(Item);
}

int ScreenModes::GetResolutionIndex(int Width, int Height)
{
	int Result = 0;

	std::vector<Resolution>::iterator Item;
	for(Item = Resolutions.begin(); Item!=Resolutions.end();Item++, Result++)
		if(
			((*Item).GetWidth() == Width) &&
			((*Item).GetHeight() == Height)
			)
			return Result;

	return -1;	
}

int ScreenModes::GetDefaultResolutionIndex()
{
	return GetResolutionIndex(CurrentWidth, CurrentHeight);
}

void ScreenModes::SetDefault(int Width, int Height, int Refresh)
{
	CurrentWidth = Width;
	CurrentHeight = Height;
	CurrentRefresh = Refresh;
}

void ScreenModes::ToString()
{
	std::vector<Resolution>::iterator Item;
	for (Item = Resolutions.begin(); Item != Resolutions.end(); ++Item)
		(*Item).ToString();
}

bool operator < (Resolution Left, Resolution Right)
{
	if(Left.GetWidth()<Right.GetWidth())
		return true;
	if(Left.GetWidth()>Right.GetWidth())
		return false;

	return Left.GetHeight()<Right.GetHeight();
}
