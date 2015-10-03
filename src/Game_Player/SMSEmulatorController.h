/**
 * SMSEmulatorController - Support for SEGA Master System
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef SMSEMULATORCONTROLLER_H
#define SMSEMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "SMSEmulatorModel.h"

using namespace std;

namespace DCE
{
  class SMSEmulatorController : public MAMEEmulatorController
  {
  public:
    SMSEmulatorController(Game_Player *pGame_Player, SMSEmulatorModel *pEmulatorModel); // ctor
    virtual ~SMSEmulatorController(); // dtor

    SMSEmulatorModel *m_pEmulatorModel;

  };

}

#endif
