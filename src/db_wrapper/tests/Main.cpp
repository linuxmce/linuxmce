#include <iostream>
#include <string>

#include "db_wrapper/db_wrapper.h"

#ifdef USE_MYSQL_WRAPPER
#undef _mysql_h
#endif

#include <mysql/mysql.h>

using namespace std;

void do_fail(string s)
{
	cout << "[FAIL]\t" << s <<endl;
	exit (1);
}

void do_pass(string s)
{
	cout << "[PASS]\t" << s <<endl;
}

void try_to(bool b, string s)
{
	if (b)
		do_pass(s);
	else
		do_fail(s);
}

//TODO add test for inserts with db_wrapper_real_query

int main()
{
	DB_LINK db_link;
	MYSQL mysql_link;

	cout << "Running tests:" << endl;
	
	// init DB structures
	cout << endl << "Initialize DB structures.." << endl;
	
	try_to( db_wrapper_init(&db_link), "db_wrapper_init");
	try_to( mysql_init(&mysql_link), "mysql_init");
	
	// connect to DB
	cout << endl << "Connecting to DB.." << endl;
	
	try_to(db_wrapper_real_connect(&db_link, "localhost", "root", "", "pluto_test", 3306, NULL, 0), "db_wrapper_real_connect");
	try_to(mysql_real_connect(&mysql_link, "localhost", "root", "", "pluto_test", 3306, NULL, 0), "mysql_real_connect");

	// execute trivial query, fetch results, compare
	{
		string query = "SELECT 2+2";
		cout << endl << "Executing simple math query: " << query << endl;
		
		try_to(db_wrapper_query(&db_link, query.c_str())==0, "db_wrapper_query");
		try_to(mysql_query(&mysql_link, query.c_str())==0, "mysql_query");
	
		DB_RES *db_res = db_wrapper_store_result(&db_link);
		try_to(db_res, "db_wrapper_store_result");
		
		MYSQL_RES *mysql_res = mysql_store_result(&mysql_link);
		try_to(mysql_res, "mysql_store_result");
		
		cout << "DB rows: " << db_res->row_count << endl << "MYSQL rows: " << mysql_res->row_count << endl;
		try_to( (mysql_res->row_count == db_res->row_count), "rows count is OK");
		try_to( (mysql_res->field_count == db_res->field_count), "field count is OK");
		
		DB_ROW db_row = db_wrapper_fetch_row(db_res);
		try_to(db_row, "db_wrapper_fetch_row");
		
		MYSQL_ROW mysql_row = mysql_fetch_row(mysql_res);
		try_to(mysql_row, "mysql_fetch_row");
		
		cout << "db_row[0] = " << db_row[0] << endl;
		cout << "mysql_row[0] = " << mysql_row[0] << endl;
		
		try_to(strcmp(db_row[0], mysql_row[0])==0, "db_wrapper_fetch_row()  == mysql_fetch_row()");
		
		db_wrapper_free_result(db_res);
		mysql_free_result(mysql_res);
	}
	
	// execute more complex query, fetch results, compare
	{
		string query = "SELECT * FROM MediaType_DesignObj ORDER BY PK_MediaType_DesignObj";
		cout << endl << "Executing some query: " << query << endl;
		
		try_to(db_wrapper_query(&db_link, query.c_str())==0, "db_wrapper_query");
		try_to(mysql_query(&mysql_link, query.c_str())==0, "mysql_query");
	
		DB_RES *db_res = db_wrapper_store_result(&db_link);
		try_to(db_res, "db_wrapper_store_result");
		
		MYSQL_RES *mysql_res = mysql_store_result(&mysql_link);
		try_to(mysql_res, "mysql_store_result");
		
		cout << "DB rows: " << db_res->row_count << endl << "MYSQL rows: " << mysql_res->row_count << endl;
		try_to( (mysql_res->row_count == db_res->row_count), "rows count is OK");
		try_to( (mysql_res->field_count == db_res->field_count), "field count is OK");
		
		uint db_field_cnt = db_wrapper_field_count(&db_link);
		uint mysql_field_cnt = mysql_field_count(&mysql_link);
		try_to(db_field_cnt = mysql_field_cnt, " db_wrapper_field_count() == mysql_field_count()");
		
		DB_ROW db_row = db_wrapper_fetch_row(db_res);
		MYSQL_ROW mysql_row = mysql_fetch_row(mysql_res);
		
		try_to( ( (db_row!=NULL && mysql_row!=NULL)  || (db_row==NULL && mysql_row==NULL) ) , "db_wrapper_fetch_row() and mysql_fetch_row() are of same kind null or non-null");
		
		int i = 0;
				
		while ( db_row && mysql_row )
		{
			cout << "row #" << i+1 << endl;

			unsigned long *db_row_lengths = db_wrapper_fetch_lengths(db_res);
			try_to(db_row_lengths, "db_wrapper_fetch_lengths");
			
			unsigned long *mysql_row_lengths = mysql_fetch_lengths(mysql_res);
			try_to(mysql_row_lengths, "mysql_fetch_lengths");
			
			for (int j=0; j<db_field_cnt; j++)
			{
				cout << "field #" << j+1 << endl;
//				cout << "db_row[" << j <<" ] = " << db_row[j] << endl;
//				cout << "mysql_row[" << j <<" ] = " << mysql_row[j] << endl;
				
				try_to(db_row_lengths[j]==mysql_row_lengths[j], "db_row_lengths[j]==mysql_row_lengths[j]");
				unsigned long length = db_row_lengths[j];
				
				if ( !db_row[j] || !mysql_row[j] )
					try_to(db_row[j] == mysql_row[j], "db_wrapper_fetch_row[j]  == mysql_fetch_row[j]");
				else
					try_to(memcmp(db_row[j], mysql_row[j], length)==0, "db_wrapper_fetch_row[j]  == mysql_fetch_row[j]");
			}
			
			db_row = db_wrapper_fetch_row(db_res);
			mysql_row = mysql_fetch_row(mysql_res);
			
			try_to( ( (db_row!=NULL && mysql_row!=NULL)  || (db_row==NULL && mysql_row==NULL) ) , "db_wrapper_fetch_row() and mysql_fetch_row() are of same kind null or non-null");
			
			i++;
		}
		
		db_wrapper_free_result(db_res);
		mysql_free_result(mysql_res);
	}
	
	// execute even more complex query, fetch results, compare
	{
		// another query
		string query_tmp = "SELECT * FROM CommandParameter ORDER BY PK_CommandParameter";
		cout << endl << "Executing some other query with binary fields also: " << query_tmp << endl;
		
		try_to(db_wrapper_real_query(&db_link, query_tmp.c_str(), query_tmp.length() )==0, "db_wrapper_real_query");
		try_to(mysql_real_query(&mysql_link, query_tmp.c_str(), query_tmp.length() )==0, "mysql_real_query");
		
		DB_RES *db_res_tmp = db_wrapper_store_result(&db_link);
		try_to(db_res_tmp, "db_wrapper_store_result");
		
		MYSQL_RES *mysql_res_tmp = mysql_store_result(&mysql_link);
		try_to(mysql_res_tmp, "mysql_store_result");
		
		cout << "DB rows: " << db_res_tmp->row_count << endl << "MYSQL rows: " << mysql_res_tmp->row_count << endl;
		try_to( (mysql_res_tmp->row_count == db_res_tmp->row_count), "rows count is OK");
		try_to( (mysql_res_tmp->field_count == db_res_tmp->field_count), "field count is OK");
		
		uint db_field_cnt_tmp = db_wrapper_field_count(&db_link);
		uint mysql_field_cnt_tmp = mysql_field_count(&mysql_link);
		try_to(db_field_cnt_tmp = mysql_field_cnt_tmp, " db_wrapper_field_count() == mysql_field_count()");
		
		
		string query = "SELECT * FROM Command ORDER BY PK_Command";
		cout << endl << "Executing some query with binary fields also: " << query << endl;
		
		try_to(db_wrapper_real_query(&db_link, query.c_str(), query.length() )==0, "db_wrapper_real_query");
		try_to(mysql_real_query(&mysql_link, query.c_str(), query.length() )==0, "mysql_real_query");
	
		DB_RES *db_res = db_wrapper_store_result(&db_link);
		try_to(db_res, "db_wrapper_store_result");
		
		MYSQL_RES *mysql_res = mysql_store_result(&mysql_link);
		try_to(mysql_res, "mysql_store_result");
		
		cout << "DB rows: " << db_res->row_count << endl << "MYSQL rows: " << mysql_res->row_count << endl;
		try_to( (mysql_res->row_count == db_res->row_count), "rows count is OK");
		try_to( (mysql_res->field_count == db_res->field_count), "field count is OK");
		
		uint db_field_cnt = db_wrapper_field_count(&db_link);
		uint mysql_field_cnt = mysql_field_count(&mysql_link);
		try_to(db_field_cnt = mysql_field_cnt, " db_wrapper_field_count() == mysql_field_count()");
		
		DB_ROW db_row = db_wrapper_fetch_row(db_res);
		MYSQL_ROW mysql_row = mysql_fetch_row(mysql_res);
		
		try_to( ( (db_row!=NULL && mysql_row!=NULL)  || (db_row==NULL && mysql_row==NULL) ) , "db_wrapper_fetch_row() and mysql_fetch_row() are of same kind null or non-null");
		
		int i = 0;
				
		while ( db_row && mysql_row )
		{
			cout << "row #" << i+1 << endl;

			unsigned long *db_row_lengths = db_wrapper_fetch_lengths(db_res);
			try_to(db_row_lengths, "db_wrapper_fetch_lengths");
			
			unsigned long *mysql_row_lengths = mysql_fetch_lengths(mysql_res);
			try_to(mysql_row_lengths, "mysql_fetch_lengths");
			
			for (int j=0; j<db_field_cnt; j++)
			{
				cout << "field #" << j+1 << endl;
//				cout << "db_row[" << j <<" ] = " << db_row[j] << endl;
//				cout << "mysql_row[" << j <<" ] = " << mysql_row[j] << endl;
				
				try_to(db_row_lengths[j]==mysql_row_lengths[j], "db_row_lengths[j]==mysql_row_lengths[j]");
				unsigned long length = db_row_lengths[j];
				
				if ( !db_row[j] || !mysql_row[j] )
					try_to(db_row[j] == mysql_row[j], "db_wrapper_fetch_row[j]  == mysql_fetch_row[j]");
				else
					try_to(memcmp(db_row[j], mysql_row[j], length)==0, "db_wrapper_fetch_row[j]  == mysql_fetch_row[j]");
			}
			
			db_row = db_wrapper_fetch_row(db_res);
			mysql_row = mysql_fetch_row(mysql_res);
			
			try_to( ( (db_row!=NULL && mysql_row!=NULL)  || (db_row==NULL && mysql_row==NULL) ) , "db_wrapper_fetch_row() and mysql_fetch_row() are of same kind null or non-null");
			
			i++;
		}
		
		db_wrapper_free_result(db_res);
		mysql_free_result(mysql_res);
		
		db_wrapper_free_result(db_res_tmp);
		mysql_free_result(mysql_res_tmp);
	}
	
	// string escaping
	{
		cout << endl << "Executing string escaper: " << endl;
		
		const int len = 14;
		char in[] = "ABC\\DEF'\0000'GHI";
		char db_str[30];
		char mysql_str[30];
		
		unsigned long db_len = db_wrapper_real_escape_string(&db_link, db_str, in, len);
		unsigned long mysql_len = mysql_real_escape_string(&mysql_link, mysql_str, in, len);
		
		cout << "db_len = " << db_len << endl << "mysql_len = " << mysql_len << endl;
		
		try_to(db_len==mysql_len, "length of escaped strings");
		try_to(strcmp(db_str, mysql_str)==0, "contents of escaped strings");
		
	}
	
	// empty string escaping
	{
		cout << endl << "Executing string escaper: " << endl;
		
		const int len = 0;
		char in[] = "";
		char db_str[30];
		char mysql_str[30];
		
		unsigned long db_len = db_wrapper_real_escape_string(&db_link, db_str, in, len);
		unsigned long mysql_len = mysql_real_escape_string(&mysql_link, mysql_str, in, len);
		
		cout << "db_len = " << db_len << endl << "mysql_len = " << mysql_len << endl;
		
		try_to(db_len==mysql_len, "length of escaped strings");
		try_to(strcmp(db_str, mysql_str)==0, "contents of escaped strings");
		
	}
	
	// error print
	{
		string query = "SELECT abc";
		cout << endl << "Executing error message fetcher after bad query " << endl;
		cout << "query: " << query << endl;
		
		try_to(db_wrapper_query(&db_link, query.c_str())!=0, "db_wrapper_query");
		try_to(mysql_query(&mysql_link, query.c_str())!=0, "mysql_query");
		
		const char *db_msg = db_wrapper_error(&db_link);
		const char *mysql_msg = mysql_error(&mysql_link);
		
		try_to(db_msg, "db_msg ==  NULL");
		try_to(mysql_msg, "mysql_msg ==  NULL");
		
		try_to(strcmp(db_msg, mysql_msg)==0, "db_wrapper_query() == mysql_query()");
		cout << "It is OK. message is: " << db_msg << endl;
		
	}
	
	// count of affected rows
	{
		cout << endl << "Checking count of affected rows after query " << endl;
		
		// move +100 and move -100
		string query1 = "UPDATE MediaType_DesignObj SET PK_MediaType_DesignObj=PK_MediaType_DesignObj+100";
		string query2 = "UPDATE MediaType_DesignObj SET PK_MediaType_DesignObj=PK_MediaType_DesignObj-100";
		
		try_to(db_wrapper_query(&db_link, query1.c_str())==0, "db_wrapper_query");
		long db_rows_cnt = db_wrapper_affected_rows(&db_link);
		try_to(db_rows_cnt!=0, "db_wrapper: query updates rows");
		
		try_to(mysql_query(&mysql_link, query2.c_str())==0, "mysql_query");
		long mysql_rows_cnt = mysql_affected_rows(&mysql_link);
		try_to(mysql_rows_cnt!=0, "mysql_wrapper: query updated rows");
		
		cout << "db_rows_cnt = " << db_rows_cnt << endl << "mysql_rows_cnt = " << mysql_rows_cnt << endl;
		try_to(mysql_rows_cnt==db_rows_cnt, "queries updated same number if rows");
	}
	
	// inserted ID
	{
		cout << endl << "Checking possibility to get last inserted id " << endl;
		
		string query1 = "INSERT INTO UI (Description) VALUES (\"TEST\")";
		string query2 = "SELECT MAX(PK_UI) FROM UI";
		
		try_to(db_wrapper_query(&db_link, query1.c_str())==0, "db_wrapper_query");
		unsigned long db_last_id = db_wrapper_insert_id(&db_link);
		
		try_to(mysql_query(&mysql_link, query2.c_str())==0, "mysql_query");
		MYSQL_RES *mysql_res = mysql_store_result(&mysql_link);
		try_to(mysql_res, "mysql_store_result");
		MYSQL_ROW mysql_row = mysql_fetch_row(mysql_res);
		try_to(mysql_row, "mysql_fetch_row");
		
		unsigned long mysql_last_id = atoi(mysql_row[0]);
		mysql_free_result(mysql_res);
		
		cout << "db_last_id = " << db_last_id  << endl << "mysql_last_id = " << mysql_last_id << endl;
		try_to(db_last_id == mysql_last_id, "db_wrapper_insert_id()");
	}
	
	// closing of DB link
	{
		cout << endl << "Checking if we can exec some query closing db link" << endl;
		
		db_wrapper_close(&db_link);
		
		string query = "SELECT 2+2";
		
		try_to(db_wrapper_query(&db_link, query.c_str())!=0, "db_wrapper_query doesn't work after disconnection");
		
		mysql_close(&mysql_link);
	}

// the second connection will fail because independent process will already exit at this point
#ifndef	INDEPENDENT_WRAPPER_PROCESS
	// similar test but for dynamic allocation of DB_LINK and MYSQL
	{
		cout << endl << "Checking dynamic allocation of DB_LINK" << endl;
		DB_LINK *db_link;
		MYSQL *mysql_link;
	
		cout << "Running tests:" << endl;
		
		// init DB structures
		cout << endl << "Initialize DB structures.." << endl;
		
		try_to( db_link = db_wrapper_init(NULL), "db_wrapper_init");
		try_to( mysql_link = mysql_init(NULL), "mysql_init");
		
		// connect to DB
		cout << endl << "Connecting to DB.." << endl;
		
		try_to(db_wrapper_real_connect(db_link, "localhost", "root", "", "pluto_test", 3306, NULL, 0), "db_wrapper_real_connect");
		try_to(mysql_real_connect(mysql_link, "localhost", "root", "", "pluto_test", 3306, NULL, 0), "mysql_real_connect");
	
		// execute trivial query, fetch results, compare
		{
			string query = "SELECT 2+2";
			cout << endl << "Executing simple math query: " << query << endl;
			
			try_to(db_wrapper_query(db_link, query.c_str())==0, "db_wrapper_query");
			try_to(mysql_query(mysql_link, query.c_str())==0, "mysql_query");
		
			DB_RES *db_res = db_wrapper_store_result(db_link);
			try_to(db_res, "db_wrapper_store_result");
			
			MYSQL_RES *mysql_res = mysql_store_result(mysql_link);
			try_to(mysql_res, "mysql_store_result");
			
			cout << "DB rows: " << db_res->row_count << endl << "MYSQL rows: " << mysql_res->row_count << endl;
			try_to( (mysql_res->row_count == db_res->row_count), "rows count is OK");
			try_to( (mysql_res->field_count == db_res->field_count), "field count is OK");
			
			DB_ROW db_row = db_wrapper_fetch_row(db_res);
			try_to(db_row, "db_wrapper_fetch_row");
			
			MYSQL_ROW mysql_row = mysql_fetch_row(mysql_res);
			try_to(mysql_row, "mysql_fetch_row");
			
			cout << "db_row[0] = " << db_row[0] << endl;
			cout << "mysql_row[0] = " << mysql_row[0] << endl;
			
			try_to(strcmp(db_row[0], mysql_row[0])==0, "db_wrapper_fetch_row()  == mysql_fetch_row()");
			
			db_wrapper_free_result(db_res);
			mysql_free_result(mysql_res);
		}
	
		db_wrapper_close(db_link);
		mysql_close(mysql_link);
	}
#endif	
	
	return 0;
}
