/**
 * A5200EmulatorController - Support for A5200
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef A5200EMULATORCONTROLLER_H
#define A5200EMULATORCONTROLLER_H

#include "A5200EmulatorModel.h"
#include "SecondGenEmulatorController.h"

using namespace std;

namespace DCE
{
  class A5200EmulatorController : public SecondGenEmulatorController
  {
  public:
    A5200EmulatorController(Game_Player *pGame_Player, A5200EmulatorModel *pEmulatorModel); // ctor
    virtual ~A5200EmulatorController(); // dtor

    A5200EmulatorModel *m_pEmulatorModel;

  };

}


#endif
