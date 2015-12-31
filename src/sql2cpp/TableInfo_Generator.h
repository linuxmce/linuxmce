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
 * @file TableInfo_Generator.h
 * @brief header file for the TableInfo_Generator class
 *
 */

#ifndef TABLEINFO_GENERATOR_H
#define TABLEINFO_GENERATOR_H

#include "TableInfo.h"

#include <string>
#include <map>

using namespace std;

// We need to create a maps using stl templates which the key of which is used to lookup any rows.
// We create a custom struct within each table class, but if we use this, then stl will rebuild an
// entire new copy of the map template for this type of object, making the database code become
// incredibly huge.  So, we identify some standard primary key, like a single long number, or 2 longs,
// and use those stock structures instead whenever possible.
enum KnownPkTypes { PKTYPES_UNKNOWN,PKTYPES_SINGLE_LONG,PKTYPES_DOUBLE_LONG,PKTYPES_TRIPLE_LONG,PKTYPES_SINGLE_STRING,PKTYPES_SINGLE_LONG64 };

class TableInfo_Generator : public TableInfo
{

private:
	KnownPkTypes m_eKnownPkTypes; /** < what type of primary key this table has */
	string m_sTableClassPrefix; /** < prefix of the table class */
	string m_sRowClassPrefix; /** < prefix of the row class */
	string m_sPrimaryKeyClassPrefix; /** < prefix of the primary class */

private:

	map<string,TableInfo_Generator*> *m_pTables; /** < tables */
	
public:

	/**
	 * @brief constructor
	 */
	TableInfo_Generator( MYSQL * pDB, map<string,TableInfo_Generator*> *pTables );

	/** @todo add destructor */
	
	/**
	 * @brief does a switch based on the parameter string and calls one of the appropiate templates below
	 */
	string ExpandString( string s );

	/**
	 * @brief override the default implementation to match primary key types
	 */
	virtual bool loadFromDB( string sTable );

	/** following: template variables -- get_VAR_NAME -- @VAR_NAME@ */
	/** they turn an ellement of the table    structure to c++ code */

	string get_fields_declaration(); /** < declarations for C++ member variables coresponding to fields */
	string get_member_variable_list(); /** < member variable names with '+' between them */

	string get_getters_declaration(); /** < returns declarations for _get() field read access member functions */
	string get_getters_definition(); /** < returns definitions for _get() field read access member functions */
	string get_setters_declaration(); /** < returns declarations for _get() field write acces member functions */
	string get_setters_definition(); /** < returns declarations for _get() field write acces member functions */
	string get_null_getters_declaration(); /** < returns declarations for _get() field check if null member functions */
	string get_null_getters_definition(); /** < returns definitions for _get() field check if null member functions */
	string get_null_setters_declaration(); /** < returns declarations for _get() field make null member functions */
	string get_null_setters_definition(); /** < returns definitions for _get() field make null member functions */
	
	string get_primary_fields_declaration(); /** < declarations for C++ member variables coresponding to primary fields */
	string get_primary_fields_in_typed_list(); /** < member variables coresponding to primary fields in a comma separated list with the type included */
	string get_primary_fields_in_untyped_list(); /** < member variables coresponding to primary fields in a comma separated list */
	string get_primary_fields_assign_list(); /** < generates code for assigning from pk_ mb to in_ for primary key fields */
	string get_primary_autoinc_key_init();  /** < if id != 0 initializes field with the value of id */
	string get_primary_fields(); /** < get all the primary keys in pRow */
	string get_primary_fields_assign_from_row(); /** < assign to the primary key values taken from row */
	string get_table_key(); /** < what struct we will use storing row pointers in a map (like Key) */
	string get_table_base(); /** < what base class to use if we have a known type of key */
	string get_map_cached_rows(); /** < get the map for the cached rows if we're not using a predefined type */
	string get_map_deleted_cached_rows(); /** < get the map for the deleted cached rows if we're not using a predefined type */
	string get_table_key_comparer(); /** < what struct we will use for comparing when we store row pointers in a map (like Key_Less) */
	string get_key_less_statement(); /** < genearates code for return sum of booth keys if they are distinct */
	string get_set_default_values(); /** < gets/sets the default value */
	string get_fields_count(); /** < returns the number of fields in mFields as a string */
	string get_primary_fields_count(); /** < returns the number of primary fields in mFields as a string */
	string get_primary_fields_where_list(); /** returns a AND separated list of fieldname=? */
	string get_bind_parameters_to_key(); /** code that binds paramaters to key */
	string get_bind_results_to_fields(); /** code that binds results to fields */
	string get_init_row_from_temp_string_fields(); /** gets initial row from string fields */
	
	string get_build_query_for_key(); /** builds query for key fields */
	string get_generate_insert_query(); /** generates code for insert query */
	string get_generate_update_query(); /** generates code for update query */
	string get_init_fields_from_row(); /** inits fields from table row */
	string get_ifdef_have_primary_keys(); /** returns \n if no primary key defined and "" otherwise */
	string get_endif_have_primary_keys(); /** returns * /\n if no primary key defined and "" otherwise */
	string get_fields_list_comma_separated(); /** returns the field names separated by comma */
	
	string get_fields_sql_getters_declaration(); /** generate declarations for sql getter methods */
	string get_fields_sql_getters_definition(); /** generate definitions for sql getter methods */
																			
	string rows_getters_declaration(); /** generate code for row getting */
	string rows_getters_definition(); /** generate definitions for row getting */
	string rows_getters_includes(); /** makes sure that all the files (classes) defined for row getting are included */ 
	
	string fk_rows_getters_declaration(); /** generates code that enables you to go back through a foreign key to the source table - declarations */
	string fk_rows_getters_definition(); /** generates code that enables you to go back through a foreign key to the source table - definitions */
	string fk_rows_getters_includes(); /** includes all the generated files that enable you to go back through a foreign key to the source table */
				
	string table_constants_defines(); /** generate defines for he table constants */
	string table_struct_defines(); /** generate defines for the table structure */
};

#endif
