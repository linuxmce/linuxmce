/**
 * VICEEmulatorController - Support for VICE based Emulators
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef VICEEMULATORCONTROLLER_H
#define VICEEMULATORCONTROLLER_H

#include "INotifyEmulatorController.h"
#include "VICEEmulatorModel.h"

using namespace std;

namespace DCE
{
  class VICEEmulatorController : public INotifyEmulatorController
  {
  public:
    VICEEmulatorController(Game_Player *pGame_Player, VICEEmulatorModel *pEmulatorModel);  // ctor
    virtual ~VICEEmulatorController(); // dtor

    VICEEmulatorModel *m_pEmulatorModel;
    virtual bool init();
    virtual bool run();
    virtual bool stop();
    virtual bool setSpeed(int iSpeed);
    virtual bool gotoMenu(int iMenu);
    virtual void insertMediaNamed(string sMediaFile, string sSlot);
    
    bool getRecordingFilename(string sPath, string &sFilename);
    virtual bool record();

  protected:
  private:
    virtual string getSlotsAndRoms();
    
  };
}

#endif
