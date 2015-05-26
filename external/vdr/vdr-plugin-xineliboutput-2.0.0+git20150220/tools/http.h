/*
 * http.h: HTTP (/RTSP) helper classes
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: http.h,v 1.5 2007-01-07 09:45:48 phintuka Exp $
 *
 */

#ifndef XINELIBOUTPUT_HTTP_H_
#define XINELIBOUTPUT_HTTP_H_

#include <vdr/tools.h>


#define HTTP_REPLY_401 \
	"HTTP/1.1 401 Unauthorized\r\n" \
        "Connection: Close\r\n" \
	"\r\n"

#define HTTP_REPLY_404 \
	"HTTP/1.1 404 Not Found\r\n" \
        "Connection: Close\r\n" \
	"\r\n"

#define HTTP_REPLY_416 \
	"HTTP/1.1 416 Requested Range Not Satisfiable\r\n" \
        "Connection: Close\r\n" \
	"\r\n"

#define HTTP_REPLY_200_PRIMARY \
	"HTTP/1.1 200 OK\r\n" \
	"Content-Type: video/mpeg\r\n" \
        "Connection: Close\r\n" \
	"\r\n"
//"Content-Type: video/mp2p\r\n"


//
// cHeader
//

class cHeader : public cListObject 
{
  protected:
    cString m_Name;
    cString m_Value;

  private:
    cHeader();

  public:
    cHeader(const char *Name, const char *Value) :
      m_Name(Name), m_Value(Value) {};

    const cString& Name(void)        { return m_Name; }
    const cString& Value(void)       { return m_Value; }
    int  IntValue(void)              { return *m_Value?atoi(m_Value):-1; }
    void SetValue(const char *Value) { m_Value = Value; }
};


//
// cHttpReq
//

class cHttpReq 
{
  private:
    cString        m_Name;
    cString        m_Uri;
    cString        m_Version;
    cList<cHeader> m_Headers;

    bool           m_Valid;

  public:
    cHttpReq() : m_Valid(false) {}

    bool           SetCommand(const char *Command);
    const cString& Name(void) { return m_Name; }
    const cString& Uri(void) { return m_Uri; }
    const cString& Version(void) { return m_Version; }
    bool           Valid(void) { return m_Valid; }

    void           AddHeader(const char *Header, bool Duplicate=false);
    void           AddHeader(const char *Name, const char *Value, bool Duplicate=false);
    cHeader       *Header(const char *Name);

    void           Reset(void);
};


//
// cConnState
//

class cConnState : public cHttpReq 
{
   public:
};


//
// cHttpStreamer
//

#include <vdr/tools.h>
#include <vdr/thread.h>

#include "cxsocket.h"

class cHttpStreamer : protected cListObject, cThread 
{
  public:
    cHttpStreamer(int fd_http, const char *filename, cConnState *Request);
    virtual ~cHttpStreamer();

    static void CloseAll(bool OnlyFinished = false);

  private:
    static cList<cHttpStreamer> m_Streamers;

    cxSocket m_fds;
    int      m_fdf;

    cString m_Filename;
    int64_t m_FileSize;
    int64_t m_Start;
    int64_t m_End;
    bool    m_KeepOpen;

    cConnState        *m_ConnState;

    bool m_Finished;

    virtual void Action(void);

    bool ParseRequest(void);
    void ParseRange(const char *Range);
    bool ReadPipelined(void);
    bool Seek(void);
};

#endif // XINELIBOUTPUT_HTTP_H_

