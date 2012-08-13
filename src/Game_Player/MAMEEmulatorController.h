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
    string getRomFromSlot();
    string getRomPathFromSlot();
  };
}

#endif
