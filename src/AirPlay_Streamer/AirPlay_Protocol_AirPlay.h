/** 
 *
 * AirPlay Protocol - AirPlay
 *
 */

#ifndef AIRPLAYPROTOCOLAIRPLAY_H
#define AIRPLAYPROTOCOLAIRPLAY_H

#include "AirPlay_Protocol.h"
#include "AirPlay_Videolan.h"
#include "HttpParser/HttpParser.h"
#include <plist/plist.h>

#include <string>
#include <map>
#include <vector>

#define EVENT_NONE     -1
#define EVENT_PLAYING   0
#define EVENT_PAUSED    1
#define EVENT_LOADING   2
#define EVENT_STOPPED   3

#define AIRPLAY_STATUS_OK                  200
#define AIRPLAY_STATUS_SWITCHING_PROTOCOLS 101
#define AIRPLAY_STATUS_NEED_AUTH           401
#define AIRPLAY_STATUS_NOT_FOUND           404
#define AIRPLAY_STATUS_METHOD_NOT_ALLOWED  405
#define AIRPLAY_STATUS_NOT_IMPLEMENTED     501
#define AIRPLAY_STATUS_NO_RESPONSE_NEEDED  1000

#define SOCKET_STATE_INITIAL 0;

using namespace std;

namespace DCE
{

  class AirPlay_Protocol_AirPlay : public AirPlay_Protocol
    {
    private:
      pthread_t m_serverThread;
    protected:
    public:
      static int m_iIsPlaying;
      int m_iServerSocket;
      AirPlay_Videolan *m_pAirPlay_Videolan;
      
      AirPlay_Protocol_AirPlay(AirPlay_Streamer *pAirPlay_Streamer);
      ~AirPlay_Protocol_AirPlay();

      virtual bool init();
      virtual bool start();
      virtual bool stop();
      bool ServerSocketSetup();
      void ServerSocketTeardown();
      bool StartServerThread();
      void ServerThread();
      void ProcessConnections();
      float GetPosition();
      float GetDuration();
      void SetTime(float fTime);

      class AirPlayTCPClient
      {
      public:
        AirPlay_Protocol_AirPlay *m_pAirPlay_Protocol_AirPlay;
	AirPlayTCPClient();
	~AirPlayTCPClient();
	AirPlayTCPClient(const AirPlayTCPClient& client);
	AirPlayTCPClient& operator=(const AirPlayTCPClient& client);
	void PushBuffer(AirPlay_Protocol_AirPlay *server, const char *buffer,
			int length, string &sessionId, std::map<string, int> &reverseSockets);

	void Disconnect();

	int m_iSocket;
	struct sockaddr m_ClientAddr;
	socklen_t m_ClientAddrLen;
	
      private:
	int ProcessRequest(string &sResponseHeader,
			   string &response,
			   string &reverseHeader,
			   string &reverseBody,
			   string &sessionId);

	void ComposeReverseEvent(string &reverseHeader,
				 string &reverseBody,
				 string sessionId,
				 int state);

	void Copy(const AirPlayTCPClient& client);
	void Encode(string& strURLData);
	HttpParser* m_pHttpParser;
	int m_iLastEvent;


      };

      vector<AirPlayTCPClient> m_connections;
      map<string, int> m_reverseSockets;

    };

}

#endif
