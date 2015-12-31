/*
 * frontend_svr.h: server for remote frontends
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: frontend_svr.h,v 1.32 2012-03-17 20:19:27 phintuka Exp $
 *
 */

#ifndef __XINELIB_FRONTEND_SVR_H
#define __XINELIB_FRONTEND_SVR_H


#include "config.h"
#include "frontend.h"

//----------------------------- cXinelibServer --------------------------------

#define CTRL_BUF_SIZE 1024

class cBackgroundWriterI;
class cUdpScheduler;
class cStcFuture;
class cCmdFutures;
class cConnState;
class cXinelibDevice;

#include "tools/cxsocket.h"

class cXinelibServer : public cXinelibThread
{

  public:
    cXinelibServer(cXinelibDevice *Dev, int listen_port);
    virtual ~cXinelibServer();

  protected:
    virtual void    Action(void);

  public:
    // Playback control
    virtual void    TrickSpeed(int Speed, bool Backwards);

    // Data transfer
    virtual int     Poll(cPoller &Poller, int TimeoutMs);
    virtual bool    Flush(int TimeoutMs);
    virtual void    Clear(void);
    virtual int     Play(const uchar *buf, int len, eStreamId StreamId = sidVdr);
    virtual void    OsdCmd(void *cmd);
    virtual int64_t GetSTC();
    virtual void    SetHDMode(bool On);

    // Image grabbing
    virtual uchar  *GrabImage(int &Size, bool Jpeg, int Quality,
			      int SizeX, int SizeY);
    // Playback files
    virtual int     PlayFileCtrl(const char *Cmd, int TimeoutMs=-1);
    virtual bool    EndOfStreamReached(void);

    // Configuration
    virtual bool    Listen(int port);

    virtual int     SupportsTrueColorOSD(void);

protected:
    // Playback control
    virtual int     Xine_Control(const char *cmd);
    virtual int     Xine_Control_Sync(const char *cmd);

    virtual int     Xine_Control_Result(const char *cmd, uint TimeoutMs);
    virtual void    Sync(void);

protected:

    // Handling of messages from client(s)

    void Handle_Discovery_Broadcast(void);
    void Handle_ClientConnected(int fd);

    void Read_Control(int cli);
    void Handle_Control(int cli, const char *cmd);

    void Handle_Control_PIPE      (int cli, const char *arg);
    void Handle_Control_RTP       (int cli, const char *arg);
    void Handle_Control_UDP       (int cli, const char *arg);
    void Handle_Control_DATA      (int cli, const char *arg);
    void Handle_Control_KEY       (int cli, const char *arg);
    void Handle_Control_UDP_RESEND(int cli, const char *arg);
    void Handle_Control_CONFIG    (int cli);
    void Handle_Control_GRAB      (int cli, const char *arg);
    void Handle_Control_CONTROL   (int cli, const char *arg);
    void Handle_Control_HTTP      (int cli, const char *arg);
    void Handle_Control_RTSP      (int cli, const char *arg);

    void CloseDataConnection(int cli);
    void CloseConnection    (int cli);

protected:

    // Data

    int  m_Port;
    int  m_ServerId;

    int  fd_listen;
    int  fd_discovery;

    cxSocket fd_control[MAXCLIENTS];
    int      fd_data   [MAXCLIENTS];

    int  m_OsdTimeouts[MAXCLIENTS];
    char m_CtrlBuf    [MAXCLIENTS][CTRL_BUF_SIZE + 1];
    int  m_CtrlBufPos [MAXCLIENTS];

    int  m_ConnType   [MAXCLIENTS];  // Control connection type. See frontend_svr.c.
    bool m_bUdp       [MAXCLIENTS];  // Client uses UDP transport
    bool m_bMulticast [MAXCLIENTS];  // Client uses multicast RTP
    bool m_bConfigOk  [MAXCLIENTS];  // Client has been configured
    bool m_bArgbOSD   [MAXCLIENTS];  // Client supports ARGB OSD
    int  m_iMulticastMask; // bit [cli] is 1 or 0. 1 == multicast in use.
    int  m_MasterCli;      // Master client (controls playback speed)

    cString m_PipesDir;
    cString m_AllowedHostsFile;

    cBackgroundWriterI *m_Writer[MAXCLIENTS]; // buffered output (pipe/tcp/http)
    cConnState         *m_State[MAXCLIENTS];  // connection state (http/rtsp)
    cUdpScheduler      *m_Scheduler;

    // Storage for return values of pending RPCs
    cStcFuture         *m_StcFuture;
    cCmdFutures        *m_Futures;

    int  m_Token;
    int  AllocToken(void);
    bool HasClients(void);

    // Cache current PAT/PMT for new clients
    uchar   *m_Header;
    size_t   m_HeaderLength; // bytes used
    size_t   m_HeaderSize;   // bytes allocated
  public:
    void SetHeader(const uchar *Data, int Length, bool Reset = false);
};

#endif // __XINELIB_FRONTEND_SVR_H
