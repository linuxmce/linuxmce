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
 *
 * @file CommonFunctions.h
 * @brief header file for the common functions
 *
 */


#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <string>
#include <map>
using namespace std;

string int2string( int i ); /** < converts the integer parameter to a string */
string int64_2string(unsigned long long i); /** < converts the 64 bit integer parameter to a string */
string getTableFromForeignKey( string sField, map<string, class TableInfo_Generator *> *mapTableNames ); /** returnes a table name from a foreign key field name */

#endif
