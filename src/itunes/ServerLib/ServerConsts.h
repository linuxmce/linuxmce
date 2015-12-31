/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
**************************************************************************/

#ifndef __HEADER_INCLUDED_SERVERCONSTS__
#define __HEADER_INCLUDED_SERVERCONSTS__

#define CMD_GET_PLAYLIST	1
#define CMD_GET_FILES		2
#define CMD_GET_FILES_ALL	3
#define CMD_QUIT			100
#define CMD_OK 0

/// Server port
#define SERVER_PORT 665

#define PACKET_STRING_LEN 256

struct TrackFileInfoPacket {
	unsigned long size;
	char name[PACKET_STRING_LEN];
};

struct TrackInfoPacket {
	unsigned long id;
	unsigned long size;
	char album[PACKET_STRING_LEN];
	char artist[PACKET_STRING_LEN];
	long durationSec;
	char kind[PACKET_STRING_LEN];
	char file[PACKET_STRING_LEN];
};

#define CRASHREPORT_EMAIL_ADDR	""
#define CRASHREPORT_EMAIL_SUBJ	"iTunes Broadcaster Crash Log"

#endif // __HEADER_INCLUDED_SERVERCONSTS__
