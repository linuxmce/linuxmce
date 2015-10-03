/**
 * IntellivisionEmulatorController - Support for Intellivision
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef INTELLIVISIONEMULATORCONTROLLER_H
#define INTELLIVISIONEMULATORCONTROLLER_H

#include "IntellivisionEmulatorModel.h"
#include "SecondGenEmulatorController.h"

using namespace std;

namespace DCE
{
  class IntellivisionEmulatorController : public SecondGenEmulatorController
  {
  public:
    IntellivisionEmulatorController(Game_Player *pGame_Player, IntellivisionEmulatorModel *pEmulatorModel); // ctor
    virtual ~IntellivisionEmulatorController(); // dtor

    IntellivisionEmulatorModel *m_pEmulatorModel;

  };

}


#endif
