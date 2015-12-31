/**
 * 
 * C64EmulatorController - Support for C64
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef C64EMULATORCONTROLLER_H
#define C64EMULATORCONTROLLER_H

#include "VICEEmulatorController.h"
#include "C64EmulatorModel.h"

using namespace std;

namespace DCE
{
  class C64EmulatorController : public VICEEmulatorController
  {
  public:
    C64EmulatorController(Game_Player *pGame_Player, C64EmulatorModel *pEmulatorModel); // ctor
    virtual ~C64EmulatorController(); // dtor

    C64EmulatorModel *m_pEmulatorModel;

    virtual void insertMediaNamed(string sMediaFile, string sSlot);
  };
}

#endif
