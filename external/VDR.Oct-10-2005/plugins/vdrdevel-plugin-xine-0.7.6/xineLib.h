
#ifndef __XINELIB_H
#define __XINELIB_H



#include "xineCommon.h"

#include <vdr/remote.h>

#if VDRVERSNUM < 10307

#include <vdr/osdbase.h>

#else

#include <vdr/osd.h>
#define  MAXNUMWINDOWS MAXOSDAREAS

#endif

#include <vdr/plugin.h>

#include "xineExternal.h"
#include "xineSettings.h"



namespace PluginXine
{

  class cXineLibEvents
  {
  protected:
    virtual ~cXineLibEvents() {}
  public:
    virtual void OnClientConnect() = 0;
    virtual void OnClientDisconnect() = 0;
    virtual void ReshowCurrentOSD(const int frameWidth, const int frameHeight) = 0;
  };


  
  class cXineLib;

// This class performs the adaption from Xine events
// to VDR's remote control events.
  class cXineRemote
    : public cRemote
    , private cThread 
  {
  private:
    bool m_active;
    cMutex m_activeMutex;
    cCondVar m_activeCondVar;
    const bool m_remoteOn;
    cXineLib *m_xineLib;
    
    virtual void Action (void);
    bool Ready(void) { return false; };
    bool isConnected();

  public:
    cXineRemote(const bool remoteOn);
    virtual ~cXineRemote();

    void setXineLib(cXineLib *const xineLib);
  };
  
 
  class cXineOsd;
  class cXineSettings;
  
  class cXineLib : public cThread
  {
  public:
    enum eNeedsScaling
    {
      no = 0
      , yes
      , shq
    };

    string m_fifoDir;
    string m_fifoNameControl;
    string m_fifoNameResult;
    string m_fifoNameRemote;
    string m_fifoNameStream;
    string m_fifoNameExtControl;
    string m_fifoNameExtResult;

  private:
    cPlugin *const m_plugin;
    cXineExternal m_external;
    
    void internalPaused(const bool paused);
    cMutex m_pausedMutex;
    cCondVar m_pausedCondVar;
    cMutex m_shutdownMutex;
    cCondVar m_shutdownCondVar;
      
    const cXineSettings &m_settings;
    
    bool m_osdWindowVisible[ MAXNUMWINDOWS ];

#if VDRVERSNUM >= 10307
    int m_osdWindowVideoLeft[ MAXNUMWINDOWS ];
    int m_osdWindowVideoTop[ MAXNUMWINDOWS ];
    int m_osdWindowVideoWidth[ MAXNUMWINDOWS ];
    int m_osdWindowVideoHeight[ MAXNUMWINDOWS ];
        
    int m_osdWindowColorsNum[ MAXNUMWINDOWS ];
    tColor m_osdWindowColors[ MAXNUMWINDOWS ][ MAXNUMCOLORS ];

    int m_scaledWindowColorsNum[ MAXNUMWINDOWS ];
    tColor m_scaledWindowColors[ MAXNUMWINDOWS ][ MAXNUMCOLORS ];

    cXineSettings::eOsdMode m_osdWindowMode[ MAXNUMWINDOWS ];
    int m_osdWindowGamma[ MAXNUMWINDOWS ];
    bool m_osdWindowSupportTransparency[ MAXNUMWINDOWS ];

    int m_osdWindowBufferSize[ MAXNUMWINDOWS ];
    tIndex *m_osdWindowBuffer[ MAXNUMWINDOWS ];
    
    tArea m_vidWin;
#endif    

    void cloneBitmap(const int windowNum, cBitmap *bitmap, int x1, int y1, int x2, int y2);
    bool bitmapDiffers(const int windowNum, cBitmap *bitmap, int &x1, int &y1, int &x2, int &y2); 
    
    bool m_osdFlushRequired;

  public:
    int getRemoteFD() const
    {
      return fd_remote;
    }
    
  private:    
    int fd_fifo0, fd_result, fd_control, fd_remote;
    cMutex m_ioMutex, m_dataMutex, m_disconnectMutex;
    cMutex &m_osdMutex;

    bool m_paused;
    bool m_frozen;
    bool m_ignore;
    bool m_shutdown;

    bool m_muted;
    int m_volume;
    int m_audioChannel;
    bool m_trickSpeedMode;

  public:    
    bool isConnected();
    void disconnect();

    int xwrite(int f, const void *b, int n);
    int xread(int f, void *b, int n);

  private:
    char m_noSignalStreamData[ 0x10000 ];
    long m_noSignalStreamSize;

    cXineLibEvents *m_eventSink;

    eNeedsScaling NeedsScaling(const int videoWidth, const int videoHeight);

  public:
    bool hasNoSignalStream() const
    {
      return m_noSignalStreamSize > 0;
    }
    
    virtual void Action(void);

    bool checkConnect();

    void enableExternal(const bool enable = true);
    void ExternalStreamFinished();

  public:
    cXineLib(cPlugin *const plugin, const cXineSettings &settings, cMutex &osdMutex, cXineRemote *const remote);
    virtual ~cXineLib();

    void SetEventSink(cXineLibEvents *const eventSink);
    
    bool Open();
    void Close();

    bool Poll(cPoller &Poller, int TimeoutMs = 0, const bool special = false);

#if VDRVERSNUM < 10307
    
    bool OpenWindow(cXineOsd *const xineOsd, cWindow *Window);
    void CommitWindow(cXineOsd *const xineOsd, cWindow *Window, const bool optimize = true);
    void ShowWindow(cXineOsd *const xineOsd, cWindow *Window);
    void HideWindow(cXineOsd *const xineOsd, cWindow *Window, bool Hide);
    void MoveWindow(cXineOsd *const xineOsd, cWindow *Window, int x, int y);
    void CloseWindow(cXineOsd *const xineOsd, cWindow *Window);
    void CloseWindow(cXineOsd *const xineOsd, int Window);

#else

    void SendWindow(cXineOsd *const xineOsd, const int windowNum, cBitmap *bitmap = 0, const int videoLeft = -1, const int videoTop = -1, const int videoWidth = -1, const int videoHeight = -1, const bool dontOptimize = false);
    
    bool execFuncOsdNew(const eNeedsScaling needsScaling, const int videoLeft, const int videoTop, const int videoWidth, const int videoHeight, int window, int x, int y, int width, int height);
    bool execFuncOsdDrawBitmap(const eNeedsScaling needsScaling, const int videoWidth, const int videoHeight, cXineOsd *const xineOsd, int window, cBitmap *const bitmap, int x, int y, int width, int height, int stride);

    void SetVideoWindow(tArea vidWin, const bool dontOptimize = false);
    
#endif    
    
    int execFuncStream1(const uchar *Data, int Length);
    int execFuncStream(const uchar *Data, int Length);
    bool execFuncStream0(const uchar *Data, int Length);
    bool execFuncStart();
    bool execFuncEnd();
    bool execFuncWait();
    bool execFuncSetup();
    bool execFuncDiscontinuity();

    bool execFuncOsdFlush();
    bool execFuncOsdNew(int window, int x, int y, int width, int height);
    bool execFuncOsdFree(int window);
    bool execFuncOsdShow(int window);
    bool execFuncOsdHide(int window);
    bool execFuncOsdSetPosition(int window, int x, int y);
    bool execFuncOsdDrawBitmap(int window, uint8_t *bitmap, int x, int y, int width, int height, int stride);

#if VDRVERSNUM < 10307
    bool execFuncSetColor(int window, int index, eDvbColor color);
    bool execFuncSetColor(int window, int index, eDvbColor *const colors, int numColors);
    eDvbColor filterAlpha(eDvbColor color);
#else
    bool execFuncSetColor(int window, int index, tColor color);
    bool execFuncSetColor(int window, int index, tColor *const colors, int numColors);
    tColor filterAlpha(tColor color);
#endif
    
    bool execFuncSetColor(int window, int index, int numColors, uint32_t *const colors);

    bool execFuncMute(bool mute = true);
    bool execFuncClear(int n);
    bool execFuncResetAudio();
    bool execFuncSelectAudio(int channel);
    bool execFuncFirstFrame();
    bool execFuncStillFrame();
    bool execFuncFlush(int TimeoutMs = -1, const bool justWait = false);
    bool execFuncSetVolume(int volume);
    bool execFuncSetSpeed(double speed);
    bool execFuncTrickSpeedMode(bool on);
    bool execFuncDelay(int usDelay);
    bool execFuncMetronom(int64_t pts, uint32_t flags = 0);
    bool execFuncNop();
    bool execFuncSetPrebuffer(int frames);
    bool execFuncGrabImage(const char *FileName, bool Jpeg, int Quality, int SizeX, int SizeY);
    bool execFuncGetPTS(int64_t &pts);
    bool execFuncVideoSize(int &videoLeft, int &videoTop, int &videoWidth, int &videoHeight);

    bool execFuncSetVideoWindow(int x, int y, int w, int h, int wRef, int hRef);

    bool execFuncPlayExternal(const char *const fileName = 0);

    bool showNoSignal();
    
    void pause(bool doPause = true);
    void freeze(bool doFreeze = true);
    void ignore(bool doIgnore = true);

    void flush();

    void ReshowCurrentOSD(const int frameWidth, const int frameHeight);

    void ReshowCurrentOSD()
    {
      ReshowCurrentOSD(-1, -1);
    }
  };

};



#endif //__XINELIB_H
