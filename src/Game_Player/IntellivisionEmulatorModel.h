/**
 * IntellivisionEmulatorModel - Based on SecondGenEmulatorModel
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */
#ifndef INTELLIVISIONEMULATORMODEL_H
#define INTELLIVISIONEMULATORMODEL_H

#include "SecondGenEmulatorModel.h"

#define INTV_CONFIG_FILE_TEMPLATE "/usr/pluto/templates/intv.cfg.template"
#define INTV_CONFIG_FILE "/home/mamedata/cfg/intv.cfg"

namespace DCE
{
  class IntellivisionEmulatorModel : public SecondGenEmulatorModel
  {
  public:
    IntellivisionEmulatorModel();
    virtual ~IntellivisionEmulatorModel();

    virtual bool updateConfig(); 
  };
}

#endif
