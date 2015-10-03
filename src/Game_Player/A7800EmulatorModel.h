/**
 * A7800EmulatorModel - Atari 7800 Game support
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef A7800EMULATORMODEL_H
#define A7800EMULATORMODEL_H

#include "A2600EmulatorModel.h"

namespace DCE
{
  class A7800EmulatorModel : public A2600EmulatorModel
  {
  public:
    A7800EmulatorModel();
    virtual ~A7800EmulatorModel();

  };
}

#endif
