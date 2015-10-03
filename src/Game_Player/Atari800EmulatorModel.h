/** 
 * Atari800EmulatorModel - Model for Atari 8-bit machines.
 */

#ifndef ATARI800EMULATORMODEL_H
#define ATARI800EMULATORMODEL_H

#include "X11EmulatorModel.h"

#define ATARI800_CONFIG_FILE_TEMPLATE "/usr/pluto/templates/atari800.cfg.template"
#define ATARI800_CONFIG_FILE "/root/.atari800.cfg"

namespace DCE
{
  class Atari800EmulatorModel : public X11EmulatorModel
  {
  public:
    Atari800EmulatorModel();
    virtual ~Atari800EmulatorModel();

    bool m_bCapsLockPressed;

  protected:
    
    virtual void initializeActionstoKeysyms(); // set key mappings for ATARI800.
    virtual void initializeButtontoKeysyms(); // keyboard mappings
    virtual bool updateConfig();
    virtual string getVideoAccelleration();
  };
}

#endif
