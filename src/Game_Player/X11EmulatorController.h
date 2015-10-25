/**
 * X11EmulatorController - A class to define common elements for 
 * emulators controlled by synthesizing X11 events. 
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef X11EMULATORCONTROLLER_H
#define X11EMULATORCONTROLLER_H

#include "EmulatorController.h"
#include "X11EmulatorModel.h"
#include <X11/Xlib.h>

/* STL includes */
#include <map>

#define CHECK_RESEND 1

using namespace std;

namespace DCE
{
  class X11EmulatorController : public EmulatorController, public AlarmEvent
  {
  public:

    bool m_bWindowIdThreadIsRunning;

    X11EmulatorController(Game_Player *pGame_Player, X11EmulatorModel *pEmulatorModel);  // ctor
    virtual ~X11EmulatorController(); // dtor

    virtual bool run();
    virtual bool stop();
    virtual bool init();
    virtual void EmulatorHasExited(int iExit_Code);
    
    void findWindow();
    virtual void pleaseResend();

    void checkResend();

    virtual bool doAction(string sAction);
    virtual void doMediaSwap(string sMediaFilename, string sSlot);
    virtual bool pressButton(int iPK_Button, Message *pMessage);
    virtual bool pressClick(int iPositionX, int iPositionY, Message *pMessage);
    virtual bool getSnap(long int iPK_Device, int iWidth, int iHeight, char **pData, int& iData_Size);
    virtual bool setSpeed(int iSpeed);
    virtual bool gotoMenu(int iMenu);
    virtual bool saveState(string& sPosition, string& sText, bool bAutoSave=false, string sAutoSaveName="");
    virtual bool loadState(string sPosition);
    virtual void waitForEmulatorExit();

    class AlarmManager *m_pAlarmManager;
    void AlarmCallback(int id, void* param);

  protected:
    X11EmulatorModel *m_pEmulatorModel;
  private:
    pthread_t m_windowIdThread;
    string m_sLastAction;
    bool m_bResend; // Resend keystroke if needed. Set by default window handler.
    bool m_bResendFired;
    bool getWindowCalled(string sWindowName, Window& windowId);
  };
}

#endif
