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
#include "VideoFrameGeometry.h"

#include <map>
#include <string>

namespace DCE
{

  class INotifyEmulatorModel : public EmulatorModel
  {
  public:
    INotifyEmulatorModel();
    virtual ~INotifyEmulatorModel();
    string m_sLastReceivedCommand;
    string m_sStateDir;
    int m_iDoneDelay; // Delay before sending 0x00 to the command socket.
    int m_iScreenWidth; // Screen width, e.g. 1920
    int m_iScreenHeight; // Screen height, e.g. 1080
    int m_iRefreshRate; // Screen refresh rate, e.g. 60Hz

    map<string, pair<unsigned char, bool> > m_mapActionsToCommandBytes;
    virtual pair<unsigned char, bool> m_mapActionsToCommandBytes_Find(string sAction)
    {
      map<string, pair<unsigned char, bool> >::iterator it = m_mapActionsToCommandBytes.find(sAction);
      return it == m_mapActionsToCommandBytes.end() ? make_pair((unsigned char)0,false) : it->second;
    }

    virtual bool m_mapActionstoCommandBytes_Exists(string sAction)
    {
      map<string, pair<unsigned char, bool> >::iterator it = m_mapActionsToCommandBytes.find(sAction);
      return it != m_mapActionsToCommandBytes.end();
    }

    map<int, string> m_mapButtonsToActions;
    virtual string m_mapButtonsToActions_Find(int iPK_Button)
    {
      map<int, string>::iterator it = m_mapButtonsToActions.find(iPK_Button);
      return it == m_mapButtonsToActions.end() ? "":it->second;
    }

    virtual bool m_mapButtonsToActions_Exists(int iPK_Button)
    {
      map<int, string>::iterator it = m_mapButtonsToActions.find(iPK_Button);
      return it != m_mapButtonsToActions.end();
    }

    virtual void updateVideoFrameGeometry(long int iPK_Device, int iWidth, int iHeight);
    virtual pair<int, int> getVideoFrameGeometry(long int iPK_Device);

    map<long int, VideoFrameGeometry *> m_mapVideoFrameGeometry;

    bool m_mapVideoFrameGeometry_Exists(long int iPK_Device)
    {
      map<long int, VideoFrameGeometry *>::iterator it = m_mapVideoFrameGeometry.find(iPK_Device);
      return it != m_mapVideoFrameGeometry.end();
    }

    VideoFrameGeometry* m_mapVideoFrameGeometry_Find(long int iPK_Device)
    {
      map<long int, VideoFrameGeometry *>::iterator it = m_mapVideoFrameGeometry.find(iPK_Device);
      return it == m_mapVideoFrameGeometry.end() ? NULL : (*it).second;
    }

  };

}

#endif
