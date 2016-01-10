/**
 * SG1000EmulatorController - Support for Nintendo Entertainment System
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef SG1000EMULATORCONTROLLER_H
#define SG1000EMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "SG1000EmulatorModel.h"

using namespace std;

namespace DCE
{
  class SG1000EmulatorController : public MAMEEmulatorController
  {
  public:
    SG1000EmulatorController(Game_Player *pGame_Player, SG1000EmulatorModel *pEmulatorModel); // ctor
    virtual ~SG1000EmulatorController(); // dtor

    SG1000EmulatorModel *m_pEmulatorModel;

  };

}

#endif
