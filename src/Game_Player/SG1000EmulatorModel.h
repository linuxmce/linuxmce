/**
 * SG1000EmulatorModel - Sega SG1000 support
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef SG1000EMULATORMODEL_H
#define SG1000EMULATORMODEL_H

#include "MAMEEmulatorModel.h"

namespace DCE
{
  class SG1000EmulatorModel : public MAMEEmulatorModel
  {
  public:
    SG1000EmulatorModel();
    virtual ~SG1000EmulatorModel();

  };
}

#endif
