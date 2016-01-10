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
#ifndef __SCREEN_MAP_H__
#define __SCREEN_MAP_H__

#include <map>
using namespace std;

void PopulateScreenMap(class Database_pluto_main *pDatabase_pluto_main, map<int,int>& mapDesignObj,
					   class Row_UI *pRow_UI, class Row_Skin *pRow_Skin, class Row_Device *pRow_Device,
					   bool bMatchingSkin);

#endif
