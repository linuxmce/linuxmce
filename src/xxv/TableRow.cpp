/*
	TableRow
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

// If using the thread logger, these generated classes create lots of activity
#ifdef NO_SQL_THREAD_LOG
#undef THREAD_LOG
#endif

#ifdef WIN32
#include <winsock.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
#include "PlutoUtils/StringUtils.h"
#include "TableRow.h"

SingleLongKey::SingleLongKey(long int in_pk)
{
	pk = in_pk;
}
bool SingleLongKey_Less::operator()(const SingleLongKey &key1, const SingleLongKey &key2) const
{
if (key1.pk!=key2.pk)
return key1.pk<key2.pk;
else
return false;	
}

SingleLong64Key::SingleLong64Key(u_int64_t in_pk)
{
	pk = in_pk;
}
bool SingleLong64Key_Less::operator()(const SingleLong64Key &key1, const SingleLong64Key &key2) const
{
if (key1.pk!=key2.pk)
return key1.pk<key2.pk;
else
return false;	
}

DoubleLongKey::DoubleLongKey(long int in_pk1,long int in_pk2)
{
	pk1 = in_pk1;
	pk2 = in_pk2;
}
bool DoubleLongKey_Less::operator()(const DoubleLongKey &key1, const DoubleLongKey &key2) const
{
	if (key1.pk1!=key2.pk1)
		return key1.pk1<key2.pk1;
	else
		if (key1.pk2!=key2.pk2)
			return key1.pk2<key2.pk2;
		else
			return false;	
}


TripleLongKey::TripleLongKey(long int in_pk1,long int in_pk2,long int in_pk3)
{
	pk1 = in_pk1;
	pk2 = in_pk2;
	pk3 = in_pk3;
}
bool TripleLongKey_Less::operator()(const TripleLongKey &key1, const TripleLongKey &key2) const
{
	if (key1.pk1!=key2.pk1)
		return key1.pk1<key2.pk1;
	else
		if (key1.pk2!=key2.pk2)
			return key1.pk2<key2.pk2;
		else
			if (key1.pk3!=key2.pk3)
				return key1.pk3<key2.pk3;
			else
				return false;	
}


SingleStringKey::SingleStringKey(string in_pk)
{
	pk = in_pk;
}
bool SingleStringKey_Less::operator()(const SingleStringKey &key1, const SingleStringKey &key2) const
{
if (key1.pk!=key2.pk)
return key1.pk<key2.pk;
else
return false;	
}

