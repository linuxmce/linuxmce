/**
 * MAMEEmulatorController - Support to control the MAME emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef MAMEEMULATORCONTROLLER_H
#define MAMEEMULATORCONTROLLER_H

#include "INotifyEmulatorController.h"
#include "MAMEEmulatorModel.h"

using namespace std;

namespace DCE
{
  class MAMEEmulatorController : public INotifyEmulatorController
  {
  public:
    MAMEEmulatorController(Game_Player *pGame_Player, MAMEEmulatorModel *pEmulatorModel); // ctor
    virtual ~MAMEEmulatorController(); // dtor

    MAMEEmulatorModel *m_pEmulatorModel;
    virtual bool init();
    virtual bool run();
    virtual bool setSpeed(int iSpeed);
    virtual bool gotoMenu(int iMenu);
    virtual void setMediaPosition(string sMediaPosition);
    virtual bool saveState(string& sPosition, string& sText, bool bAutoSave=false, string sAutoSaveName="");
    virtual bool loadState(string sPosition);

    bool getRecordingFilename(string sPath, string &sFilename);
    virtual bool record();

  protected:
  private:
    virtual string getRomFromSlot();
    virtual string getRomPathFromSlot();
    virtual string getSlotsAndRoms();
  };
}

#endif
