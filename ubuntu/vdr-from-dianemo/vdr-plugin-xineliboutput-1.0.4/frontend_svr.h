/*
 * frontend_svr.h: server for remote frontends
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: frontend_svr.h,v 1.18.2.1 2008/12/10 22:56:15 phintuka Exp $
 *
 */

#ifndef __XINELIB_FRONTEND_SVR_H
#define __XINELIB_FRONTEND_SVR_H


#include "frontend.h"

//----------------------------- cXinelibServer --------------------------------

#define MAXCLIENTS 10

class cBackgroundWriterI;
class cUdpScheduler;
class cStcFuture;
class cCmdFutures;
class cConnState;

#include "tools/cxsocket.h"

class cXinelibServer : public cXinelibThread 
{

  public:
    cXinelibServer(int listen_port);
    virtual ~cXinelibServer();

    // Thread control
    virtual void Stop(void);

  protected:
    virtual void Action(void);

  public:
    // Playback control
    virtual void TrickSpeed(int Speed);

    // Data transfer
    virtual int  Poll(cPoller &Poller, int TimeoutMs);
    virtual bool Flush(int TimeoutMs);
    virtual void Clear(void);
    virtual int  Play_PES(const uchar *buf, int len);
    virtual void OsdCmd(void *cmd);
    virtual int64_t GetSTC();
    virtual void SetHDMode(bool On);

    // Image grabbing
    virtual uchar *GrabImage(int &Size, bool Jpeg, int Quality, 
			     int SizeX, int SizeY);
    // Playback files
    virtual int  PlayFileCtrl(const char *Cmd, int TimeoutMs = -1);
    virtual bool EndOfStreamReached(void);

    // Configuration						  
    virtual bool Listen(int port);

protected:
    // Playback control
    virtual int  Xine_Control(const char *cmd);  
    virtual int  Xine_Control_Sync(const char *cmd);  

protected:

    // Handling of messages from client(s) 

    void Handle_Discovery_Broadcast(void);
    void Handle_ClientConnected(int fd);

    void Read_Control(int cli);
    void Handle_Control(int cli, const char *cmd);

    void Handle_Control_PIPE(int cli, const char *arg);
    void Handle_Control_RTP(int cli, const char *arg);
    void Handle_Control_UDP(int cli, const char *arg);
    void Handle_Control_DATA(int cli, const char *arg);
    void Handle_Control_KEY(int cli, const char *arg);
    void Handle_Control_UDP_RESEND(int cli, const char *arg);
    void Handle_Control_CONFIG(int cli);
    void Handle_Control_GRAB(int cli, const char *arg);
    void Handle_Control_CONTROL(int cli, const char *arg);
    void Handle_Control_HTTP(int cli, const char *arg);
    void Handle_Control_RTSP(int cli, const char *arg);

    void CloseDataConnection(int cli);
    void CloseConnection(int cli);

protected:

    // Data

    int  m_Port;
    int  m_ServerId;

    int  fd_listen;
    int  fd_discovery;

    cxSocket fd_control[MAXCLIENTS];
    int      fd_data[MAXCLIENTS];

    int  m_OsdTimeouts[MAXCLIENTS];
    char m_CtrlBuf[MAXCLIENTS][1024+1];
    int  m_CtrlBufPos[MAXCLIENTS];

    int  m_MasterCli;
    bool m_bUdp[MAXCLIENTS];
    int  m_ConnType[MAXCLIENTS];
    bool m_bMulticast[MAXCLIENTS];
    bool m_bConfigOk[MAXCLIENTS];
    int  m_iMulticastMask; // bit [cli] is 1 or 0. 1 == multicast in use.
    int  m_iUdpFlowMask;   // bit [cli] is 1 or 0. 1 == buffer full.

    cString m_PipesDir;

    cBackgroundWriterI *m_Writer[MAXCLIENTS]; // buffered output (pipe/tcp/http)
    cConnState        *m_State[MAXCLIENTS];  // connection state (http/rtsp)
    cUdpScheduler     *m_Scheduler;
    bool               m_Master;
    cStcFuture        *m_StcFuture;
    cCmdFutures       *m_Futures;

    int  m_Token;
    int  AllocToken(void);
    bool HasClients(void);
};

#endif // __XINELIB_FRONTEND_SVR_H
