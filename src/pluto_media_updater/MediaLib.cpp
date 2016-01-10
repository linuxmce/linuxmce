// Implementation of the MedialibClient class

#include "MediaLib.h"
#include <iostream>

MedialibClient::MedialibClient () : m_sock ( -1 )
{
	memset( &m_addr, 0,sizeof ( m_addr ) );
	m_sock = socket( AF_INET, SOCK_STREAM, 0 );
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons( port );
	struct hostent *hp;

	if ( m_sock == -1 )
		throw MedialibException( "Could not create client socket." );
    
	int on = 1;
	if ( setsockopt( m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof(on) ) < 0 )
		throw MedialibException( "Could not create client socket." );

	hp = gethostbyname(host.c_str());
        memcpy(&m_addr.sin_addr, hp->h_addr, sizeof(m_addr.sin_addr));
	//if ( inet_pton( AF_INET, host.c_str(), &m_addr.sin_addr ) < 0)
	//if ( inet_pton( AF_INET, "10.12.34.1", &m_addr.sin_addr ) < 0) // DEBUG
	//	throw MedialibException( "Could not create client socket." );

	if ( connect( m_sock, (sockaddr *) &m_addr, sizeof(m_addr) ) < 0)
		throw MedialibException( "Could not bind to port." );
}

MedialibClient::~MedialibClient()
{
	if ( m_sock != -1 )
		close( m_sock );
}

std::string MedialibClient::get( unsigned int type, unsigned int pkid ) const
{
	uint64_t key = type;
	key = (key << 32) + pkid;
	std::string result = "";
	char buf [ MAXRECV + 1 ];
	
	snprintf( buf, MAXRECV, "get %lld", key );
	if ( send( m_sock, buf, strlen(buf), MSG_NOSIGNAL ) < 0 )
		throw MedialibException( "Could not write to socket." );
	
	memset( buf, 0, MAXRECV + 1 );
	if ( recv( m_sock, buf, MAXRECV, 0 ) > 0 )
		result = buf;
	else
		throw MedialibException( "Could not read from socket." );
	
	return result;
}

void MedialibClient::set( std::string filename, unsigned int type, unsigned int pkid ) const
{
	uint64_t key = type;
	key = (key << 32) + pkid;
	std::string result = "";
	char buf [ MAXRECV + 1 ];
	
	snprintf( buf, MAXRECV, "set %s=%lld", filename.c_str(), key );
	if ( send( m_sock, buf, strlen(buf), MSG_NOSIGNAL ) < 0 )
		throw MedialibException( "Could not write to socket." );
	
	recv( m_sock, buf, MAXRECV, 0 );
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cout << "Usage: " << basename(argv[0]) << " -s {type} {id} {filename} | -g {type} {id}\n";
		return 1;
	}
	try
	{
		MedialibClient ml;
		unsigned int type = atoi( argv[2] );
		unsigned int pkid = atoi( argv[3] );
		if (!strcasecmp(argv[1], "-g"))
		{
			std::cout << ml.get( type, pkid);
		}
		else if (!strcasecmp(argv[1], "-s") && argc == 5)
		{
			ml.set( argv[4], type, pkid);
		}
		return 0;
	}
	catch (MedialibException& e)
	{
		std::cout << "Error: " << e.description() << std::endl;
		return 1;
	}
}
