/**
 * SMSEmulatorController - Support for SEGA Master System
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef SMSEMULATORCONTROLLER_H
#define SMSEMULATORCONTROLLER_H

#include "MESSEmulatorController.h"
#include "SMSEmulatorModel.h"

using namespace std;

namespace DCE
{
  class SMSEmulatorController : public MESSEmulatorController
  {
  public:
    SMSEmulatorController(Game_Player *pGame_Player, SMSEmulatorModel *pEmulatorModel); // ctor
    ~SMSEmulatorController(); // dtor

    SMSEmulatorModel *m_pEmulatorModel;

  };

}

#endif
