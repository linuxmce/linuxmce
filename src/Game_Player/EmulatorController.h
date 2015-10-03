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
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Message.h"
#include <list>

namespace DCE
{

  class EmulatorModel;
  class Game_Player;

  class EmulatorController
  {

    friend class Game_Player;

  private:
  protected:
    Game_Player *m_pGame_Player;
    EmulatorModel *m_pEmulatorModel;
    virtual bool sanityCheck(string sFuncName);
  public:

    EmulatorController(Game_Player *pGame_Player, EmulatorModel *pEmulatorModel);
    virtual ~EmulatorController();

    // not so pure virtuals. :P
    virtual bool init();
    virtual bool run();
    virtual bool stop();
    virtual void EmulatorHasExited(int iExit_Code);
    virtual void insertMediaNamed(string sMediaFile, string sSlot="default");
    virtual void ejectMediaFromSlot(string sSlot="default");
    virtual void ejectAllMedia();
    virtual bool getMediaInSlot(string& sMediaFile, string sSlot="default");
    virtual void setStreaming(bool bStreaming);
    virtual void setStreamingMaster(bool bStreamingMaster);
    virtual void setHostName(string sHostName);
    virtual void setStreamID(int iStreamID);
    virtual void setMediaPosition(string sMediaPosition);
    virtual void setOrbiter(long int dwPK_Device_Orbiter);
    virtual void setSystemConfiguration(string sSystemConfiguration);
    // Pure virtuals
    virtual bool doAction(string sAction) = 0;
    virtual void pleaseResend() = 0;
    // actions
    virtual bool P1Start();
    virtual bool P2Start();
    virtual bool P3Start();
    virtual bool P4Start();
    virtual bool coin1();
    virtual bool coin2();
    virtual bool pause();
    virtual bool unpause();
    virtual bool pressButton(int iPK_Button, Message *pMessage) = 0;
    virtual bool pressClick(int iPositionX, int iPositionY, Message *pMessage) = 0;
    virtual bool setSpeed(int iSpeed);
    virtual bool getSnap(long int iPK_Device, int iWidth, int iHeight, char **pData, int& iData_Size) = 0;
    virtual bool gotoMenu(int iMenu);
    virtual bool uiUp();
    virtual bool uiDown();
    virtual bool uiLeft();
    virtual bool uiRight();
    virtual bool uiOK();
    virtual bool press0(Message *pMessage);
    virtual bool press1(Message *pMessage);
    virtual bool press2(Message *pMessage);
    virtual bool press3(Message *pMessage);
    virtual bool press4(Message *pMessage);
    virtual bool press5(Message *pMessage);
    virtual bool press6(Message *pMessage);
    virtual bool press7(Message *pMessage);
    virtual bool press8(Message *pMessage);
    virtual bool press9(Message *pMessage);
    virtual bool pressAsterisk();
    virtual bool pressPound();
    virtual bool uiBack();
    virtual bool saveState(string& sPosition, string& sText,bool bAutoSave=false,string sAutoSaveName="") = 0;
    virtual bool loadState(string sPosition) = 0;
    virtual bool emulatorExited();
    virtual bool service1();
    virtual bool service2();
    virtual bool start();
    virtual bool select();
    virtual bool option();
    virtual bool reset();
    virtual bool record();
    virtual bool gracefulExit();
    virtual bool selectA();
    virtual bool selectB();
    bool canSaveState();
    virtual void getOption(string sPath, string &sValue_To_Assign, Message *pMessage);
    virtual void setOption(string sPath, string sValue_To_Assign, Message *pMessage);
    virtual void waitForEmulatorExit();
  };

}

#endif
