/**
 * INotifyEmulatorModel - Creates a simple model that can
 * be used to track state for emulators controlled by the
 * kernel inotify method.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#ifndef INOTIFYEMULATORMODEL_H
#define INOTIFYEMULATORMODEL_H

#define INOTIFY_SEND_FILE "send"
#define INOTIFY_RECV_FILE "recv"
#define INOTIFY_PATH "/run/Game_Player/"

using namespace std;

#include "EmulatorModel.h"

#include <map>
#include <string>

namespace DCE
{

  class INotifyEmulatorModel : public EmulatorModel
  {
  public:
    INotifyEmulatorModel();
    ~INotifyEmulatorModel();
    string m_sLastReceivedCommand;

    map<string, pair<char, bool> > m_mapActionsToCommandBytes;
    virtual pair<char, bool> m_mapActionsToCommandBytes_Find(string sAction)
    {
      map<string, pair<char, bool> >::iterator it = m_mapActionsToCommandBytes.find(sAction);
      return it == m_mapActionsToCommandBytes.end() ? make_pair((char)0,false) : it->second;
    }

    virtual bool m_mapActionstoCommandBytes_Exists(string sAction)
    {
      map<string, pair<char, bool> >::iterator it = m_mapActionsToCommandBytes.find(sAction);
      return it != m_mapActionsToCommandBytes.end();
    }

  };

}

#endif
