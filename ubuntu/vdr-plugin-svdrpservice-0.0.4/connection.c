#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <vdr/config.h>
#include "connection.h"
#include "setup.h"

#if DEBUG == 1
#define DEBUG_PRINTF(args...) fprintf(stderr, args)
#elif DEBUG == 2
#define DEBUG_PRINTF(args...) dsyslog(args)
#else
#define DEBUG_PRINTF(args...)
#endif

int charsetcmp(const char* s, const char* t)
{
	int ret;
	do
	{
		while (*s && !isalnum(*s))
			s++;
		while (*t && !isalnum(*t))
			t++;
		ret = toupper(*t) - toupper(*s);
	}
	while (ret == 0 && *(t++) && *(s++));
	return ret;
}

#if VDRVERSNUM < 10503
// cCharSetConv -------------------------------------------------------
#include <iconv.h>

class cCharSetConv {
private:
  iconv_t cd;
  char *result;
  size_t length;
public:
  cCharSetConv(const char *FromCode, const char *ToCode);
  ~cCharSetConv();
  const char *Convert(const char *From, char *To = NULL, size_t ToLength = 0);
  static const char *SystemCharacterTable(void) { return I18nCharSets()[Setup.OSDLanguage]; }
};

cCharSetConv::cCharSetConv(const char *FromCode, const char *ToCode)
{
  if (!FromCode)
     FromCode = "UTF-8";
  if (!ToCode)
     ToCode = "UTF-8";
  cd = (FromCode && ToCode) ? iconv_open(ToCode, FromCode) : (iconv_t)-1;
  result = NULL;
  length = 0;
}

cCharSetConv::~cCharSetConv()
{
  free(result);
  iconv_close(cd);
}

const char *cCharSetConv::Convert(const char *From, char *To, size_t ToLength)
{
  if (cd != (iconv_t)-1 && From && *From) {
     char *FromPtr = (char *)From;
     size_t FromLength = strlen(From);
     char *ToPtr = To;
     if (!ToPtr) {
        length = max(length, FromLength * 2); // some reserve to avoid later reallocations
        result = (char *)realloc(result, length);
        ToPtr = result;
        ToLength = length;
        }
     else if (!ToLength)
        return From; // can't convert into a zero sized buffer
     ToLength--; // save space for terminating 0
     char *Converted = ToPtr;
     while (FromLength > 0) {
           if (iconv(cd, &FromPtr, &FromLength, &ToPtr, &ToLength) == size_t(-1)) {
              if (errno == E2BIG || errno == EILSEQ && ToLength < 1) {
                 if (To)
                    break; // caller provided a fixed size buffer, but it was too small
                 // The result buffer is too small, so increase it:
                 size_t d = ToPtr - result;
                 size_t r = length / 2;
                 length += r;
                 Converted = result = (char *)realloc(result, length);
                 ToLength += r;
                 ToPtr = result + d;
                 }
              if (errno == EILSEQ) {
                 // A character can't be converted, so mark it with '?' and proceed:
                 FromPtr++;
                 FromLength--;
                 *ToPtr++ = '?';
                 ToLength--;
                 }
              else if (errno != E2BIG)
                 return From; // unknown error, return original string
              }
           }
     *ToPtr = 0;
     return Converted;
     }
  return From;
}
#endif

// cSvdrpConnection -------------------------------------------------------

int cSvdrpConnection::Connect() {
	if (!serverIp) {
		esyslog("svdrpservice: No server IP specified");
		return -1;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverPort);
	if (!::inet_aton(serverIp, &server_addr.sin_addr)) {
		esyslog("svdrpservice: Invalid server IP '%s'", serverIp);
		return -1;
	}

	int sock = ::socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		esyslog("svdrpservice: Error creating socket for connection to %s: %m", serverIp);
		return -1;
	}
	// set nonblocking
	int flags = ::fcntl(sock, F_GETFL, 0);
	if (flags < 0 || ::fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		esyslog("svdrpservice: Unable to use nonblocking I/O for %s: %m", serverIp);
		return -1;
	}
	if (::connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		if (errno != EINPROGRESS) {
			esyslog("svdrpservice: connect to %s:%hu failed: %m", serverIp, serverPort);
			return -1;
		}

		int result;
		fd_set fds;
		struct timeval tv;
		cTimeMs starttime;
		int timeout = SvdrpServiceSetup.connectTimeout * 1000;
		do {
			FD_ZERO(&fds);
			FD_SET(sock, &fds);
			tv.tv_usec = (timeout % 1000) * 1000;
			tv.tv_sec = timeout / 1000;
			result = ::select(sock + 1, NULL, &fds, NULL, &tv);
		} while (result == -1 && errno == EINTR &&
				(timeout = SvdrpServiceSetup.connectTimeout * 1000 - starttime.Elapsed()) > 100);

		if (result == 0) {	// timeout
			result = -1;
			errno = ETIMEDOUT;
		}
		else if (result == 1) {	// check socket for errors
			int error;
			socklen_t size = sizeof(error);
			result = ::getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &size);
			if (result == 0 && error != 0) {
				result = -1;
				errno = error;
			}
		}

		if (result != 0) {
			esyslog("svdrpservice: Error connecting to %s:%hu: %m", serverIp, serverPort);
			::close(sock);
			return -1;
		}
	}
	return sock;
}

cSvdrpConnection::cSvdrpConnection(const char *ServerIp, unsigned short ServerPort, bool Shared):
		serverPort(ServerPort), convIn(NULL), convOut(NULL),
		refCount(0), shared(Shared) {
	serverIp = ServerIp ? ::strdup(ServerIp) : NULL;
	bufSize = BUFSIZ;
	buffer = MALLOC(char, bufSize);
}

cSvdrpConnection::~cSvdrpConnection(void) {
	Close();
	delete convIn;
	delete convOut;
	free(serverIp);
	free(buffer);
}

bool cSvdrpConnection::HasDestination(const char *ServerIp, unsigned short ServerPort) const {
	if (ServerIp == NULL || serverIp == NULL)
		return false;
	return serverPort == ServerPort && !strcmp(serverIp, ServerIp);
}

bool cSvdrpConnection::Open() {
	if (file.IsOpen()) {
		//TODO: make sure the connection is still alive (e.g. send STAT command)
	}
	if (!file.IsOpen()) {
		int fd = Connect();
		if (fd < 0)
			return false;

		if (!file.Open(fd)) {
			::close(fd);
			return false;
		}
		
		// check for greeting
		cList<cLine> greeting;
		if (Receive(&greeting, true) != 220) {
			esyslog("svdrpservice: did not receive greeting from %s. Closing...", serverIp);
			Abort();
			return false;
		}

		// do we need to convert between different charsets?
		DELETENULL(convIn);
		DELETENULL(convOut);
		cString convMsg = "";
		if (greeting.First() && greeting.First()->Text())
		{
			const char *l;
			const char *r = strrchr(greeting.First()->Text(), ';');
			// at least two semicolons found
			if (r != strchr(greeting.First()->Text(), ';'))
			{
				r = skipspace(++r);
				l = cCharSetConv::SystemCharacterTable() ? cCharSetConv::SystemCharacterTable() : "UTF-8";
				if (charsetcmp(r, l) != 0)
				{
					convIn = new cCharSetConv(r, l);
					convOut = new cCharSetConv(l, r);
					convMsg = cString::sprintf(" (enabled charset conversion %s - %s)", r, l);
				}
			}
		}

		isyslog("SvdrpService: connected to %s:%hu%s", serverIp, serverPort, *convMsg);
	}
	return true;
}

void cSvdrpConnection::Close(void) {
	if (Send("QUIT\r\n", false))
		Receive();
	file.Close();
}

void cSvdrpConnection::Abort(void) {
	file.Close();
}

bool cSvdrpConnection::Send(const char *Cmd, bool Reconnect) {
	if (!Cmd)
		return false;

	if (Reconnect && !file.IsOpen())
		Open();
	if (!file.IsOpen()) {
		esyslog("svdrpservice: unable to send command to %s. Socket is closed", serverIp);
		return false;
	}

	DEBUG_PRINTF("SEND %s", Cmd);
	if (convOut)
		Cmd = convOut->Convert(Cmd);
	unsigned int len = ::strlen(Cmd);
	if (safe_write(file, Cmd, len) < 0) {
		esyslog("svdrpservice: error while writing to %s: %m", serverIp);
		Abort();
		return false;
	}
	return true;
}

unsigned short cSvdrpConnection::Receive(cList<cLine>* List, bool Connecting) {
	int timeoutMs = (Connecting ? SvdrpServiceSetup.connectTimeout : SvdrpServiceSetup.readTimeout) * 1000;
	while (ReadLine(timeoutMs)) {
		char *tail;
		long int code = ::strtol(buffer, &tail, 10);
		if (tail - buffer == 3 &&
				code >= 100 && code <= 999 &&
				(*tail == ' ' || *tail =='-')) {
			if (List)
			{
				const char* s = buffer + 4;
				if (convIn)
					s = convIn->Convert(s);
				List->Add(new cLine(s));
			}
			if (*tail == ' ')
				return (unsigned short) code;
		}
		else {
			esyslog("svdrpservice: invalid reply from %s: '%s'", serverIp, buffer);
			Close();
			break;
		}
	}
	if (List)
		List->Clear();
	return 0;
}

bool cSvdrpConnection::ReadLine(int TimeoutMs) {
	if (!file.IsOpen())
		return false;

	unsigned int tail = 0;
	while (cFile::FileReady(file, TimeoutMs)) {
		unsigned char c;
		int r = safe_read(file, &c, 1);
		if (r > 0) {
			if (c == '\n' || c == 0x00) {
				// line complete, make sure the string is terminated
				buffer[tail] = 0;
				DEBUG_PRINTF("READ %s\n", buffer);
				return true;
			}
			else if ((c <= 0x1F || c == 0x7F) && c != 0x09) {
				// ignore control characters
				}
			else {
				if (tail >= bufSize - 1) {
					bufSize += BUFSIZ;
					buffer = (char*) realloc(buffer, bufSize);
					if (!buffer) {
						esyslog("svdrpservice: unable to increase buffer size to %d byte", bufSize);
						Close();
						return false;
					}
				}
				buffer[tail++] = c;
			}
		}
		else {
			esyslog("svdrpservice: lost connection to %s", serverIp);
			buffer[0] = 0;
			Abort();
			return false;
		}
	}
	esyslog("svdrpservice: timeout waiting for reply from %s", serverIp);
	buffer[0] = 0;
	Abort();
	return false;
}
