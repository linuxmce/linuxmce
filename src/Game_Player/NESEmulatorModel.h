/**
 * NESEmulatorModel - Nintendo Entertainment System support
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef NESEMULATORMODEL_H
#define NESEMULATORMODEL_H

#include "MAMEEmulatorModel.h"

namespace DCE
{
  class NESEmulatorModel : public MAMEEmulatorModel
  {
  public:
    NESEmulatorModel();
    virtual ~NESEmulatorModel();

  };
}

#endif
