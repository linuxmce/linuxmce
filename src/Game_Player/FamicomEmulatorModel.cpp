/**
 * FamicomEmulatorModel - Nintendo Family Computer support
 *
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "FamicomEmulatorModel.h"

namespace DCE
{

  FamicomEmulatorModel::FamicomEmulatorModel() : NESEmulatorModel()
  {
    m_sSystemName = "famicom";
  }

  FamicomEmulatorModel::~FamicomEmulatorModel()
  {
  
  }
}
