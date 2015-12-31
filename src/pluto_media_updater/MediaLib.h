/* 
 * written by Dan Dennedy for 1Control
 *
 * This class communicates with a medialibd daemon running
 * on the Pluto server over port 3442. It requests the daemon
 * to set the file user extended attribute "ID" to a requested
 * value (set). It provides an application to query for the 
 * file name when supplied with an ID (get). An "ID" consists
 * of two parts: a type and a key. The purpose of medialib is to
 * connect the filesystem with a database. The file attribute
 * holds a primary key. The "type" portion is stored in the upper
 * 32 bits of ID and identifies which entity corresponds to this
 * primary key. It is beyond the scope of this application to
 * assign type identification numbers.
 *
 */
 
#ifndef _MEDIALIB_H
#define _MEDIALIB_H

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

const std::string host("pluto");
const int port = 3442;
const int MAXRECV = 500;

class MedialibException
{
	public:
		MedialibException(std::string s) : m_s(s) {};
		~MedialibException(){};
		std::string description() { return m_s; }
	
	private:
		std::string m_s;
};

class MedialibClient
{
	public:
		MedialibClient();
		virtual ~MedialibClient();
		
		std::string get(unsigned int type, unsigned int pkid) const;
		void set(std::string filename, unsigned int type, unsigned int pkid) const;
		
	private:
		int m_sock;
		sockaddr_in m_addr;
};

#endif /* _MEDIALIB_H */
