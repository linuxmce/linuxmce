/**
 * GenesisEmulatorController - Support for SEGA Genesis
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef GENESISEMULATORCONTROLLER_H
#define GENESISEMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "GenesisEmulatorModel.h"

using namespace std;

namespace DCE
{
  class GenesisEmulatorController : public MAMEEmulatorController
  {
  public:
    GenesisEmulatorController(Game_Player *pGame_Player, GenesisEmulatorModel *pEmulatorModel); // ctor
    virtual ~GenesisEmulatorController(); // dtor

    GenesisEmulatorModel *m_pEmulatorModel;

  };

}


#endif
