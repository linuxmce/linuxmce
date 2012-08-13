/**
 * IntellivisionEmulatorModel - Based on SecondGenEmulatorModel
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */
#ifndef INTELLIVISIONEMULATORMODEL_H
#define INTELLIVISIONEMULATORMODEL_H

#include "SecondGenEmulatorModel.h"

namespace DCE
{
  class IntellivisionEmulatorModel : public SecondGenEmulatorModel
  {
  public:
    IntellivisionEmulatorModel();
    ~IntellivisionEmulatorModel();

    virtual void initializeActionstoKeysyms();

  };
}

#endif
