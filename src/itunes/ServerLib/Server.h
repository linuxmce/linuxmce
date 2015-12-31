/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
**************************************************************************/

#include "api.h"

#ifndef __HEADER_INCLUDED_SERVER__
#define __HEADER_INCLUDED_SERVER__

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {

class PITMCB_API Server {

private:
	
	Server();
	~Server(void);
	Server(const Server&);
	void operator=(const Server&);

public:

	static Server& getInstance();

public:

	/// Parameters for the server thread
	struct PITMCB_API ServerThreadParams {
		/// Shutdown event
		HANDLE shutdownEvent;
		/// Thread handle
		HANDLE thread;
		/// Thread ID
		unsigned int threadId;
		/// Local DB update event
		HANDLE dbUpdateEvent;
		SOCKET socket;
		SOCKADDR_IN internetAddr;
		/// Server connections number in pool
		unsigned int connectionsPoolSize;
		ServerThreadParams()
		:	shutdownEvent(NULL)
		,	thread(NULL) {
			internetAddr.sin_family = AF_INET;
			internetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		}
	};

public:

	void start(	const unsigned short int port,
				const unsigned short int connectionsPoolSize,
				HANDLE dbUpdateEvent);
	void stop();

protected:

	ServerThreadParams _threadParams;

};

	}
}

#endif // __HEADER_INCLUDED_SERVER__
