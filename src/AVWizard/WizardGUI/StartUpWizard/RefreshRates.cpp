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
#include "RefreshRates.h"

#include <algorithm>
#include <iostream>

RefreshRates::RefreshRates()
{
    this->RefreshList.empty();
}

void RefreshRates::AddRefreshRate(int RefreshRate)
{
	this->RefreshList.push_back(RefreshRate);
	std::sort(RefreshList.begin(), RefreshList.end());
}


int RefreshRates::GetRefreshRate(int RefreshRate)
{
	std::vector<int>::iterator RefreshValue;
	int Position = 0;
	for(RefreshValue = RefreshList.begin();
		RefreshValue<RefreshList.end();
		RefreshValue++, Position++)
		if(*RefreshValue == RefreshRate)
			return Position;
			
	return -1;
}

int RefreshRates::GetCount()
{
	return (int) RefreshList.size();
}

int RefreshRates::GetRefreshByIndex(int Index)
{
	if((Index<0)|| (Index>= GetCount()))
		return -1;
	return RefreshList[Index];
}

void RefreshRates::ToString()
{
	std::cout<<"Refreshes: "<<std::endl;
	for(int i = 0; i<GetCount(); i++)
		std::cout << GetRefreshByIndex(i) << " ";
	std::cout<<std::endl;
}
