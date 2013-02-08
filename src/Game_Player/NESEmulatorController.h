/**
 * NESEmulatorController - Support for Nintendo Entertainment System
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef NESEMULATORCONTROLLER_H
#define NESEMULATORCONTROLLER_H

#include "MESSEmulatorController.h"
#include "NESEmulatorModel.h"

using namespace std;

namespace DCE
{
  class NESEmulatorController : public MESSEmulatorController
  {
  public:
    NESEmulatorController(Game_Player *pGame_Player, NESEmulatorModel *pEmulatorModel); // ctor
    ~NESEmulatorController(); // dtor

    NESEmulatorModel *m_pEmulatorModel;

  };

}


#endif
