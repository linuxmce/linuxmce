#ifndef __DB_WRAPPER_MYSQL_H__
#define __DB_WRAPPER_MYSQL_H__

#if defined(WIN32) || defined(__APPLE_CC__)
	#include <mysql.h>
#else
	#include <mysql/mysql.h>
#endif

// data structures
#define DB_LINK MYSQL
#define DB_ROW  MYSQL_ROW
#define DB_RES  MYSQL_RES

// functions
/*#define db_wrapper_init				mysql_init*/
DB_LINK* db_wrapper_init(DB_LINK *db_link);
#define db_wrapper_free_result		mysql_free_result
#define db_wrapper_close			mysql_close
#define db_wrapper_real_connect		mysql_real_connect
#define db_wrapper_set_character_set		mysql_set_character_set
#define db_wrapper_error			mysql_error
#define db_wrapper_query			mysql_query
#define db_wrapper_real_query		mysql_real_query
#define db_wrapper_fetch_row		mysql_fetch_row
#define db_wrapper_store_result		mysql_store_result
#define db_wrapper_affected_rows	mysql_affected_rows
#define db_wrapper_insert_id		mysql_insert_id
#define db_wrapper_real_escape_string	mysql_real_escape_string
#define db_wrapper_fetch_lengths		mysql_fetch_lengths
#define db_wrapper_field_count			mysql_field_count
#define db_wrapper_thread_init			mysql_thread_init
#define db_wrapper_thread_end			mysql_thread_end

#endif

