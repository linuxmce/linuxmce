#ifndef __DB_WRAPPER_H__
#define __DB_WRAPPER_H__

#ifdef USE_MYSQL_WRAPPER

// fake define to make it die on non-converted functions
#define _mysql_h

#define DB_WRAPPER_SOCKET "/tmp/db_wrapper_socket"

//#define DB_HELPER_APP "/home/fenikso/work/pluto/pluto_public/branches/Kirill/database_wrapper/src/mysql_wrapper/mysql_wrapper"
#define DB_HELPER_APP "/usr/pluto/bin/mysql_wrapper"

//#define INDEPENDENT_WRAPPER_PROCESS

#include "mysql_wrapper/TLV.h"

namespace DB_Wrapper
{
class ClientSocket;
};

class DB_LINK
{
	// for access to isAllocated
	friend DB_LINK* db_wrapper_init(DB_LINK *db_link);
	friend void db_wrapper_close(DB_LINK *db_link);
	friend const char *db_wrapper_error(DB_LINK *db_link);

	pthread_mutex_t access_mutex;
	
	DB_Wrapper::ClientSocket *client_socket;
	bool isConnected;

	// if it was allocated during db_wrapper_init call
	bool isAllocated;

	// last error message
	char *last_error;

public:
	bool real_connect(const char *host, const char *user, const char *passwd, const char *db, unsigned int port);
	bool real_disconnect();
	bool send_tlv(TLV &tlv);
	bool recv_tlv(TLV &tlv);
	bool connected();
	
	DB_LINK();
	~DB_LINK();

	void lock();
	void unlock();
};

typedef char** DB_ROW;

class DB_RES
{
	// for access to current_row
	friend DB_ROW db_wrapper_fetch_row(DB_RES *db_res);
	friend void db_wrapper_free_result(DB_RES *db_res);
	friend unsigned long *db_wrapper_fetch_lengths(DB_RES *db_res);

	char** current_row;
	unsigned long current_cols;
	unsigned long *current_lengths;
public:
	unsigned long result_id;
	unsigned long row_count;
	unsigned long field_count;

	DB_LINK *db_link;
	
	DB_RES();
	~DB_RES();
};

DB_LINK* db_wrapper_init(DB_LINK *db_link);

void db_wrapper_free_result(DB_RES *db_res);

void db_wrapper_close(DB_LINK *db_link);

DB_LINK *db_wrapper_real_connect(DB_LINK *db_link, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag);

const char *db_wrapper_error(DB_LINK *db_link);

int db_wrapper_query(DB_LINK *db_link, const char *query);

int db_wrapper_real_query(DB_LINK *db_link, const char *query, uint length);

DB_ROW db_wrapper_fetch_row(DB_RES *db_res);

DB_RES *db_wrapper_store_result(DB_LINK *db_link);

unsigned long db_wrapper_affected_rows(DB_LINK *db_link);

unsigned long db_wrapper_insert_id(DB_LINK *db_link);

unsigned long db_wrapper_real_escape_string(DB_LINK *db_link, char *to, const char *from, unsigned long length);

unsigned long *db_wrapper_fetch_lengths(DB_RES *db_res);

unsigned int db_wrapper_field_count(DB_LINK *db_link);

void db_wrapper_thread_init();

void db_wrapper_thread_end();

#else	// USE_MYSQL_WRAPPER

#include "db_wrapper_mysql.h"

#endif

#endif	// USE_MYSQL_WRAPPER
