/**
 * 
 * VIC20EmulatorController - Support for VIC20
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef VIC20EMULATORCONTROLLER_H
#define VIC20EMULATORCONTROLLER_H

#include "VICEEmulatorController.h"
#include "VIC20EmulatorModel.h"

using namespace std;

namespace DCE
{
  class VIC20EmulatorController : public VICEEmulatorController
  {
  public:
    VIC20EmulatorController(Game_Player *pGame_Player, VIC20EmulatorModel *pEmulatorModel); // ctor
    virtual ~VIC20EmulatorController(); // dtor

    VIC20EmulatorModel *m_pEmulatorModel;

    virtual void insertMediaNamed(string sMediaFile, string sSlot);
  };
}

#endif
