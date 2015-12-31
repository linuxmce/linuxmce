/**
 *
 * @file MythBackEnd_Socket.h
 * @brief header file for the  MythTvStream, MythBackEnd_Socket classes
 *
 */

#ifndef MythBackEnd_Socket_h
#define MythBackEnd_Socket_h

#include "DCE/ClientSocket.h"

#define MYTH_SOCKET_BUFFER_SIZE		1024
#define MYTH_RESPONSE_TIMEOUT		5

namespace DCE
{
	class MythTV_PlugIn;

	class MythBackEnd_Socket_Wrapper : public ClientSocket
	{
		class MythBackEnd_Socket *m_pMythBackEnd_Socket;

	public:
		MythBackEnd_Socket_Wrapper(MythBackEnd_Socket *pMythBackEnd_Socket,string sIP)
			: ClientSocket(0,sIP,"MythPluginSocket")
		{ 
			m_pMythBackEnd_Socket=pMythBackEnd_Socket; 
		}
		virtual ~MythBackEnd_Socket_Wrapper() {}

		// overriding on connect event handler, because we use custom protocol
		virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" ){return true;};

		virtual void Close();
	};

	enum eReceiveCharResult { eReceiveCharResult_Error, eReceiveCharResult_Nothing, eReceiveCharResult_String, eReceiveCharResult_Char };

	class MythBackEnd_Socket
	{
	private:
		bool m_bConnected;
		bool InternalSendMythString(string sValue,string *sResponse,string sContaining); // For internal use only.  Use SendMythString which will retry on 1 failure
		MythBackEnd_Socket_Wrapper *m_pSocket;
		string m_sIPAddress;
		MythTV_PlugIn *m_pMythTV_PlugIn;
		char *m_pSocketBuffer;
		int m_iCharsInBuffer; // How many characters we've placed in m_pSocketBuffer

		// If we start to receive something that is 8 characters of only numbers and space, we will be receiving m_iSizeIncomingMessage message
		// and should keep receiving until that many bytes are received
		int m_iSizeIncomingMessage; 
		string m_sStringBuffer;
void PurgeSocketBuffer();  // move to socket.h

	public:
		
		MythBackEnd_Socket(MythTV_PlugIn *pMythTV_PlugIn,string sIPAddress);
		~MythBackEnd_Socket();

		bool Connect( );

		void Close();

		void DeleteSocket()
		{
			delete m_pSocket;
			m_pSocket=NULL;
			m_bConnected=false;
		}
	
		// Receive 1 character from the Myth socket, put it in m_pSocketBuffer, until we receive a \n
		// at which point it's moved to m_sStringBuffer and the return value is eReceiveCharResult_String, 
		// meaning m_sStringBuffer should be processed
		eReceiveCharResult ReceiveChar();

		// Called when m_sStringBuffer has a string that needs to be processed.  This will reset the m_sStringBuffer to empty
		// If nothing is passed in, it will process whatever string is in m_sStringBuffer
		void ProcessIncomingString(string sResponse="");

		// Send a string to the myth backend.  If sResponse is not NULL, this will contain whatever \n terminated string
		// Myth sends in return within MYTH_RESPONSE_TIMEOUT seconds.  If sContaining is not empty, it will only return
		// a string containing sContaining, and any other strings received will be assumed to be unrelated strings due to
		// asynchronous processing, and will be sent to ProcessIncomingString();
		bool SendMythString(string sValue,string *sResponse=NULL,string sContaining="");
		bool SendMythStringGetOk(string sValue);

		// Will check for incoming data on the socket with ReceiveChar, and process any incoming tokens
		// Returns true if it got something and processed it.
		bool ProcessSocket();
	};
}

#endif
