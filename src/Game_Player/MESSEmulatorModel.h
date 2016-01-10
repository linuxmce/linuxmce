/** 
 * MESSEmulatorModel - Base configuration model for MESS based emulators
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#ifndef MESSEMULATORMODEL_H
#define MESSEMULATORMODEL_H

#include "X11EmulatorModel.h"
#include "EmulatorModel.h"

#define MESS_CONFIG_FILE_TEMPLATE "/usr/pluto/templates/mess.ini.template"
#define MESS_CONFIG_FILE "/root/.mess/mess.ini"

namespace DCE
{
  class MESSEmulatorModel : public X11EmulatorModel
  {
  private:
  public:
    MESSEmulatorModel();
    virtual ~MESSEmulatorModel();

    string m_sSystemName;   // Emulator System name
    string m_sState;
    string m_sRomPath;
    bool m_bIsComputer; // Is machine a computer, and therefore needs partial keyboard toggle?

  protected:

    virtual void initializeActionstoKeysyms(); // set key mappings for MESS.
    virtual bool updateConfig();
    virtual string getVideoAccelleration();
    virtual string getResolution();
    
  };
}

#endif
