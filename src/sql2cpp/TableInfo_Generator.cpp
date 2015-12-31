/*
	TableInfo_Generator
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"
#include "TableInfo_Generator.h"

#include <iostream>
using namespace std;

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "CommonFunctions.h"

TableInfo_Generator::TableInfo_Generator( MYSQL * pDB, map<string,TableInfo_Generator*> *pTables ) :
	TableInfo( pDB ),
	m_sTableClassPrefix( "Table_" ),
	m_sRowClassPrefix( "Row_" ),
	m_sPrimaryKeyClassPrefix( "PrimaryKey_" )
{
	m_pTables = pTables;
	m_eKnownPkTypes = PKTYPES_UNKNOWN;
}

bool TableInfo_Generator::loadFromDB( string sTable )
{
	bool bResult = TableInfo::loadFromDB( sTable ); // Fill the fields structure 

	// See if this hass a known pattern of primary key, like a single int, etc.
	int iNumLongPrimaryKeys=0,iNumStringPrimaryKeys=0,iNumLong64PrimaryKeys=0;
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
	{
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
		{
			string sType = (*i)->getCType();
			if( sType=="long int" )
				iNumLongPrimaryKeys++;
			else if( sType=="u_int64_t" )
				iNumLong64PrimaryKeys++;
			else if( sType=="string" )
				iNumStringPrimaryKeys++;
			else
			{
				cout << m_sTableName << " will have a custom Key" << endl;
				return bResult; // Leave it as PKTYPES_UNKNOWN -- we don't know what this is
			}
		}
	}

	// We had no primary keys of unknown types
	if( iNumLong64PrimaryKeys==1 )
		m_eKnownPkTypes = PKTYPES_SINGLE_LONG64;
	if( iNumStringPrimaryKeys==0 && iNumLongPrimaryKeys==1 )
		m_eKnownPkTypes = PKTYPES_SINGLE_LONG;
	else if( iNumStringPrimaryKeys==0 && iNumLongPrimaryKeys==2 )
		m_eKnownPkTypes = PKTYPES_DOUBLE_LONG;
	else if( iNumStringPrimaryKeys==0 && iNumLongPrimaryKeys==3 )
		m_eKnownPkTypes = PKTYPES_TRIPLE_LONG;
	else if( iNumStringPrimaryKeys==1 && iNumLongPrimaryKeys==0 )
		m_eKnownPkTypes = PKTYPES_SINGLE_STRING;
	else
		cout << m_sTableName << " will have a custom Key" << endl;
	return bResult;
}

string TableInfo_Generator::ExpandString(string s)
{
	if (s=="table_name")
		return get_table_name();
	else
	if (s=="fields_declaration")
		return get_fields_declaration();
	else
		if (s=="getters_declaration")
			return get_getters_declaration();
	else
		if (s=="setters_declaration")
			return get_setters_declaration();
	else
		if (s=="null_getters_declaration")
			return get_null_getters_declaration();
	else
		if (s=="null_setters_declaration")
			return get_null_setters_declaration();
	else
		if (s=="getters_definition")
			return get_getters_definition();
	else
		if (s=="setters_definition")
			return get_setters_definition();
	else
		if (s=="null_getters_definition")
			return get_null_getters_definition();
	else
		if (s=="null_setters_definition")
			return get_null_setters_definition();
	else
		if (s=="primary_fields_declaration")
			return get_primary_fields_declaration();
	else
		if (s=="primary_fields_in_typed_list")
			return get_primary_fields_in_typed_list();
	else
		if (s=="primary_fields_in_untyped_list")
			return get_primary_fields_in_untyped_list();
	else
		if (s=="primary_fields_assign_list")
			return get_primary_fields_assign_list();
	else
		if (s=="table_key")
			return get_table_key();
	else
		if (s=="table_base")
			return get_table_base();
	else
		if (s=="map_cached_rows")
			return get_map_cached_rows();
	else
		if (s=="map_deleted_cached_rows")
			return get_map_deleted_cached_rows();
	else
		if (s=="table_key_comparer")
			return get_table_key_comparer();
	else
		if (s=="key_less_statement")
			return get_key_less_statement();
	else
		if (s=="set_default_values")
			return get_set_default_values();
	else
		if (s=="fields_count")
			return get_fields_count();
	else
		if (s=="primary_fields_count")
			return get_primary_fields_count();
	else
		if (s=="primary_fields_where_list")
			return get_primary_fields_where_list();
	else
		if (s=="bind_parameters_to_key")
			return get_bind_parameters_to_key();
	else
		if (s=="bind_results_to_fields")
			return get_bind_results_to_fields();
	else
		if (s=="init_row_from_temp_string_fields")
			return get_init_row_from_temp_string_fields();
	else
		if (s=="primary_fields_assign_from_row")
			return get_primary_fields_assign_from_row();
	else
		if (s=="primary_fields")
			return get_primary_fields();
	else
		if (s=="build_query_for_key")
			return get_build_query_for_key();
	else
		if (s=="init_fields_from_row")
			return get_init_fields_from_row();
	else
		if (s=="ifdef_have_primary_keys")
			return get_ifdef_have_primary_keys();
	else
		if (s=="endif_have_primary_keys")
			return get_endif_have_primary_keys();
	else
		if (s=="generate_insert_query")
			return get_generate_insert_query();
	else
		if (s=="generate_update_query")
			return get_generate_update_query();
	else 
		if (s=="fields_list_comma_separated")
			return get_fields_list_comma_separated();
	else
		if (s=="primary_autoinc_key_init")
			return get_primary_autoinc_key_init();
	else
		if (s=="fields_sql_getters_declaration")
			return get_fields_sql_getters_declaration();
	else
		if (s=="fields_sql_getters_definition")
			return get_fields_sql_getters_definition();
	else
		if (s=="rows_getters_declaration")
			return rows_getters_declaration();
	else
		if (s=="rows_getters_definition")
			return rows_getters_definition();
	else
		if (s=="rows_getters_includes")
			return rows_getters_includes();
	else
		if (s=="fk_rows_getters_declaration")
			return fk_rows_getters_declaration();
	else
		if (s=="fk_rows_getters_definition")
			return fk_rows_getters_definition();
	else
		if (s=="fk_rows_getters_includes")
			return fk_rows_getters_includes();
	else
		if (s=="table_constants_defines")
			return table_constants_defines();
	else
		if (s=="table_struct_defines")
			return table_struct_defines();
	else
		if (s=="member_variable_list")
			return get_member_variable_list();
	else
	{
		cerr << "Unknown: " << s << endl;
		return "";
	}
}

string TableInfo_Generator::get_fields_declaration()
{
	string s;
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		s = s + (*i)->getCType() + " m_" + (*i)->m_pcFieldName + ";" + "\n";
	return s;
}

string TableInfo_Generator::get_member_variable_list()
{
	string s;
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		s = s + "+" + " m_" + (*i)->m_pcFieldName;
	return s;
}

string TableInfo_Generator::get_getters_declaration()
{
	string s;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		s = s + (*i)->getCType() + " " + (*i)->m_pcFieldName + "_get();" + "\n";
	
	return s;
}

string TableInfo_Generator::get_getters_definition()
{
	string s;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
	{
		s = s + (*i)->getCType() + " Row_"+m_sTableName+"::" + (*i)->m_pcFieldName + "_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);\n\nreturn m_" + (*i)->m_pcFieldName + ";}" + "\n";
	}

	return s;
}

string TableInfo_Generator::get_setters_declaration()
{
	string s;
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
	{
		s = s + "void " + (*i)->m_pcFieldName + "_set(" + (*i)->getCType() + " val);" + "\n";
	}

	return s;
}

string TableInfo_Generator::get_setters_definition()
{
	string s;
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
	{
		s = s + "void " + "Row_"+m_sTableName+"::"+ (*i)->m_pcFieldName + "_set(" + (*i)->getCType() + " val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);\n\nm_" + (*i)->m_pcFieldName + " = val; is_modified=true; is_null["+int2string(iIndex)+"]=false;}" + "\n";
	}

	return s;
}

string TableInfo_Generator::get_null_getters_declaration()
{
	string s;
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
	{
		if (!((*i)->m_iFlags & NOT_NULL_FLAG))
			s = s + "bool " + (*i)->m_pcFieldName + "_isNull();" + "\n";
	}

	return s;
}

string TableInfo_Generator::get_null_getters_definition()
{
	string s;
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i=m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
	{
		if (!((*i)->m_iFlags & NOT_NULL_FLAG))
			s = s + "bool " + "Row_"+m_sTableName+"::" + (*i)->m_pcFieldName + "_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);\n\nreturn is_null["+int2string(iIndex)+"];}" + "\n";
	}

	return s;
}


string TableInfo_Generator::get_null_setters_declaration()
{
	string s;
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i!=m_Fields.end(); i++, iIndex++ )
	{
		if (!((*i)->m_iFlags & NOT_NULL_FLAG))
			s = s + "void " + (*i)->m_pcFieldName + "_setNull(bool val);" + "\n";
	}

	return s;
}

string TableInfo_Generator::get_null_setters_definition()
{
	string s;
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
	{
		if (!((*i)->m_iFlags & NOT_NULL_FLAG))
			s = s + "void " +  "Row_"+m_sTableName+"::"+(*i)->m_pcFieldName + "_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);\nis_null["+int2string(iIndex)+"]=val;\nis_modified=true;\n}\n";
	}

	return s;
}

string TableInfo_Generator::get_primary_fields_declaration()
{
	string s;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
	{
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
			s = s + (*i)->getCType() + " pk_" + (*i)->m_pcFieldName + ";" + "\n";
	}

	return s;
}

string TableInfo_Generator::get_primary_fields_in_typed_list()
{
	string s;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
	{
		if (s != "") s = s + ", ";
		s = s + (*i)->getCType() + " in_" + (*i)->m_pcFieldName;
	}
	
	return s;
}

string TableInfo_Generator::get_table_key()
{
	if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG )
		return "SingleLongKey";
	else if( m_eKnownPkTypes==PKTYPES_DOUBLE_LONG )
		return "DoubleLongKey";
	else if( m_eKnownPkTypes==PKTYPES_TRIPLE_LONG )
		return "TripleLongKey";
	else if( m_eKnownPkTypes==PKTYPES_SINGLE_STRING )
		return "SingleStringKey";
	else if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG64 )
		return "SingleLong64Key";
	else
		return "Table_" + m_sTableName + "::Key";
}

string TableInfo_Generator::get_table_base()
{
	if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG )
		return ", SingleLongKeyBase";
	else if( m_eKnownPkTypes==PKTYPES_DOUBLE_LONG )
		return ", DoubleLongKeyBase";
	else if( m_eKnownPkTypes==PKTYPES_TRIPLE_LONG )
		return ", TripleLongKeyBase";
	else if( m_eKnownPkTypes==PKTYPES_SINGLE_STRING )
		return ", SingleStringKeyBase";
	else if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG64 )
		return ", SingleLong64KeyBase";
	else
		return ""; // We'll declare the maps locally
}

string TableInfo_Generator::get_map_cached_rows()
{
	if( m_eKnownPkTypes!=PKTYPES_UNKNOWN )
		return "";
	return "map<" + get_table_key() + ", class TableRow*, " + get_table_key_comparer() + "> cachedRows;";
}
string TableInfo_Generator::get_map_deleted_cached_rows()
{
	if( m_eKnownPkTypes!=PKTYPES_UNKNOWN )
		return "";
	return "map<" + get_table_key() + ", class TableRow*, " + get_table_key_comparer() + "> deleted_cachedRows;";
}

string TableInfo_Generator::get_table_key_comparer()
{
	if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG )
		return "SingleLongKey_Less";
	else if( m_eKnownPkTypes==PKTYPES_DOUBLE_LONG )
		return "DoubleLongKey_Less";
	else if( m_eKnownPkTypes==PKTYPES_TRIPLE_LONG )
		return "TripleLongKey_Less";
	else if( m_eKnownPkTypes==PKTYPES_SINGLE_STRING )
		return "SingleStringKey_Less";
	else if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG64 )
		return "SingleLong64Key_Less";
	else
		return "Table_" + m_sTableName + "::Key_Less";
}

string TableInfo_Generator::get_primary_fields_in_untyped_list()
{
	string s;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
	{
		if (s != "")
			s = s + ", ";
			
		s = s + "in_" + (*i)->m_pcFieldName;
	}
	
	return s;
}

string TableInfo_Generator::get_primary_fields_assign_list()
{
	string s;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
			s = s + "pk_" + (*i)->m_pcFieldName + " = in_" + (*i)->m_pcFieldName + ";\n";
	
	return s;
}

string TableInfo_Generator::get_primary_autoinc_key_init()
{
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if (((*i)->m_iFlags & PRI_KEY_FLAG) && ((*i)->m_iFlags & AUTO_INCREMENT_FLAG ))
			return string( "if (id!=0)\n\t\tpRow->m_" ) + (*i)->m_pcFieldName + "=id;\nelse \n\t\tLoggerWrapper::GetInstance()->Write(LV_CRITICAL,\"" + (*i)->m_pcFieldName + " is auto increment but has no value %s\",database->m_sLastDBError.c_str());";
	
	return "";
}

string TableInfo_Generator::get_primary_fields_assign_from_row()
{
	string s;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
			s = s + "pk_" + (*i)->m_pcFieldName + " = pRow->m_" + (*i)->m_pcFieldName + ";\n";
	
	return s;
}

string TableInfo_Generator::get_primary_fields()
{
	string s;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
			s = s + (s.length()>0 ? "," : "") + "pRow->m_" + (*i)->m_pcFieldName;
	
	return s;
}

string TableInfo_Generator::get_key_less_statement()
{
	string s;

	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
			s = s + "if (key1.pk_" + (*i)->m_pcFieldName + "!=key2.pk_" + (*i)->m_pcFieldName + ")"
					+ "\n" + "return key1.pk_" + (*i)->m_pcFieldName + "<key2.pk_" + (*i)->m_pcFieldName + ";"
					+ "\n" + "else" + "\n";
	
	s = s + "return false;";
	
	return s;
}

string TableInfo_Generator::get_set_default_values()
{
	string s;
	
	int field_index=0;
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, field_index++ )
	{
		if ((*i)->m_pcFieldDefaultValue == NULL)
		{
			if (!((*i)->m_iFlags & NOT_NULL_FLAG))
				s = s + "is_null[" + int2string(field_index) + "] = true;\n";
			else
				s = s + "is_null[" + int2string(field_index) + "] = false;\n";
			if ((*i)->getCType() != "string" && ((*i)->getCType() != "unsupported_type") && ((*i)->getCType() != "MYSQL_DATE"))
				s = s + "m_" + (*i)->m_pcFieldName + " = 0;\n";
		}
		else
			if ((*i)->getCType() == "string")
			{
				unsigned long max_length = (unsigned long)(strlen((*i)->m_pcFieldDefaultValue)*2 + 1);
				
				char *to = new char[max_length];
				if (max_length>1)
					mysql_real_escape_string( m_pDB, to, (*i)->m_pcFieldDefaultValue, (unsigned long) strlen((*i)->m_pcFieldDefaultValue) ); 
				else
					to[0] = 0;

				s = s + "m_" + (*i)->m_pcFieldName + " = \"" + to + "\";\n";
				s = s + "is_null[" + int2string(field_index) + "] = false;\n";
				delete[] to;
			}
			else
				if (((*i)->getCType() != "unsupported_type") && ((*i)->getCType() != "MYSQL_DATE")) /** @todo how to handle it? */
				{
					s = s + "m_" + (*i)->m_pcFieldName + " = " + (*i)->m_pcFieldDefaultValue + ";\n";
					s = s + "is_null[" + int2string( field_index ) + "] = false;\n";
				}
		
	}
	
	return s;
}
	
string TableInfo_Generator::get_fields_count()
{
	return int2string((int) m_Fields.size());
}

string TableInfo_Generator::get_primary_fields_count()
{
	int count=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
			count++;

	return int2string(count);
}

string TableInfo_Generator::get_primary_fields_where_list()
{
	string s="";
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
		{
			if (s != "")
				s = s + " AND ";
			s = s + (*i)->m_pcFieldName + "=?";
		}
		
	return s;
}

string TableInfo_Generator::get_bind_parameters_to_key()
{
	string s="";
	int iIndex=0;
	int iPrimaryIndex=0;
	int iPKCounter=0;  // How many primary keys we've found so far for the KnownPkTypes with double
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
		{
			iPKCounter++;
			s = s + "is_param_null[" + int2string( iPrimaryIndex) + "]=false;\n";
			s = s + "parameters[" + int2string(iPrimaryIndex) + "].buffer_type=" + (*i)->getMType() + ";\n";
			
			if ((*i)->getCType() != "string")
			{
				if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG )
					s = s + "parameters[" + int2string(iPrimaryIndex) + "].buffer=&(key.pk);\n";
				else if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG64 )
					s = s + "parameters[" + int64_2string(iPrimaryIndex) + "].buffer=&(key.pk);\n";
				else if( m_eKnownPkTypes==PKTYPES_DOUBLE_LONG || m_eKnownPkTypes==PKTYPES_TRIPLE_LONG )
					s = s + "parameters[" + int2string(iPrimaryIndex) + "].buffer=&(key.pk" + StringUtils::itos(iPKCounter) + ");\n";
				else
					s = s + "parameters[" + int2string(iPrimaryIndex) + "].buffer=&(key.pk_" + (*i)->m_pcFieldName + ");\n";
				s = s + "parameters[" + int2string(iPrimaryIndex) + "].length=0;\n";
			}
			else
			{
				s = s + "char tmp_" + (*i)->m_pcFieldName + "[" + int2string((*i)->m_iLength+1) + "];\n";
				if( m_eKnownPkTypes==PKTYPES_SINGLE_STRING )
					s = s + "snprintf(tmp_" + (*i)->m_pcFieldName + ", "+ int2string((*i)->m_iLength+1)+", \"%*s\", key.pk"
							+ ".length(), key.pk.c_str());\n";
				else
					s = s + "snprintf(tmp_" + (*i)->m_pcFieldName + ", "+ int2string((*i)->m_iLength+1)+", \"%*s\", key.pk_" + (*i)->m_pcFieldName
							+ ".length(), key.pk_" + (*i)->m_pcFieldName + ".c_str());\n";
				s = s + "parameters[" + int2string(iPrimaryIndex) + "].buffer=tmp_" + (*i)->m_pcFieldName + ";\n";
				s = s + "parameters[" + int2string(iPrimaryIndex) + "].length=&param_lengths["  + int2string(iPrimaryIndex) +  "];\n";
			}
			
			s = s + "parameters[" + int2string(iPrimaryIndex) + "].is_null=0;\n";

			s = s + "\n";
					
			iPrimaryIndex++;
		}
	
		return s;
}

string TableInfo_Generator::get_bind_results_to_fields()
{
	string s="";
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
	{
		s = s + "results[" + int2string(iIndex) + "].buffer_type=" + (*i)->getMType() + ";\n";
			
		if ((*i)->getCType() != "string")
		{
			s = s + "results[" + int2string(iIndex) + "].buffer=&(pRow->m_" + (*i)->m_pcFieldName + ");\n";
			s = s + "results[" + int2string(iIndex) + "].length=0;\n";
		}
		else
		{
			s = s + "char rtmp_" + (*i)->m_pcFieldName + "[" + int2string((*i)->m_iLength+1) + "];\n";
			s = s + "results[" + int2string(iIndex) + "].buffer=rtmp_" + (*i)->m_pcFieldName + ";\n";
			s = s + "results[" + int2string(iIndex) + "].length=&result_lengths["  + int2string(iIndex) +  "];\n";
		}
			
		s = s + "results[" + int2string(iIndex) + "].is_null=&(pRow->is_null["+int2string(iIndex)+"]);\n";
		
		s = s + "\n";
	}
	
	return s;
}

string TableInfo_Generator::get_init_row_from_temp_string_fields()
{
	string s="";
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
	{
		if ((*i)->getCType() == "string")
			s = s + "pRow->m_" + (*i)->m_pcFieldName + " = string(rtmp_" + (*i)->m_pcFieldName + ",result_lengths["  + int2string(iIndex) +  "]);\n" ;
		
		s = s + "\n";
	}
	
	return s;
}

string TableInfo_Generator::get_build_query_for_key()
{
	string sCode="";
	string sCondition=""; //default true to keep ..AND stuff happy
	int iPKCounter=0;  // How many primary keys we've found so far for the KnownPkTypes with double
		
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ( (*i)->m_iFlags & PRI_KEY_FLAG )
		{
			iPKCounter++;
			if (sCondition!="")
				sCondition = sCondition + "+\" AND \"+";
			
			sCondition = sCondition + "\"`" + (*i)->m_pcFieldName + "`=\"";
			
			//temporary buffer			
			
			if (((*i)->getCType() != "string") && ((*i)->getCType() != "unsupported_type"))
			{
				sCode = sCode + "char tmp_" + (*i)->m_pcFieldName + "[" ;
				sCode = sCode + "32];\n";
				if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG )
					sCode = sCode + "sprintf(tmp_" + (*i)->m_pcFieldName+", \"" + (*i)->getPrintFormat() + "\", " + "key.pk);\n\n";
				else if( m_eKnownPkTypes==PKTYPES_SINGLE_LONG64 )
					sCode = sCode + "sprintf(tmp_" + (*i)->m_pcFieldName+", \"" + (*i)->getPrintFormat() + "\", " + "key.pk);\n\n";
				else if( m_eKnownPkTypes==PKTYPES_DOUBLE_LONG || m_eKnownPkTypes==PKTYPES_TRIPLE_LONG )
					sCode = sCode + "sprintf(tmp_" + (*i)->m_pcFieldName+", \"" + (*i)->getPrintFormat() + "\", " + "key.pk" + StringUtils::itos(iPKCounter) + ");\n\n";
				else
					sCode = sCode + "sprintf(tmp_" + (*i)->m_pcFieldName+", \"" + (*i)->getPrintFormat() + "\", " + "key.pk_" + (*i)->m_pcFieldName + ");\n\n";
				sCondition = sCondition + " + tmp_" + (*i)->m_pcFieldName;
			}
			else
				if ((*i)->getCType() == "string")
				{
					sCode = sCode + "char tmp_" + (*i)->m_pcFieldName + "[" ;
					sCode = sCode + int2string(1+2*(*i)->m_iLength) + "];\n";

					if( m_eKnownPkTypes==PKTYPES_SINGLE_STRING )
						sCode = sCode + "db_wrapper_real_escape_string(database->m_pDB,tmp_" + (*i)->m_pcFieldName 
								+ ", key.pk.c_str(), (unsigned long) key.pk.size());\n\n";
					else
						sCode = sCode + "db_wrapper_real_escape_string(database->m_pDB,tmp_" + (*i)->m_pcFieldName 
								+ ", key.pk_" + (*i)->m_pcFieldName + ".c_str(), (unsigned long) key.pk_" + (*i)->m_pcFieldName + ".size());\n\n";
					sCondition = sCondition + " + \"\\\"\" + tmp_" + (*i)->m_pcFieldName + "+ \"\\\"\"";
				}
				else
				{
					sCondition = sCondition + " + \"\\\"\" + key.pk_" + (*i)->m_pcFieldName + ".c_str()+ \"\\\"\"";
				}
			
			
		}
		return  sCode + "\n" + "string condition;\n" + "condition = condition + " + sCondition + ";\n";
}

string TableInfo_Generator::get_generate_insert_query()
{
	string sCode="";
	string sCondition="";
	int iIndex=0;	
	
	string values_list="";
	
		
	for (vector<FieldInfo*>::iterator i=m_Fields.begin(); i!=m_Fields.end(); i++, iIndex++)
	{
		if( strcmp((*i)->m_pcFieldName,"psc_mod")==0 )
			continue;

		if (values_list!="")
			values_list = values_list + "+\", \"+";
		
		values_list = values_list + "pRow->"+(*i)->m_pcFieldName + "_asSQL()";
	}
	
	return  sCode + "\n" + "string values_list_comma_separated;\n" + "values_list_comma_separated = values_list_comma_separated + " + values_list + ";\n";
}

string TableInfo_Generator::get_generate_update_query()
{
	string code="";
	string condition="";
	int iIndex=0;	
	
	string values_list="";
	
		
	for (vector<FieldInfo*>::iterator i=m_Fields.begin(); i!=m_Fields.end(); i++, iIndex++)
	{
		if( strcmp((*i)->m_pcFieldName,"psc_mod")==0 )
			continue;

		if (values_list!="")
			values_list = values_list + "+\", `" + (*i)->m_pcFieldName + "`=\"+";
		else
			values_list = values_list + "\"`"+(*i)->m_pcFieldName + "`=\"+";
		
		values_list = values_list + "pRow->"+(*i)->m_pcFieldName + "_asSQL()";
	}
	
	return  code + "\n" + "string update_values_list;\n" + "update_values_list = update_values_list + " + values_list + ";\n";
}

string TableInfo_Generator::get_init_fields_from_row()
{
	string s="";
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
	{
		s = s + "if (row[" + int2string(iIndex) + "] == NULL)\n" +
				"{\n" + "pRow->is_null["+int2string(iIndex)+"]=true;\n";
		
		if (((*i)->getCType() == "string")||((*i)->getCType() == "unsupported_type"))
			s = s + "pRow->m_" + (*i)->m_pcFieldName + " = \"\";\n" ;
		else
			s = s + "pRow->m_" + (*i)->m_pcFieldName + " = 0;\n" ;
		
		s = s + "}\nelse\n{\n";
		s = s + "pRow->is_null["+int2string(iIndex)+"]=false;\n";
		
		if (((*i)->getCType() == "string")||((*i)->getCType() == "unsupported_type"))
			s = s + "pRow->m_" + (*i)->m_pcFieldName + " = string(row[" + int2string(iIndex) + "],lengths["  + int2string(iIndex) +  "]);\n" ;
		else
			s = s + "sscanf(row[" + int2string(iIndex) + "], \"" + (*i)->getScanFormat() + "\", &(pRow->m_" + (*i)->m_pcFieldName + "));\n";
		
		s = s + "}\n\n";
	}

	return s;
}

string TableInfo_Generator::get_ifdef_have_primary_keys()
{
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ( (*i)->m_iFlags & PRI_KEY_FLAG )
			return "";
	return "/*\n";
}

string TableInfo_Generator::get_endif_have_primary_keys()
{
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
		if ((*i)->m_iFlags & PRI_KEY_FLAG)
			return "";
	return "*/\n";
}

string TableInfo_Generator::get_fields_list_comma_separated()
{
	string s;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
	{
		if( strcmp((*i)->m_pcFieldName,"psc_mod")==0 )
			continue;

		if (s != "")
			s = s + ", ";	
		s = s + "`" + (*i)->m_pcFieldName + "`";
	}
	
	return s;
}

string TableInfo_Generator::get_fields_sql_getters_declaration()
{
	string sCode="";
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
		sCode = sCode + "string "+(*i)->m_pcFieldName+"_asSQL();\n";
		
	return sCode;
}


string TableInfo_Generator::get_fields_sql_getters_definition()
{
	string sCode="";
	int iIndex=0;
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++, iIndex++ )
	{
		sCode = sCode + "string Row_"+m_sTableName+"::"+(*i)->m_pcFieldName+"_asSQL()\n{\nPLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);\n\n";
		sCode = sCode + "if (is_null["+int2string(iIndex)+"])\n";
		sCode = sCode + "return \"NULL\";\n\n";
		
		
		if (((*i)->getCType() != "string") && ((*i)->getCType() != "unsupported_type"))
		{
			sCode = sCode + "char buf[32];\n";
			sCode = sCode + "sprintf(buf, \"" + (*i)->getPrintFormat() + "\", " + "m_" + (*i)->m_pcFieldName + ");\n\n";
			sCode = sCode + "return buf;\n";
		}
		else
			if ((*i)->getCType() == "string")
		{
			unsigned int iSize = (unsigned int)(1+2*(*i)->m_iLength);
			if( iSize>5000000 )
				iSize=5000000;  // Don't allow runaway fields with big text.  Assume 5MB is the maximum we would need to use

			sCode = sCode + "char *buf = new char[" + int2string(iSize) + "];\n";		
			sCode = sCode + "db_wrapper_real_escape_string(table->database->m_pDB, buf, m_" + (*i)->m_pcFieldName + 
				".c_str(), (unsigned long) min((size_t)" + StringUtils::itos((*i)->m_iLength) + ",m_" + (*i)->m_pcFieldName + ".size()));\n";

			sCode = sCode + "string s=string("")+\"\\\"\"+buf+\"\\\"\";\n";
			sCode = sCode + "delete[] buf;\n";
			sCode = sCode + "return s;\n";
		}
		else
			sCode = sCode + "return string("")+\"\\\"\"+m_" + (*i)->m_pcFieldName+"+\"\\\"\";\n";
		
		sCode = sCode + "}\n\n";
	}	
		
	return sCode;
}

string TableInfo_Generator::rows_getters_declaration()
{
	string s="";
	
	for ( vector<FieldInfo*>::iterator i = m_Fields.begin(); i != m_Fields.end(); i++ )
	{
		string sTableName2 =  getTableFromForeignKey( (*i)->m_pcFieldName, m_pTables );
		if( sTableName2.length()==0 )
			continue;

		s = s + "class Row_" + sTableName2 + "* "+(*i)->m_pcFieldName + "_getrow();\n";
	}
	
	return s;
}

string TableInfo_Generator::rows_getters_definition()
{
	string s="";
	
	for ( vector<FieldInfo*>::iterator i=m_Fields.begin(); i!=m_Fields.end(); i++ )
	{
			
		string sRefTable =  getTableFromForeignKey((*i)->m_pcFieldName,m_pTables);
		if( sRefTable.length()==0 )
			continue;

		string field_name = (*i)->m_pcFieldName;
							
		s = s + "class Row_" + sRefTable + "* Row_" + m_sTableName + "::" + (*i)->m_pcFieldName+"_getrow()\n";
		s = s + "{\nPLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);\n\n";
		s = s + "class Table_"+sRefTable+" *pTable = table->database->"+sRefTable+"_get();\n";
		s = s + "return pTable->GetRow(m_" + field_name + ");\n";
		s = s + "}\n";
	}
	
	return s;
}

string TableInfo_Generator::rows_getters_includes()
{
	string s="";
	
	for ( vector<FieldInfo*>::iterator i=m_Fields.begin(); i != m_Fields.end(); i++ )
	{
	      	string m_sTableName = getTableFromForeignKey((*i)->m_pcFieldName,m_pTables);
	        if( m_sTableName.length()==0 && m_sTableName!=get_table_name() )
			continue;
		// don't include entries to pluto sqlcvs accessor files, they are not created
		if ( m_sTableName.find("_psch")!=string::npos )
			continue;
		s = s + "#include \"Table_"+m_sTableName+".h\"\n";

	}
	
	return s;
}

string TableInfo_Generator::fk_rows_getters_declaration()
{
    string s="";
	
	map<string,TableInfo_Generator *>::iterator i;
    for ( i = m_pTables->begin(); i != m_pTables->end(); ++i )
    {
		TableInfo_Generator *info = (*i).second;
		for( size_t i=0; i < info->m_Fields.size(); ++i )
		{
			FieldInfo *field = info->m_Fields[i];

			// don't include entries to pluto sqlcvs accessor files, they are not created
			if ( info->get_table_name().find("_psch")!=string::npos )
				continue;

			if( getTableFromForeignKey(field->m_pcFieldName,m_pTables)==get_table_name() )
			{
				s = s + "void " + info->get_table_name() + "_" + field->m_pcFieldName + "_getrows(vector <class Row_" + info->get_table_name() + "*> *rows);\n";
			}
		}
    }

    return s;
}

string TableInfo_Generator::fk_rows_getters_definition()
{
    string s="";

    map<string,TableInfo_Generator *>::iterator i;
    for ( i=m_pTables->begin(); i != m_pTables->end(); ++i )
    {
        TableInfo_Generator *info = (*i).second;
        for( size_t i=0; i < info->m_Fields.size(); ++i )
        {
            FieldInfo *field = info->m_Fields[i];

	    // don't include entries to pluto sqlcvs accessor files, they are not created
	    if ( info->get_table_name().find("_psch")!=string::npos )
		continue;

            if( getTableFromForeignKey(field->m_pcFieldName,m_pTables)==get_table_name() )
            {
                s = s + "void Row_" + get_table_name() + "::"+ info->get_table_name() + "_" + field->m_pcFieldName + "_getrows(vector <class Row_" + info->get_table_name() + "*> *rows)\n";
				s = s + "{\nPLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);\n\n";
				s = s + "class Table_"+info->get_table_name()+" *pTable = table->database->"+info->get_table_name()+"_get();\n";
				s = s + "pTable->GetRows(\"`"+ field->m_pcFieldName + "`=\" + StringUtils::itos(m_PK_" + get_table_name() + "),rows);\n";
                s = s + "}\n";
            }
        }
    }

    return s;
}

string TableInfo_Generator::fk_rows_getters_includes()
{
    string s="";

   	map<string,TableInfo_Generator *>::iterator i;
    for ( i=m_pTables->begin(); i != m_pTables->end(); ++i )
    {
        TableInfo_Generator *info = (*i).second;

	// don't include entries to pluto sqlcvs accessor files, they are not created
	if ( info->get_table_name().find("_psch")!=string::npos )
	{
		continue;
	}

        for( size_t i=0; i < info->m_Fields.size(); ++i )
        {
            FieldInfo *field = info->m_Fields[i];
            if( getTableFromForeignKey( field->m_pcFieldName, m_pTables ) == get_table_name() )
            {
	            s = s + "#include \"Table_" + info->get_table_name() + ".h\"\n";
            }
        }
    }

    return s;
}

string TableInfo_Generator::table_constants_defines()
{
	string s="";

	string sSQL = "SELECT PK_" + get_table_name() + ",Description,Define FROM " + get_table_name() + " ORDER BY PK_" + get_table_name();
	int iresult=mysql_query(m_pDB,sSQL.c_str());
	if( iresult!=0 )
	{
		return "";
	}
	MYSQL_RES *res = mysql_store_result(m_pDB);
	MYSQL_ROW row;

	map<string,int> mapDefines; // Be sure there's no dups
	while ((row = mysql_fetch_row(res)))
	{
		if( !row[1] && !row[2] )
			continue; // There must be either a description or define

		const char *define = row[2] && row[2][0]!=0 ? row[2] : row[1]; // Use the Define column unless it's empty
		
		string DefineString = FileUtils::ValidCPPName(define);
		if( mapDefines.find(DefineString)!=mapDefines.end() )
			cout << "#define table: " << get_table_name() << " define: " << DefineString << " is a dup" << endl;
		else
		{
			mapDefines[DefineString]=1;
			s += "#define " + StringUtils::ToUpper(get_table_name()) + "_" + DefineString + "_CONST " + row[0] + "\n";
		}
	}

	return s;
}

string TableInfo_Generator::table_struct_defines()
{
	string s="#define " + StringUtils::ToUpper(get_table_name()) + "_TABLE \"" + get_table_name() + "\"\n";
	for (vector<FieldInfo*>::iterator i=m_Fields.begin(); i!=m_Fields.end(); i++)
	{
		s += "#define " + StringUtils::ToUpper(get_table_name()) + "_" + StringUtils::ToUpper((*i)->m_pcFieldName) + "_FIELD \"" + (*i)->m_pcFieldName + "\"\n";
		s += "#define " + StringUtils::ToUpper(get_table_name()) + "_" + StringUtils::ToUpper((*i)->m_pcFieldName) + "_TABLE_FIELD \"" + get_table_name() + "." + (*i)->m_pcFieldName + "\"\n";
	}
	return s;
}
