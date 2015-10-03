/**
 * TI994AEmulatorModel - TI 99/4A support
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef TI994AEMULATORMODEL_H
#define TI994AEMULATORMODEL_H

#include "MESSEmulatorModel.h"

namespace DCE
{
  class TI994AEmulatorModel : public MESSEmulatorModel
  {
  public:
    TI994AEmulatorModel();
    virtual ~TI994AEmulatorModel();

  };
}

#endif
