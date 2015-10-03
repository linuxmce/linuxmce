/**
 * A2600EmulatorController - Support for Atari 2600
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef A2600EMULATORCONTROLLER_H
#define A2600EMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "A2600EmulatorModel.h"

using namespace std;

namespace DCE
{
  class A2600EmulatorController : public MAMEEmulatorController
  {
  public:
    A2600EmulatorController(Game_Player *pGame_Player, A2600EmulatorModel *pEmulatorModel); // ctor
    virtual ~A2600EmulatorController(); // dtor

    A2600EmulatorModel *m_pEmulatorModel;

  };

}


#endif
