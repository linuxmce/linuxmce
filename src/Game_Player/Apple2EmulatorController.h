/**
 * Apple2EmulatorController - Support for Apple //e
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef APPLE2EMULATORCONTROLLER_H
#define APPLE2EMULATORCONTROLLER_H

#include "MESSEmulatorController.h"
#include "Apple2EmulatorModel.h"

using namespace std;

namespace DCE
{
  class Apple2EmulatorController : public MESSEmulatorController
  {
  public:
    Apple2EmulatorController(Game_Player *pGame_Player, Apple2EmulatorModel *pEmulatorModel); // ctor
    virtual ~Apple2EmulatorController(); // dtor

    Apple2EmulatorModel *m_pEmulatorModel;

    virtual void insertMediaNamed(string sMediaFile, string sSlot);
    virtual string getRomPathFromSlot();

  };

}


#endif
