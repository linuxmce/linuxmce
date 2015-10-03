/**
 * VICEEmulatorModel - Model for VICE based emulations (e.g. C64 and VIC-20)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef VICEEMULATORMODEL_H
#define VICEEMULATORMODEL_H

#include "INotifyEmulatorModel.h"

#define VICE_CONFIG_FILE_TEMPLATE "/usr/pluto/templates/sdl-vicerc.template"
#define VICE_CONFIG_FILE "/root/.vice/sdl-vicerc"

namespace DCE
{
  class VICEEmulatorModel : public INotifyEmulatorModel
  {
  public:
    VICEEmulatorModel();
    virtual ~VICEEmulatorModel();

  protected:
    virtual void initializeActionstoKeysyms(); // set key mappings for VICE.
    virtual void initializeButtontoKeysyms();  // keyboard mappings
    virtual bool updateConfig();
    virtual string getVideoAccelleration();
  };
}

#endif

