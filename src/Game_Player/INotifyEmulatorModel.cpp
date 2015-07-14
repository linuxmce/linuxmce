/**
 * INotifyEmulatorModel - Creates a simple model that can
 * be used to track state for emulators controlled by the
 * kernel inotify method.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "INotifyEmulatorModel.h"
#include "pluto_main/Define_Button.h"

namespace DCE
{
  INotifyEmulatorModel::INotifyEmulatorModel()
    : EmulatorModel()
  {
    m_sLastReceivedCommand="";
  }

  INotifyEmulatorModel::~INotifyEmulatorModel()
  {
  
  }

}
