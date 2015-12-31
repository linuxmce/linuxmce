/** 
 * X11EmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * an emulator controlled via X11 keys and events
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#ifndef X11EMULATORMODEL_H
#define X11EMULATORMODEL_H

using namespace std;
#include "EmulatorModel.h"
#include "VideoFrameGeometry.h"

#include <map>
#include <string>

#include <X11/Xlib.h>


namespace DCE
{

  class X11EmulatorModel : public EmulatorModel
  {
  public:
    X11EmulatorModel();
    virtual ~X11EmulatorModel();
    Display *m_pDisplay;
    Window m_iWindowId;
    Window m_iPreviousWindowId;
    string m_sWindowName;
    int m_iEventSerialNum;

    // The screen geometry of the TV connected to media director.
    int m_iScreenWidth, m_iScreenHeight, m_iRefreshRate;

    enum eKeyModifier 
    {
      SHIFT_LEFT     = 1,
      SHIFT_RIGHT    = 2,
      ALT_LEFT       = 4,
      ALT_RIGHT      = 8,
      CONTROL        = 16
    } m_eKeyModifier;

    int m_iCurrentKeyModifier;

    map<string, pair<int, int> > m_mapActionsToKeysyms;
    map<int, int> m_mapButtonToKeysyms;    

    virtual pair<int, int> m_mapActionsToKeysyms_Find(string sAction) 
    {
      map<string, pair<int, int> >::iterator it = m_mapActionsToKeysyms.find(sAction);
      return it == m_mapActionsToKeysyms.end() ? make_pair(0,0) : (*it).second;
    }
    virtual bool m_mapActionsToKeysyms_Exists(string sAction)
    {
      map<string, pair<int, int> >::iterator it = m_mapActionsToKeysyms.find(sAction);
      return it != m_mapActionsToKeysyms.end();
    }
    int m_mapButtonToKeysyms_Exists(int iPK_Button)
    {
      map<int, int>::iterator it = m_mapButtonToKeysyms.find(iPK_Button);
      return it != m_mapButtonToKeysyms.end();
    }
    int m_mapButtonToKeysyms_Find(int iPK_Button) 
    {
      map<int, int>::iterator it = m_mapButtonToKeysyms.find(iPK_Button);
      return it == m_mapButtonToKeysyms.end() ? 0 : (*it).second;
    }

    virtual void updateVideoFrameGeometry(long int iPK_Device, int iWidth, int iHeight);
    virtual pair<int, int> getVideoFrameGeometry(long int iPK_Device);
  protected:
    virtual void initializeActionstoKeysyms() = 0; // subclass and set key mappings.
    virtual bool updateConfig();
    virtual void initializeButtontoKeysyms();
  private:
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
