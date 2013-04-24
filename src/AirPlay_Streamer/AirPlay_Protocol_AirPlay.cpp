/**
 * 
 * AirPlay Protocol - AirPlay
 *
 */

#include "AirPlay_Protocol_AirPlay.h"
#include "PlutoUtils/CommonIncludes.h"

/** inet includes */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

DCE::AirPlay_Streamer *gAirPlay_StreamerI;

const char *eventStrings[] = {"playing", "paused", "loading", "stopped"};
int DCE::AirPlay_Protocol_AirPlay::m_iIsPlaying = 0;

#define AIRPLAY_SERVER_VERSION_STR "101.28"

/* Server Responses */
#define SERVER_INFO  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"\
"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\r\n"\
"<plist version=\"1.0\">\r\n"\
"<dict>\r\n"\
"<key>deviceid</key>\r\n"\
"<string>%s</string>\r\n"\
"<key>features</key>\r\n"\
"<integer>119</integer>\r\n"\
"<key>model</key>\r\n"\
"<string>LinuxMCE,1</string>\r\n"\
"<key>protovers</key>\r\n"\
"<string>1.0</string>\r\n"\
"<key>srcvers</key>\r\n"\
"<string>"AIRPLAY_SERVER_VERSION_STR"</string>\r\n"\
"</dict>\r\n"\
"</plist>\r\n"

#define EVENT_INFO "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\r\n"\
"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n\r\n"\
"<plist version=\"1.0\">\r\n"\
"<dict>\r\n"\
"<key>category</key>\r\n"\
"<string>video</string>\r\n"\
"<key>state</key>\r\n"\
"<string>%s</string>\r\n"\
"</dict>\r\n"\
"</plist>\r\n"\

#define PLAYBACK_INFO  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"\
"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\r\n"\
"<plist version=\"1.0\">\r\n"\
"<dict>\r\n"\
"<key>duration</key>\r\n"\
"<real>%f</real>\r\n"\
"<key>loadedTimeRanges</key>\r\n"\
"<array>\r\n"\
"\t\t<dict>\r\n"\
"\t\t\t<key>duration</key>\r\n"\
"\t\t\t<real>%f</real>\r\n"\
"\t\t\t<key>start</key>\r\n"\
"\t\t\t<real>0.0</real>\r\n"\
"\t\t</dict>\r\n"\
"</array>\r\n"\
"<key>playbackBufferEmpty</key>\r\n"\
"<true/>\r\n"\
"<key>playbackBufferFull</key>\r\n"\
"<false/>\r\n"\
"<key>playbackLikelyToKeepUp</key>\r\n"\
"<true/>\r\n"\
"<key>position</key>\r\n"\
"<real>%f</real>\r\n"\
"<key>rate</key>\r\n"\
"<real>%d</real>\r\n"\
"<key>readyToPlay</key>\r\n"\
"<true/>\r\n"\
"<key>seekableTimeRanges</key>\r\n"\
"<array>\r\n"\
"\t\t<dict>\r\n"\
"\t\t\t<key>duration</key>\r\n"\
"\t\t\t<real>%f</real>\r\n"\
"\t\t\t<key>start</key>\r\n"\
"\t\t\t<real>0.0</real>\r\n"\
"\t\t</dict>\r\n"\
"</array>\r\n"\
"</dict>\r\n"\
"</plist>\r\n"

#define PLAYBACK_INFO_NOT_READY  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"\
"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\r\n"\
"<plist version=\"1.0\">\r\n"\
"<dict>\r\n"\
"<key>readyToPlay</key>\r\n"\
"<false/>\r\n"\
"</dict>\r\n"\
"</plist>\r\n"

/** Static method for pthread entry. */
void * StartAirPlayThread(void * Arg)
{
  DCE::AirPlay_Protocol_AirPlay *pAirPlay_Protocol_AirPlay = (DCE::AirPlay_Protocol_AirPlay *) Arg;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pAirPlay_Protocol_AirPlay->ServerThread();
  return NULL;
}

/* Format a string */
string string_format(const std::string fmt, ...) {
  int size = 100;
  std::string str;
  va_list ap;
  while (1) {
    str.resize(size);
    va_start(ap, fmt);
    int n = vsnprintf((char *)str.c_str(), size, fmt.c_str(), ap);
    va_end(ap);
    if (n > -1 && n < size) {
      str.resize(n);
      return str;
    }
    if (n > -1)
      size = n + 1;
    else
      size *= 2;
  }
  return str;
}

namespace DCE
{

  AirPlay_Protocol_AirPlay::AirPlay_Protocol_AirPlay(AirPlay_Streamer *pAirPlay_Streamer)
    : AirPlay_Protocol(pAirPlay_Streamer)
  {
    gAirPlay_StreamerI = pAirPlay_Streamer;
    Port_set(7000);
    ServiceType_set("_airplay._tcp");
    string tmpStr = string_format("deviceid=%s",pAirPlay_Streamer->m_sCurrentMacAddress.c_str()); 
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ADDR IS %s",tmpStr.c_str());
    TXTRecord_Add(tmpStr);
    TXTRecord_Add("features=0x77");
    TXTRecord_Add("model=LinuxMCE,1");
    TXTRecord_Add("srcvers=101.10");
    m_pAirPlay_Videolan=new AirPlay_Videolan();
  }

  AirPlay_Protocol_AirPlay::~AirPlay_Protocol_AirPlay()
  {
    // Wait for the Server thread to go byebye, first.
    if (m_serverThread != (pthread_t)NULL)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Protocol_AirPlay::~AirPlay_Protocol_AirPlay() - Waiting for Server Thread to go byebye...");
	pthread_join(m_serverThread, NULL);
      }
    
  }

  bool AirPlay_Protocol_AirPlay::ServerSocketSetup()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Protocol_AirPlay::ServerSocketSetup() - Setting up listen for port %d",Port_get());

    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(myaddr));

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(Port_get());
    myaddr.sin_addr.s_addr = INADDR_ANY; // TODO: come back here, and only bind on main interface.

    m_iServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (m_iServerSocket == INVALID_SOCKET)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Protocol_AirPlay::ServerSocketSetup() - Could not grab a server socket. errno # %d",errno);
	return false;
      }

    // Set SO_REUSEADDR to get around binding problems
    int optval = 1;
    setsockopt(m_iServerSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    if (bind(m_iServerSocket, (struct sockaddr *)&myaddr, sizeof myaddr) < 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Protocol_AirPlay::ServerSocketSetup() - could not bind to server socket, errno # %d",errno);
	return false;
      }

    if (listen(m_iServerSocket, 10) < 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Protocol_AirPlay::ServerSocketSetup() - could not set listen for socket. Errno # %d",errno);
	return false;
      }

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Protocol_AirPlay::ServerSocketSetup() - server thread started.");
    return true;
  }

  void AirPlay_Protocol_AirPlay::ServerSocketTeardown()
  {
    for (unsigned int i = 0; i < m_connections.size(); i++)
      m_connections[i].Disconnect();
    
    m_connections.clear();
    m_reverseSockets.clear();
    
    if (m_iServerSocket != INVALID_SOCKET)
      {
	shutdown(m_iServerSocket, SHUT_RDWR);
	close(m_iServerSocket);
	m_iServerSocket = INVALID_SOCKET;
      }
  }

  bool AirPlay_Protocol_AirPlay::init()
  {

    if (!m_pAirPlay_Videolan->init())
      {
	return false;
      }

    if (!ServerSocketSetup())
      {
	return false;
      }

    if (!StartServerThread())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Protocol_AirPlay::ServerSocketSetup() - could not spawn server thread. Bailing!");
	return false;
      }
    
    return true;
  }

  bool AirPlay_Protocol_AirPlay::start()
  {
    return true;
  }

  bool AirPlay_Protocol_AirPlay::stop()
  {
    return true;
  }

  bool AirPlay_Protocol_AirPlay::StartServerThread()
  {
    
    if (pthread_create(&m_serverThread, NULL, StartAirPlayThread, (void *) this))
      {
        // failed, bail.
        return false;
      }
    
    return true;

  }

  void AirPlay_Protocol_AirPlay::ProcessConnections()
  {
    int max_fd = 0;
    fd_set rfds;
    struct timeval to = {1, 0};
    
    FD_ZERO(&rfds);
    FD_SET(m_iServerSocket, &rfds);
    max_fd = m_iServerSocket;

    // Come back here and finish implementing once AirPlayTCPClient is ready
    
    for (unsigned int i = 0; i < m_connections.size(); i++)
      {
	FD_SET(m_connections[i].m_iSocket, &rfds);
	if (m_connections[i].m_iSocket > max_fd)
	  max_fd = m_connections[i].m_iSocket;
      }
    
    int res = select(max_fd+1, &rfds, NULL, NULL, &to);
    if (res < 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessConnections() Server: Select failed");
	Sleep(100);
	ServerSocketSetup();
      }
    else if (res > 0)
      {
	for (int i = m_connections.size() - 1; i >= 0; i--)
	  {
	    int socket = m_connections[i].m_iSocket;
	    if (FD_ISSET(socket, &rfds))
	      {
		char buffer[1024] = {};
		int  nread = 0;
		nread = recv(socket, (char*)&buffer, 1024, 0);
		if (nread > 0)
		  {
		    string sessionId;
		    m_connections[i].PushBuffer(this, buffer, nread, sessionId, m_reverseSockets);
		  }
		if (nread <= 0)
		  {
		    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessConnections(): Disconnection detected");
		    m_connections[i].Disconnect();
		    m_connections.erase(m_connections.begin() + i);
		  }
	      }
	  }
	
	if (FD_ISSET(m_iServerSocket, &rfds))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessConnections(): New connection detected");
	    AirPlayTCPClient newconnection;
	    newconnection.m_pAirPlay_Protocol_AirPlay = this;
	    newconnection.m_iSocket = accept(m_iServerSocket, &newconnection.m_ClientAddr, &newconnection.m_ClientAddrLen);
	    
	    if (newconnection.m_iSocket == INVALID_SOCKET)
	      {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessConnections(): Accept of new connection failed: %d", errno);
		if (EBADF == errno)
		  {
		    Sleep(1000);
		    ServerSocketSetup();
		  }
	      }
	    else
	      {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessConnections(): New connection added");
		m_connections.push_back(newconnection);
	      }
	  }
      }   
  }
  
  void AirPlay_Protocol_AirPlay::ServerThread()
  {
    while (!m_pAirPlay_Streamer->m_bQuit_get())
      {

	if (m_pAirPlay_Videolan->IsPlaying())
	  {
	    m_pAirPlay_Videolan->UpdateStatus();
	  }

	ProcessConnections();
      }
  }

  float AirPlay_Protocol_AirPlay::GetDuration()
  {
    return m_pAirPlay_Videolan->GetDuration();
  }
  
  float AirPlay_Protocol_AirPlay::GetPosition()
  {
    return m_pAirPlay_Videolan->GetPosition();
  }

  void AirPlay_Protocol_AirPlay::SetTime(float fTime)
  {
    m_pAirPlay_Videolan->SetTime(fTime);
  }

  AirPlay_Protocol_AirPlay::AirPlayTCPClient::AirPlayTCPClient()
  {
    m_iSocket = INVALID_SOCKET;
    m_pHttpParser = new HttpParser();
    m_ClientAddrLen = sizeof(struct sockaddr);
    m_iLastEvent = EVENT_NONE;
    m_pAirPlay_Protocol_AirPlay = NULL;
  }

  AirPlay_Protocol_AirPlay::AirPlayTCPClient::AirPlayTCPClient(const AirPlayTCPClient& client)
  {
    Copy(client);
    m_pHttpParser = new HttpParser();
  }

  AirPlay_Protocol_AirPlay::AirPlayTCPClient::~AirPlayTCPClient()
  {
    delete m_pHttpParser;
  }

  AirPlay_Protocol_AirPlay::AirPlayTCPClient& AirPlay_Protocol_AirPlay::AirPlayTCPClient::operator=(const AirPlayTCPClient& client)
  {
    Copy(client);
    m_pHttpParser = new HttpParser();
    return *this;
  }
  
  void AirPlay_Protocol_AirPlay::AirPlayTCPClient::PushBuffer(AirPlay_Protocol_AirPlay *server, 
							      const char *buffer, int length,
							      string &sessionId, 
							      map<string, int> &reverseSockets)
  {
    HttpParser::status_t status = m_pHttpParser->addBytes(buffer, length);
    
    if (status == HttpParser::Done)
      {
	// Parse the request
	string responseHeader;
	string responseBody;
	string reverseHeader;
	string reverseBody;
	int status = ProcessRequest(responseHeader, responseBody, reverseHeader, reverseBody, sessionId);
	string statusMsg = "OK";
	int reverseSocket = INVALID_SOCKET;
	
	switch(status)
	  {
	  case AIRPLAY_STATUS_NOT_IMPLEMENTED:
	    statusMsg = "Not Implemented";
	    break;
	  case AIRPLAY_STATUS_SWITCHING_PROTOCOLS:
	    statusMsg = "Switching Protocols";
	    reverseSockets[sessionId] = m_iSocket;//save this socket as reverse http socket for this sessionid
	    break;
	  case AIRPLAY_STATUS_NEED_AUTH:
	    statusMsg = "Unauthorized";
	    break;
	  case AIRPLAY_STATUS_NOT_FOUND:
	    statusMsg = "Not Found";
	    break;
	  case AIRPLAY_STATUS_METHOD_NOT_ALLOWED:
	    statusMsg = "Method Not Allowed";
	    break;
	  }
	
	// Prepare the response
	string response;
	const time_t ltime = time(NULL);
	char *date = asctime(gmtime(&ltime)); //Fri, 17 Dec 2010 11:18:01 GMT;
	date[strlen(date) - 1] = '\0'; // remove \n
	response = string_format("HTTP/1.1 %d %s\nDate: %s\r\n", status, statusMsg.c_str(), date);
	if (responseHeader.size() > 0)
	  {
	    response += responseHeader;
	  }
	
	if (responseBody.size() > 0)
	  {
	    response = string_format("%sContent-Length: %d\r\n",response.c_str(), responseBody.size());
	  }
	response += "\r\n";
	
	if (responseBody.size() > 0)
	  {
	    response += responseBody;
	  }
	
	// Send the response
	//don't send response on AIRPLAY_STATUS_NO_RESPONSE_NEEDED
	if (status != AIRPLAY_STATUS_NO_RESPONSE_NEEDED)
	  {
	    send(m_iSocket, response.c_str(), response.size(), 0);
	  }
	
	// Send event status per reverse http socket (play, loading, paused)
	// if we have a reverse header and a reverse socket
	if (reverseHeader.size() > 0 && reverseSockets.find(sessionId) != reverseSockets.end())
	  {
	    //search the reverse socket to this sessionid
	    response = string_format("POST /event HTTP/1.1\r\n");
	    reverseSocket = reverseSockets[sessionId]; //that is our reverse socket
	    response += reverseHeader;
	  }
	response += "\r\n";
	
	if (reverseBody.size() > 0)
	  {
	    response += reverseBody;
	  }
	
	if (reverseSocket != INVALID_SOCKET)
	  {
	    send(reverseSocket, response.c_str(), response.size(), 0);//send the event status on the eventSocket
	  }

	// We need a new parser...
	delete m_pHttpParser;
	m_pHttpParser = new HttpParser;
      }
  }
  
  void AirPlay_Protocol_AirPlay::AirPlayTCPClient::Disconnect()
  {
    if (m_iSocket != INVALID_SOCKET)
      {
	shutdown(m_iSocket, SHUT_RDWR);
	close(m_iSocket);
	m_iSocket = INVALID_SOCKET;
	delete m_pHttpParser;
	m_pHttpParser = NULL;
      }
  }

  void AirPlay_Protocol_AirPlay::AirPlayTCPClient::Copy(const AirPlayTCPClient& client)
  {
    m_iSocket = client.m_iSocket;
    m_ClientAddr = client.m_ClientAddr;
    m_ClientAddrLen = client.m_ClientAddrLen;
    m_pHttpParser = client.m_pHttpParser;
    m_pAirPlay_Protocol_AirPlay=client.m_pAirPlay_Protocol_AirPlay;
  }
  
  void AirPlay_Protocol_AirPlay::AirPlayTCPClient::ComposeReverseEvent(string& reverseHeader,
								       string& reverseBody,
								       string sessionId,
								       int state)
  {
    if ( m_iLastEvent != state )
      { 
	switch(state)
	  {
	  case EVENT_PLAYING:
	  case EVENT_LOADING:
	  case EVENT_PAUSED:
	  case EVENT_STOPPED:
	    char cReverseBody[4096];
	    sprintf(cReverseBody, EVENT_INFO, eventStrings[state]);
	    reverseBody = cReverseBody;
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ComposeReverseEvent: sending event: %s", eventStrings[state]);
	    break;
	  }
	reverseHeader = "Content-Type: text/x-apple-plist+xml\r\n";
	char tmp1[1000];
	sprintf(tmp1,"%sContent-Length: %d\r\n",reverseHeader.c_str(),reverseBody.size());
	reverseHeader = tmp1;
	char tmp2[1000];
	sprintf(tmp2,"%sx-apple-session-id: %s\r\n",reverseHeader.c_str(),sessionId.c_str());
	reverseHeader = tmp2;
	m_iLastEvent = state;
	cout << endl << reverseHeader << reverseBody << endl;
      }
    
  }

  void AirPlay_Protocol_AirPlay::AirPlayTCPClient::Encode(string& strURLData)
  {
    string strResult;
    
    /* wonder what a good value is here is, depends on how often it occurs */
    strResult.reserve( strURLData.length() * 2 );
    
    for (int i = 0; i < (int)strURLData.size(); ++i)
      {
	int kar = (unsigned char)strURLData[i];
	//if (kar == ' ') strResult += '+'; // obsolete
	if (isalnum(kar) || strchr("-_.!()" , kar) ) // Don't URL encode these according to RFC1738
	  {
	    strResult += kar;
	  }
	else
	  {
	    string strTmp;
	    char cTmp[5];
	    sprintf(cTmp, "%%%02.2x", kar);
	    strTmp = cTmp;
	    strResult += strTmp;
	  }
      }
    strURLData = strResult;
  }

  int AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest( string& responseHeader,
								  string& responseBody,
								  string& reverseHeader,
								  string& reverseBody,
								  string& sessionId)
  {
    string method, uri, queryString, body, contentType, authorization;
    if (m_pHttpParser->getMethod())
      method = m_pHttpParser->getMethod();
    if (m_pHttpParser->getUri())
      uri = m_pHttpParser->getUri();
    if (m_pHttpParser->getQueryString())
      queryString = m_pHttpParser->getQueryString();
    if (m_pHttpParser->getBody())
      body = m_pHttpParser->getBody();
    if (m_pHttpParser->getValue("content-type"))
      contentType = m_pHttpParser->getValue("content-type");
    if (m_pHttpParser->getValue("x-apple-session-id"))
      sessionId = m_pHttpParser->getValue("x-apple-session-id");
    if (m_pHttpParser->getValue("authorization"))
      authorization = m_pHttpParser->getValue("authorization");
    int status = AIRPLAY_STATUS_OK;
    bool needAuth = false;
    
    int startQs = uri.find('?');
    if (startQs != string::npos)
      {
	uri = uri.substr(0, startQs);
      }
    
    // This is the socket which will be used for reverse HTTP
    // negotiate reverse HTTP via upgrade
    if (uri == "/reverse")
      {
	status = AIRPLAY_STATUS_SWITCHING_PROTOCOLS;
	responseHeader = "Upgrade: PTTH/1.0\r\nConnection: Upgrade\r\n";
      }

    // The rate command is used to play/pause media.
    // A value argument should be supplied which indicates media should be played or paused.
    // 0.000000 => pause
    // 1.000000 => play
    else if (uri == "/rate")
      {
	const char* found = strstr(queryString.c_str(), "value=");
	int rate = found ? (int)(atof(found + strlen("value=")) + 0.5f) : 0;

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest(): got request %s with rate %i", uri.c_str(), rate);
	
	if (rate == 0)
	  {
	    if (!gAirPlay_StreamerI->IsPaused())
	      {
		gAirPlay_StreamerI->CMD_Pause_Media(1000);
		ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_PAUSED);
	      }
	  }
	else
	  {
	    if (gAirPlay_StreamerI->IsPaused())
	      {
		gAirPlay_StreamerI->CMD_Restart_Media(1000);
		ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_PLAYING);
	      }
	  }
      }
    
    // The volume command is used to change playback volume.
    // A value argument should be supplied which indicates how loud we should get.
    // 0.000000 => silent
    // 1.000000 => loud
    else if (uri == "/volume")
      {
	const char* found = strstr(queryString.c_str(), "volume=");
	float volume = found ? (float)strtod(found + strlen("volume="), NULL) : 0;
	
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest() got request %s with volume %f", uri.c_str(), volume);
	
	if (volume >= 0 && volume <= 1)
	  {
	    // TODO: Implement this.
	  }
      }
    
    
    // Contains a header like format in the request body which should contain a
    // Content-Location and optionally a Start-Position
    else if (uri == "/play")
      {
	string location;
	float position = 0.0;
	m_iLastEvent = EVENT_NONE;
	
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest(): got request %s", uri.c_str());
	
	if (contentType == "application/x-apple-binary-plist")
	  {
	    AirPlay_Protocol_AirPlay::m_iIsPlaying++;    
	    
	    const char* bodyChr = m_pHttpParser->getBody();
	    
	    plist_t dict = NULL;
	    plist_from_bin(bodyChr, m_pHttpParser->getContentLength(), &dict);

	    if (plist_dict_get_size(dict))
	      {
		plist_t tmpNode = plist_dict_get_item(dict, "Start-Position");
		if (tmpNode)
		  {
		    double tmpDouble = 0;
		    plist_get_real_val(tmpNode, &tmpDouble);
		    position = (float)tmpDouble;
		  }
		
		tmpNode = plist_dict_get_item(dict, "Content-Location");
		if (tmpNode)
		  {
		    char *tmpStr = NULL;
		    plist_get_string_val(tmpNode, &tmpStr);
		    location=tmpStr;
		    
		    free(tmpStr); 
		  }
		
		if (dict)
		  {
		    plist_free(dict);
		  }
	      }
	    else
	      {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest() - Error parsing plist");
	      }
	    
	    
	  }
	else
	  {
	    AirPlay_Protocol_AirPlay::m_iIsPlaying++;        
	    // Get URL to play
	    int start = body.find("Content-Location: ");
	    if (start == string::npos)
	      return AIRPLAY_STATUS_NOT_IMPLEMENTED;
	    start += strlen("Content-Location: ");
	    int end = body.find('\n', start);
	    location = body.substr(start, end - start);
	    
	    start = body.find("Start-Position");
	    if (start != string::npos)
	      {
		start += strlen("Start-Position: ");
		int end = body.find('\n', start);
		string positionStr = body.substr(start, end - start);
		position = (float)atof(positionStr.c_str());
	      }
	  }
	
	if (status != AIRPLAY_STATUS_NEED_AUTH)
	  {
	    string userAgent="AppleCoreMedia/1.0.0.8F455 (AppleTV; U; CPU OS 4_3 like Mac OS X; de_de)";
	    Encode(userAgent);
	    location += "|User-Agent=" + userAgent;
	    
	    gAirPlay_StreamerI->StartAirPlayPlayback(location, position);
	    ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_PLAYING);
	  }
      }
    
    // Used to perform seeking (POST request) and to retrieve current player position (GET request).
    // GET scrub seems to also set rate 1 - strange but true
    else if (uri == "/scrub")
      {
	if (method == "GET")
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest(): got GET request %s", uri.c_str());
	    
	    float fPosition = m_pAirPlay_Protocol_AirPlay->GetPosition();
	    int fDuration = 0;

	    char cResponseBody[256];
	    stringstream ssResponseBody;
	    sprintf(cResponseBody,"duration: %d\r\nposition: %f\r\n",fDuration,fPosition);
	    
	    ssResponseBody << cResponseBody;
	    responseBody = ssResponseBody.str();

	    responseHeader = "Content-Type: text/parameters\r\n";

	    /* if (g_application.m_pPlayer && g_application.m_pPlayer->GetTotalTime())
	       {
	       float position = ((float) g_application.m_pPlayer->GetTime()) / 1000;
	       responseBody.Format("duration: %d\r\nposition: %f", g_application.m_pPlayer->GetTotalTime() / 1000, position);
	       }
	       else 
	       {
	       status = AIRPLAY_STATUS_METHOD_NOT_ALLOWED;
	       } */
	  }
	else
	  {
	    const char* found = strstr(queryString.c_str(), "position=");
	    float fPosition = (atof(found + strlen("position=")));
	    m_pAirPlay_Protocol_AirPlay->SetTime(fPosition);
	    /*if (found && g_application.m_pPlayer)
	      {
	      int64_t position = (int64_t) (atof(found + strlen("position=")) * 1000.0);
	      g_application.m_pPlayer->SeekTime(position);
	      CLog::Log(LOGDEBUG, "AIRPLAY: got POST request %s with pos %"PRId64, uri.c_str(), position);
	      } */
	  }
      }
    
  // Sent when media playback should be stopped
  else if (uri == "/stop")
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest(): got request %s", uri.c_str());
    {
      if (gAirPlay_StreamerI->IsPlaying()) //only stop player if we started him
	{
	  gAirPlay_StreamerI->StopAirPlayPlayback();
	  AirPlay_Protocol_AirPlay::m_iIsPlaying--;
	}
      else //if we are not playing and get the stop request - we just wanna stop picture streaming
      {
        gAirPlay_StreamerI->StopAirPlayPlayback();
      }
      ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_STOPPED);
    }
  }
    
    // RAW JPEG data is contained in the request body
  else if (uri == "/photo")
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest(): got request %s", uri.c_str());
      if (m_pHttpParser->getContentLength() > 0)
	{
	  string sTmpFileName = "/tmp/airplay_photo.jpg";
	  if( m_pHttpParser->getContentLength() > 3 &&
	      m_pHttpParser->getBody()[1] == 'P' &&
	      m_pHttpParser->getBody()[2] == 'N' &&
	      m_pHttpParser->getBody()[3] == 'G')
	    {
	      sTmpFileName = "/tmp/airplay_photo.png";
	    }

	  if (FileUtils::WriteBufferIntoFile(sTmpFileName, 
					     m_pHttpParser->getBody(),
					     m_pHttpParser->getContentLength()))
	    {
	      // File written, send to be displayed
	      gAirPlay_StreamerI->StartAirPlayPhotos(sTmpFileName);
	    }
	    
	  
	  
	}
    }
    
  else if (uri == "/playback-info")
    {
      float position = m_pAirPlay_Protocol_AirPlay->GetPosition();
      float duration = m_pAirPlay_Protocol_AirPlay->GetDuration();
      float cachePosition = 0.0f;
      bool playing = false;

      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest: got request %s", uri.c_str());
      
      responseBody = string_format(PLAYBACK_INFO, duration, cachePosition, position, (m_pAirPlay_Protocol_AirPlay->m_pAirPlay_Streamer->m_bIsPlaying ? 1 : 0), duration);
      responseHeader = "Content-Type: text/x-apple-plist+xml\r\n";

      if (m_pAirPlay_Protocol_AirPlay->m_pAirPlay_Streamer->m_bIsPlaying)
	{
	  ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_PLAYING);
	}
      else
	{
	  ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_PAUSED);
	}
      
      //      fPosition = m_pAirPlay_Protocol_AirPlay->GetPosition();
      //fDuration = 3505.00;
      

      /* if (g_application.m_pPlayer)
	{
	  if (g_application.m_pPlayer->GetTotalTime())
	    {
        position = ((float) g_application.m_pPlayer->GetTime()) / 1000;
        duration = ((float) g_application.m_pPlayer->GetTotalTime()) / 1000;
        playing = g_application.m_pPlayer ? !g_application.m_pPlayer->IsPaused() : false;
        cachePosition = position + (duration * g_application.m_pPlayer->GetCachePercentage() / 100.0f);
	    }
	  
	  responseBody.Format(PLAYBACK_INFO, duration, cachePosition, position, (playing ? 1 : 0), duration);
	  responseHeader = "Content-Type: text/x-apple-plist+xml\r\n";
	  
	  if (g_application.m_pPlayer->IsCaching())
	    {
	      ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_LOADING);
	    }
	  else if (playing)
	    {
	      ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_PLAYING);
	    }
	  else
	    {
	      ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_PAUSED);
	    }
	}
      else
	{
	  responseBody.Format(PLAYBACK_INFO_NOT_READY, duration, cachePosition, position, (playing ? 1 : 0), duration);
	  responseHeader = "Content-Type: text/x-apple-plist+xml\r\n";     
	  ComposeReverseEvent(reverseHeader, reverseBody, sessionId, EVENT_STOPPED);
	} */
    }

  else if (uri == "/server-info")
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::Process_Request(): got request %s", uri.c_str());
    char *cResponseBody;
    cResponseBody = (char *)malloc(1024);
    sprintf(cResponseBody, SERVER_INFO, m_pAirPlay_Protocol_AirPlay->m_pAirPlay_Streamer->m_sCurrentMacAddress.c_str());
    stringstream ssResponseBody;
    ssResponseBody << cResponseBody;
    responseBody = ssResponseBody.str();
    free(cResponseBody);
    responseHeader = "Content-Type: text/x-apple-plist+xml\r\n";
  }

  else if (uri == "/slideshow-features")
  {
    // Ignore for now.
  }

  else if (uri == "/authorize")
  {
    // DRM, ignore for now.
  }
  
  else if (uri == "/setProperty")
  {
    status = AIRPLAY_STATUS_NOT_FOUND;
  }

  else if (uri == "/getProperty")
  {
    status = AIRPLAY_STATUS_NOT_FOUND;
  }  

  else if (uri == "200") //response OK from the event reverse message
  {
    status = AIRPLAY_STATUS_NO_RESPONSE_NEEDED;
  }
  else
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Protocol_AirPlay::AirPlayTCPClient::ProcessRequest(): unhandled request [%s]\n", uri.c_str());
    status = AIRPLAY_STATUS_NOT_IMPLEMENTED;
  }

  return status;
}


}

