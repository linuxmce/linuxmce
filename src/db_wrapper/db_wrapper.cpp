#ifdef USE_MYSQL_WRAPPER

#include <cstdlib>
#include <stdio.h>
#include <string>
#include <signal.h>
#include <sys/stat.h>

#include <pthread.h>

#include "db_wrapper.h"
#include "mysql_wrapper/TLV.h"
#include "SocketException.h"
#include "ClientSocket.h"

static void signal_swallow( int num )
{
}

using namespace std;

DB_LINK::DB_LINK() : client_socket(NULL), isConnected(false)
{
	pthread_mutex_init(&access_mutex, NULL);
	isAllocated = false;
	last_error = NULL;
}

DB_LINK::~DB_LINK()
{
	delete[] last_error;
	last_error = NULL;

	pthread_mutex_destroy(&access_mutex);
}

void DB_LINK::lock()
{
	pthread_mutex_lock(&access_mutex);
//	cout << "mutex locked" << endl;
}

void DB_LINK::unlock()
{
	pthread_mutex_unlock(&access_mutex);
//	cout << "mutex unlocked" << endl;
}

bool DB_LINK::connected()
{
	return isConnected;
}

bool DB_LINK::real_connect(const char *host, const char *user, const char *passwd, const char *db, unsigned int port)
{
	signal( SIGPIPE, signal_swallow );
	
#ifdef INDEPENDENT_WRAPPER_PROCESS
	client_socket = new DB_Wrapper::ClientSocket(DB_WRAPPER_SOCKET);
	isConnected = true;

	return true;
#else
	char unix_socket[4096];
	snprintf( unix_socket, 4096, "%s-%i-XXXXXX", DB_WRAPPER_SOCKET, getpid() );
	int fd = mkstemp(unix_socket);
	if (fd<0)
		return false;

	char log_file[4096];
	snprintf (log_file, 4096, "%s.log", unix_socket );
	
	char cmd[4096];
	
	snprintf(cmd, 4096, "%s -h %s -u %s -D %s -P %i -s %s > %s &",
		 DB_HELPER_APP,  host, user,  db, port, unix_socket, log_file );
	
	// cleaning old socket
	close(fd);
	unlink(unix_socket);
	
	int iRet = system(cmd);
	if (iRet != 0)
	{
//		cout << "Error: " << strerror(iRet) << endl;
		return false;
	}
	
	// timeout = 10 seconds = 10000 ms
	int iTimeout=100;
	do 
	{
//		cout << "Sleeping 100ms" << endl;
		usleep(100*1000);	// sleep 100ms
		iTimeout--;
		struct stat file_info;
		iRet = stat(unix_socket, &file_info);
	}
	while ( iRet && iTimeout);
	
	if (iRet)
	{
//		cout << "Error waiting for mysql_wrapper socket creation" << endl;
		return false;
	}
	
	try
	{
		client_socket = new DB_Wrapper::ClientSocket(unix_socket);
		isConnected = true;
		return true;
	}
	catch ( SocketException& e )
	{
//		std::cout << "Exception was caught:" << e.description() << "\n";
		return false;
	}
#endif
}

bool DB_LINK::send_tlv(TLV &tlv)
{
	if (client_socket)
		return client_socket->send(tlv);
	else 
		return false;
}

bool DB_LINK::recv_tlv(TLV &tlv)
{
	if (client_socket)
		return client_socket->recv(tlv);
	else 
		return false;
}

bool DB_LINK::real_disconnect()
{
	isConnected = false;
	
	TLV out(TLV::typeCommand_SHUTDOWN, "PAYLOAD");
	send_tlv(out);

	delete client_socket;
	client_socket = NULL;
	
	return true;
}

DB_RES:: DB_RES() : current_row(NULL), current_cols(0), current_lengths(NULL),  row_count(0), field_count(0), db_link(NULL)
{
}

DB_RES::~DB_RES()
{
	if (current_row)
	{
		for (uint i=0; i<current_cols; i++)
			delete[] current_row[i];
		delete[] current_row;
	}
		
	current_cols = 0;
	current_row = NULL;
	
	delete[] current_lengths;
}

DB_LINK* db_wrapper_init(DB_LINK *db_link)
{
	DB_LINK *link = NULL;
	
	if (db_link)
		link = db_link;
	else
	{
		link = new DB_LINK();
		link->isAllocated = true;
	}
	
	return link;
}

void db_wrapper_free_result(DB_RES *db_res)
{
	if (!db_res || !db_res->db_link)
		return;
	
	TLV out(db_res->result_id);
	out.type = TLV::typeFunction_FreeResult;
	db_res->db_link->send_tlv(out);
	
	delete db_res;
}

void db_wrapper_close(DB_LINK *db_link)
{
	if (!db_link)
		return;
	
	db_link->real_disconnect();
	
	if (db_link->isAllocated)
		delete db_link;
}

DB_LINK *db_wrapper_real_connect(DB_LINK *db_link, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag)
{
	if (!db_link)
		return NULL;
	
	db_link->lock();
	bool bRes = db_link->real_connect(host, user, passwd, db, port);
	db_link->unlock();
	
	return bRes?db_link:NULL;
}

const char *db_wrapper_error(DB_LINK *db_link)
{
	if (!db_link)
		return "No DB connection";
	
	db_link->lock();
	
	TLV out(TLV::typeFunction_Error, "PAYLOAD"), in;
	if ( !db_link->send_tlv(out) || !db_link->recv_tlv(in) )
	{
		db_link->unlock();
		return "Failed to get error message";
	}
	else
	{
		db_link->unlock();
		delete[] db_link->last_error;
		return db_link->last_error = in.c_str();
	}
}

int db_wrapper_query(DB_LINK *db_link, const char *query)
{
	if (!db_link || !db_link->connected())
		return -1;
	
	db_link->lock();
	
	TLV out(TLV::typeFunction_Query, query), in;
	if ( !db_link->send_tlv(out) || !db_link->recv_tlv(in) )
	{
		db_link->unlock();
		return -1;
	}
	else
	{
		db_link->unlock();
		return in.to_bool()?0:-1;
	}
}

int db_wrapper_real_query(DB_LINK *db_link, const char *query, uint length)
{
	if (!db_link)
		return -1;
	
	db_link->lock();
	
	TLV out(TLV::typeFunction_RealQuery, query, length), in;
	if ( !db_link->send_tlv(out) || !db_link->recv_tlv(in) )
	{
		db_link->unlock();
		return -1;
	}
	else
	{
		db_link->unlock();
		return in.to_bool()?0:-1;
	}
}

DB_ROW db_wrapper_fetch_row(DB_RES *db_res)
{
	if (!db_res || !db_res->db_link)
		return NULL;
	
	db_res->db_link->lock();
	
	TLV out(db_res->result_id);
	out.type = TLV::typeFunction_FetchRow;
	TLV in;
	if ( !db_res->db_link->send_tlv(out) || !db_res->db_link->recv_tlv(in) )
	{
		db_res->db_link->unlock();
		return NULL;
	}
	else
	{
		if ( in.type != TLV::typeData_ROW )
		{
			db_res->db_link->unlock();
			return NULL;
		}
		
		if (in.length < sizeof(uint))
		{
			db_res->db_link->unlock();
			return NULL;
		}
		
		char *p = in.value;
		uint cols = 0;
		memcpy(&cols, p, sizeof(uint));
		p += sizeof(uint);
		
		char **row = new char*[cols];
		for (uint i=0; i<cols; i++)
		{
			bool isNull = false;
			memcpy(&isNull, p, sizeof(bool));
			p += sizeof(bool);
			
			uint colLength=0;
			memcpy(&colLength, p, sizeof(uint));
			p += sizeof(uint);
			
			if (isNull)
				row[i] = NULL;
			else
			{
				row[i] = new char[colLength+1];
				memcpy(row[i], p, colLength);
				row[i][colLength] = 0;
				p += colLength;
			}
		}

		if (db_res->current_row)
		{
			for (uint i=0; i<db_res->current_cols; i++)
				delete[] db_res->current_row[i];
			delete[] db_res->current_row;
		}
		
		db_res->current_row = row;
		db_res->current_cols = cols;
		
		db_res->db_link->unlock();
		return row;
	}
	
}

DB_RES *db_wrapper_store_result(DB_LINK *db_link)
{
	if (!db_link)
		return NULL;
	
	db_link->lock();
	
	TLV out(TLV::typeFunction_StoreResult, "PAYLOAD"), in, in_data0, in_data1, in_data2;
	if ( !db_link->send_tlv(out) || !db_link->recv_tlv(in) || !db_link->recv_tlv(in_data0) || !db_link->recv_tlv(in_data1) || !db_link->recv_tlv(in_data2) )
	{
		db_link->unlock();
		return NULL;
	}
	else
	{
		if (!in.to_bool())
		{
			db_link->unlock();
			return NULL;
		}
		else
		{
			DB_RES *p = new DB_RES();
			p->db_link = db_link;
			p->result_id = in_data0.to_ulong();
			p->row_count = in_data1.to_ulong();
			p->field_count = in_data2.to_ulong();
			db_link->unlock();
			return p;
		}
	}
}

unsigned long db_wrapper_affected_rows(DB_LINK *db_link)
{
	if (!db_link)
		return 0;
	
	db_link->lock();
	
	TLV out(TLV::typeFunction_AffectedRows, "PAYLOAD"), in;
	if ( !db_link->send_tlv(out) || !db_link->recv_tlv(in) )
	{
		db_link->unlock();
		return 0;
	}
	else
	{
		db_link->unlock();
		return in.to_ulong();
	}
}

unsigned long db_wrapper_insert_id(DB_LINK *db_link)
{
	if (!db_link)
		return 0;
	
	db_link->lock();
	
	TLV out(TLV::typeFunction_InsertID, "PAYLOAD"), in;
	if ( !db_link->send_tlv(out) || !db_link->recv_tlv(in) )
	{
		db_link->unlock();
		return 0;
	}
	else
	{
		db_link->unlock();
		return in.to_ulong();
	}
}

unsigned long db_wrapper_real_escape_string(DB_LINK *db_link, char *to, const char *from, unsigned long length)
{
	// fallback
	to[0] = 0;
	
	if (!db_link)
		return 0;
	
	db_link->lock();
	
	TLV out(TLV::typeFunction_RealEscapeString, from, length), in;
	if ( !db_link->send_tlv(out) || !db_link->recv_tlv(in) )
	{
		db_link->unlock();
		return 0;
	}
	else
	{
		memcpy(to, in.value, in.length);
		to[in.length] = 0;
		db_link->unlock();
		return in.length;
	}
}

unsigned long *db_wrapper_fetch_lengths(DB_RES *db_res)
{
	if (!db_res || !db_res->db_link)
		return NULL;
	
	db_res->db_link->lock();
	
	TLV out(db_res->result_id);
	out.type = TLV::typeFunction_FetchLengths;
	TLV in;
	if ( !db_res->db_link->send_tlv(out) || !db_res->db_link->recv_tlv(in) )
	{
		db_res->db_link->unlock();
		return NULL;
	}
	else
	{
		int elements = in.length / sizeof(unsigned long);
		unsigned long *p = new unsigned long[elements];
		memcpy(p, in.value, in.length);
		delete[] db_res->current_lengths;
		db_res->current_lengths = p;
		db_res->db_link->unlock();
		return p;
	}
}

unsigned int db_wrapper_field_count(DB_LINK *db_link)
{
	if (!db_link)
		return 0;
	
	db_link->lock();
	
	TLV out(TLV::typeFunction_FieldCount, "PAYLOAD"), in;
	if ( !db_link->send_tlv(out) || !db_link->recv_tlv(in) )
	{
		db_link->unlock();
		return 0;
	}
	else
	{
		db_link->unlock();
		return in.to_uint();
	}
}

void db_wrapper_thread_init()
{
//	cout << "db_wrapper_thread_init()" << endl;
}

void db_wrapper_thread_end()
{
//	cout << "db_wrapper_thread_end()" << endl;
}

#else // USE_MYSQL_WRAPPER
#include "db_wrapper.h"
DB_LINK* db_wrapper_init(DB_LINK *db_link)
{
	my_bool reconnect = true;
	DB_LINK *result = mysql_init(db_link);
	if( result )
	{
		mysql_options(result, MYSQL_OPT_RECONNECT, &reconnect);
	}
	return result;
}	
#endif // USE_MYSQL_WRAPPER
