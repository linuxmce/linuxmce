/**
 * A7800EmulatorController - Support for Atari 7800
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef A7800EMULATORCONTROLLER_H
#define A7800EMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "A7800EmulatorModel.h"

using namespace std;

namespace DCE
{
  class A7800EmulatorController : public MAMEEmulatorController
  {
  public:
    A7800EmulatorController(Game_Player *pGame_Player, A7800EmulatorModel *pEmulatorModel); // ctor
    virtual ~A7800EmulatorController(); // dtor

    A7800EmulatorModel *m_pEmulatorModel;

  };

}


#endif
