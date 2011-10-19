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

#include <map>
#include <string>

#include <X11/Xlib.h>


namespace DCE
{

  class X11EmulatorModel : public EmulatorModel
  {
  public:
    X11EmulatorModel();
    ~X11EmulatorModel();
    Display *m_pDisplay;
    Window m_iWindowId;

    map<string, pair<int, int> > m_mapActionsToKeysyms;
    pair<int, int> m_mapActionsToKeysyms_Find(string sAction) 
    {
      map<string, pair<int, int> >::iterator it = m_mapActionsToKeysyms.find(sAction);
      return it == m_mapActionsToKeysyms.end() ? make_pair(0,0) : (*it).second;
    }
    bool m_mapParameters_Exists(string sAction)
    {
      map<string, pair<int, int> >::iterator it = m_mapActionsToKeysyms.find(sAction);
      return it != m_mapActionsToKeysyms.end();
    }
  protected:
    virtual void initializeActionstoKeysyms() = 0; // subclass and set key mappings.
  private:
  };

}

#endif
