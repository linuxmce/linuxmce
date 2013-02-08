
#ifndef __XINEEXTERNAL_H
#define __XINEEXTERNAL_H



#include "xineCommon.h"

#include <vdr/thread.h>



#define FIFO_NAME_EXT_CONTROL "/external.control"
#define FIFO_NAME_EXT_RESULT  "/external.result"

#define EXTERNAL_COMMAND_MAX_LEN (1000)



namespace PluginXine
{

  class cXineLib;
  
  class cXineExternal : public cThread
  {
    int m_fdControl;
    int m_fdResult;

    bool m_shutdown;
    cMutex m_shutdownMutex;
    cCondVar m_shutdownCondVar;

    bool m_connected;

    cXineLib *m_xineLib;

    bool m_enabled;
    
    char m_command[ EXTERNAL_COMMAND_MAX_LEN ];
    
    virtual void Action(void);

    bool checkConnect();
    bool isConnected();

    bool readCommand();

    void cmdPlay(const char *const mrl);
    bool writeResult(const char *result);

  public:
    cXineExternal();
    virtual ~cXineExternal();

    void setXineLib(cXineLib *const xineLib)
    {
      m_xineLib = xineLib;
    }

    void enable(const bool enable);
    void externalStreamFinished();
    void disconnect();
    
    void Start();
    void Stop();
  };

};



#endif //__XINEEXTERNAL_H
