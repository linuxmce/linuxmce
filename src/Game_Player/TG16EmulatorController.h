/**
 * TG16EmulatorController - Support for NEC TurboGrafx-16
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef TG16EMULATORCONTROLLER_H
#define TG16EMULATORCONTROLLER_H

#include "PCEngineEmulatorController.h"
#include "TG16EmulatorModel.h"

using namespace std;

namespace DCE
{
  class TG16EmulatorController : public PCEngineEmulatorController
  {
  public:
    TG16EmulatorController(Game_Player *pGame_Player, TG16EmulatorModel *pEmulatorModel); // ctor
    virtual ~TG16EmulatorController(); // dtor

    TG16EmulatorModel *m_pEmulatorModel;

  };

}


#endif
