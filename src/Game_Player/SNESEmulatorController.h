/**
 * SNESEmulatorController - Support for Super Nintendo Entertainment System
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef SNESEMULATORCONTROLLER_H
#define SNESEMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "SNESEmulatorModel.h"

using namespace std;

namespace DCE
{
  class SNESEmulatorController : public MAMEEmulatorController
  {
  public:
    SNESEmulatorController(Game_Player *pGame_Player, SNESEmulatorModel *pEmulatorModel); // ctor
    virtual ~SNESEmulatorController(); // dtor

    SNESEmulatorModel *m_pEmulatorModel;

  };

}


#endif
