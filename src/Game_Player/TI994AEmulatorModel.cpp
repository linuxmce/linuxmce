/**
 * TI994AEmulatorModel - TI 99/4A System support
 *
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "TI994AEmulatorModel.h"

namespace DCE
{

  TI994AEmulatorModel::TI994AEmulatorModel() : MESSEmulatorModel()
  {
    m_sSystemName = "ti99_4a";
    m_bIsComputer=true;
  }

  TI994AEmulatorModel::~TI994AEmulatorModel()
  {
  
  }

}
