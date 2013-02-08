/**
 * VICEEmulatorController - Support for VICE based Emulators
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef VICEEMULATORCONTROLLER_H
#define VICEEMULATORCONTROLLER_H

#include "X11EmulatorController.h"
#include "VICEEmulatorModel.h"

using namespace std;

namespace DCE
{
  class VICEEmulatorController : public X11EmulatorController
  {
  public:
    VICEEmulatorController(Game_Player *pGame_Player, VICEEmulatorModel *pEmulatorModel);  // ctor
    ~VICEEmulatorController(); // dtor

    VICEEmulatorModel *m_pEmulatorModel;
    virtual bool init();
    virtual bool run();
    virtual bool setSpeed(int iSpeed);
    virtual bool gotoMenu(int iMenu);
    virtual void setMediaPosition(string sMediaPosition);
    virtual bool saveState(string& sPosition, string& sText, bool bAutoSave=false, string sAutoSaveName="");
    virtual bool loadState(string sPosition);
    virtual void insertMediaNamed(string sMediaFile, string sSlot);
    
    bool getRecordingFilename(string sPath, string &sFilename);
    virtual bool record();

  protected:
  private:
    virtual string getSlotsAndRoms();
    
  };
}

#endif
