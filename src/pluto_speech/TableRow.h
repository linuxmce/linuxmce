/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __TableRow_H_
#define __TableRow_H_

#ifdef WIN32
	#pragma warning (disable : 4251)

    #ifdef EXPORT_DLL
    #ifndef DECLSPECIFIER
    #define DECLSPECIFIER __declspec(dllexport)
    #endif
    #else
    #ifndef DECLSPECIFIER
    #define DECLSPECIFIER __declspec(dllimport)
    #endif
    #endif
#else
    #define DECLSPECIFIER
#endif

class DECLSPECIFIER SerializeClass;

struct DECLSPECIFIER SingleLongKey
{
	long int pk;
	SingleLongKey(long int in);
};

struct DECLSPECIFIER SingleLongKey_Less
{			
	bool operator()(const SingleLongKey &key1, const SingleLongKey &key2) const;
};

struct DECLSPECIFIER DoubleLongKey
{
	long int pk1,pk2;
	DoubleLongKey(long int in1,long int in2);
};

struct DECLSPECIFIER DoubleLongKey_Less
{			
	bool operator()(const DoubleLongKey &key1, const DoubleLongKey &key2) const;
};

struct DECLSPECIFIER TripleLongKey
{
	long int pk1,pk2,pk3;
	TripleLongKey(long int in1,long int in2,long int in3);
};

struct DECLSPECIFIER TripleLongKey_Less
{			
	bool operator()(const TripleLongKey &key1, const TripleLongKey &key2) const;
};

struct DECLSPECIFIER SingleStringKey
{
	string pk;
	SingleStringKey(string in);
};

struct DECLSPECIFIER SingleStringKey_Less
{			
	bool operator()(const SingleStringKey &key1, const SingleStringKey &key2) const;
};

class DECLSPECIFIER TableRow
{
protected:
	bool is_deleted;
	bool is_added;
	bool is_modified;
public:
	bool is_modified_get() { return is_modified; }
	bool is_added_get() { return is_added; }
	bool is_deleted_get() { return is_deleted; }
};

class DECLSPECIFIER TableBase
{
public:
	vector<class TableRow*> addedRows;
	vector<class TableRow*> deleted_addedRows;	
};

class DECLSPECIFIER SingleLongKeyBase
{
public:
	map<SingleLongKey, class TableRow*, SingleLongKey_Less> cachedRows;
	map<SingleLongKey, class TableRow*, SingleLongKey_Less> deleted_cachedRows;
};
class DECLSPECIFIER DoubleLongKeyBase
{
public:
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less> cachedRows;
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less> deleted_cachedRows;
};
class DECLSPECIFIER TripleLongKeyBase
{
public:
	map<TripleLongKey, class TableRow*, TripleLongKey_Less> cachedRows;
	map<TripleLongKey, class TableRow*, TripleLongKey_Less> deleted_cachedRows;
};
class DECLSPECIFIER SingleStringKeyBase
{
public:
	map<SingleStringKey, class TableRow*, SingleStringKey_Less> cachedRows;
	map<SingleStringKey, class TableRow*, SingleStringKey_Less> deleted_cachedRows;
};

#endif
