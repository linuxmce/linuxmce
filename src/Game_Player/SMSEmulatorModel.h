/**
 * SMSEmulatorModel - Sega Master System support
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef SMSEMULATORMODEL_H
#define SMSEMULATORMODEL_H

#include "MAMEEmulatorModel.h"

namespace DCE
{
  class SMSEmulatorModel : public MAMEEmulatorModel
  {
  public:
    SMSEmulatorModel();
    virtual ~SMSEmulatorModel();

  };
}

#endif
