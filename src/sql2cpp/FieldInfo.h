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
 * @file FieldInfo.h
 * @brief defining class FieldInfo
 *
 */

#ifndef FIELDINFO_H
#define FIELDINFO_H

#include "mysql.h"
#include <string>

using namespace std;

/**
 * @brief class for storing table row field or table column parameters
 */
class FieldInfo
{
public:
	char *m_pcFieldName; /** < field name */
	char *m_pcFieldDefaultValue; /** < default value, can be NULL */
	enum_field_types m_iType; /** < field types, int-compatible-enum */
	unsigned long m_iLength; /** < field length */
	unsigned int m_iFlags; /** < (not yet known) */
	unsigned int m_iDecimals;  /** < field decimal places */
	
	/**
	 * @brief constructs a FieldInfo object from MYSQL_FIELD
	 */
	FieldInfo( MYSQL_FIELD *info )
	{
		m_pcFieldName = strdup( info->name  ); //is it correct? for BLOBs, etc.
		m_pcFieldDefaultValue = info->def ? strdup( info->def ) : NULL;
		m_iType = info->type;
		m_iLength = info->length;
		m_iFlags = info->flags;
		m_iDecimals = info->decimals;
	}

	/** @todo add destructor */
	
	/**
	 * @brief returns C type for the field
	 */
	string getCType();
	
	/**
	 * @brief returns C type for the field
	 */
	string getMType();
	
	/**
	 * @brief returns print format for the field
	 */
	string getPrintFormat();

	/**
	 * @brief returns scan format for the field
	 */
	string getScanFormat();
};

#endif
