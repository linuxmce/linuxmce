/**
 * ATARI800EmulatorController - Support to control the ATARI800 emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef ATARI800EMULATORCONTROLLER_H
#define ATARI800EMULATORCONTROLLER_H

#include "X11EmulatorController.h"
#include "Atari800EmulatorModel.h"

using namespace std;

namespace DCE
{
  class Atari800EmulatorController : public X11EmulatorController
  {
  public:
    Atari800EmulatorController(Game_Player *pGame_Player, Atari800EmulatorModel *pEmulatorModel); // ctor
    virtual ~Atari800EmulatorController(); // dtor

    Atari800EmulatorModel *m_pEmulatorModel;
    virtual bool init();
    virtual bool run();
    virtual bool stop();
    virtual bool pause();
    virtual bool unpause();
    virtual bool getSnap(long int iPK_Device, int iWidth, int iHeight, char **pData, int& iData_Size);
    virtual bool setSpeed(int iSpeed);
    virtual bool gotoMenu(int iMenu);
    virtual void setMediaPosition(string sMediaPosition);
    virtual bool saveState(string& sPosition, string& sText, bool bAutoSave=false, string sAutoSaveName="");
    virtual bool loadState(string sPosition);
    virtual void insertMediaNamed(string sMediaFile, string sSlot);
    bool getRecordingFilename(string sPath, string &sFilename);
    virtual bool record();
    virtual bool pressButton(int iPK_Button, Message *pMessage);

  protected:
  private:
    virtual string getSlotsAndRoms();
  };
}

#endif
