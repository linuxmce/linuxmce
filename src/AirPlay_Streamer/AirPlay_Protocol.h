/**
 *
 * AirPlay Protocol
 *
 */

#ifndef AIRPLAYPROTOCOL_H
#define AIRPLAYPROTOCOL_H

#include "AirPlay_Streamer.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include <string>

#include <avahi-common/strlst.h>

namespace DCE
{

  class AirPlay_Streamer;

  class AirPlay_Protocol
  {
  
    friend class AirPlay_Streamer;

  private:
    int m_iPort;
    char *m_cServiceType;
    AvahiStringList *m_aslTXTRecords;
    
  protected:
    AirPlay_Streamer *m_pAirPlay_Streamer;

  public:
    AirPlay_Protocol(AirPlay_Streamer *pAirPlay_Streamer);
    ~AirPlay_Protocol();
  
    /** Control methods. */ 
    virtual bool init();
    virtual bool start();
    virtual bool stop();

    /** accessors */
    void Port_set(int iPort);
    int Port_get();
    void ServiceType_set(string sServiceType);
    char* ServiceType_get();
    void TXTRecord_Add(string sVal);
    AvahiStringList* TXTRecords_get();

  };

}

#endif
