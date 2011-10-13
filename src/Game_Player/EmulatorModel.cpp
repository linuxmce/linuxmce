/** 
 * EmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * emulator in question
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "EmulatorModel.h"

namespace DCE
{
  
  EmulatorModel::EmulatorModel()
  {
    m_bRunning_set(false);
  }

  EmulatorModel::~EmulatorModel()
  {
    m_bRunning_set(false);
  }

}
