/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
**************************************************************************/

#include "Library.h"
#include "ServerConsts.h"

#ifndef __HEADER_INCLUDED_SERVERCONNECTION__
#define __HEADER_INCLUDED_SERVERCONNECTION__

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {

class ServerConnection {

public:
	
	ServerConnection(SOCKET socket);
	virtual ~ServerConnection(void);

private:

	ServerConnection(const ServerConnection&);
	void operator=(const ServerConnection&);

public:

	Library& getLibrary();

	static inline void convertStringToPacketString(	const std::wstring& str,
													char* const target);

protected:

	enum Cmds {
		SCMD_FAIL			= -1,
		SCMD_GET_FILES		= CMD_GET_FILES,
		SCMD_GET_PLAYLIST	= CMD_GET_PLAYLIST,
		SCMD_GET_FILES_ALL	= CMD_GET_FILES_ALL,
		SCMD_QUIT			= CMD_QUIT,
		SCMD_OK				= CMD_OK
	};

	BOOL sendPlaylist();
	BOOL sendAllFiles();
	BOOL sendFiles();
	BOOL sendFile(	const unsigned long id,
					const BOOL sendZero4Inaccessible = TRUE);
	BOOL sendFile(	const std::wstring &file,
					const BOOL sendZero4Inaccessible = TRUE);

	inline Cmds receiveCmd(const unsigned long timeout = 10);
	BOOL receive(	char *const buffer,
					const unsigned long dataSize,
					const unsigned long timeout = 10);
	inline BOOL send(	const char *const buffer,
						const unsigned long dataSize,
						const unsigned long timeout = 10);
	inline BOOL sendZeroFile();

	

private:

	SOCKET _socket;

	Library* _lib;

};

	}
}

#endif // __HEADER_INCLUDED_SERVERCONNECTION__
