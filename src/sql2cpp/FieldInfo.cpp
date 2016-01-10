/*
	FieldInfo
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
 * @file FieldInfo.cpp
 * @brief source file for the FieldInfo class
 *
 */


#include "PlutoUtils/CommonIncludes.h"
#include "FieldInfo.h"

string FieldInfo::getCType()
{	
	switch( m_iType )
	{
		case FIELD_TYPE_TINY:
#ifdef C99_FORMAT_SPECIFIERS
			return "char";
#else
			return "short int";
#endif
			
		case FIELD_TYPE_SHORT:
			return "short int";
			
		case FIELD_TYPE_LONG:
			return "long int";
			
		case FIELD_TYPE_INT24:
			/** @todo can it fail? */
		case FIELD_TYPE_LONGLONG:
			return "unsigned long long";
			break;
					
		case FIELD_TYPE_FLOAT:
			return "float";
			break;
			
		case FIELD_TYPE_DOUBLE:
			return "double";
			break;
		
		case FIELD_TYPE_TIMESTAMP:
		case FIELD_TYPE_DATE:
		case FIELD_TYPE_TIME:
		case FIELD_TYPE_DATETIME:
		case FIELD_TYPE_YEAR:
			return "string";

		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
		case FIELD_TYPE_BLOB:
			return "string";
			
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
		case FIELD_TYPE_NULL:
		case FIELD_TYPE_DECIMAL:
			
		default:
			return "unsupported_type";
	}	
}

string FieldInfo::getPrintFormat()
{	
	switch( m_iType )
	{
		case FIELD_TYPE_TINY:
#ifdef C99_FORMAT_SPECIFIERS
			return "%hhi";
#else
			return "%hi";
#endif
			
		case FIELD_TYPE_SHORT:
			return "%hi";
			
		case FIELD_TYPE_LONG:
			return "%li";
			
		case FIELD_TYPE_INT24:
			//TODO can it fail?			

		case FIELD_TYPE_LONGLONG:
			return UINT64_PRINTF;
					
		case FIELD_TYPE_FLOAT:
			return "%f";

		case FIELD_TYPE_DOUBLE:
			return "%lf";
		
		case FIELD_TYPE_TIMESTAMP:
		case FIELD_TYPE_DATE:
		case FIELD_TYPE_TIME:
		case FIELD_TYPE_DATETIME:
		case FIELD_TYPE_YEAR:
			return "%s";

		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
		case FIELD_TYPE_BLOB:
			return "%s";
			
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
		case FIELD_TYPE_NULL:
		case FIELD_TYPE_DECIMAL:
			
		default:
			return "unsupported_type";
	}	
}

string FieldInfo::getScanFormat()
{	
	switch( m_iType )
	{
		case FIELD_TYPE_TINY:
#ifdef C99_FORMAT_SPECIFIERS
			return "%hhi";
#else
			return "%hi";
#endif
			
		case FIELD_TYPE_SHORT:
			return "%hi";
			
		case FIELD_TYPE_LONG:
			return "%li";
			
		case FIELD_TYPE_INT24:
			//TODO can it fail?			
		
		case FIELD_TYPE_LONGLONG:
			return UINT64_PRINTF;
					
		case FIELD_TYPE_FLOAT:
			return "%f";
			
		case FIELD_TYPE_DOUBLE:
			return "%lf";
		
		case FIELD_TYPE_TIMESTAMP:
		case FIELD_TYPE_DATE:
		case FIELD_TYPE_TIME:
		case FIELD_TYPE_DATETIME:
		case FIELD_TYPE_YEAR:
			return "%s";

		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
		case FIELD_TYPE_BLOB:
			return "%s";
			
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
		case FIELD_TYPE_NULL:
		case FIELD_TYPE_DECIMAL:
			
		default:
			return "unsupported_type";
	}	
}

string FieldInfo::getMType()
{	
	switch(m_iType)
	{
		case FIELD_TYPE_TINY:
			return "FIELD_TYPE_TINY";
			
		case FIELD_TYPE_SHORT:
			return "FIELD_TYPE_SHORT";
			
		case FIELD_TYPE_LONG:
			return "FIELD_TYPE_LONG";
			
		case FIELD_TYPE_INT24:
			//TODO can it fail?			
		case FIELD_TYPE_LONGLONG:
			return "FIELD_TYPE_LONGLONG";
			break;
					
		case FIELD_TYPE_FLOAT:
			return "FIELD_TYPE_FLOAT";
			break;
			
		case FIELD_TYPE_DOUBLE:
			return "FIELD_TYPE_DOUBLE";
			break;
		
		case FIELD_TYPE_TIMESTAMP:
			return "FIELD_TYPE_TIMESTAMP";
		
		case FIELD_TYPE_DATE:
			return "FIELD_TYPE_DATE";
		
		case FIELD_TYPE_TIME:
			return "FIELD_TYPE_TIME";
		
		case FIELD_TYPE_DATETIME:
			return "FIELD_TYPE_DATETIME";
		
		case FIELD_TYPE_YEAR:
			return "FIELD_TYPE_YEAR";

		case FIELD_TYPE_STRING:
			return "FIELD_TYPE_STRING";
		
		case FIELD_TYPE_VAR_STRING:
			return "FIELD_TYPE_VAR_STRING";
		
		case FIELD_TYPE_BLOB:
			return "FIELD_TYPE_BLOB";
			
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
		case FIELD_TYPE_NULL:
		case FIELD_TYPE_DECIMAL:
			
		default:
			return "unsupported_type";
	}	
}
