/**
 * FSUAEEmulatorModel - a model for Commodore-Amiga emulations supported by
 * the FS-UAE emulator
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef FSUAEEMULATORMODEL_H
#define FSUAEEMULATORMODEL_H

#include "X11EmulatorModel.h"

#define FSUAE_CONFIG_FILE_TEMPLATE "/usr/pluto/templates/fs-uae.config.template"
#define FSUAE_CONFIG_FILE "/root/FS-UAE/Configurations/Default"

namespace DCE
{
  class FSUAEEmulatorModel : public X11EmulatorModel
  {
  public:
    FSUAEEmulatorModel();
    virtual ~FSUAEEmulatorModel();

  protected:
    virtual void initializeActionstoKeysyms(); // set key mappings for VICE.
    virtual void initializeButtontoKeysyms();  // keyboard mappings
    virtual bool updateConfig();
    virtual string getVideoAccelleration();
  };
}

#endif
