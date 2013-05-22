/** 
 * X11EmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * an emulator controlled via X11 keys and events
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#ifndef ATARI800EMULATORMODEL_H
#define ATARI800EMULATORMODEL_H

#include "X11EmulatorModel.h"
#include "EmulatorModel.h"

#define ATARI800_CONFIG_FILE_TEMPLATE "/usr/pluto/templates/atari800.cfg.template"
#define ATARI800_CONFIG_FILE "/root/.atari800.cfg.template"

namespace DCE
{
  class Atari800EmulatorModel : public X11EmulatorModel
  {
  public:
    Atari800EmulatorModel();
    ~Atari800EmulatorModel();

  protected:

    virtual void initializeActionstoKeysyms(); // set key mappings for ATARI800.
    virtual bool updateConfig();
    virtual string getVideoAccelleration();
    
  private:
  };
}

#endif
