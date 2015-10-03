/**
 * 
 * VIC20EmulatorModel - VIC-20 Emulation Support
 * 
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "VIC20EmulatorModel.h"

namespace DCE
{
  VIC20EmulatorModel::VIC20EmulatorModel() : VICEEmulatorModel()
  {
    m_sEmulatorBinary += "xvic";
    m_sProcessName = "xvic";
  }

  VIC20EmulatorModel::~VIC20EmulatorModel()
  {
  
  }

}
