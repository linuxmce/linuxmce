/**
 * NESEmulatorController - Support for Nintendo Entertainment System
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef NESEMULATORCONTROLLER_H
#define NESEMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "NESEmulatorModel.h"

using namespace std;

namespace DCE
{
  class NESEmulatorController : public MAMEEmulatorController
  {
  public:
    NESEmulatorController(Game_Player *pGame_Player, NESEmulatorModel *pEmulatorModel); // ctor
    virtual ~NESEmulatorController(); // dtor

    NESEmulatorModel *m_pEmulatorModel;

  };

}


#endif
