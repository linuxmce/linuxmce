/*
 * http.c: HTTP (/RTSP) helper classes
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: http.c,v 1.8 2012-01-29 19:30:55 phintuka Exp $
 *
 */

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <string.h>

#include <vdr/config.h>
#include <vdr/tools.h>

#include "../logdefs.h"

#include "http.h"

//
// cHttpReq
//

bool cHttpReq::SetCommand(const char *Command) 
{
  char *tmp = strdup(Command);
  char *pt = strchr(tmp, ' '), *uri;

  m_Valid = false;
  if(pt) {
    *pt++ = 0;
    m_Name = tmp;

    while(*pt && *pt == ' ') pt++;

    uri = pt;
    pt = strrchr(uri, ' ');
    if(pt) {
      m_Version = pt+1;
      while(*pt && *pt == ' ') *pt-- = 0;
      m_Uri = uri;
      m_Valid = true;
    }
  }

  free(tmp);
  return m_Valid;
}

cHeader *cHttpReq::Header(const char *Name) 
{
  for(cHeader *i = m_Headers.First(); i; i = m_Headers.Next(i))
    if(!strcmp(Name, i->Name()))
      return i;
  return NULL;
}

void cHttpReq::AddHeader(const char *Header, bool Duplicate) 
{
  if(strlen(Header) < 4096) {
    char *name = strdup(Header);
    char *val = strchr(name, ':');
    if(val) {
      *val++ = 0;
      while(*val == ' ') val++;
      AddHeader(name, val, Duplicate);
    }
    free(name);
  } else {
    LOGMSG("cConnState::AddHeader: header length exceeds 4096 !");
  }
}

void cHttpReq::AddHeader(const char *Name, const char *Value, bool Duplicate) 
{
  if(strlen(Name) > 64 || strlen(Value) > 4096) {
    LOGMSG("cConnState::AddHeader: header length exceeds limit !");
  } else {
    cHeader *h = Header(Name);
    if(!Duplicate && h)
      h->SetValue(Value);
    else {
      if(m_Headers.Count() < 50)
	m_Headers.Add(new cHeader(Name, Value));
      else
	LOGMSG("cConnState::AddHeader: header count exceeds 50 !");
    }
  }
}    

void cHttpReq::Reset(void) 
{
  m_Name = NULL;
  m_Uri = NULL;
  m_Version = NULL;
  m_Valid = false;
  m_Headers.Clear();
}

//
// Map file extensions to mime types
//

static const char *mimetype(const char *ext)
{
  static const struct {
    const char *ext;
    const char *mime;
  } ext2mime[] = {
    {"avi",  "video/avi"},
    {"vob",  "video/mpeg"},
    {"mpg",  "video/mpeg"},
    {"mpeg", "video/mpeg"},
    {"vdr",  "video/mp2p"},
    
    {"mp3",  "audio/mp3"},
    {"flac", "audio/flac"},
    
    {"jpg",  "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"gif",  "image/gif"},
    
    {NULL, NULL}
  };

  int i = -1;
  while(ext2mime[++i].ext)
    if(!strcmp(ext, ext2mime[i].ext))
      return ext2mime[i].mime;
  return NULL;
}

static char *unescape_uri(const char *uri)
{
  char *d = strdup(uri), *s = d, *result = d;
  while(*s) {
    if(s[0] == '%' && s[1] && s[2]) {
      unsigned int c;
      if (sscanf(s+1, "%02x", &c) == 1) {
	*d++ = (char)c;
	s += 3;
	continue;
      }
    }
    *d++ = *s++;
  }
  *d = 0;
  return result;
}

//
// cHttpStreamer
//

cList<cHttpStreamer> cHttpStreamer::m_Streamers;

void cHttpStreamer::CloseAll(bool OnlyFinished)
{
  if(!OnlyFinished) {
    while(m_Streamers.First())
      m_Streamers.Del(m_Streamers.First());

  } else {
    /*  purge finished streamers from list */
    cHttpStreamer *it = m_Streamers.First();
    while(it) {
      if(it->Active()) {
	it = (cHttpStreamer*)it->Next();
      } else {
	m_Streamers.Del(it);
	it = m_Streamers.First();
      }
    }
  }
}

cHttpStreamer::cHttpStreamer(int fd_http, const char *filename, 
			     cConnState *Request) :
    m_Filename(unescape_uri(filename), true)
{
  m_fds.set_handle(fd_http);
  m_fds.set_cork(true);
  m_fdf = -1;

  //m_Filename = filename;
  m_FileSize = -1;
  m_Start = 0;
  m_End = -1;
  m_KeepOpen = true;

  m_ConnState = Request;

  m_Finished = false;

  CloseAll(true);

  m_Streamers.Add(this);

  if(m_Streamers.Count() > 5) {
    LOGMSG("WARNING: There are %d running HTTP streamers !", m_Streamers.Count());
    if(m_Streamers.Count() > 20) {
      errno = 0;
      LOGERR("ERROR: There are %d running HTTP streamers, cancelling first", 
	     m_Streamers.Count());
      m_Streamers.Del(m_Streamers.First());
    }
  }

  Start();
}

cHttpStreamer::~cHttpStreamer()
{
  Cancel(3);
  if(m_ConnState)
    delete m_ConnState;
  if(m_fdf >= 0) 
    close(m_fdf);
  m_fdf = -1;
}

void cHttpStreamer::ParseRange(const char *Range)
{
  m_Start = 0;
  m_End = -1;
  if(Range) {
    LOGDBG("cHttpStreamer: Request range is \'%s\'", Range);
    switch(sscanf(Range, "bytes=%" PRId64 "-%" PRId64, &m_Start, &m_End)) {
    case 2: LOGMSG("  Range: %s (%" PRId64 " - %" PRId64 ")", Range, m_Start, m_End);
            break;
    case 1: m_End = -1;
            LOGMSG("  Range start: %s (%" PRId64 " - )", Range, m_Start);
	    break;
    default:
    case 0: m_Start = 0;
            m_End = -1;
	    break;
    }
  }
}

bool cHttpStreamer::ParseRequest(void)
{
  cHeader *h;

  if((h = m_ConnState->Header("Range")) != NULL)
    ParseRange(h->Value());

  m_KeepOpen = false;
  if((h = m_ConnState->Header("Connection")) != NULL) {
    m_KeepOpen = !strcasecmp(h->Value(), "keep-alive");
    if(m_KeepOpen) 
      LOGDBG("cHttpStreamer: client wants to keep connection open");
  }

  return true;
}

bool cHttpStreamer::Seek(void)
{
  if(m_fdf < 0) {
    m_fdf = open(m_Filename, O_RDONLY);
    if(m_fdf < 0) {
      LOGERR("cHttpStreamer: error opening %s", *m_Filename);
      m_fds.write_cmd(HTTP_REPLY_401); // 401 Not Found
      return false;
    }

    m_FileSize = lseek(m_fdf, 0, SEEK_END);
    if(m_FileSize <= 0) {
      LOGERR("cHttpStreamer: error seeking %s to end", *m_Filename);
      m_fds.write_cmd(HTTP_REPLY_401); // 401 Not Found
      return false;
    }
  }

  if(m_Start >= m_FileSize) {
    LOGERR("cHttpStreamer: Requested range not available "
	   "(%s:%" PRId64 "-%" PRId64 " ; len=%" PRIu64 ")", 
	   *m_Filename, m_Start, m_End, (uint64_t)m_FileSize);
    m_fds.write_cmd(HTTP_REPLY_416); // 416 Requested Range Not Satisfiable
    return false;
  }
  
  if(m_Start > 0) {
    if(m_End >= m_FileSize || m_End < 0)
      m_End = m_FileSize-1;

    m_fds.write_cmd("HTTP/1.1 206 Partial Content\r\n");
    m_fds.printf("Content-Range: bytes %" PRId64 "-%" PRId64 "/%" PRIu64 "\r\n", 
		 m_Start, m_End, (uint64_t)m_FileSize);
  } else {
    m_fds.write_cmd("HTTP/1.1 200 OK\r\n");
  }

  /* content type */
  const char *ext = strrchr(m_Filename, '.');
  if(ext) {
    const char *mime = mimetype(ext+1);
    if(mime)
      m_fds.printf("Content-Type: %s\r\n", mime);
  }

  /* Content-Length */
  if(m_FileSize >= 0) {
    int64_t len = m_FileSize;
    if(m_End >= 0)
      len = m_End + 1;
    if(m_Start >= 0)
      len -= m_Start;
    m_fds.printf("Content-Length: %" PRId64 "\r\n", len);
  }

  /* Connection and end of reply */
  if(m_KeepOpen)
    m_fds.write_cmd("Connection: Keep-Alive\r\n"
		    "\r\n");
  else
    m_fds.write_cmd("Connection: Close\r\n"
		    "\r\n");

  if(m_Start)
    lseek(m_fdf, (off_t)m_Start, SEEK_SET);
  else
    lseek(m_fdf, 0, SEEK_SET);

  return true;
}

bool cHttpStreamer::ReadPipelined(void)
{
  char buf[2048];
  int r;

  if(m_ConnState)
    delete m_ConnState;
  m_ConnState = new cConnState;

  do {
    r = m_fds.readline(buf, sizeof(buf), 1000);
    if(r < 0 || errno == EAGAIN || r >= (int)sizeof(buf)) {
      LOGMSG("cHttpStreamer: disconnected");
      return false;
    }

    LOGMSG("cHttpStreamer: pipelined request: %s", buf);

    if(!*m_ConnState->Name()) {
      if(!m_ConnState->SetCommand(buf) ||
	 strcmp(m_ConnState->Name(), "GET") ||
	 strncmp(m_ConnState->Uri(), "/PLAYFILE", 9) ||
	 strncmp(m_ConnState->Version(), "HTTP/1.", 7)) {
	LOGMSG("Incorrect HTTP request: %s", buf);
	return false;
      }
    }
    else if(r > 0)
      m_ConnState->AddHeader(buf);
  } while(r>0);

  return true;
}

void cHttpStreamer::Action(void)
{
  int      n = 0;
  cxPoller p(m_fds);
  bool     Disc = !(ParseRequest() && Seek());
  //uint64_t pos  = m_Start;
  off_t    start = (off_t)m_Start;

  while(Running() && !Disc) {

    n = m_End>0 ? (m_End-start+1) : m_FileSize - start;
    if(n > 0) {
      errno = 0;
      pthread_testcancel();
      n = m_fds.sendfile(m_fdf, &start, n);
      pthread_testcancel();
      if(n <= 0) {
	if(errno == EAGAIN || errno == EINTR) {
	  p.Poll(100);
	  pthread_testcancel();
	} else {
	  LOGERR("cHttpStreamer: sendfile() failed");
	  Disc=true;
	}
      } else if(n == 0) {
	LOGMSG("cHttpStreamer: disconnected at %" PRId64, (int64_t)start);
	Disc = true;
      }
      continue;
    }

    LOGDBG("cHttpStreamer: Hit to EOF or end of requested range");

    m_fds.flush_cork();

    if(!m_KeepOpen) {
      LOGMSG("cHttpStreamer: disconnecting (request complete)");
      Disc = true;
      continue;
    }

    // keep connection open for new range for max. 30 sec
    n = 30; 
    do {
      pthread_testcancel();
      //cxPoller p(m_fds);
      LOGDBG("cHttpStreamer: Request complete, waiting...");
      if(p.Poll(1000)) {
	LOGDBG("cHttpStreamer: Reading pipelined request");
	pthread_testcancel();
	Disc = !(ReadPipelined() && ParseRequest() && Seek());
	//pos = m_Start;
      }
    } while(--n && Running() && !Disc);

    if(n <= 0) {
      LOGMSG("cHttpStreamer: Disconnecting (timeout)");
      Disc = true;
    }
  }

  close(m_fdf);
  m_fdf = -1;

  m_fds.close();

  m_Finished = true;
}
