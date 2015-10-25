/**
 * TI994AEmulatorController - Support for TI 99/4A
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef TI994AEMULATORCONTROLLER_H
#define TI994AEMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "TI994AEmulatorModel.h"

using namespace std;

namespace DCE
{
  class TI994AEmulatorController : public MAMEEmulatorController
  {
  public:
    TI994AEmulatorController(Game_Player *pGame_Player, TI994AEmulatorModel *pEmulatorModel); // ctor
    virtual ~TI994AEmulatorController(); // dtor

    TI994AEmulatorModel *m_pEmulatorModel;
    string getAdditionalOptions();

  };

}


#endif
