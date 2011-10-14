/**
 * EmulatorController - A base class to implement a controller
 * to control emulation engines.
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef EMULATORCONTROLLER_H
#define EMULATORCONTROLLER_H

#include "EmulatorModel.h"
#include "Game_Player.h"

namespace DCE
{
  class EmulatorController
  {
  private:
  protected:
    Game_Player *m_pGame_Player;
    EmulatorModel *m_pEmulatorModel;
  public:

    EmulatorController(Game_Player *pGame_Player, EmulatorModel *pEmulatorModel);
    ~EmulatorController();

    // not so pure virtuals. :P
    virtual bool run();
    virtual bool stop();
    // Pure virtuals
    virtual bool P1Start() = 0;
    virtual bool P2Start() = 0;
    virtual bool P3Start() = 0;
    virtual bool P4Start() = 0;
    virtual bool coin1() = 0;
    virtual bool coin2() = 0;
    virtual bool pause() = 0;
    virtual bool unpause() = 0;
    virtual bool pressButton(int iPK_Button) = 0;
    virtual bool pressClick(int iPositionX, int iPositionY, int iButtons) = 0;
    virtual bool setSpeed(int iSpeed) = 0;
    virtual bool getSnap(char **pData, int iData_Size) = 0;
    virtual bool gotoMenu(int iMenu) = 0;
    virtual bool uiUp() = 0;
    virtual bool uiDown() = 0;
    virtual bool uiLeft() = 0;
    virtual bool uiRight() = 0;
    virtual bool uiOK() = 0;
    virtual bool press0() = 0;
    virtual bool press1() = 0;
    virtual bool press2() = 0;
    virtual bool press3() = 0;
    virtual bool press4() = 0;
    virtual bool press5() = 0;
    virtual bool press6() = 0;
    virtual bool press7() = 0;
    virtual bool press8() = 0;
    virtual bool press9() = 0;
    virtual bool uiBack() = 0;
    virtual bool saveState() = 0;
    virtual bool loadState() = 0;
    virtual bool emulatorExited() = 0;
    virtual bool service1() = 0;
    virtual bool service2() = 0;
    virtual bool start() = 0;
    virtual bool select() = 0;
    virtual bool option() = 0;
    virtual bool reset() = 0;
  };

}

#endif
