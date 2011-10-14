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

namespace DCE
{

  class X11EmulatorModel : public EmulatorModel
  {
  public:
    X11EmulatorModel();
    ~X11EmulatorModel();
  protected:
    map<string, int> m_mapActionsToKeysyms;
    int m_mapParameters_Find(string sAction) 
    {
      map<string, int>::iterator it = m_mapActionsToKeysyms.find(sAction);
      return it == m_mapActionsToKeysyms.end() ? 0 : (*it).second;
    }
    bool m_mapParameters_Exists(string sAction)
    {
      map<string, int>::iterator it = m_mapActionsToKeysyms.find(sAction);
      return it != m_mapActionsToKeysyms.end();
    }
    virtual void initializeActionstoKeysyms() = 0; // subclass and set key mappings.
  private:
  };

}

#endif
