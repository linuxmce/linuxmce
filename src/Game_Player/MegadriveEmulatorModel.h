/**
 * MegadriveEmulatorModel - SEGA Mega Drive support
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef MEGADRIVEEMULATORMODEL_H
#define MEGADRIVEEMULATORMODEL_H

#include "GenesisEmulatorModel.h"

namespace DCE
{
  class MegadriveEmulatorModel : public GenesisEmulatorModel
  {
  public:
    MegadriveEmulatorModel();
    virtual ~MegadriveEmulatorModel();

  };
}

#endif
