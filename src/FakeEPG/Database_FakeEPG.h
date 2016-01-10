/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __Database_FakeEPG_H_
#define __Database_FakeEPG_H_
#include <mysql.h>
#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif
class DLL_EXPORT Database_FakeEPG
{
public:
MYSQL *db_handle;
Database_FakeEPG();
~Database_FakeEPG();
void DeleteAllTables();
private:
class Table_Listing* tblListing;
public:
class Table_Listing* Listing_get() { return tblListing; }
bool Connect(string host, string user, string pass, string DBName, int port);
void Disconnect();
private:
void CreateTable_Listing();
void DeleteTable_Listing();
};
#endif
