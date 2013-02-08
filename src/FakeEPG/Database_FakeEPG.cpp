/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifdef WIN32
	#include <winsock.h>
#endif
#include <iostream>
#include <mysql.h>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_FakeEPG.h"

Database_FakeEPG::Database_FakeEPG()
{
CreateTable_Listing();
}

Database_FakeEPG::~Database_FakeEPG()
{
	DeleteAllTables();
}

void Database_FakeEPG::DeleteAllTables()
{
DeleteTable_Listing();
}

bool Database_FakeEPG::Connect(string host, string user, string pass, string DBName, int port)
{
my_bool reconnect = true;
db_handle = mysql_init(NULL);
mysql_options(db_handle, MYSQL_OPT_RECONNECT, &reconnect);
if (mysql_real_connect(db_handle, host.c_str(), user.c_str(), pass.c_str(), DBName.c_str(), port, NULL, 0) == NULL)
{return false;}
else
{return true;}
}

