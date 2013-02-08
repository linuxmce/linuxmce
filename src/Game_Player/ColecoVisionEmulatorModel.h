/**
 * ColecoVisionEmulatorModel - Based on SecondGenEmulatorModel
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */
#ifndef COLECOVISIONEMULATORMODEL_H
#define COLECOVISIONEMULATORMODEL_H

#include "SecondGenEmulatorModel.h"

namespace DCE
{
  class ColecoVisionEmulatorModel : public SecondGenEmulatorModel
  {
  public:
    ColecoVisionEmulatorModel();
    ~ColecoVisionEmulatorModel();

    virtual void initializeActionstoKeysyms();
    virtual void initializeButtontoKeysyms();

  };
}

#endif
