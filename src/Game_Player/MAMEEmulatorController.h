/**
 * MAMEEmulatorController - Support to control the MAME emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef MAMEEMULATORCONTROLLER_H
#define MAMEEMULATORCONTROLLER_H

#include "X11EmulatorController.h"
#include "MAMEEmulatorModel.h"

using namespace std;

namespace DCE
{
  class MAMEEmulatorController : public X11EmulatorController
  {
  public:
    MAMEEmulatorController(Game_Player *pGame_Player, MAMEEmulatorModel *pEmulatorModel); // ctor
    ~MAMEEmulatorController(); // dtor

    MAMEEmulatorModel *m_pEmulatorModel;
 
  protected:
  private:
  };
}

#endif
