/** 
 * MAMEEmulatorEngine - a model class supporting MAME.
 * 
 * This one is special, because no other emulations are decended from it.
 * It handles only one nebulous system type, an arcade game of some sort.
 * And all the arcade games are known within the system.
 * 
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "EmulatorEngine.h"

#ifndef MAMEEMULATORENGINE_H
#define MAMEEMULATORENGINE_H

namespace DCE
{
  class MAMEEmulatorEngine : EmulatorEngine
    {
    private:
      string getVideoDriver();
      void setupDefaultControls();
    protected:
      bool updateINI(); // Update the /root/.mame/mame.ini.
      map<string, pair<int, int> > m_mapKeysToPorts;

      // Functions to deal with m_mapKeysToPorts - mapping MAME input ports to XKeysyms.
      pair<int, int> m_mapKeysToPorts_Find(string key) { map<string,pair<int, int> >::iterator it = m_mapKeysToPorts.find( key ); return it == m_mapKeysToPorts.end() ? make_pair(0,0) : (*it).second; }
      bool m_mapKeysToPorts_Exists(string key) { map<string,pair<int, int> >::iterator it = m_mapKeysToPorts.find( key ); return it != m_mapKeysToPorts.end(); }

    public:
      MAMEEmulatorEngine(); // ctor
      ~MAMEEmulatorEngine(); // dtor
      virtual bool setMedia(string sMediaFilePath) { m_sMediaFileName = sMediaFilePath; return true; }
      virtual bool setup(); 
      virtual bool updateConfig();

    };
}

#endif /* MAMEEMULATORENGINE_H */
