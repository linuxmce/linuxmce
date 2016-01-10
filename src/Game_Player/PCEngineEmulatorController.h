/**
 * PCEngineEmulatorController - Support for NEC PC-Engine
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef PCENGINEEMULATORCONTROLLER_H
#define PCENGINEEMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "PCEngineEmulatorModel.h"

using namespace std;

namespace DCE
{
  class PCEngineEmulatorController : public MAMEEmulatorController
  {
  public:
    PCEngineEmulatorController(Game_Player *pGame_Player, PCEngineEmulatorModel *pEmulatorModel); // ctor
    virtual ~PCEngineEmulatorController(); // dtor

    PCEngineEmulatorModel *m_pEmulatorModel;

  };

}


#endif
