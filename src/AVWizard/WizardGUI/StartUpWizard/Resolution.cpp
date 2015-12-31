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
#include "Resolution.h"

#include <iostream>

Resolution::Resolution(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
}

void Resolution::AddRefreshRate(int RefreshRate)
{
	this->Refreshes.AddRefreshRate(RefreshRate);
}

void Resolution::ToString()
{
	std::cout<<"Width: "<<Width<<" Height: "<<Height<<" "<<std::endl;
	Refreshes.ToString();
}

int Resolution::GetClosestRefresh(int Refresh)
{
	//test if the refresh match exactly
	for(int i = 0; i<Refreshes.GetCount(); i++)
	{
		int RefreshTest = Refreshes.GetRefreshByIndex(i);
		if(RefreshTest == Refresh)
			return Refresh;
	}

    //compare the closest higher and lower refreshes
	int LowerRefresh, HigherRefresh;
	LowerRefresh = GetLowerRefresh(Refresh);
	HigherRefresh = GetHigherRefresh(Refresh);

	if (LowerRefresh == -1)
		return HigherRefresh;

	if (HigherRefresh == -1)
		return LowerRefresh;

    if (Refresh - LowerRefresh < HigherRefresh - Refresh)
		return LowerRefresh;
	
	return HigherRefresh;
    
}

int Resolution::GetHigherRefresh(int Refresh)
{
	int Result = -1;
	for(int i = 0; i<Refreshes.GetCount(); i++)
	{
		int RefreshTest = Refreshes.GetRefreshByIndex(i);
		if (RefreshTest > Refresh)
		{
			if(Result == -1)
			{
				Result = RefreshTest;
			}
			else
			{
				if(Result>RefreshTest)
					Result = RefreshTest;
			}
		}
	}

	return Result;
}

int Resolution::GetLowerRefresh(int Refresh)
{
	int Result = -1;
	for(int i = 0; i<Refreshes.GetCount(); i++)
	{
		int RefreshTest = Refreshes.GetRefreshByIndex(i);
		if (RefreshTest < Refresh)
		{
			if(Result == -1)
			{
				Result = RefreshTest;
			}
			else
			{
				if(Result<RefreshTest)
					Result = RefreshTest;
			}
		}
	}

	return Result;
}
