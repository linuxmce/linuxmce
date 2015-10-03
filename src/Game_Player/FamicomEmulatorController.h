/**
 * FamicomEmulatorController - Support for Nintendo Family Computer Support
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef FamicomEMULATORCONTROLLER_H
#define FamicomEMULATORCONTROLLER_H

#include "NESEmulatorController.h"
#include "FamicomEmulatorModel.h"

using namespace std;

namespace DCE
{
  class FamicomEmulatorController : public NESEmulatorController
  {
  public:
    FamicomEmulatorController(Game_Player *pGame_Player, FamicomEmulatorModel *pEmulatorModel); // ctor
    virtual ~FamicomEmulatorController(); // dtor

    virtual void insertMediaNamed(string sMediaFile, string sSlot);

    FamicomEmulatorModel *m_pEmulatorModel;
    virtual string getRomPathFromSlot();
    

  };

}

#endif
