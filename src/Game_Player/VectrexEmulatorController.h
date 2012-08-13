/**
 * VectrexEmulatorController - Support for GCE Vectrex
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef VECTREXEMULATORCONTROLLER_H
#define VECTREXEMULATORCONTROLLER_H

#include "MESSEmulatorController.h"
#include "VectrexEmulatorModel.h"

using namespace std;

namespace DCE
{
  class VectrexEmulatorController : public MESSEmulatorController
  {
  public:
    VectrexEmulatorController(Game_Player *pGame_Player, VectrexEmulatorModel *pEmulatorModel); // ctor
    ~VectrexEmulatorController(); // dtor

    VectrexEmulatorModel *m_pEmulatorModel;

  };

}


#endif
