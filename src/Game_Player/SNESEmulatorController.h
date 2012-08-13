/**
 * SNESEmulatorController - Support for Super Nintendo Entertainment System
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef SNESEMULATORCONTROLLER_H
#define SNESEMULATORCONTROLLER_H

#include "MESSEmulatorController.h"
#include "SNESEmulatorModel.h"

using namespace std;

namespace DCE
{
  class SNESEmulatorController : public MESSEmulatorController
  {
  public:
    SNESEmulatorController(Game_Player *pGame_Player, SNESEmulatorModel *pEmulatorModel); // ctor
    ~SNESEmulatorController(); // dtor

    SNESEmulatorModel *m_pEmulatorModel;

  };

}


#endif
