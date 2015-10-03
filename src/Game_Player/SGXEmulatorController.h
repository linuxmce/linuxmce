/**
 * SGXEmulatorController - Support for NEC SuperGrafx
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef SGXEMULATORCONTROLLER_H
#define SGXEMULATORCONTROLLER_H

#include "PCEngineEmulatorController.h"
#include "SGXEmulatorModel.h"

using namespace std;

namespace DCE
{
  class SGXEmulatorController : public PCEngineEmulatorController
  {
  public:
    SGXEmulatorController(Game_Player *pGame_Player, SGXEmulatorModel *pEmulatorModel); // ctor
    virtual ~SGXEmulatorController(); // dtor

    SGXEmulatorModel *m_pEmulatorModel;

  };

}

#endif
