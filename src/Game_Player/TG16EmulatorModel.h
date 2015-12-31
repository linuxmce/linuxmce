/**
 * TG16EmulatorModel - NEC TurboGrafx-16 Support
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef TG16EMULATORMODEL_H
#define TG16EMULATORMODEL_H

#include "PCEngineEmulatorModel.h"

namespace DCE
{
  class TG16EmulatorModel : public PCEngineEmulatorModel
  {
  public:
    TG16EmulatorModel();
    virtual ~TG16EmulatorModel();

  };
}

#endif
