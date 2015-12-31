/**
 * ColecoVisionEmulatorController - Support for ColecoVision
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef COLECOVISIONEMULATORCONTROLLER_H
#define COLECOVISIONEMULATORCONTROLLER_H

#include "ColecoVisionEmulatorModel.h"
#include "SecondGenEmulatorController.h"

using namespace std;

namespace DCE
{
  class ColecoVisionEmulatorController : public SecondGenEmulatorController
  {
  public:
    ColecoVisionEmulatorController(Game_Player *pGame_Player, ColecoVisionEmulatorModel *pEmulatorModel); // ctor
    virtual ~ColecoVisionEmulatorController(); // dtor

    ColecoVisionEmulatorModel *m_pEmulatorModel;

  };

}

#endif
