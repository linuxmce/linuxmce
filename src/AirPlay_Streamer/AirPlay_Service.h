/**
 *
 * AirPlay Service
 *
 */

#ifndef AIRPLAYSERVICE_H
#define AIRPLAYSERVICE_H

#include "AirPlay_Streamer.h"
#include "AirPlay_Protocol.h"

#include "AirPlay_Protocol_Factory.h"

#include <avahi-client/client.h>
#include <avahi-client/publish.h>

#include <avahi-common/alternative.h>
#include <avahi-common/thread-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>

namespace DCE
{

  class AirPlay_Streamer;
  class AirPlay_Protocol;
  class AirPlay_Protocol_Factory;

  class AirPlay_Service
  {
  public:
    AirPlay_Service(AirPlay_Streamer *pAirPlay_Streamer);
    ~AirPlay_Service();

    AirPlay_Streamer *m_pAirPlay_Streamer;
    AvahiClient *m_pAvahi_Client;
    AvahiEntryGroup *m_pAvahi_Entry_Group;
    AvahiThreadedPoll *m_pAvahi_Threaded_Poll;
    AirPlay_Protocol_Factory *m_pAirPlay_Protocol_Factory;

    char *m_cName;
    char *m_cAirPlayName;
    string m_sMacAddress;

    bool init();
    bool start();
    void Name_set(string sName);
    void Name_AirPlay_set(string sName);
    void MacAddress_set(string sMacAddress);
    void sanityCheck();
  };

}

#endif /* AIRPLAYSERVICE_H */
