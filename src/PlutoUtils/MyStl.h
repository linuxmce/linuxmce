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
 * @file MyStl.h
 Header file for ??? .
 */
#ifndef MyStl_h
#define MyStl_h

#ifndef SYMBIAN

#define MYSTL_CREATE_LIST(CollectionName,ValueObjectType) list<class ValueObjectType *> CollectionName
#define MYSTL_CREATE_VECT(CollectionName,ValueObjectType) vector<class ValueObjectType *> CollectionName
#define MYSTL_ITERATE_LIST(CollectionName,ValueObjectType,ValueObject,IteratorName) 	ValueObjectType *ValueObject; list<class ValueObjectType *>::iterator IteratorName;	for(IteratorName=CollectionName.begin(),ValueObject=(IteratorName!=CollectionName.end() ? (*IteratorName) : NULL);			IteratorName!=CollectionName.end();++IteratorName,ValueObject=(IteratorName!=CollectionName.end() ? (*IteratorName) : NULL))
#define MYSTL_ITERATE_VECT(CollectionName,ValueObjectType,ValueObject,IteratorName) 	ValueObjectType *ValueObject; vector<class ValueObjectType *>::iterator IteratorName;	for(IteratorName=CollectionName.begin(),ValueObject=(IteratorName!=CollectionName.end() ? (*IteratorName) : NULL);			IteratorName!=CollectionName.end();++IteratorName,ValueObject=(IteratorName!=CollectionName.end() ? (*IteratorName) : NULL))
#define MYSTL_ADDTO_LIST(CollectionName,ValueObject) CollectionName.push_back(ValueObject)
#define MYSTL_CLEAR_LIST(CollectionName) CollectionName.clear()
#define MYSTL_SIZEOF_LIST(CollectionName) CollectionName.size()
#define MYSTL_POSITION_INLIST(CollectionName,ValueObjectType,ValueObject,Position) ValueObjectType *ValueObject; int iListCounter=0; list<class ValueObjectType *>::iterator IteratorName;	for(IteratorName=CollectionName.begin(),ValueObject=(IteratorName!=CollectionName.end() ? (*IteratorName) : NULL);	IteratorName!=CollectionName.end() && iListCounter<Position;++IteratorName,++iListCounter,ValueObject=(IteratorName!=CollectionName.end() ? (*IteratorName) : NULL));
#define MYSTL_EXTRACT_FIRST(CollectionName,ValueObjectType,ValueObject) ValueObjectType *ValueObject=CollectionName.front(); CollectionName.pop_front();

#define MYSTL_CREATE_LONG_MAP(CollectionName,ValueObjectType) map<long, class ValueObjectType *> CollectionName
#define MYSTL_ADD_TO_LONG_MAP(CollectionName,ValueObject,Key) CollectionName[Key] = ValueObject
#define MYSTL_LONGMAP_SIZE(CollectionName) CollectionName.size()
#define MYSTL_ITERATE_LONG_MAP(CollectionName,ValueObjectType,ValueObject,IteratorName) 	\
	ValueObjectType *ValueObject; \
	map<long,class ValueObjectType *>::iterator IteratorName; \
	for(IteratorName=CollectionName.begin(),ValueObject=((IteratorName!=CollectionName.end()) ? (*IteratorName).second : NULL); \
		IteratorName!=CollectionName.end(); \
		++IteratorName,ValueObject=((IteratorName!=CollectionName.end()) ? (*IteratorName).second : NULL))

#define MYSTL_SIZEOF_LONG_MAP(CollectionName) CollectionName.size()
#define MYSTL_GET_SECOND_LONG_MAP(IteratorName) (*IteratorName).second
#define MYSTL_GET_AT_LONG_MAP(CollectionName,ValueObjectType,ValueObject,Position) \
    map<long, ValueObjectType*>::iterator it = CollectionName.find(Position); \
    ValueObjectType* ValueObject = it == CollectionName.end() ? NULL : (*it).second;

#else
// Symbian version of the <string>
#include "MyString.h"
#include "MyMap.h"

#define MYSTL_CREATE_LIST(CollectionName,ValueObjectType) RPointerArray<class ValueObjectType> CollectionName
#define MYSTL_ITERATE_LIST(CollectionName,ValueObjectType,ValueObject,IteratorName) ValueObjectType *ValueObject = NULL; for (int IteratorName = 0; IteratorName < CollectionName.Count() && (ValueObject = CollectionName[IteratorName]); IteratorName++)
#define MYSTL_CREATE_VECT(CollectionName,ValueObjectType) RPointerArray<class ValueObjectType> CollectionName
#define MYSTL_ITERATE_VECT(CollectionName,ValueObjectType,ValueObject,IteratorName) ValueObjectType *ValueObject = NULL; for (int IteratorName = 0; IteratorName < CollectionName.Count() && (ValueObject = CollectionName[IteratorName]); IteratorName++)
#define MYSTL_ADDTO_LIST(CollectionName,ValueObject) CollectionName.Append(ValueObject)
#define MYSTL_CLEAR_LIST(CollectionName) CollectionName.Reset()
#define MYSTL_SIZEOF_LIST(CollectionName) CollectionName.Count()
#define MYSTL_POSITION_INLIST(CollectionName,ValueObjectType,ValueObject,Position) ValueObjectType *ValueObject = CollectionName[Position];
#define MYSTL_EXTRACT_FIRST(CollectionName,ValueObjectType,ValueObject) ValueObjectType *ValueObject = CollectionName[0]; CollectionName.Remove(0)

#define MYSTL_CREATE_LONG_MAP(CollectionName,ValueObjectType) TLongMap<class ValueObjectType *> CollectionName
#define MYSTL_ADD_TO_LONG_MAP(CollectionName,ValueObject,Key) CollectionName.AddData(Key, ValueObject)
#define MYSTL_LONGMAP_SIZE(CollectionName) CollectionName.Count()
#define MYSTL_ITERATE_LONG_MAP(CollectionName,ValueObjectType,ValueObject,IteratorName) ValueObjectType *ValueObject; for (CollectionName.GetFirst(); ValueObject = CollectionName.GetCurrent(); CollectionName.GetNext())
#define MYSTL_GET_SECOND_LONG_MAP(IteratorName) IteratorName
#define MYSTL_SIZEOF_LONG_MAP(CollectionName) CollectionName.Count()
#define MYSTL_GET_AT_LONG_MAP(CollectionName,ValueObjectType,ValueObject,Position) ValueObjectType *ValueObject=CollectionName.GetData(Position)


#endif

#endif
