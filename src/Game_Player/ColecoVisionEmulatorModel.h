/**
 * ColecoVisionEmulatorModel - Based on SecondGenEmulatorModel
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */
#ifndef COLECOVISIONEMULATORMODEL_H
#define COLECOVISIONEMULATORMODEL_H

#include "SecondGenEmulatorModel.h"

#define COLECO_CONFIG_FILE_TEMPLATE "/usr/pluto/templates/coleco.cfg.template"
#define COLECO_CONFIG_FILE "/usr/pluto/templates/coleco.cfg"

namespace DCE
{
  class ColecoVisionEmulatorModel : public SecondGenEmulatorModel
  {
  public:
    ColecoVisionEmulatorModel();
    virtual ~ColecoVisionEmulatorModel();

    virtual void initializeActionstoKeysyms();
    virtual void initializeButtontoKeysyms();
    virtual bool updateConfig();

  };
}

#endif
