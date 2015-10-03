/**
 * MegadriveEmulatorController - Support for SEGA Megadrive
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef MEGADRIVEEMULATORCONTROLLER_H
#define MEGADRIVEEMULATORCONTROLLER_H

#include "GenesisEmulatorController.h"
#include "MegadriveEmulatorModel.h"

using namespace std;

namespace DCE
{
  class MegadriveEmulatorController : public GenesisEmulatorController
  {
  public:
    MegadriveEmulatorController(Game_Player *pGame_Player, MegadriveEmulatorModel *pEmulatorModel); // ctor
    virtual ~MegadriveEmulatorController(); // dtor

    MegadriveEmulatorModel *m_pEmulatorModel;

  };

}


#endif
