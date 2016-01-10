// Definition of the Socket class

#ifndef Socket_class
#define Socket_class


#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>

#include "mysql_wrapper/TLV.h"

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Socket
{
 public:
  Socket();
  virtual ~Socket();

  // Server initialization
  bool create();
  bool bind ( const std::string unix_socket );
  bool listen() const;
  bool accept ( Socket& ) const;

  // Client initialization
  bool connect ( const std::string unix_socket );

  // Data Transimission
/*
  bool send ( const std::string ) const;
  int recv ( std::string& ) const;
*/

  bool send ( const TLV& ) const;
  int recv ( TLV& ) const;

  void flush() const;

 private:
	 int m_sock;
	 FILE *m_sockAsFile;
	 socklen_t m_socklen;
	 sockaddr_un m_sockaddr;
};


#endif
