/**
 * MESSEmulatorController - Support to control the MESS emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef MESSEMULATORCONTROLLER_H
#define MESSEMULATORCONTROLLER_H

#include "X11EmulatorController.h"
#include "MESSEmulatorModel.h"

using namespace std;

namespace DCE
{
  class MESSEmulatorController : public X11EmulatorController
  {
  public:
    MESSEmulatorController(Game_Player *pGame_Player, MESSEmulatorModel *pEmulatorModel); // ctor
    ~MESSEmulatorController(); // dtor

    MESSEmulatorModel *m_pEmulatorModel;
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
    virtual void setSystemName(string sSystemName);
    virtual string getSystemName();
  private:
    string getRomFromSlot();
    string getRomPathFromSlot();
    string getSlotTypeForFilename(string sFilename);
    string getSystemFromSlot();
    string getSlotsAndRoms();
  };
}

#endif
