/** 
 * MESSEmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * an emulator controlled via X11 keys and events
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
  public:
    MESSEmulatorModel();
    ~MESSEmulatorModel();

    string m_sSystemName;   // Emulator System name
    
  protected:

    virtual void initializeActionstoKeysyms(); // set key mappings for MESS.
    virtual bool updateConfig();
    virtual void updateTemplateVariables();
    virtual string getVideoAccelleration();
    
  private:
  };
}

#endif
