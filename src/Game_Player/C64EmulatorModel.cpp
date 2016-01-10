/**
 * 
 * C64EmulatorModel - C-64 Emulation Support
 * 
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "C64EmulatorModel.h"

namespace DCE
{
  C64EmulatorModel::C64EmulatorModel() : VICEEmulatorModel()
  {
    m_sEmulatorBinary += "x64";
    m_sProcessName = "x64";
  }

  C64EmulatorModel::~C64EmulatorModel()
  {
  
  }

  bool C64EmulatorModel::updateConfig()
  {
    return VICEEmulatorModel::updateConfig();
  }

}
