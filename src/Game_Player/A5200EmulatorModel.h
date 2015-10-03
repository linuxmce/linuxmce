/**
 * A5200EmulatorModel - Based on SecondGenEmulatorModel
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */
#ifndef A5200EMULATORMODEL_H
#define A5200EMULATORMODEL_H

#include "SecondGenEmulatorModel.h"

namespace DCE
{
  class A5200EmulatorModel : public SecondGenEmulatorModel
  {
  public:
    A5200EmulatorModel();
    virtual ~A5200EmulatorModel();

    virtual void initializeActionstoKeysyms();
    virtual void initializeButtontoKeysyms();

  };
}

#endif
