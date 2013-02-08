/**
 * 
 * C64EmulatorModel - C-64 Emulation Support
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
    ~C64EmulatorModel();
  };
}

#endif
