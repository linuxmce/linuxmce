/**
 * 
 * C64EmulatorModel - Commodore 64 Emulation Support
 * 
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef C64EMULATORMODEL_H
#define C64EMULATORMODEL_H

#include "VICEEmulatorModel.h"

namespace DCE
{
  class C64EmulatorModel : public VICEEmulatorModel
  {
  public:
    C64EmulatorModel();
    virtual ~C64EmulatorModel();

    virtual bool updateConfig();

  };
}

#endif
