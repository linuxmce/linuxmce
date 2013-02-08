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
//
// C++ Helpers methods
//
// Author: Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef __PLUTO_HELPERS_H__
#define __PLUTO_HELPERS_H__

template<class KEY, class VALUE> static const VALUE FindValueInMap(const map<KEY, VALUE>& myMap, const KEY& key, const VALUE default_value)
{
	typename map<KEY, VALUE>::const_iterator it = myMap.find(key); 
	return it != myMap.end() ? it->second : default_value;
}

template<class KEY, class OBJECT> static const OBJECT& FindValueInMapRef(const map<KEY, OBJECT>& myMap, const KEY& key, const OBJECT& default_value)
{
	typename map<KEY, OBJECT>::const_iterator it = myMap.find(key); 
	return it != myMap.end() ? it->second : default_value;
}

#endif
